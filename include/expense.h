#ifndef EXPENSE_H
#define EXPENSE_H

typedef struct {
    char category[20];
    char note[50];
    float amount;
    char date[11]; // DD-MM-YYYY
} Expense;

void addExpense();
void viewExpenses();

#endif
