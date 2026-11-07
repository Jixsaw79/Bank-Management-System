# Bank Management System in C

A menu-driven, console-based banking simulation built in **C** as a university learning project. The project keeps the complete implementation in one C source file and uses standard file handling to preserve account records and transaction history between runs.

> **Educational project only.** This program is not connected to a real bank and is not suitable for real financial data or production use.

## Features

- Create accounts with automatically assigned account numbers
- View all accounts and check an individual account's balance and outstanding loan
- Deposit and withdraw money with input, amount, and balance validation
- Transfer money between two different accounts
- Apply for a simulated loan and repay an outstanding loan
- View timestamped transaction history for an individual account
- Validate numeric input, reject malformed values, and handle overlong input lines
- Save account information after successful operations and when exiting
- Load saved account information when the program starts
- Preserve transaction history by appending new records to `transactions.txt`
- Support up to **100 accounts**

## Code organization

The code is still a single beginner-friendly C file, but repeated tasks are handled by reusable helper functions:

| Function | Responsibility |
|---|---|
| `readLine()` | Reads one complete line and safely discards excess input |
| `readInt()` | Converts and validates integer input |
| `readFloat()` | Converts and validates finite floating-point input |
| `readName()` | Trims and validates customer names |
| `findAccountIndex()` | Finds an account once, returning its array index or `-1` |
| `canAddAmount()` | Checks whether a positive addition is safe |
| `loadAccounts()` / `saveAccounts()` | Loads and saves account records |
| `recordTransaction()` | Appends a timestamped transaction record |
| `finishTransaction()` | Records a single-account operation and saves account data |

The menu uses a `switch` statement, and account lookup is shared by operations instead of repeating the same search loop. These changes reduce duplication without removing any banking feature.

## Built with

- **Language:** C (C11)
- **Compiler:** GCC
- **Concepts:** structures, arrays, functions, pointers, loops, conditionals, input validation, and file handling
- **Storage:** local plain-text files

## Getting started

### Requirements

- GCC or another C11-compatible compiler
- A terminal, such as the VS Code integrated terminal
- Git (optional, for cloning the repository)

### 1. Clone the repository

```bash
git clone https://github.com/Jixsaw79/Bank-Management-System.git
cd Bank-Management-System
```

Alternatively, download the repository as a ZIP and open the extracted folder in VS Code.

### 2. Compile

**Windows (PowerShell with GCC/MinGW):**

```powershell
gcc -std=c11 -Wall -Wextra -Wpedantic Bank-Management-System.c -o bank.exe
```

**Linux or macOS:**

```bash
gcc -std=c11 -Wall -Wextra -Wpedantic Bank-Management-System.c -o bank
```

The project can also be compiled with `-Werror` to treat compiler warnings as errors:

```bash
gcc -std=c11 -Wall -Wextra -Wpedantic -Werror Bank-Management-System.c -o bank
```

### 3. Run

**Windows:**

```powershell
.\bank.exe
```

**Linux or macOS:**

```bash
./bank
```

Follow the numbered menu and enter the number of the operation you want to perform.

## Data persistence

The program uses two files in the **current working directory**:

- `accounts.txt` stores account number, customer name, balance, and outstanding loan.
- `transactions.txt` stores the account number, transaction type, amount, resulting balance, and local date/time.

An account record has this format:

```text
accountNumber|name|balance|outstandingLoan
```

A transaction record has this format:

```text
accountNumber|transactionType|amount|balanceAfter|timestamp
```

Account numbering starts at `1001` and continues after the highest account number loaded from the account file. The pipe character (`|`) is used as a field separator, so it should not be included in account names.

The `.gitignore` file excludes the generated data files and compiled executables. Keep your local `accounts.txt` and `transactions.txt` files if you want to preserve test data and history. Do not store real or sensitive personal information in them.

**Compatibility:** The refactored program keeps the existing account and transaction text formats, so existing data files from earlier versions remain compatible.

## Testing

The refactored source was compiled with strict warnings enabled:

```bash
gcc -std=c11 -Wall -Wextra -Wpedantic -Werror Bank-Management-System.c -o bank
```

A local end-to-end smoke test exercised account creation, deposit, withdrawal, transfer, loan application, loan repayment, transaction logging, and saving account data. The detailed manual QA scope and previously recorded checks are documented in [`docs/TESTING.md`](docs/TESTING.md).

These checks do not make the program production-ready. Use fictional account details when testing.

## Known limitations

- This is a learning exercise, not production banking software.
- Account data is stored in plain text without authentication or encryption.
- The program uses `float` for money; real financial systems should use integer minor units or suitable decimal arithmetic.
- Loan requests are recorded immediately for demonstration; there is no credit assessment or approval workflow.
- Transaction history is a local text log and is not tamper-proof.
- File writes are not a full database transaction. A crash or disk error can leave account data and transaction history out of sync.
- There is no concurrent-user handling or database-backed recovery.

## What this project demonstrates

- Defining and using a `struct` to represent an account
- Managing records with an array of structures
- Organizing a program with reusable functions
- Using pointers to update values through function parameters
- Validating user input and checking banking operations
- Reading and writing structured records with standard C file I/O
- Appending transaction records and formatting local date/time
- Using Git and GitHub to version and publish a project

## Project links

- **Source repository:** https://github.com/Jixsaw79/Bank-Management-System
- **Portfolio project page:** https://jixsaw79.github.io/projects/bank-management-system/

## Academic note

This project was developed for learning and demonstration. Be prepared to explain the code and design decisions in your own words during an assignment presentation or viva.
