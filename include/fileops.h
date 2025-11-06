#ifndef FILEOPS_H
#define FILEOPS_H

#include "expense.h"

int getNextID(void);
void saveExpense(Expense* e);

/* Load all expenses into dynamically allocated array (caller must free).
   Returns pointer (or NULL) and sets outCount to number of records.
   This function will attempt to decrypt using the password set via setEncryptionPassword().
*/
Expense* loadAllExpenses(int* outCount);

/* Overwrite all expenses (will encrypt using set password)
   Returns 0 on success, -1 on failure.
*/
int overwriteAllExpenses(Expense* arr, int count);

/* Set encryption password (must be called at startup for encrypted file ops).
   Copies string internally.
*/
void setEncryptionPassword(const char* password);

#endif
