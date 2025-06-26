#include "mod2.h" // Include custom module with expense-related definitions
// ==================== HASH TABLE IMPLEMENTATION ====================
#define HASH_SIZE 100

typedef struct HashTable
{
    Expense *buckets[HASH_SIZE];
    int size;
} HashTable;

// Global variable;
HashTable *expense_hash = NULL;

// Hash function for expense categories
unsigned int hash_function(const char *key)
{
    unsigned int hash = 0;
    for (int i = 0; key[i] != '\0'; i++)
    {
        hash = 31 * hash + key[i]; // compiler optimize it as hash = (hash >> 5) - hash + key[i];
    }
    return hash % HASH_SIZE;
}

// Initialize hash table
HashTable *create_hash_table()
{
    HashTable *ht = (HashTable *)malloc(sizeof(HashTable));
    for (int i = 0; i < HASH_SIZE; i++)
    {
        ht->buckets[i] = NULL;
    }
    ht->size = 0;
    return ht;
}

// Insert expense into hash table (by category)
void hash_insert(HashTable *ht, Expense *expense)
{
    unsigned int index = hash_function(expense->category);

    // Create newNode expense node
    Expense *new_expense = (Expense *)malloc(sizeof(Expense));
    *new_expense = *expense;                // creating copy by value
    new_expense->next = ht->buckets[index]; // insertion at head in chain list
    ht->buckets[index] = new_expense;
    ht->size++;
}

// Search expenses by category
void hash_search_by_category(HashTable *ht, const char *category)
{
    unsigned int index = hash_function(category);
    Expense *current = ht->buckets[index];
    double total = 0;
    int count = 0;

    printf("\n=== Expenses in category: %s ===\n", category);
    while (current != NULL)
    {
        if (strcmp(current->category, category) == 0)
        {

            displayOneExpense(current);
            total += current->amount;
            count++;
        }
        current = current->next;
    }
    printf("Total expenses in %s: $%.2f (%d items)\n", category, total, count);
}

// Free hash table memory
void free_hash_table(HashTable *ht)
{
    for (int i = 0; i < HASH_SIZE; i++)
    {
        Expense *current = ht->buckets[i];
        while (current != NULL)
        {
            Expense *temp = current;
            current = current->next;
            free(temp);
        }
    }
    free(ht);
}

void insert(double amount, time_t date, char *category, char *description)
{
    Expense *newNode = createExpense(amount, date, category, description);
    appendExpense(newNode);
    hash_insert(expense_hash, newNode);
    AmountSorted = insertAVL(AmountSorted, newNode, 1);
    DateSorted = insertAVL(DateSorted, newNode, 2);
    CategorySorted = insertAVL(CategorySorted, newNode, 3);
}

// Displays current financial status
void updateStatus()
{
    clear();
    char dateStr[26];
    time_t now = time(NULL);
    struct tm *local = localtime(&now);
    strftime(dateStr, sizeof(dateStr), "%Y-%m-%d %H:%M:%S", local);

    // Right-aligned formatted output
    printf("%80sCurrent time is:   %-15s\n", "", dateStr);
    printf("%80sCurrent balance:            %10.2lf\n", "", balance);
    printf("%80sMandated balance for bills: %10.2lf\n", "", fixedDue);
    printf("%80sExtra balance:              %10.2lf\n", "", extra);
    printf("%80sEmergency savings:          %10.2lf\n", "", savings);
    printf("%80s--------------------------------------\n", "");
    printf("%80sYour Monthly income:        %10.2lf\n", "", income);
    strftime(dateStr, sizeof(dateStr), "%Y-%m-%d %H:%M:%S", &incomeDate);
    printf("%80sIncome credit day: %-15s\n", "", dateStr);
}

// Prompts user for a valid day and month, returns the corresponding time_t
time_t getDatefromUser()
{
    int day, month;
    int currentYear = incomeDate.tm_year + 1900;

    int maxDaysInCurrentMonth = getDaysInMonth(currentYear, incomeDate.tm_mon + 1);
    int maxDaysInNextMonth = getDaysInMonth(currentYear, incomeDate.tm_mon + 2);

    // Limit next month's allowed day to the same as income date
    if (incomeDate.tm_mday < maxDaysInNextMonth)
        maxDaysInNextMonth = incomeDate.tm_mday;

    // Prompt user for valid month
    do
    {
        printf("Enter due Month [between %d to %d]: ", incomeDate.tm_mon + 1, incomeDate.tm_mon + 2);
        while (!validateIntInput(&month))
            ;
    } while (month < incomeDate.tm_mon + 1 || month > incomeDate.tm_mon + 2);

    // Prompt user for valid day
    if (month == incomeDate.tm_mon + 1)
    {
        do
        {
            printf("Enter due Day [between %d to %d]: ", incomeDate.tm_mday, maxDaysInCurrentMonth);
            while (!validateIntInput(&day))
                ;
        } while (day < incomeDate.tm_mday || day > maxDaysInCurrentMonth);
    }
    else
    {
        do
        {
            printf("Enter due Day [between 1 to %d]: ", maxDaysInNextMonth);
            while (!validateIntInput(&day))
                ;
        } while (day < 1 || day > maxDaysInNextMonth);
    }

    // Convert to time_t
    struct tm date = getDatefromDay(day, month);
    return mktime(&date);
}

// Main function to set up a new monthly budget plan
void createPlan()
{
    clear();
    double amount, p1 = 15, p2 = 65, p3 = 20;
    char type[CAT_SIZE], desc[DESC_SIZE];
    time_t now = time(NULL);
    struct tm *local = localtime(&now), date;
    int days = getDaysInMonth(local->tm_year + 1900, local->tm_mon + 1);
    int day, option;
    expense_hash = create_hash_table();

    printf("\n**************************************** Creating A New Budget Plan **********************************************");

    // Prompt user for monthly income
    printf("\nPlease enter your monthly income: ");
    do
    {
        while (!validateDoubleInput(&income))
            ;
    } while (income <= 0);

    // Ask user for income day
    do
    {
        printf("What day of the month do you receive your income[between 1 to %d]: ", days);
        while (!validateIntInput(&day))
            ;
    } while (day < 1 || day > days);

    // Save income date
    incomeDate = getDatefromDay(day, local->tm_mon + 1);

    clear();

    // Display recommended plan
    printf("Recommended budget plan is:\n1. %.2lf%% or %.2lf for emergency savings[eg. Accident]", p1, income * p1 / 100);
    printf("\n2. %.2lf%% or %.2lf for main expenses[eg. Bills,Food,Cloth]", p2, income * p2 / 100);
    printf("\n3. %.2lf%% or %.2lf for enjoy-life expenses[eg. Vacation fund]", p3, income * p3 / 100);
    printf("\nDo you want to edit your budget percentage?");
    checkYesNO(&option);

    // Let user customize budget plan
    if (option)
    {
        do
        {
            printf("Enter a valid percentage value for emergency savings[0 - 100]: ");
            while (!validateDoubleInput(&p1))
                ;
        } while (p1 > 100 || p1 < 0);

        do
        {
            printf("Enter a valid percentage for main expenses[0 - %.2lf]: ", 100 - p1);
            while (!validateDoubleInput(&p2))
                ;
        } while (p2 > 100 - p1 || p2 < 0);

        p3 = 100 - p1 - p2;

        clear();
        printf("\n******************************************* Your New Budget Plan *************************************************");
        printf("\n1. %.2lf%% or %.2lf for emergency savings", p1, income * p1 / 100);
        printf("\n2. %.2lf%% or %.2lf for main expenses", p2, income * p2 / 100);
        printf("\n3. %.2lf%% or %.2lf for enjoy-life expenses\n", p3, income * p3 / 100);
    }

    // Allocate funds according to budget
    savings = income * p1 / 100;
    balance = income * p2 / 100;
    extra = income * p3 / 100;

    // Ask if user has fixed main expenses (bills, EMI, etc.)
    printf("\nDo you have fixed main expenses like Bills & EMI?");
    checkYesNO(&option);

    while (option)
    {
        updateStatus();
        printf("\n*********************************** Fixed main Expenses[eg.Bills & EMI & Rent] *********************************");

        // Prompt for type
        printf("\nEnter the type of expense[eg.Water bill,EMI,Netflix]: ");
        do
        {
            fgets(type, sizeof(type), stdin);
        } while (!isNotOnlySpace(type));
        toFirstCap(type);

        // Prompt for amount
        do
        {
            printf("Enter the amount [Greater 0 and less than equal to %.2lf]: ", balance);
            while (!validateDoubleInput(&amount))
                ;
        } while (amount <= 0 || amount > balance);

        balance -= amount;
        fixedDue += amount;
        insert(amount, getDatefromUser(), type, "Fixed bills");
        printf("Expense Added Successfully!\n");

        // Ask if more bills need to be added
        printf("\nDo you want to add more bills?");
        checkYesNO(&option);
    }

    // Handle already-made expenditures
    if (local->tm_mday >= incomeDate.tm_mday)
    {
        updateStatus();
        printf("\nHave you already made few expenditures this month [between %d to %d]?", day, local->tm_mday);
        checkYesNO(&option);

        while (option)
        {
            updateStatus();
            printf("******************* Past Expenditures between %d to %d ***********************", day, local->tm_mday);

            // Expense type
            printf("\nEnter the type of expense[eg.Food,Cloth,Fuel]: ");
            do
            {
                fgets(type, sizeof(type), stdin);
            } while (!isNotOnlySpace(type));
            toFirstCap(type);

            // Expense amount
            do
            {
                printf("Enter the amount [Greater 0 and less than equal to %.2lf]: ", balance);
                while (!validateDoubleInput(&amount))
                    ;
            } while (amount <= 0 || amount > balance);

            // Description
            printf("\nEnter description of expense: ");
            do
            {
                fgets(desc, sizeof(desc), stdin);
            } while (!isNotOnlySpace(desc));
            toFirstCap(desc);

            // Expense date
            do
            {
                printf("Enter expense day [between %d to %d]: ", incomeDate.tm_mday, local->tm_mday);
                while (!validateIntInput(&day))
                    ;
            } while (day < incomeDate.tm_mday || day > local->tm_mday);

            balance -= amount;
            date = getDatefromDay(day, local->tm_mon + 1);
            insert(amount, mktime(&date), type, desc);
            printf("Expense Added Successfully!\n");

            // Add more?
            printf("\nDo you want to add more expenses?");
            checkYesNO(&option);
        }
    }
}