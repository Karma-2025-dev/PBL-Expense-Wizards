#include "module1.h"  // Include custom module with expense-related definitions

// Check if a string contains any non-space characters
int isNotOnlySpace(const char *str)
{
    while (*str)
    {
        if (!isspace((unsigned char)*str))  // Check for non-space character
        {
            return 1;  // Valid: contains at least one non-space character
        }
        str++;
    }
    printf("Please Enter value other than space!\n");
    return 0;  // Invalid: all spaces or empty
}

// Clears the input buffer until newline is found
int clear_input_buffer()
{
    int check = 0;
    while (getchar() != '\n')  // Read and discard characters until newline
        check++;
    return check;  // Return number of discarded characters
}

// Validate user input for a double value
int validateDoubleInput(double *value)
{
    int result = scanf("%lf", value);  // Try to read a double
    if (clear_input_buffer() == 0 && result == 1)
    {
        return 1;  // Success: valid double with no extra input
    }
    printf("Please enter only valid double value\n");
    return 0;  // Invalid input
}

// Validate user input for an integer value
int validateIntInput(int *value)
{
    int result = scanf("%d", value);  // Try to read an integer
    if (clear_input_buffer() == 0 && result == 1)
    {
        return 1;  // Success: valid integer with no extra input
    }
    printf("Please enter only valid int value\n");
    return 0;  // Invalid input
}

// Clears the screen (Windows version)
// Uncomment and modify for Linux if needed
void clear()
{
    system("cls");  // Use "clear" for Unix/Linux
}

// Ask the user for a Yes/No confirmation
void checkYesNO(int *option)
{
    char c;
    printf("\nEnter 'Y' or 'y' for yes [Any other value means no]: ");
    c = getchar();           // Get single character input
    clear_input_buffer();    // Clear remaining input
    if (c == 'y' || c == 'Y')
        *option = 1;         // User confirmed with yes
    else
        *option = 0;         // Any other input is treated as no
}

// Save current expense data and metadata to a binary file
int saveExpensesToFile(const char *filename)
{
    FILE *file = fopen(filename, "wb");  // Open file for binary write
    if (!file)
    {
        perror("Error opening file for writing");
        return 0;  // File couldn't be opened
    }

    // Save metadata first
    fwrite(&maxID, sizeof(maxID), 1, file);
    fwrite(&income, sizeof(income), 1, file);
    fwrite(&balance, sizeof(balance), 1, file);
    fwrite(&fixedDue, sizeof(fixedDue), 1, file);
    fwrite(&extra, sizeof(extra), 1, file);
    fwrite(&savings, sizeof(savings), 1, file);
    fwrite(&incomeDate, sizeof(incomeDate), 1, file);

    // Save all expenses
    fwrite(ex, sizeof(Expense), maxID, file);
    fclose(file);  // Close file
    return 1;      // Success
}

// Load expense data and metadata from a binary file
int loadExpensesFromFile(const char *filename)
{
    FILE *file = fopen(filename, "rb");  // Open file for binary read
    if (!file)
    {
        // File may not exist yet — that's okay
        return 0;
    }

    // Read metadata
    fread(&maxID, sizeof(maxID), 1, file);
    fread(&income, sizeof(income), 1, file);
    fread(&balance, sizeof(balance), 1, file);
    fread(&fixedDue, sizeof(fixedDue), 1, file);
    fread(&extra, sizeof(extra), 1, file);
    fread(&savings, sizeof(savings), 1, file);
    fread(&incomeDate, sizeof(incomeDate), 1, file);

    // Read all stored expenses
    fread(ex, sizeof(Expense), maxID, file);
    fclose(file);  // Close file
    return 1;      // Success
}

// Delete the saved expenses file from disk
int delete_expenses_file()
{
    if (remove("expenses.dat") == 0)
    {
        printf("File 'expenses.dat' deleted successfully.\n");
        return 1;  // Success
    }
    else
    {
        perror("Error deleting 'expenses.dat'");
        return 0;  // Failure
    }
}
