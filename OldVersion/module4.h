#include "module3.h"
#include <locale.h>

// Displays ASCII art welcome UI banner
void UI()
{
    // Set locale for proper character formatting
    // setlocale(LC_ALL, "en_US.UTF-8");

    // Print stylized welcome message
    printf("\n");
    printf("=======================================================================================\n");
    printf("|                                                                                      |\n");
    printf("|                                      Welcome to                                      |\n");
    if(setlocale(LC_ALL, "en_US.UTF-8"))
    {
    printf("|                                                                                      |\n");
    printf("|                       ████████╗██████╗  █████╗  ██████╗██╗  ██╗                      |\n");
    printf("|                       ╚══██╔══╝██╔══██╗██╔══██╗██╔════╝██║ ██╔╝                      |\n");
    printf("|                          ██║   ██████╔╝███████║██║     █████╔╝                       |\n");
    printf("|                          ██║   ██╔══██║██╔══██║██║     ██╔═██╗                       |\n");
    printf("|                          ██║   ██║  ██║██║  ██║╚██████╗██║  ██╗                      |\n");
    printf("|                          ╚═╝   ╚═╝  ╚═╝╚═╝  ╚═╝ ╚═════╝╚═╝  ╚═╝                      |\n");
    printf("|                                  ███╗   ███╗ ██╗   ██╗                               |\n");
    printf("|                                  ████╗ ████║ ╚██╗ ██╔╝                               |\n");
    printf("|                                  ██╔████╔██║  ╚████╔╝                                |\n");
    printf("|                                  ██║╚██╔╝██║   ╚██╔╝                                 |\n");
    printf("|                                  ██║ ╚═╝ ██║    ██║                                  |\n");
    printf("|                                  ╚═╝     ╚═╝    ╚═╝                                  |\n");
    printf("|                       ███████╗██████╗ ███████╗███╗   ██╗██████╗                      |\n");
    printf("|                       ██╔════╝██╔══██╗██╔════╝████╗  ██║██╔══██╗                     |\n");
    printf("|                       ███████╗██████╔╝█████╗  ██╔██╗ ██║██║  ██║                     |\n");
    printf("|                       ╚═══ ██║██╔═══╝ ██╔══╝  ██║╚██╗██║██║  ██║                     |\n");
    printf("|                       ███████║██║     ███████╗██║ ╚████║██████╔╝                     |\n");
    printf("|                       ╚══════╝╚═╝     ╚══════╝╚═╝  ╚═══╝╚═════╝                      |\n");
    printf("|                                                                                      |\n");
    }
    printf("|                      Your personal companion for expense tracking                    |\n");
    printf("|                                                                                      |\n");
    printf("=======================================================================================\n");
    printf("\n");
    printf("Press enter to continue: ");
    getchar(); // Wait for user input to proceed
    clear_input_buffer(); // <-- ADD THIS to fix the skipping
}

// Resets all stored data and starts a new financial plan
void resetAllAndCreatePlan()
{
    maxID = 0;
    income = balance = fixedDue = extra = savings = 0;
    incomeDate = (struct tm){0}; // Reset income date

    // Clear all expense arrays
    memset(ex, 0, sizeof(ex));
    memset(sortedCategory, 0, sizeof(sortedCategory));
    memset(sortedAmount, 0, sizeof(sortedAmount));
    memset(sortedDate, 0, sizeof(sortedDate));

    printf("\nAll previous data has been cleared.\n");
    printf("Creating a new financial plan...\n");

    // Call function to set up a new plan
    createPlan();
}

// Main interactive menu for user to access program functionality
void menu()
{
    int choice, manual;
    char type[TypeSize], desc[DescSize];
    double amount;

    do
    {
        clear(); // Clear screen
        updateStatus(); // Display current status

        // Show menu options
        printf("\n%-40s\n", "========= MAIN MENU =========");
        printf("1. View All Expenses (Unsorted)\n");
        printf("2. View Expenses Sorted by Category\n");
        printf("3. View Expenses Sorted by Amount\n");
        printf("4. View Expenses Sorted by Date\n");
        printf("5. Add a New Expense (Main Balance)\n");
        printf("6. Spend from Extra Balance\n");
        printf("7. Create New Plan (Resets All)\n");
        printf("8. Save Data\n");
        printf("9. Exit\n");
        printf("-------------------------------\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);
        clear_input_buffer(); // Flush newline character from input

        switch (choice)
        {
        case 1:
            // View unsorted list
            displayAllExpenses(ex, maxID);
            break;

        case 2:
            // View expenses sorted by category
            displaySortedExpenses(sortedCategory, maxID);
            break;

        case 3:
            // View expenses sorted by amount
            displaySortedExpenses(sortedAmount, maxID);
            break;

        case 4:
            // View expenses sorted by date
            displaySortedExpenses(sortedDate, maxID);
            break;

        case 5:
            // Add new expense using main balance
            if (balance <= 0)
            {
                printf("No balance available to spend.\n");
                break;
            }

            printf("Enter type of expense: ");
            do { fgets(type, sizeof(type), stdin); } while (!isNotOnlySpace(type));
            toFirstCap(type);

            do {
                printf("Enter amount (Greater 0 and less than equal to %.2lf): ", balance);
                while (!validateDoubleInput(&amount));
            } while (amount < 0 || amount > balance);

            printf("Enter short description: ");
            do { fgets(desc, sizeof(desc), stdin); } while (!isNotOnlySpace(desc));
            toFirstCap(desc);

            // Ask if date should be manually entered
            printf("Do you want to manually enter date?[Default:Current's date and time]");
            checkYesNO(&manual);
            if (manual)
                addExpense(amount, type, desc, getDatefromUser());
            else
                addExpense(amount, type, desc, time(NULL));

            balance -= amount; // Deduct from balance
            printf("\nExpense Added Successfully!");
            break;

        case 6:
            // Add new expense using extra balance
            if (extra <= 0)
            {
                printf("No extra balance available to spend.\n");
                break;
            }

            printf("Enter type of extra expense: ");
            do { fgets(type, sizeof(type), stdin); } while (!isNotOnlySpace(type));
            toFirstCap(type);

            do {
                printf("Enter amount (Greater 0 and less than equal to %.2lf): ", extra);
                while (!validateDoubleInput(&amount));
            } while (amount < 0 || amount > extra);

            printf("Enter short description: ");
            do { fgets(desc, sizeof(desc), stdin); } while (!isNotOnlySpace(desc));
            toFirstCap(desc);

            // Ask if date should be manually entered
            printf("Do you want to manually enter date?[Default:Current's date and time]");
            checkYesNO(&manual);
            if (manual)
                addExpense(amount, type, desc, getDatefromUser());
            else
                addExpense(amount, type, desc, time(NULL));

            extra -= amount; // Deduct from extra
            printf("\nExpense Added Successfully!");
            break;

        case 7:
            // Reset data and create a new plan
            resetAllAndCreatePlan();
            break;

        case 8:
            // Save data to file
            if (saveExpensesToFile("expenses.dat"))
                printf("Data saved successfully!\n");
            break;

        case 9:
            // Exit with optional save
            printf("Do you want to save before exiting?");
            checkYesNO(&manual);
            if (manual)
                if (saveExpensesToFile("expenses.dat"))
                    printf("Data saved successfully!\n");

            printf("Exiting the program. Goodbye!\n");
            break;

        default:
            // Invalid choice handling
            printf("Invalid option! Please enter a valid choice.\n");
            break;
        }

        if (choice != 9)
        {
            printf("\nPress Enter to continue...");
            getchar(); // Pause before returning to menu
        }

    } while (choice != 9); // Loop until user chooses to exit
}

// Entry point to the application
void app()
{
    clear(); // Clear screen
    UI();    // Show splash UI

    // Try loading saved data from file
    if (loadExpensesFromFile("expenses.dat"))
        mergeSortEx(); // Sort loaded data
    else
        createPlan();  // No saved data; start new plan

    menu(); // Show main menu
}
