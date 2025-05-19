#include "module2.h"

// Merge function for Merge Sort
// Sorts by category (type = 1), amount (type = 2), or date (type = 3)
void merge(Expense *EX[], int left, int mid, int right, int type)
{
    int n1 = mid - left + 1;
    int n2 = right - mid;

    // Temporary arrays
    Expense *L[n1], *R[n2];

    // Copy data to temp arrays
    for (int i = 0; i < n1; i++)
        L[i] = EX[left + i];
    for (int j = 0; j < n2; j++)
        R[j] = EX[mid + 1 + j];

    int i = 0, j = 0, k = left;

    // Merge sorted subarrays based on selected type
    if (type == 1) // Sort by category (lexical order)
        while (i < n1 && j < n2)
            EX[k++] = (strcmp(L[i]->category, R[j]->category) <= 0) ? L[i++] : R[j++];
    if (type == 2) // Sort by amount (ascending)
        while (i < n1 && j < n2)
            EX[k++] = (L[i]->amount <= R[j]->amount) ? L[i++] : R[j++];
    if (type == 3) // Sort by date (oldest to newest)
        while (i < n1 && j < n2)
            EX[k++] = (L[i]->date <= R[j]->date) ? L[i++] : R[j++];

    // Copy remaining elements
    while (i < n1)
        EX[k++] = L[i++];
    while (j < n2)
        EX[k++] = R[j++];
}

// Recursive Merge Sort function
void mergeSort(Expense *EX[], int left, int right, int type)
{
    if (left < right)
    {
        int mid = left + (right - left) / 2;

        // Recursively sort the halves
        mergeSort(EX, left, mid, type);
        mergeSort(EX, mid + 1, right, type);

        // Merge sorted halves
        merge(EX, left, mid, right, type);
    }
}

// Wrapper to sort all 3 sorted arrays (by category, amount, date)
void mergeSortEx()
{
    // Initialize sorted arrays with pointers to original expenses
    for (int i = 0; i < maxID; i++)
    {
        sortedCategory[i] = &ex[i];
        sortedAmount[i] = &ex[i];
        sortedDate[i] = &ex[i];
    }

    // Sort them using merge sort
    mergeSort(sortedCategory, 0, maxID - 1, 1);
    mergeSort(sortedAmount, 0, maxID - 1, 2);
    mergeSort(sortedDate, 0, maxID - 1, 3);
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
        while (!validateIntInput(&month));
    } while (month < incomeDate.tm_mon + 1 || month > incomeDate.tm_mon + 2);

    // Prompt user for valid day
    if (month == incomeDate.tm_mon + 1)
    {
        do
        {
            printf("Enter due Day [between %d to %d]: ", incomeDate.tm_mday, maxDaysInCurrentMonth);
            while (!validateIntInput(&day));
        } while (day < incomeDate.tm_mday || day > maxDaysInCurrentMonth);
    }
    else
    {
        do
        {
            printf("Enter due Day [between 1 to %d]: ", maxDaysInNextMonth);
            while (!validateIntInput(&day));
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
    char type[TypeSize], desc[DescSize];
    time_t now = time(NULL);
    struct tm *local = localtime(&now), date;
    int days = getDaysInMonth(local->tm_year + 1900, local->tm_mon + 1);
    int day, option;

    printf("\n**************************************** Creating A New Budget Plan **********************************************");

    // Prompt user for monthly income
    printf("\nPlease enter your monthly income: ");
    do{while (!validateDoubleInput(&income));}while(income<=0);

    // Ask user for income day
    do
    {
        printf("What day of the month do you receive your income[between 1 to %d]: ", days);
        while (!validateIntInput(&day));
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
            while (!validateDoubleInput(&p1));
        } while (p1 > 100 || p1 < 0);

        do
        {
            printf("Enter a valid percentage for main expenses[0 - %.2lf]: ", 100 - p1);
            while (!validateDoubleInput(&p2));
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
        do {
            fgets(type, sizeof(type), stdin);
        } while (!isNotOnlySpace(type));
        toFirstCap(type);

        // Prompt for amount
        do {
            printf("Enter the amount [Greater 0 and less than equal to %.2lf]: ", balance);
            while (!validateDoubleInput(&amount));
        } while (amount <= 0 || amount > balance);

        balance -= amount;
        fixedDue += amount;
        addExpense(amount, type, "Fixed bills", getDatefromUser());
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
            do {
                fgets(type, sizeof(type), stdin);
            } while (!isNotOnlySpace(type));
            toFirstCap(type);

            // Expense amount
            do {
                printf("Enter the amount [Greater 0 and less than equal to %.2lf]: ", balance);
                while (!validateDoubleInput(&amount));
            } while (amount <= 0 || amount > balance);

            // Description
            printf("\nEnter description of expense: ");
            do {
                fgets(desc, sizeof(desc), stdin);
            } while (!isNotOnlySpace(desc));
            toFirstCap(desc);

            // Expense date
            do {
                printf("Enter expense day [between %d to %d]: ", incomeDate.tm_mday, local->tm_mday);
                while (!validateIntInput(&day));
            } while (day < incomeDate.tm_mday || day > local->tm_mday);

            balance -= amount;
            date = getDatefromDay(day, local->tm_mon + 1);
            addExpense(amount, type, desc, mktime(&date));
            printf("Expense Added Successfully!\n");

            // Add more?
            printf("\nDo you want to add more expenses?");
            checkYesNO(&option);
        }
    }
}
