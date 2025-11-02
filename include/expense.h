#ifndef EXPENSE_H
#define EXPENSE_H

#define CAT_LEN 32
#define NOTE_LEN 128
#define DATE_LEN 11

typedef struct {
    int id;
    char category[CAT_LEN];
    char note[NOTE_LEN];
    float amount;
    char date[DATE_LEN]; // DD-MM-YYYY
} Expense;

void addExpense();
void viewExpenses();
void searchByMonth();
void searchByCategory();
void deleteExpenseByID();
void editExpenseByID();
void monthlySummary();

#endif
