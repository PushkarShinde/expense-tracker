#include <stdio.h>
#include "ui.h"
#include "expense.h"

int main() {
    int choice;

    while (1) {
        printMenu();
        scanf("%d", &choice);

        if (choice == 1) {
            addExpense();
        } else if (choice == 2) {
            viewExpenses();
        } else if (choice == 3) {
            printf("Exiting...\n");
            break;
        }
    }

    return 0;
}
