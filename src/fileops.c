#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "fileops.h"
#include "encrypt.h"

#define DATA_FILE "data/expenses.enc"
#define ID_FILE "data/id_counter.txt"
#define TEMP_FILE "data/expenses.tmp" /* temp unencrypted for safety not used now */

static char g_password[256] = {0};

/* set the global password */
void setEncryptionPassword(const char* password) {
    if (!password) { g_password[0] = '\0'; return; }
    strncpy(g_password, password, sizeof(g_password)-1);
    g_password[sizeof(g_password)-1] = '\0';
}

/* read file into memory buffer */
static unsigned char* read_file(const char* path, size_t* outLen) {
    *outLen = 0;
    FILE* f = fopen(path, "rb");
    if (!f) return NULL;
    if (fseek(f, 0, SEEK_END) != 0) { fclose(f); return NULL; }
    long sz = ftell(f);
    rewind(f);
    if (sz <= 0) { fclose(f); return NULL; }
    unsigned char* buf = (unsigned char*)malloc(sz);
    if (!buf) { fclose(f); return NULL; }
    if (fread(buf, 1, sz, f) != (size_t)sz) { free(buf); fclose(f); return NULL; }
    fclose(f);
    *outLen = (size_t)sz;
    return buf;
}

/* write raw buffer to file (overwrite) */
static int write_file(const char* path, const unsigned char* buf, size_t len) {
    FILE* f = fopen(path, "wb");
    if (!f) return -1;
    if (fwrite(buf, 1, len, f) != len) { fclose(f); return -1; }
    fclose(f);
    return 0;
}

/* getNextID unchanged (reads/writes id_counter.txt) */
int getNextID(void){
    FILE* f=fopen(ID_FILE, "r+");
    int id=0;
    if(!f){ //if no id file yet, create and start from 1
        f=fopen(ID_FILE, "w");
        if(!f) return 1;//fallback
        fprintf(f, "%d\n", 1);
        fclose(f);
        return 1;
    }
    if(fscanf(f, "%d", &id)!=1){
        id=0;
    }
    id++;
    rewind(f);
    fprintf(f, "%d\n", id);
    fclose(f);
    return id;
}

/* Internal: write the whole Expense array encrypted to DATA_FILE using g_password */
static int write_expenses_encrypted(Expense* arr, int count) {
    if (!g_password || strlen(g_password) == 0) {
        printf("Encryption password not set. Cannot save.\n");
        return -1;
    }

    /* serialize array to contiguous bytes */
    size_t plainLen = (size_t)count * sizeof(Expense);
    unsigned char* plain = (unsigned char*)malloc(plainLen);
    if (!plain) return -1;
    for (int i = 0; i < count; ++i) {
        memcpy(plain + (size_t)i * sizeof(Expense), &arr[i], sizeof(Expense));
    }

    unsigned char* outBuf = NULL;
    size_t outLen = 0;
    int r = encrypt_buffer_with_password(plain, plainLen, g_password, &outBuf, &outLen);
    free(plain);
    if (r != 0 || !outBuf) {
        if (outBuf) free(outBuf);
        return -1;
    }

    /* write to DATA_FILE atomically (temp then rename) */
    char tmpPath[] = "data/expenses.enc.tmp";
    if (write_file(tmpPath, outBuf, outLen) != 0) {
        free(outBuf);
        return -1;
    }
    free(outBuf);
    /* rename */
    remove(DATA_FILE); /* ignore errors */
    if (rename(tmpPath, DATA_FILE) != 0) {
        /* attempt to write directly */
        fprintf(stderr, "Warning: rename failed while saving encrypted file.\n");
        return -1;
    }
    return 0;
}

/* loadAllExpenses: decrypt DATA_FILE using g_password and return array */
Expense* loadAllExpenses(int* outCount) {
    *outCount = 0;
    /* if file doesn't exist, return NULL */
    size_t inLen = 0;
    unsigned char* inBuf = read_file(DATA_FILE, &inLen);
    if (!inBuf) {
        /* no encrypted file found -> empty dataset */
        return NULL;
    }

    if (!g_password || strlen(g_password) == 0) {
        printf("Encryption password not set. Cannot decrypt file.\n");
        free(inBuf);
        return NULL;
    }

    unsigned char* plain = NULL;
    size_t plainLen = 0;
    int r = decrypt_buffer_with_password(inBuf, inLen, g_password, &plain, &plainLen);
    free(inBuf);
    if (r != 0 || !plain) {
        /* decryption failed */
        return NULL;
    }

    /* plainLen should be multiple of sizeof(Expense) */
    if (plainLen % sizeof(Expense) != 0) {
        free(plain);
        return NULL;
    }

    int count = (int)(plainLen / sizeof(Expense));
    Expense* arr = (Expense*)malloc(sizeof(Expense) * count);
    if (!arr) { free(plain); return NULL; }
    memcpy(arr, plain, plainLen);
    free(plain);
    *outCount = count;
    return arr;
}

/* overwriteAllExpenses: encrypt and write all records using g_password */
int overwriteAllExpenses(Expense* arr, int count) {
    return write_expenses_encrypted(arr, count);
}

/* saveExpense: load all, append, write encrypted back */
void saveExpense(Expense* e) {
    int n = 0;
    Expense* arr = loadAllExpenses(&n);
    int newCount = n + 1;
    Expense* newArr = (Expense*)malloc(sizeof(Expense) * newCount);
    if (!newArr) {
        if (arr) free(arr);
        printf("Memory error: cannot save expense\n");
        return;
    }
    if (arr && n > 0) {
        memcpy(newArr, arr, sizeof(Expense) * n);
        free(arr);
    }
    newArr[newCount-1] = *e;
    if (write_expenses_encrypted(newArr, newCount) != 0) {
        printf("Failed to save encrypted data.\n");
    }
    free(newArr);
}