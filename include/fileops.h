#ifndef FILEOPS_H
#define FILEOPS_H

#include "expense.h"

int getNextID(void);
void saveExpense(Expense* e);
Expense* loadAllExpenses(int* outCount);
int overwriteAllExpenses(Expense* arr, int count); //returns 0 on success

#endif
