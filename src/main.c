#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "ui.h"
#include "expense.h"
#include "fileops.h"

/* reuse small readLine helper */
static void readLine(const char* prompt, char* buf, int bufsize) {
    if (prompt) printf("%s", prompt);
    if (!fgets(buf, bufsize, stdin)) { buf[0] = '\0'; return; }
    size_t len = strlen(buf);
    if (len > 0 && buf[len-1] == '\n') buf[len-1] = '\0';
}

int main(void) {
    char pwd[128];

    /* Ask for password on startup (P1). Allow 3 tries if file exists. */
    int tries = 0;
    int maxTries = 3;
    int valid = 0;

    /* Prompt user for password — if no file exists, any password will be accepted and used when saving. */
    /* We attempt to decrypt once to verify; if there's no encrypted file it's ok. */
    while (tries < maxTries) {
        readLine("Enter encryption password: ", pwd, sizeof(pwd));
        setEncryptionPassword(pwd);

        /* Try loading to verify password, but don't print error if file not present */
        int n = 0;
        Expense* arr = loadAllExpenses(&n);
        if (!arr) {
            /* Either file doesn't exist (ok) or decryption failed (bad pwd). */
            /* Check if file exists by trying to open it */
            FILE* f = fopen("data/expenses.enc", "rb");
            if (!f) {
                /* no encrypted file — accept this password */
                valid = 1;
                break;
            } else {
                /* file exists but decryption failed */
                fclose(f);
                printf("Decryption failed — wrong password or corrupted file. Try again.\n");
                tries++;
                continue;
            }
        } else {
            /* success: decryption worked; free arr and proceed */
            free(arr);
            valid = 1;
            break;
        }
    }

    if (!valid) {
        printf("Failed to provide correct password. Exiting.\n");
        return 1;
    }

    /* main menu loop */
    char buf[16];
    while (1) {
        printMenu();
        if (!fgets(buf, sizeof(buf), stdin)) break;
        int choice = 0;
        if (sscanf(buf, "%d", &choice) != 1) {
            printf("Invalid selection.\n");
            continue;
        }

        switch (choice) {
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
