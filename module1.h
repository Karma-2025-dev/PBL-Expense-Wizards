#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <ctype.h>

#define MAX_EXPENSES 200    // Maximum number of expenses allowed
#define TypeSize 20         // Maximum length of category name
#define DescSize 30         // Maximum length of description

// Structure representing a single expense
typedef struct expense {
    int id;                     // Unique identifier for each expense
    double amount;              // Expense amount
    time_t date;                // Timestamp of when the expense occurred
    char category[TypeSize];    // Expense category (e.g., food, bills)
    char description[DescSize]; // Brief description of the expense
} Expense;

// Global variables
Expense ex[MAX_EXPENSES];                   // Main expense array
Expense *sortedCategory[MAX_EXPENSES];      // Array of pointers sorted by category
Expense *sortedAmount[MAX_EXPENSES];        // Array of pointers sorted by amount
Expense *sortedDate[MAX_EXPENSES];          // Array of pointers sorted by date

int maxID = 0;           // Current number of expenses
double income = 0;       // Monthly income
double balance = 0;      // Remaining main balance (post bills)
double fixedDue = 0;     // Total fixed bills amount
double extra = 0;        // Extra (flexible spending) balance
double savings = 0;      // Emergency savings
struct tm incomeDate = {0}; // Date of monthly income receipt

// Insert an expense pointer into a sorted array of pointers at a given index
void insert(Expense **exPtr, Expense *value, int index) {
    for (int i = maxID; i > index; i--) {
        exPtr[i] = exPtr[i - 1];
    }
    exPtr[index] = value;
}

// Insert a new expense into the sorted arrays by category, amount, and date
void sortInsert(Expense *newEx) {
    int i;

    // Sort by category (lexicographically)
    for (i = 0; i < maxID; i++) {
        if (strcmp(newEx->category, sortedCategory[i]->category) < 0) break;
    }
    insert(sortedCategory, newEx, i);

    // Sort by amount (ascending)
    for (i = 0; i < maxID; i++) {
        if (newEx->amount < sortedAmount[i]->amount) break;
    }
    insert(sortedAmount, newEx, i);

    // Sort by date (oldest to newest)
    for (i = 0; i < maxID; i++) {
        if (newEx->date < sortedDate[i]->date) break;
    }
    insert(sortedDate, newEx, i);
}

// Add a new expense to the system
void addExpense(double amount, char *category, char *description, time_t date) {
    if (maxID >= MAX_EXPENSES) {
        printf("Error: Expense array is full!\n");
        return;
    }

    ex[maxID].id = maxID + 1;
    ex[maxID].amount = amount;
    ex[maxID].date = date;

    strncpy(ex[maxID].category, category, sizeof(ex[maxID].category) - 1);
    ex[maxID].category[sizeof(ex[maxID].category) - 1] = '\0';

    strncpy(ex[maxID].description, description, sizeof(ex[maxID].description) - 1);
    ex[maxID].description[sizeof(ex[maxID].description) - 1] = '\0';

    sortInsert(&ex[maxID]);
    maxID++;
}

// Display all expenses in original (unsorted) order
void displayAllExpenses(Expense expenses[], int count) {
    int k = printf("\n%-5s %-10s %-20s %-20s %-30s\n", "ID", "Amount", "Date", "Category", "Description");
    while (k-- > 0) printf("-");
    printf("\n");

    for (int i = 0; i < count; i++) {
        char dateStr[26];
        struct tm *tm_info = localtime(&expenses[i].date);
        strftime(dateStr, sizeof(dateStr), "%Y-%m-%d %H:%M:%S", tm_info);

        printf("%-5d %-10.2f %-20s %-20s %-30s\n",
               expenses[i].id, expenses[i].amount, dateStr, expenses[i].category, expenses[i].description);
    }
}

// Display expenses using a sorted pointer array
void displaySortedExpenses(Expense *expenses[], int count) {
    int k = printf("\n%-5s %-10s %-20s %-20s %-30s\n", "ID", "Amount", "Date", "Category", "Description");
    while (k-- > 0) printf("-");
    printf("\n");

    for (int i = 0; i < count; i++) {
        char dateStr[26];
        struct tm *tm_info = localtime(&expenses[i]->date);
        strftime(dateStr, sizeof(dateStr), "%Y-%m-%d %H:%M:%S", tm_info);

        printf("%-5d %-10.2f %-20s %-20s %-30s\n",
               expenses[i]->id, expenses[i]->amount, dateStr, expenses[i]->category, expenses[i]->description);
    }
}

// Capitalizes the first letter of a string, removes leading spaces, and trims newline
void toFirstCap(char *str) {
    int i = 0, j = 0;
    char result[DescSize];

    while (str[i]) {
        if (isspace(str[i]) && j == 0)
            ; // skip leading spaces
        else {
            if (j == 0)
            result[j++] = toupper(str[i]);
        else
            result[j++] = str[i];
        }
        i++;
    }

    result[j] = '\0';
    j = strcspn(result, "\n");
    result[j] = '\0';

    strcpy(str, result);
}

// Check if a given year is a leap year
int isLeapYear(int year) {
    return ((year % 4 == 0 && year % 100 != 0) || (year % 400 == 0));
}

// Return number of days in a given month of a year
int getDaysInMonth(int year, int month) {
    switch (month) {
        case 1: case 3: case 5: case 7: case 8: case 10: case 12: return 31;
        case 4: case 6: case 9: case 11: return 30;
        case 2: return isLeapYear(year) ? 29 : 28;
        default: return -1;
    }
}

// Validate date format as "YYYY-MM-DD"
int validateDate(const char *dateStr) {
    if (strlen(dateStr) != 10) return 0;

    for (int i = 0; i < 10; i++) {
        if ((i == 4 || i == 7) && dateStr[i] != '-') return 0;
        if (i != 4 && i != 7 && !isdigit(dateStr[i])) return 0;
    }

    int year, month, day;
    if (sscanf(dateStr, "%4d-%2d-%2d", &year, &month, &day) != 3) return 0;
    if (year < 1900 || year > 2100) return 0;
    if (month < 1 || month > 12) return 0;
    if (day < 1 || day > getDaysInMonth(year, month)) return 0;

    return 1;
}

// Build and return a tm struct based on day and month of the current year
struct tm getDatefromDay(int day, int month) {
    time_t now = time(NULL);
    struct tm *current = localtime(&now);

    struct tm target = *current;
    target.tm_mon = month - 1;
    target.tm_mday = day;
    target.tm_hour = 0;
    target.tm_min = 0;
    target.tm_sec = 0;

    return target;
}
