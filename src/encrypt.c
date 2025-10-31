#include "encrypt.h"

#define KEY 0x5A //90

void xorEncryptDecrypt(char* data, int len) {
    for(int i = 0; i < len; i++) {
        data[i] ^= KEY;
    }
}
