#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <math.h>
#include <float.h>
#include <limits.h>
#include <time.h>

#define MAX_ACCOUNTS 100
#define NAME_LENGTH 50
#define INPUT_LENGTH 100
#define DATA_FILE "accounts.txt"
#define TRANSACTION_FILE "transactions.txt"

struct Account
{
    int accountNumber;
    char name[NAME_LENGTH];
    float balance;
    float loan;
};

static struct Account accounts[MAX_ACCOUNTS];
static int accountCount = 0;
static int nextAccountNumber = 1001;

/* Input and validation helpers */
int readLine(const char prompt[], char input[], size_t size);
int readInt(const char prompt[], int *value);
int readFloat(const char prompt[], float *value);
int readName(char name[]);
int canAddAmount(float current, float amount);
int findAccountIndex(int accountNumber);
int readAccountIndex(const char prompt[]);

/* File and transaction helpers */
int loadAccounts(void);
int saveAccounts(void);
int recordTransaction(int accountNumber, const char type[], float amount, float balance);
int finishTransaction(int accountIndex, const char type[], float amount);
void viewTransactionHistory(void);

/* Banking operations */
void createAccount(void);
void viewAccounts(void);
void depositMoney(void);
void withdrawMoney(void);
void checkBalance(void);
void transferMoney(void);
void applyLoan(void);
void repayLoan(void);

int main(void)
{
    int choice;

    if (loadAccounts())
        printf("Loaded %d account(s) from %s.\n", accountCount, DATA_FILE);
    else
        printf("No saved account data loaded. Starting with an empty account list.\n");

    do
    {
        printf("\n==== BANK MANAGEMENT SYSTEM ====\n");
        printf("1. Create Account\n");
        printf("2. View All Accounts\n");
        printf("3. Deposit Money\n");
        printf("4. Withdraw Money\n");
        printf("5. Check Balance\n");
        printf("6. Transfer Money\n");
        printf("7. Apply For Loan\n");
        printf("8. Repay Loan\n");
        printf("9. View Transaction History\n");
        printf("0. Exit\n");

        if (!readInt("Enter choice: ", &choice))
        {
            printf("Please enter a valid menu number.\n");
            continue;
        }

        switch (choice)
        {
            case 1: createAccount(); break;
            case 2: viewAccounts(); break;
            case 3: depositMoney(); break;
            case 4: withdrawMoney(); break;
            case 5: checkBalance(); break;
            case 6: transferMoney(); break;
            case 7: applyLoan(); break;
            case 8: repayLoan(); break;
            case 9: viewTransactionHistory(); break;
            case 0:
                if (!saveAccounts())
                    printf("Warning: account data could not be saved.\n");
                break;
            default:
                printf("Invalid choice! Please select 0 to 9.\n");
        }
    } while (choice != 0);

    printf("Thank you for using the Bank Management System.\n");
    return 0;
}

/* Read one complete line and discard excess characters safely. */
int readLine(const char prompt[], char input[], size_t size)
{
    size_t length;
    int ch;

    printf("%s", prompt);
    if (fgets(input, (int)size, stdin) == NULL)
        return 0;

    length = strlen(input);
    if (length > 0 && input[length - 1] == '\n')
    {
        input[length - 1] = '\0';
        return 1;
    }

    ch = getchar();
    if (ch != '\n' && ch != EOF)
    {
        while (ch != '\n' && ch != EOF)
            ch = getchar();
        return 0;
    }

    return 1;
}

/* Convert a complete input line to an int. */
int readInt(const char prompt[], int *value)
{
    char input[INPUT_LENGTH];
    char *end;
    long number;

    if (!readLine(prompt, input, sizeof(input)))
        return 0;

    errno = 0;
    number = strtol(input, &end, 10);
    if (end == input)
        return 0;

    while (isspace((unsigned char)*end))
        end++;

    if (*end != '\0' || errno == ERANGE || number < INT_MIN || number > INT_MAX)
        return 0;

    *value = (int)number;
    return 1;
}

/* Convert a complete input line to a finite float. */
int readFloat(const char prompt[], float *value)
{
    char input[INPUT_LENGTH];
    char *end;
    float number;

    if (!readLine(prompt, input, sizeof(input)))
        return 0;

    errno = 0;
    number = strtof(input, &end);
    if (end == input)
        return 0;

    while (isspace((unsigned char)*end))
        end++;

    if (*end != '\0' || errno == ERANGE || !isfinite(number))
        return 0;

    *value = number;
    return 1;
}

/* Read a name, trim its edges, and allow common name punctuation. */
int readName(char name[])
{
    char input[INPUT_LENGTH];
    size_t start = 0;
    size_t end;
    size_t i;
    int hasLetter = 0;

    if (!readLine("Enter name: ", input, sizeof(input)))
    {
        printf("Name is too long. Please enter a shorter name.\n");
        return 0;
    }

    end = strlen(input);
    while (start < end && isspace((unsigned char)input[start]))
        start++;
    while (end > start && isspace((unsigned char)input[end - 1]))
        end--;

    if (start == end || end - start >= NAME_LENGTH)
    {
        printf("Invalid name. Enter a name containing letters.\n");
        return 0;
    }

    for (i = start; i < end; i++)
    {
        unsigned char ch = (unsigned char)input[i];

        if (isalpha(ch) || ch >= 128)
            hasLetter = 1;
        else if (!(isspace(ch) || ch == '-' || ch == '\'' || ch == '.'))
        {
            printf("Invalid name. Use letters, spaces, hyphens, apostrophes, or periods.\n");
            return 0;
        }
    }

    if (!hasLetter)
    {
        printf("Invalid name. A name cannot contain only numbers or punctuation.\n");
        return 0;
    }

    memcpy(name, input + start, end - start);
    name[end - start] = '\0';
    return 1;
}

/* Check whether adding a positive amount stays within the float range. */
int canAddAmount(float current, float amount)
{
    return isfinite(current) && isfinite(amount) && current >= 0.0f &&
           amount > 0.0f && amount <= FLT_MAX - current;
}

/* Return an account's array index, or -1 if it does not exist. */
int findAccountIndex(int accountNumber)
{
    int i;

    for (i = 0; i < accountCount; i++)
        if (accounts[i].accountNumber == accountNumber)
            return i;

    return -1;
}


/* Read an account number and return its array index, or -1 on failure. */
int readAccountIndex(const char prompt[])
{
    int accountNumber;
    int index;

    if (!readInt(prompt, &accountNumber))
    {
        printf("Invalid account number.\\n");
        return -1;
    }

    index = findAccountIndex(accountNumber);
    if (index == -1)
        printf("Account not found.\\n");

    return index;
}

/* Load saved accounts without changing the existing text-file format. */
int loadAccounts(void)
{
    FILE *file = fopen(DATA_FILE, "r");
    int loaded = 0;

    if (file == NULL)
        return 0;

    accountCount = 0;
    nextAccountNumber = 1001;

    while (accountCount < MAX_ACCOUNTS &&
           fscanf(file, "%d|%49[^|]|%f|%f",
                  &accounts[accountCount].accountNumber,
                  accounts[accountCount].name,
                  &accounts[accountCount].balance,
                  &accounts[accountCount].loan) == 4)
    {
        int ch;
        struct Account *account = &accounts[accountCount];

        if (account->accountNumber <= 0 || !isfinite(account->balance) ||
            !isfinite(account->loan) || account->balance < 0 || account->loan < 0)
        {
            printf("Warning: invalid account record found; loading stopped.\n");
            break;
        }

        if (account->accountNumber >= nextAccountNumber)
        {
            if (account->accountNumber >= INT_MAX - 1)
                nextAccountNumber = INT_MAX;
            else
                nextAccountNumber = account->accountNumber + 1;
        }

        accountCount++;
        loaded = 1;
        while ((ch = fgetc(file)) != '\n' && ch != EOF)
            ;
    }

    if (ferror(file))
        printf("Warning: an error occurred while reading account data.\n");

    fclose(file);
    return loaded;
}

/* Save all accounts using the same format as previous project versions. */
int saveAccounts(void)
{
    FILE *file = fopen(DATA_FILE, "w");
    int i;

    if (file == NULL)
    {
        printf("Error: could not open %s for writing.\n", DATA_FILE);
        return 0;
    }

    for (i = 0; i < accountCount; i++)
    {
        if (fprintf(file, "%d|%s|%.2f|%.2f\n", accounts[i].accountNumber,
                    accounts[i].name, accounts[i].balance, accounts[i].loan) < 0)
        {
            fclose(file);
            printf("Error: failed while writing account data.\n");
            return 0;
        }
    }

    if (fclose(file) != 0)
    {
        printf("Error: could not finish saving account data.\n");
        return 0;
    }

    return 1;
}

/* Append one transaction, preserving the existing transaction-file format. */
int recordTransaction(int accountNumber, const char type[], float amount, float balance)
{
    FILE *file;
    time_t now = time(NULL);
    struct tm *localTime = localtime(&now);
    char timestamp[20];

    if (localTime == NULL ||
        strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", localTime) == 0)
    {
        printf("Error: could not create a transaction timestamp.\n");
        return 0;
    }

    file = fopen(TRANSACTION_FILE, "a");
    if (file == NULL)
    {
        printf("Error: could not open %s for writing.\n", TRANSACTION_FILE);
        return 0;
    }

    if (fprintf(file, "%d|%s|%.2f|%.2f|%s\n", accountNumber, type,
                amount, balance, timestamp) < 0)
    {
        fclose(file);
        printf("Error: failed while writing transaction history.\n");
        return 0;
    }

    if (fclose(file) != 0)
    {
        printf("Error: could not finish saving transaction history.\n");
        return 0;
    }

    return 1;
}

/* Record a successful operation and save the updated account list. */
int finishTransaction(int accountIndex, const char type[], float amount)
{
    int historySaved = recordTransaction(accounts[accountIndex].accountNumber,
                                         type, amount, accounts[accountIndex].balance);
    int accountsSaved = saveAccounts();

    if (!historySaved)
        printf("Warning: transaction history could not be saved.\n");
    if (!accountsSaved)
        printf("Warning: this change is not safely saved to disk.\n");

    return historySaved && accountsSaved;
}

void viewTransactionHistory(void)
{
    FILE *file;
    int accountNumber;
    int savedAccountNumber;
    int foundTransaction = 0;
    float amount;
    float balance;
    char type[32];
    char timestamp[64];

    accountNumber = readAccountIndex("Enter account number: ");
    if (accountNumber == -1)
        return;

    accountNumber = accounts[accountNumber].accountNumber;

    file = fopen(TRANSACTION_FILE, "r");
    if (file == NULL)
    {
        printf("No transaction history found for this account yet.\\n");
        return;
    }

    printf("\\n========== TRANSACTION HISTORY ==========\\n");
    printf("Account: %d\\n", accountNumber);
    printf("%-20s %-18s %-12s %s\\n", "Date / Time", "Transaction", "Amount", "Balance After");
    printf("--------------------------------------------------------------------------\\n");

    while (fscanf(file, "%d|%31[^|]|%f|%f|%63[^\\n]\\n", &savedAccountNumber,
                  type, &amount, &balance, timestamp) == 5)
    {
        if (savedAccountNumber == accountNumber)
        {
            printf("%-20s %-18s %10.2f %16.2f\\n", timestamp, type, amount, balance);
            foundTransaction = 1;
        }
    }

    if (ferror(file))
        printf("Warning: an error occurred while reading transaction history.\\n");
    fclose(file);

    if (!foundTransaction)
        printf("No transactions recorded for this account yet.\\n");

    printf("==========================================\\n");
}

void createAccount(void)
{
    struct Account *account;
    float initialDeposit;

    if (accountCount >= MAX_ACCOUNTS)
    {
        printf("Account limit reached. No more accounts can be created.\n");
        return;
    }
    if (nextAccountNumber <= 0 || nextAccountNumber == INT_MAX)
    {
        printf("Cannot create another account because the account number range has been reached.\n");
        return;
    }
    if (!readName(accounts[accountCount].name))
        return;
    if (!readFloat("Enter initial deposit (0 or more): ", &initialDeposit))
    {
        printf("Invalid amount. Please enter a number.\n");
        return;
    }
    if (initialDeposit < 0)
    {
        printf("Initial deposit cannot be negative.\n");
        return;
    }

    account = &accounts[accountCount];
    account->accountNumber = nextAccountNumber++;
    account->balance = initialDeposit;
    account->loan = 0.0f;
    accountCount++;

    printf("Account created successfully!\nAccount number: %d\n", account->accountNumber);
    if (initialDeposit > 0 &&
        !recordTransaction(account->accountNumber, "INITIAL_DEPOSIT", initialDeposit, account->balance))
        printf("Warning: the initial deposit was not added to transaction history.\n");
    if (!saveAccounts())
        printf("Warning: this change is not safely saved to disk.\n");
}

void viewAccounts(void)
{
    int i;

    if (accountCount == 0)
    {
        printf("No accounts found.\n");
        return;
    }

    for (i = 0; i < accountCount; i++)
    {
        printf("\nAccount number: %d\nName: %s\nBalance: %.2f\nOutstanding loan: %.2f\n",
               accounts[i].accountNumber, accounts[i].name,
               accounts[i].balance, accounts[i].loan);
    }
}

void depositMoney(void)
{
    int index = readAccountIndex("Enter account number: ");
    float amount;

    if (index == -1)
        return;

    if (!readFloat("Enter deposit amount: ", &amount))
    {
        printf("Invalid amount.\\n");
        return;
    }
    if (!canAddAmount(accounts[index].balance, amount))
    {
        printf("Deposit amount must be positive and within the supported balance range.\\n");
        return;
    }

    accounts[index].balance += amount;
    printf("Deposit successful!\\nNew balance: %.2f\\n", accounts[index].balance);
    finishTransaction(index, "DEPOSIT", amount);
}

void withdrawMoney(void)
{
    int index = readAccountIndex("Enter account number: ");
    float amount;

    if (index == -1)
        return;

    if (!readFloat("Enter withdrawal amount: ", &amount) || amount <= 0.0f)
    {
        printf("Invalid withdrawal amount. Enter a number greater than zero.\\n");
        return;
    }
    if (amount > accounts[index].balance)
    {
        printf("Insufficient balance.\\n");
        return;
    }

    accounts[index].balance -= amount;
    printf("Withdrawal successful!\\nRemaining balance: %.2f\\n", accounts[index].balance);
    finishTransaction(index, "WITHDRAWAL", amount);
}

void checkBalance(void)
{
    int index = readAccountIndex("Enter account number: ");

    if (index == -1)
        return;

    printf("Current balance: %.2f\\nOutstanding loan: %.2f\\n",
           accounts[index].balance, accounts[index].loan);
}

void transferMoney(void)
{
    int senderNumber;
    int receiverNumber;
    int senderIndex;
    int receiverIndex;
    float amount;

    if (!readInt("Enter sender account number: ", &senderNumber) ||
        !readInt("Enter receiver account number: ", &receiverNumber))
    {
        printf("Invalid account number.\n");
        return;
    }

    senderIndex = findAccountIndex(senderNumber);
    receiverIndex = findAccountIndex(receiverNumber);
    if (senderIndex == -1)
    {
        printf("Sender account not found.\n");
        return;
    }
    if (receiverIndex == -1)
    {
        printf("Receiver account not found.\n");
        return;
    }
    if (senderIndex == receiverIndex)
    {
        printf("Sender and receiver must be different accounts.\n");
        return;
    }
    if (!readFloat("Enter transfer amount: ", &amount) || amount <= 0.0f)
    {
        printf("Invalid transfer amount. Enter a number greater than zero.\n");
        return;
    }
    if (amount > accounts[senderIndex].balance)
    {
        printf("Insufficient balance.\n");
        return;
    }
    if (!canAddAmount(accounts[receiverIndex].balance, amount))
    {
        printf("Transfer would exceed the receiver's supported balance range.\n");
        return;
    }

    accounts[senderIndex].balance -= amount;
    accounts[receiverIndex].balance += amount;
    printf("Transfer successful!\nSender's remaining balance: %.2f\nReceiver's new balance: %.2f\n",
           accounts[senderIndex].balance, accounts[receiverIndex].balance);

    if (!recordTransaction(accounts[senderIndex].accountNumber, "TRANSFER_OUT",
                           amount, accounts[senderIndex].balance))
        printf("Warning: sender transfer history could not be saved.\n");
    if (!recordTransaction(accounts[receiverIndex].accountNumber, "TRANSFER_IN",
                           amount, accounts[receiverIndex].balance))
        printf("Warning: receiver transfer history could not be saved.\n");
    if (!saveAccounts())
        printf("Warning: this change is not safely saved to disk.\n");
}

void applyLoan(void)
{
    int index = readAccountIndex("Enter account number: ");
    float amount;

    if (index == -1)
        return;

    if (!readFloat("Enter loan amount: ", &amount))
    {
        printf("Invalid amount.\\n");
        return;
    }
    if (!canAddAmount(accounts[index].loan, amount) ||
        !canAddAmount(accounts[index].balance, amount))
    {
        printf("Loan amount must be positive and within the supported numeric range.\\n");
        return;
    }

    accounts[index].loan += amount;
    accounts[index].balance += amount;
    printf("Loan recorded successfully for this educational simulation.\\nOutstanding loan: %.2f\\nAvailable balance: %.2f\\n",
           accounts[index].loan, accounts[index].balance);
    finishTransaction(index, "LOAN", amount);
}

void repayLoan(void)
{
    int index = readAccountIndex("Enter account number: ");
    float amount;

    if (index == -1)
        return;

    if (accounts[index].loan <= 0.0f)
    {
        printf("This account has no outstanding loan.\\n");
        return;
    }

    printf("Outstanding loan: %.2f\\n", accounts[index].loan);
    if (!readFloat("Enter repayment amount: ", &amount) || amount <= 0.0f)
    {
        printf("Invalid repayment amount. Enter a number greater than zero.\\n");
        return;
    }
    if (amount > accounts[index].loan)
    {
        printf("Repayment cannot exceed the outstanding loan.\\n");
        return;
    }
    if (amount > accounts[index].balance)
    {
        printf("Insufficient balance to make this repayment.\\n");
        return;
    }

    accounts[index].balance -= amount;
    accounts[index].loan -= amount;
    printf("Loan repayment successful!\\nRemaining loan: %.2f\\nRemaining balance: %.2f\\n",
           accounts[index].loan, accounts[index].balance);
    finishTransaction(index, "LOAN_REPAYMENT", amount);
}
