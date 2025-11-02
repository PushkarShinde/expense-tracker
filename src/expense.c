#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#include "expense.h"
#include "fileops.h"
#include "colors.h"


void clearInputBuffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

/* Helper: read a line into buf (strips newline). Uses fgets to avoid scanf problems. */
static void readLine(const char* prompt, char* buf, int bufsize) {
    if (prompt) printf("%s", prompt);
    if (!fgets(buf, bufsize, stdin)) {
        // input error — clear buffer
        buf[0] = '\0';
        return;
    }
    // strip newline
    size_t len = strlen(buf);
    if (len > 0 && buf[len-1] == '\n') buf[len-1] = '\0';
}

/* Add Expense — uses getNextID and saveExpense */
void addExpense() {
    Expense e;
    // clear
    memset(&e, 0, sizeof(Expense));
    char tmp[64];

    // category
    readLine("\nEnter category: ", e.category, CAT_LEN);

    // note
    readLine("Enter note: ", e.note, NOTE_LEN);

    // amount — read into tmp then parse float
    while (1) {
        readLine("Enter amount: ", tmp, sizeof(tmp));
        char* endptr;
        e.amount = strtof(tmp, &endptr);
        
        // Check if:
        // 1. Conversion happened (endptr moved from start)
        // 2. No extra characters after the number
        // 3. Amount is positive
        if (endptr != tmp && *endptr == '\0' && e.amount > 0) {
            break;
        } else {
            printf(FG_RED "Invalid amount. Please enter a valid positive number.\n" RESET);
        }
    }

    // date
    while (1) {
        readLine("Enter date (DD-MM-YYYY): ", e.date, DATE_LEN);
        
        // Basic date format validation
        if (strlen(e.date) == 10 && 
            isdigit(e.date[0]) && isdigit(e.date[1]) && e.date[2] == '-' &&
            isdigit(e.date[3]) && isdigit(e.date[4]) && e.date[5] == '-' &&
            isdigit(e.date[6]) && isdigit(e.date[7]) && isdigit(e.date[8]) && isdigit(e.date[9])) {
            break;
        } else {
            printf(FG_RED "Invalid date format. Please use DD-MM-YYYY (e.g., 24-09-2024).\n" RESET);
        }
    }

    // id
    e.id = getNextID();

    saveExpense(&e);
    printf(FG_GREEN "\nExpense added successfully! (ID: %d)\n" RESET, e.id);
}

/* Utility: print header */
static void printTableHeader() {
    printf("\n" FG_GREEN BOLD "-----------------------------------------------------------------------------" RESET "\n");
    printf(BOLD "| ID |   Category        |   Date       |   Amount     |   Note              |\n" RESET);
    printf(FG_GREEN "-----------------------------------------------------------------------------" RESET "\n");
}

/* View All */
void viewExpenses() {
    int n = 0;
    Expense* arr = loadAllExpenses(&n);
    if (!arr || n == 0) {
        printf(FG_RED "\nNo expense records found.\n" RESET);
        free(arr);
        return;
    }

    printTableHeader();
    for (int i = 0; i < n; ++i) {
        Expense* e = &arr[i];
        printf("| %2d | %-16s | %-11s | %10.2f | %-18s |\n",
               e->id, e->category, e->date, e->amount, e->note);
    }
    printf(FG_GREEN "-----------------------------------------------------------------------------" RESET "\n");
    free(arr);
}

/* Search by Month (MM-YYYY) where our stored date is DD-MM-YYYY */
void searchByMonth() {
    char target[16];
    readLine("\nEnter month-year to search (MM-YYYY): ", target, sizeof(target));
    if (strlen(target) < 7) {
        printf(FG_RED "Invalid format. Use MM-YYYY\n" RESET);
        return;
    }

    int n = 0;
    Expense* arr = loadAllExpenses(&n);
    if (!arr || n == 0) {
        printf(FG_RED "\nNo expense records found.\n" RESET);
        free(arr);
        return;
    }

    double total = 0.0;
    int foundCount = 0;

    printTableHeader();
    for (int i = 0; i < n; ++i) {
        if (strlen(arr[i].date) >= 10) {
            char monthYear[8] = {0};
            memcpy(monthYear, &arr[i].date[3], 7);
            monthYear[7] = '\0';
            if (strcmp(monthYear, target) == 0) {
                printf("| %2d | %-16s | %-11s | %10.2f | %-18s |\n",
                       arr[i].id, arr[i].category, arr[i].date, arr[i].amount, arr[i].note);
                total += arr[i].amount;
                foundCount++;
            }
        }
    }
    printf(FG_GREEN "-----------------------------------------------------------------------------" RESET "\n");

    // Summary 
    printf("\n" BOLD "Summary for %s" RESET "\n", target);
    printf("Records found: %d\n", foundCount);
    printf(BOLD "Total Spent in %s: %.2f\n" RESET, target, total);

    free(arr);
}

/* Search by Category (case-insensitive, substring match) */
void searchByCategory() {
    char target[CAT_LEN];
    readLine("\nEnter category to search: ", target, sizeof(target));
    if (strlen(target) == 0) {
        printf(FG_RED "Empty category.\n" RESET);
        return;
    }

    // lowercase target for substring matching
    for (char* p = target; *p; ++p) *p = (char)tolower((unsigned char)*p);

    int n = 0;
    Expense* arr = loadAllExpenses(&n);
    if (!arr || n == 0) {
        printf(FG_RED "\nNo expense records found.\n" RESET);
        free(arr);
        return;
    }

    printTableHeader();
    int foundCount = 0;
    double total = 0.0;
    for (int i = 0; i < n; ++i) {
        char catLower[CAT_LEN];
        strncpy(catLower, arr[i].category, CAT_LEN);
        catLower[CAT_LEN-1] = '\0';
        for (char* p = catLower; *p; ++p) *p = (char)tolower((unsigned char)*p);

        if (strstr(catLower, target) != NULL) {
            printf("| %2d | %-16s | %-11s | %10.2f | %-18s |\n",
                   arr[i].id, arr[i].category, arr[i].date, arr[i].amount, arr[i].note);
            foundCount++;
            total += arr[i].amount;
        }
    }
    printf(FG_GREEN "-----------------------------------------------------------------------------" RESET "\n");

    // Small summary for category search as well
    printf("\nRecords found: %d\n", foundCount);
    if (foundCount > 0) printf(BOLD "Total for category '%s': %.2f\n" RESET, target, total);

    free(arr);
}

/* Delete by ID */
void deleteExpenseByID() {
    char tmp[64];
    int id;
    readLine("\nEnter ID to delete: ", tmp, sizeof(tmp));
    if (sscanf(tmp, "%d", &id) != 1) {
        printf(FG_RED "Invalid ID.\n" RESET);
        return;
    }

    int n = 0;
    Expense* arr = loadAllExpenses(&n);
    if (!arr || n == 0) {
        printf(FG_RED "No records to delete.\n" RESET);
        free(arr);
        return;
    }

    int found = 0;
    Expense* newArr = (Expense*)malloc(sizeof(Expense) * n);
    int newCount = 0;
    for (int i = 0; i < n; ++i) {
        if (arr[i].id == id) {
            found = 1;
            continue; // skip this record
        }
        newArr[newCount++] = arr[i];
    }

    if (!found) {
        printf(FG_RED "ID %d not found.\n" RESET, id);
        free(arr);
        free(newArr);
        return;
    }

    if (overwriteAllExpenses(newArr, newCount) == 0) {
        printf(FG_GREEN "Deleted expense with ID %d\n" RESET, id);
    } else {
        printf(FG_RED "Failed to update file.\n" RESET);
    }

    free(arr);
    free(newArr);
}

/* Edit by ID — allow user to press enter to keep existing value */
void editExpenseByID() {
    char tmp[256];
    int id;
    readLine("\nEnter ID to edit: ", tmp, sizeof(tmp));
    if (sscanf(tmp, "%d", &id) != 1) {
        printf(FG_RED "Invalid ID.\n" RESET);
        return;
    }

    int n = 0;
    Expense* arr = loadAllExpenses(&n);
    if (!arr || n == 0) {
        printf(FG_RED "No records to edit.\n" RESET);
        free(arr);
        return;
    }

    int idx = -1;
    for (int i = 0; i < n; ++i) if (arr[i].id == id) { idx = i; break; }

    if (idx == -1) {
        printf(FG_RED "No record found for ID %d.\n" RESET, id);
        free(arr);
        return;
    }

    Expense e;
    memset(&e, 0, sizeof(Expense));
    e.id = id;

    readLine("Enter new category: ", e.category, CAT_LEN);
    readLine("Enter new note: ", e.note, NOTE_LEN);

    while (1) {
        readLine("Enter new amount: ", tmp, sizeof(tmp));
        if (sscanf(tmp, "%f", &e.amount) == 1) break;
        printf(FG_RED "Invalid amount. Try again.\n" RESET);
    }

    readLine("Enter new date (DD-MM-YYYY): ", e.date, DATE_LEN);

    // replace in array and overwrite file
    arr[idx] = e;
    if (overwriteAllExpenses(arr, n) == 0) {
        printf(FG_GREEN "Expense (ID %d) updated.\n" RESET, id);
    } else {
        printf(FG_RED "Failed to save changes.\n" RESET);
    }

    free(arr);
}

/* Monthly summary: total + per-category totals */
void monthlySummary() {
    char target[16];
    readLine("\nEnter month-year for summary (MM-YYYY): ", target, sizeof(target));
    if (strlen(target) < 7) {
        printf(FG_RED "Invalid format. Use MM-YYYY\n" RESET);
        return;
    }

    int n = 0;
    Expense* arr = loadAllExpenses(&n);
    if (!arr || n == 0) {
        printf(FG_RED "No records.\n" RESET);
        free(arr);
        return;
    }

    double grandTotal = 0.0;
    int foundCount = 0;

    for (int i = 0; i < n; ++i) {
        if (strlen(arr[i].date) < 10) continue;
        char my[8] = {0};
        memcpy(my, &arr[i].date[3], 7);
        my[7] = '\0';
        if (strcmp(my, target) == 0) {
            grandTotal += arr[i].amount;
            foundCount++;
        }
    }

    printf("\n" BOLD "Monthly Summary for %s\n" RESET, target);
    printf("Records: %d\n", foundCount);
    printf(BOLD "Total Spent: %.2f\n" RESET, grandTotal);

    free(arr);
}