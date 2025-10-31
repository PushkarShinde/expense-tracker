#include <stdio.h>
#include "expense.h"
#include "fileops.h"

void addExpense() {
    Expense e;
    printf("\nEnter category: ");
    scanf("%s", e.category);

    printf("Enter note: ");
    scanf("%s", e.note);

    printf("Enter amount: ");
    scanf("%f", &e.amount);

    printf("Enter date (DD-MM-YYYY): ");
    scanf("%s", e.date);

    saveExpense(&e);
    printf("\nExpense added successfully!\n");
}
