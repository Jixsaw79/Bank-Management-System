# Bank Management System in C

A menu-driven, console-based banking simulation built in **C** as a university learning project. It keeps the implementation in a single C source file and uses standard file I/O to save account records between runs.

> **Educational project only.** This program is not connected to a real bank and is not suitable for real financial data or production use.

## Features

- Create accounts with automatically assigned account numbers
- View all saved accounts and check an account's balance
- Deposit and withdraw money with basic amount and balance checks
- Transfer funds between two different accounts
- Record a loan and repay an outstanding loan
- Handle invalid menu choices, non-numeric input, unknown account numbers, and invalid amounts
- Save account records to `accounts.txt` and load them on the next launch
- Record initial deposits, deposits, withdrawals, transfers, loans, and loan repayments in `transactions.txt`
- View transaction history for a specific account, including date/time, transaction type, amount, and resulting balance
- Append transaction records so history is preserved between program runs
- Save account changes after successful operations and when exiting

## Built with

- **Language:** C (C11-compatible compiler recommended)
- **Compiler:** GCC
- **Concepts:** structures, arrays, functions, loops, conditionals, console input/output, and file handling
- **Storage:** local plain-text files

## Getting started

### Requirements

- [Git](https://git-scm.com/) (optional, for cloning the repository)
- GCC or another compatible C compiler
- A terminal, such as the VS Code integrated terminal

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

Review compiler warnings rather than ignoring them. If your compiler reports a platform-specific issue, use a current GCC version.

### 3. Run

**Windows:**

```powershell
.\bank.exe
```

**Linux or macOS:**

```bash
./bank
```

The program runs in the terminal and presents a numbered menu. Choose an operation by entering its menu number.

## Data persistence

The program stores account records in `accounts.txt` and transaction records in `transactions.txt`, both in the **current working directory** from which it is run. On startup, it attempts to load saved accounts. Successful balance-changing operations append transaction records to `transactions.txt`; account changes are written back to `accounts.txt`.

Each account record uses this format:

```text
accountNumber|name|balance|outstandingLoan
```

- Account numbering starts at `1001` and continues after the highest account number loaded from the file.
- The program supports up to **100 accounts**.
- The pipe character (`|`) is the field separator, so avoid using it in account names.
- The `.gitignore` file excludes `accounts.txt`, `transactions.txt`, and compiled executables so local generated data is not accidentally committed.

**Important:** Keep both data files if you want to preserve local test records and their history. Deleting either file removes that file's saved data from the program's view. Do not put real or sensitive personal information in these files.

## Project structure

```text
Bank-Management-System/
├── Bank-Management-System.c   # Complete program in one C source file
├── README.md                  # Project overview and run instructions
├── .gitignore                 # Excludes generated files and build artifacts
└── docs/
    └── TESTING.md             # Manual QA results and scope
```

## Testing and QA

The project has been manually tested for core menu/input validation, valid and invalid deposits and withdrawals, transfers between existing accounts, transfers to unknown accounts, transaction history, and account persistence after restarting. The recorded results and test scope are in [`docs/TESTING.md`](docs/TESTING.md).

These are manual checks, not an automated test suite. Missing-file recovery, corrupted-file handling, and the maximum account-capacity limit are not claimed as tested.

## Known limitations

- This is a learning exercise, not production banking software.
- Account data is stored in plain text without authentication, encryption, or database-level protection.
- The program uses `float` for monetary values; real financial systems should use a carefully designed representation such as integer minor units or a suitable decimal type.
- Loan requests are recorded immediately for demonstration; there is no credit assessment or approval workflow.
- Transaction history is a simple local text log; it is not tamper-proof and does not provide database transactions or crash recovery.
- There is no concurrent-user handling or transaction recovery mechanism.
- Input validation is intended for a local, single-user console application.

## What this project demonstrates

- Defining and using a `struct` to represent an account
- Managing a collection of records with an array
- Separating tasks into functions
- Validating console input and checking account operations
- Reading and writing structured records with standard C file I/O
- Appending transaction records and formatting local date/time with the C time library
- Using Git and GitHub to version and publish a project

## Project links

- **Source repository:** https://github.com/Jixsaw79/Bank-Management-System
- **Portfolio project page:** https://jixsaw79.github.io/projects/bank-management-system/

## Academic note

This project was developed for learning and demonstration. Be prepared to explain the code and design decisions in your own words during an assignment presentation or viva.
