#include <stdio.h>
#include "colors.h"

void printMenu() {
    printf("\n" FG_CYAN "==============================" RESET "\n");
    printf(BOLD "      Expense Tracker\n" RESET);
    printf(FG_CYAN "==============================" RESET "\n");
    printf(FG_YELLOW "1." RESET " Add Expense\n");
    printf(FG_YELLOW "2." RESET " View Expenses\n");
    printf(FG_YELLOW "3." RESET " Search by Month\n");
    printf(FG_YELLOW "4." RESET " Search by Category\n");
    printf(FG_YELLOW "5." RESET " Delete Expense by ID\n");
    printf(FG_YELLOW "6." RESET " Edit Expense by ID\n");
    printf(FG_YELLOW "7." RESET " Monthly Summary\n");
    printf(FG_YELLOW "8." RESET " Exit\n");
    printf("Choose an option: ");
}
