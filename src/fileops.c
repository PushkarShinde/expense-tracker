#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "fileops.h"
#include "encrypt.h"

#define DATA_FILE "data/expenses.dat"
#define ID_FILE "data/id_counter.txt"
#define TEMP_FILE "data/expenses.tmp"

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

void saveExpense(Expense* e) {
    FILE* fp = fopen(DATA_FILE, "ab");
    if(!fp){
        printf("Error: cannot open data file for writing.\n");
        return;
    }

    char buffer[sizeof(Expense)];
    memcpy(buffer, e, sizeof(Expense));

    xorEncryptDecrypt(buffer, sizeof(Expense));

    fwrite(buffer, sizeof(Expense), 1, fp);
    fclose(fp);
}

/* Load all records into a dynamically allocated array.
   Caller must free() the returned pointer.
*/
Expense* loadAllExpenses(int* outCount){
    *outCount=0;
    FILE* fp=fopen(DATA_FILE, "rb");
    if(!fp) return NULL;

    Expense* arr=NULL;
    int cap=0;
    Expense temp;
    while(fread(&temp, sizeof(Expense),1,fp)==1){
        xorEncryptDecrypt((char*)&temp, sizeof(Expense));

        if(*outCount>=cap){
            cap=(cap==0)?8:cap*2;
            arr=(Expense*)realloc(arr, sizeof(Expense)*cap);
            if(!arr){
                fclose(fp);
                *outCount=0;
                return NULL;
            }
        }
        arr[*outCount]=temp;
        (*outCount)++;
    }
    fclose(fp);
    return arr;
}

/* Overwrite all expenses atomically (write to temp then rename)
   Returns 0 on success, -1 on failure.
*/
int overwriteAllExpenses(Expense* arr, int count) {
    FILE* fp = fopen(TEMP_FILE, "wb");
    if (!fp) {
        printf("Error: cannot open temporary file for writing.\n");
        return -1;
    }

    for (int i=0;i<count; ++i) {
        char buffer[sizeof(Expense)];
        memcpy(buffer, &arr[i], sizeof(Expense));
        xorEncryptDecrypt(buffer, sizeof(Expense));
        if (fwrite(buffer, sizeof(Expense), 1, fp) != 1) {
            fclose(fp);
            remove(TEMP_FILE);
            return -1;
        }
    }

    fclose(fp);

    // remove original, rename temp -> original
    remove(DATA_FILE); // ignore failure if not exist
    if (rename(TEMP_FILE, DATA_FILE) != 0) {
        printf("Error: could not rename temp file.\n");
        return -1;
    }
    return 0;
}
