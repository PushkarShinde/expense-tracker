#include <stdio.h>
#include <string.h>
#include "fileops.h"
#include "encrypt.h"

void saveExpense(Expense* e) {
    FILE* fp = fopen("data/expenses.dat", "ab");

    char buffer[sizeof(Expense)];
    memcpy(buffer, e, sizeof(Expense));

    xorEncryptDecrypt(buffer, sizeof(Expense));

    fwrite(buffer, sizeof(Expense), 1, fp);
    fclose(fp);
}
