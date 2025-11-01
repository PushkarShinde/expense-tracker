#include <stdio.h>
#include "colors.h"

void printMenu() {
    printf("\n" FG_CYAN "==============================" RESET "\n");
    printf(BOLD "     Expense Tracker\n" RESET);
    printf(FG_CYAN "==============================" RESET "\n");
    printf(FG_YELLOW "1." RESET "Add Expense\n");
    printf(FG_YELLOW "2." RESET "View Expenses\n");
    printf(FG_YELLOW "3." RESET "Exit\n");
    printf("Choose an option: ");
}
