#include <stdio.h>
#include <string.h>
#include "expense.h"
#include "fileops.h"
#include "colors.h"
#include "encrypt.h"

void addExpense() {
    Expense e;
    printf("\nEnter category: ");
    scanf(" %[^\n]", e.category);

    printf("Enter note: ");
    scanf(" %[^\n]", e.note);

    printf("Enter amount: ");
    scanf("%f", &e.amount);

    printf("Enter date (DD-MM-YYYY): ");
    scanf(" %[^\n]", e.date);

    saveExpense(&e);
    printf("\nExpense added successfully!\n");
}

void viewExpenses() {
    FILE* fp = fopen("data/expenses.dat", "rb");
    if (!fp) {
        printf(FG_RED "\nNo expense records found.\n" RESET);
        return;
    }

    Expense e;
    printf("\n" FG_GREEN BOLD "-------------------------------------------------------------------\n" RESET);
    printf(BOLD "|   Category        |   Date       |   Amount    |     Note       |\n" RESET);
    printf(FG_GREEN "-------------------------------------------------------------------\n" RESET);

    while (fread(&e, sizeof(Expense), 1, fp)) {
        // decrypt
        xorEncryptDecrypt((char*)&e, sizeof(Expense));

        printf("| %-16s | %-11s | %-10.2f | %-12s |\n",
               e.category, e.date, e.amount, e.note);
    }

    printf(FG_GREEN "-------------------------------------------------------------------\n" RESET);
    fclose(fp);
}
