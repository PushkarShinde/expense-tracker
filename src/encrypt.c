#include "encrypt.h"

#include <openssl/evp.h>
#include <openssl/rand.h>
#include <openssl/err.h>
#include <openssl/sha.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#define MAGIC "EXPENC1V1"     /* 9 bytes including null? We'll write 8 bytes (no null) */
#define MAGIC_LEN 8           /* we'll store exactly 8 bytes, "EXPENC1v" */
#define SALT_LEN 16
#define IV_LEN 16
#define KEY_LEN 32            /* 256 bits */
#define PBKDF2_ITERS 100000

/* For portability: ensure MAGIC is exactly MAGIC_LEN */
static const unsigned char FILE_MAGIC[MAGIC_LEN] = { 'E','X','P','E','N','C','1','v' };

/* random bytes helper */
int random_bytes(unsigned char* buf, int num) {
    if (RAND_bytes(buf, num) != 1) return -1;
    return 0;
}

/* encrypt plaintext using password; output format: MAGIC(8) | salt(16) | iv(16) | ciphertext */
int encrypt_buffer_with_password(const unsigned char* plaintext, size_t plainLen,
                                 const char* password,
                                 unsigned char** outBuf, size_t* outLen) {
    if (!plaintext || !password || !outBuf || !outLen) return -1;

    int ret = -1;
    unsigned char salt[SALT_LEN];
    unsigned char iv[IV_LEN];
    unsigned char key[KEY_LEN];

    /* generate salt & iv */
    if (random_bytes(salt, SALT_LEN) != 0) goto cleanup;
    if (random_bytes(iv, IV_LEN) != 0) goto cleanup;

    /* derive key using PBKDF2 */
    if (!PKCS5_PBKDF2_HMAC(password, (int)strlen(password),
                           salt, SALT_LEN,
                           PBKDF2_ITERS, EVP_sha256(), KEY_LEN, key)) {
        goto cleanup;
    }

    EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
    if (!ctx) goto cleanup;

    if (EVP_EncryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, key, iv) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        goto cleanup;
    }

    /* allocate output: ciphertext <= plainLen + block_size */
    int block_size = EVP_CIPHER_block_size(EVP_aes_256_cbc());
    size_t max_ct_len = plainLen + block_size;
    unsigned char* ciphertext = (unsigned char*)malloc(max_ct_len);
    if (!ciphertext) { EVP_CIPHER_CTX_free(ctx); goto cleanup; }

    int outl = 0;
    int tmplen = 0;
    if (EVP_EncryptUpdate(ctx, ciphertext, &outl, plaintext, (int)plainLen) != 1) {
        free(ciphertext);
        EVP_CIPHER_CTX_free(ctx);
        goto cleanup;
    }
    if (EVP_EncryptFinal_ex(ctx, ciphertext + outl, &tmplen) != 1) {
        free(ciphertext);
        EVP_CIPHER_CTX_free(ctx);
        goto cleanup;
    }
    outl += tmplen;

    /* build output buffer: MAGIC + salt + iv + ciphertext */
    size_t total = MAGIC_LEN + SALT_LEN + IV_LEN + outl;
    unsigned char* buf = (unsigned char*)malloc(total);
    if (!buf) { free(ciphertext); EVP_CIPHER_CTX_free(ctx); goto cleanup; }

    unsigned char* p = buf;
    memcpy(p, FILE_MAGIC, MAGIC_LEN); p += MAGIC_LEN;
    memcpy(p, salt, SALT_LEN); p += SALT_LEN;
    memcpy(p, iv, IV_LEN); p += IV_LEN;
    memcpy(p, ciphertext, outl);

    *outBuf = buf;
    *outLen = total;

    free(ciphertext);
    EVP_CIPHER_CTX_free(ctx);
    ret = 0;

cleanup:
    /* wipe key material */
    OPENSSL_cleanse(key, sizeof(key));
    return ret;
}

/* decrypt input containing MAGIC|salt|iv|ciphertext */
int decrypt_buffer_with_password(const unsigned char* inBuf, size_t inLen,
                                 const char* password,
                                 unsigned char** out, size_t* outLen) {
    if (!inBuf || !password || !out || !outLen) return -1;
    if (inLen <= (size_t)(MAGIC_LEN + SALT_LEN + IV_LEN)) return -1;

    /* check magic */
    if (memcmp(inBuf, FILE_MAGIC, MAGIC_LEN) != 0) return -1;

    const unsigned char* p = inBuf + MAGIC_LEN;
    const unsigned char* salt = p; p += SALT_LEN;
    const unsigned char* iv = p; p += IV_LEN;
    const unsigned char* ciphertext = p;
    size_t ct_len = inLen - (MAGIC_LEN + SALT_LEN + IV_LEN);

    unsigned char key[KEY_LEN];
    if (!PKCS5_PBKDF2_HMAC(password, (int)strlen(password),
                           salt, SALT_LEN,
                           PBKDF2_ITERS, EVP_sha256(), KEY_LEN, key)) {
        return -1;
    }

    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    if (!ctx) { OPENSSL_cleanse(key, sizeof(key)); return -1; }

    if (EVP_DecryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, key, iv) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        OPENSSL_cleanse(key, sizeof(key));
        return -1;
    }

    unsigned char* plaintext = (unsigned char*)malloc(ct_len + EVP_CIPHER_block_size(EVP_aes_256_cbc()));
    if (!plaintext) { EVP_CIPHER_CTX_free(ctx); OPENSSL_cleanse(key, sizeof(key)); return -1; }

    int outl = 0;
    int tmplen = 0;
    if (EVP_DecryptUpdate(ctx, plaintext, &outl, ciphertext, (int)ct_len) != 1) {
        free(plaintext);
        EVP_CIPHER_CTX_free(ctx);
        OPENSSL_cleanse(key, sizeof(key));
        return -1;
    }
    if (EVP_DecryptFinal_ex(ctx, plaintext + outl, &tmplen) != 1) {
        /* likely wrong password or tampered data */
        free(plaintext);
        EVP_CIPHER_CTX_free(ctx);
        OPENSSL_cleanse(key, sizeof(key));
        return -1;
    }
    outl += tmplen;

    *out = plaintext;
    *outLen = outl;

    EVP_CIPHER_CTX_free(ctx);
    OPENSSL_cleanse(key, sizeof(key));
    return 0;
}
