#ifndef ENCRYPT_H
#define ENCRYPT_H

#include <stddef.h>

/* High-level helpers used by fileops.c */

/* Encrypt `plaintext` (len bytes) using password.
 * On success: returns 0 and *outBuf points to malloc'ed buffer with (MAGIC|salt|iv|ciphertext),
 * *outLen set to total size. Caller must free(*outBuf).
 * On failure: return -1.
 */
int encrypt_buffer_with_password(const unsigned char* plaintext, size_t plainLen, const char* password, unsigned char** outBuf, size_t* outLen);

/* Decrypt a buffer produced by encrypt_buffer_with_password.
 * On success: returns 0 and *out points to malloc'ed plaintext buffer, *outLen set.
 * Caller must free(*out).
 * On failure: return -1.
 */
int decrypt_buffer_with_password(const unsigned char* inBuf, size_t inLen, const char* password, unsigned char** out, size_t* outLen);

/* Convenience: produce cryptographically-secure random bytes.
 * Returns 0 on success, -1 on failure.
 */
int random_bytes(unsigned char* buf, int num);

#endif
