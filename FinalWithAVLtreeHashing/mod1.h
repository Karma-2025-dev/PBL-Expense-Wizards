#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>

#define CAT_SIZE 20  // Maximum length of category name
#define DESC_SIZE 30 // Maximum length of description

// ==================== EXPENSE STRUCTURE ====================
// Expense structure as linked list
typedef struct expense
{
    int id;                      // Unique identifier for each expense
    double amount;               // Expense amount
    time_t date;                 // Timestamp of when the expense occurred
    char category[CAT_SIZE];     // Expense category (e.g., food, bills)
    char description[DESC_SIZE]; // Brief description of the expense
    struct expense *next;
} Expense;

// Global variables
int ID = 0;                         // Current number of expenses
double income = 0;                  // Monthly income
double balance = 0;                 // Remaining main balance (post bills)
double fixedDue = 0;                // Total fixed bills amount
double extra = 0;                   // Extra (flexible spending) balance
double savings = 0;                 // Emergency savings
struct tm incomeDate = {0};         // Date of monthly income receipt
Expense *Start = NULL, *End = NULL; // Expense Linked List

// Function to create a newNode Expense
Expense *createExpense(double amount, time_t date, char *category, char *description)
{
    Expense *newNode = (Expense *)malloc(sizeof(Expense));
    newNode->id = ++ID;                                        // Unique identifier for each expense
    newNode->amount = amount;                                  // Expense amount
    newNode->date = date;                                      // Timestamp of when the expense occurred
    strncpy(newNode->category, category, CAT_SIZE - 1);        // Expense category (e.g., food, bills)
    newNode->category[CAT_SIZE - 1] = '\0';                    // always putting '\0' at end
    strncpy(newNode->description, description, DESC_SIZE - 1); // Brief description of the expense
    newNode->description[DESC_SIZE - 1] = '\0';
    newNode->next = NULL;
    return newNode;
}

// Append an expense in O(1) using tail pointer
void appendExpense(Expense *new_exp)
{
    if (Start == NULL)
    {
        Start = End = new_exp;
    }
    else
    {
        End->next = new_exp;
        End = new_exp;
    }
}

void displayOneExpense(Expense *head)
{
    char dateStr[30];
    struct tm *tm_info = localtime(&head->date);
    strftime(dateStr, sizeof(dateStr), "%Y-%m-%d %H:%M:%S", tm_info);

    printf("ID: %d | Rs.%.2f | %s | %s | %s\n",
           head->id, head->amount, dateStr, head->category, head->description);
}
// Display all expenses in the list
void displayExpenses(Expense *head)
{
    if (head == NULL)
    {
        printf("No expenses recorded.\n");
        return;
    }

    printf("=== Expense List ===\n");
    while (head != NULL)
    {
        displayOneExpense(head);
        head = head->next;
    }
}

// Free the entire list
void freeExpenses(Expense *head)
{
    while (head != NULL)
    {
        Expense *temp = head;
        head = head->next;
        free(temp);
    }
    End = NULL;
}
// ======================= AVL TREE IMPLEMENTATION ===============================
// AVL Tree sorted Expense structure
typedef struct sortedExpense
{
    Expense *PointerToExpense;
    struct sortedExpense *left;
    struct sortedExpense *right;
    int height;
} SortedExpense;

// Global variables for Sorted AVL trees
SortedExpense *AmountSorted = NULL, *DateSorted = NULL, *CategorySorted = NULL;

// Function to get height of a node
int getHeight(SortedExpense *node)
{
    if (node == NULL)
        return 0;
    return node->height;
}

// Function to get maximum of two integers
int max(int a, int b)
{
    return (a > b) ? a : b;
}

// Function to create a newNode node
SortedExpense *createNode(Expense *PointerToExpense)
{
    SortedExpense *node = (SortedExpense *)malloc(sizeof(SortedExpense));
    node->PointerToExpense = PointerToExpense;
    node->left = NULL;
    node->right = NULL;
    node->height = 1; // New node is initially at height 1
    return node;
}

// Function to get balance factor of a node
int getBalance(SortedExpense *node)
{
    if (node == NULL)
        return 0;
    return getHeight(node->left) - getHeight(node->right);
}

// Right rotate
SortedExpense *rightRotate(SortedExpense *y)
{
    SortedExpense *x = y->left;
    SortedExpense *T2 = x->right;

    // Perform rotation
    x->right = y;
    y->left = T2;

    // Update heights
    y->height = max(getHeight(y->left), getHeight(y->right)) + 1;
    x->height = max(getHeight(x->left), getHeight(x->right)) + 1;

    // Return newNode root
    return x;
}

// Left rotate
SortedExpense *leftRotate(SortedExpense *x)
{
    SortedExpense *y = x->right;
    SortedExpense *T2 = y->left;

    // Perform rotation
    y->left = x;
    x->right = T2;

    // Update heights
    x->height = max(getHeight(x->left), getHeight(x->right)) + 1;
    y->height = max(getHeight(y->left), getHeight(y->right)) + 1;

    // Return newNode root
    return y;
}

int IsFirstSmaller(Expense *First, Expense *Second, int SortBy)
{
    if (SortBy == 1)
    {
        if (First->amount < Second->amount)
            return 1;
        else if (First->amount == Second->amount)
        {
            if (First->id < Second->id)
                return 1;
        }
        else
            return 0;
    }
    if (SortBy == 2)
    {
        if (First->date < Second->date)
            return 1;
        else if (First->date == Second->date)
        {
            if (First->id < Second->id)
                return 1;
        }
        else
            return 0;
    }
    if (SortBy == 3)
    {
        if (strcmp(First->category, Second->category) < 0)
            return 1;
        else if (strcmp(First->category, Second->category) == 0)
        {
            if (First->id < Second->id)
                return 1;
        }
        else
            return 0;
    }
    return 0;
}
// Insert a node in AVL tree
SortedExpense *insertAVL(SortedExpense *node, Expense *PointerToExpense, int SortBy)
{
    // 1. Perform normal BST insertion
    if (node == NULL)
        return createNode(PointerToExpense);

    if (IsFirstSmaller(PointerToExpense, node->PointerToExpense, SortBy))
        node->left = insertAVL(node->left, PointerToExpense, SortBy);
    else if (!IsFirstSmaller(PointerToExpense, node->PointerToExpense, SortBy))
        node->right = insertAVL(node->right, PointerToExpense, SortBy);
    else // Equal PointerToExpense not allowed
        return node;

    // 2. Update height of current node
    node->height = 1 + max(getHeight(node->left), getHeight(node->right));

    // 3. Get the balance factor
    int balance = getBalance(node);

    // 4. If node becomes unbalanced, there are 4 cases

    // Left Left Case
    if (balance > 1 && IsFirstSmaller(PointerToExpense, node->left->PointerToExpense, SortBy))
        return rightRotate(node);

    // Right Right Case
    if (balance < -1 && !IsFirstSmaller(PointerToExpense, node->right->PointerToExpense, SortBy))
        return leftRotate(node);

    // Left Right Case
    if (balance > 1 && !IsFirstSmaller(PointerToExpense, node->left->PointerToExpense, SortBy))
    {
        node->left = leftRotate(node->left);
        return rightRotate(node);
    }

    // Right Left Case
    if (balance < -1 && IsFirstSmaller(PointerToExpense, node->right->PointerToExpense, SortBy))
    {
        node->right = rightRotate(node->right);
        return leftRotate(node);
    }

    // Return unchanged node
    return node;
}

// Inorder traversal
void inorder(SortedExpense *root)
{
    if (root != NULL)
    {
        inorder(root->left);
        displayOneExpense(root->PointerToExpense);
        inorder(root->right);
    }
}

// Free AVL tree
void deleteAVLTree(SortedExpense *root)
{
    if (root == NULL)
        return;

    // First delete left and right subtrees
    deleteAVLTree(root->left);
    deleteAVLTree(root->right);

    // Free the node itself
    free(root);
}
