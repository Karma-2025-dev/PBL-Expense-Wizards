#include "mod1.h"
// Capitalizes the first letter of a string, removes leading spaces, and trims newline
void toFirstCap(char *str)
{
    int i = 0, j = 0;
    char result[DESC_SIZE];

    while (str[i])
    {
        if (isspace(str[i]) && j == 0)
            ; // skip leading spaces
        else
        {
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
int isLeapYear(int year)
{
    return ((year % 4 == 0 && year % 100 != 0) || (year % 400 == 0));
}

// Return number of days in a given month of a year
int getDaysInMonth(int year, int month)
{
    switch (month)
    {
    case 1:
    case 3:
    case 5:
    case 7:
    case 8:
    case 10:
    case 12:
        return 31;
    case 4:
    case 6:
    case 9:
    case 11:
        return 30;
    case 2:
        return isLeapYear(year) ? 29 : 28;
    default:
        return -1;
    }
}

// Validate date format as "YYYY-MM-DD"
int validateDate(const char *dateStr)
{
    if (strlen(dateStr) != 10)
        return 0;

    for (int i = 0; i < 10; i++)
    {
        if ((i == 4 || i == 7) && dateStr[i] != '-')
            return 0;
        if (i != 4 && i != 7 && !isdigit(dateStr[i]))
            return 0;
    }

    int year, month, day;
    if (sscanf(dateStr, "%4d-%2d-%2d", &year, &month, &day) != 3)
        return 0;
    if (year < 1900 || year > 2100)
        return 0;
    if (month < 1 || month > 12)
        return 0;
    if (day < 1 || day > getDaysInMonth(year, month))
        return 0;

    return 1;
}

// Build and return a tm struct based on day and month of the current year
struct tm getDatefromDay(int day, int month)
{
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
// Check if a string contains any non-space characters
int isNotOnlySpace(const char *str)
{
    while (*str)
    {
        if (!isspace((unsigned char)*str)) // Check for non-space character
        {
            return 1; // Valid: contains at least one non-space character
        }
        str++;
    }
    printf("Please Enter value other than space!\n");
    return 0; // Invalid: all spaces or empty
}

// Clears the input buffer until newline is found
int clear_input_buffer()
{
    int check = 0;
    while (getchar() != '\n') // Read and discard characters until newline
        check++;
    return check; // Return number of discarded characters
}

// Validate user input for a double value
int validateDoubleInput(double *value)
{
    int result = scanf("%lf", value); // Try to read a double
    if (clear_input_buffer() == 0 && result == 1)
    {
        return 1; // Success: valid double with no extra input
    }
    printf("Please enter only valid double value\n");
    return 0; // Invalid input
}

// Validate user input for an integer value
int validateIntInput(int *value)
{
    int result = scanf("%d", value); // Try to read an integer
    if (clear_input_buffer() == 0 && result == 1)
    {
        return 1; // Success: valid integer with no extra input
    }
    printf("Please enter only valid int value\n");
    return 0; // Invalid input
}

// Clears the screen (Windows version)
// Uncomment and modify for Linux if needed
void clear()
{
    system("cls"); // Use "clear" for Unix/Linux
}

// Ask the user for a Yes/No confirmation
void checkYesNO(int *option)
{
    char c;
    printf("\nEnter 'Y' or 'y' for yes [Any other value means no]: ");
    c = getchar();        // Get single character input
    clear_input_buffer(); // Clear remaining input
    if (c == 'y' || c == 'Y')
        *option = 1; // User confirmed with yes
    else
        *option = 0; // Any other input is treated as no
}

typedef struct
{
    int ID;
    double income;
    double balance;
    double fixedDue;
    double extra;
    double savings;
    struct tm incomeDate;
} AppState;

// Save the linked list of expenses to a file
int saveExpensesToFile(const char *filename)
{
    FILE *file = fopen(filename, "wb");
    if (!file)
    {
        perror("Failed to open file for writing");
        return 0;
    }

    // Save global state
    AppState state = {ID, income, balance, fixedDue, extra, savings, incomeDate};
    fwrite(&state, sizeof(AppState), 1, file);

    // Save linked list
    Expense *current = Start;
    while (current)
    {
        fwrite(current, sizeof(Expense), 1, file);
        current = current->next;
    }

    fclose(file);
    printf("Data saved successfully.\n");
    return 1;
}

int loadExpensesFromFile(const char *filename)
{
    FILE *file = fopen(filename, "rb");
    if (!file)
    {
        perror("Failed to open file for reading");
        return 0;
    }

    // Load global state
    AppState state;
    if (fread(&state, sizeof(AppState), 1, file) != 1)
    {
        perror("Error reading app state");
        fclose(file);
        return 0;
    }

    ID = state.ID;
    income = state.income;
    balance = state.balance;
    fixedDue = state.fixedDue;
    extra = state.extra;
    savings = state.savings;
    incomeDate = state.incomeDate;

    // Load linked list
    freeExpenses(Start);
    Start = End = NULL;

    Expense temp;
    while (fread(&temp, sizeof(Expense), 1, file) == 1)
    {
        Expense *newNode = (Expense *)malloc(sizeof(Expense));
        *newNode = temp;
        newNode->next = NULL;
        appendExpense(newNode);
    }

    fclose(file);
    printf("Data loaded successfully.\n");
    return 1;
}

// Deletes the data file used for saving expenses
void deleteDataFile(const char *filename)
{
    if (remove(filename) == 0)
    {
        printf("File '%s' deleted successfully.\n", filename);
    }
    else
    {
        perror("Error deleting file");
    }
}
