#include <stdio.h>
#include <string.h>
#include "ui.h"
#include "expense.h"

int main(void) {
    char buf[8];
    while(1){
        printMenu();
        if(!fgets(buf, sizeof(buf), stdin)) break;

        buf[strcspn(buf, "\n")] = 0;// Remove newline character
        int choice=0;
        if(sscanf(buf,"%d",&choice)!=1){
            printf("Invalid selection.\n");
            continue;
        }

        switch(choice){
            case 1: addExpense(); break;
            case 2: viewExpenses(); break;
            case 3: searchByMonth(); break;
            case 4: searchByCategory(); break;
            case 5: deleteExpenseByID(); break;
            case 6: editExpenseByID(); break;
            case 7: monthlySummary(); break;
            case 8: printf("Goodbye!\n"); return 0;
            default: printf("Invalid choice.\n"); break;
        }
    }
    return 0;
}
