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
            printf("Exiting...\n");
            break;
        } else {
            printf("Invalid choice. Try again.\n");
        }
    }

    return 0;
}
