<h1 align="center">Expense Tracker (C + AES-256 Encryption)</h1>

<p align="center">
A fast, secure, CLI-based personal expense manager written in pure C — featuring AES-256 encryption, persistent storage, and a hacker-friendly modular design.
</p>

<p align="center">
<img src="assets/Expense-gif.gif" width="600" alt="Expense Tracker Running Animation">
</p>

---

## Features

> - Add, view, search, edit and delete expenses  
> - Password-based AES-256 encryption using OpenSSL  
> - Persistent encrypted database (`expenses.enc`)  
> - Fully modular codebase (`ui`, `expense`, `fileops`, `encrypt`, `main`)  
> - Follows safe input practices (`fgets` + `sscanf`), no `scanf` bugs  
> - Works on Linux, WSL and macOS  
> - Human-readable table output with aligned formatting  

---

## Security Overview (AES-256)

- On startup, the program asks for a password  
- Password → converted to a 256-bit key using SHA-256  
- Encrypted file (`expenses.enc`) is decrypted into memory  
- On exit, all data is re-encrypted using AES-256 in CBC mode

If a wrong password is entered, the file **cannot be decrypted**.  
Only ciphertext is stored — nothing is saved in plain text.

---

## Tech Stack

| Component | Technology |
|----------|------------|
| Language | C (ISO C99) |
| Encryption | OpenSSL `EVP` AES-256-CBC |
| Build | GCC |
| Input Safety | `fgets` + `sscanf` |
| Storage | Binary encrypted file |

---

## Project Structure
```
expense-tracker/
│
├── include/ # Header files
│ ├── ui.h
│ ├── expense.h
│ ├── fileops.h
│ └── encrypt.h
│
├── src/ # Implementation files
│ ├── main.c
│ ├── ui.c
│ ├── expense.c
│ ├── fileops.c
│ └── encrypt.c
│
├── assets/ # Place screenshots / GIFs here
│ └── demo.gif
│
└── build/ # Compiled binary output
└── expense_tracker
```


---

## How to Run

### 1. Install OpenSSL (only required once)

```bash
sudo apt update
sudo apt install libssl-dev
```

### 2. Clone the project
```bash
git clone https://github.com/<your-username>/expense-tracker.git
cd expense-tracker
```
### 3. Compile the program
```bash
gcc src/main.c \
    src/ui.c \
    src/expense.c \
    src/fileops.c \
    src/encrypt.c \
    -Iinclude -lcrypto \
    -o build/expense_tracker
```
### 4. Run it
```bash
./build/expense_tracker
```

You will be prompted:
```bash
Enter password:
```
Enter any new password -> new encrypted database is created
Enter previously used password -> decrypts existing data

## Example Menu Output
```bash
====================================
          Expense Tracker
====================================
1. Add Expense
2. View Expenses
3. Search by Month
4. Search by Category
5. Delete Expense by ID
6. Edit Expense by ID
7. Monthly Summary
8. Exit
Choose an option:
```

## Future Upgrade plans

> - CSV export (open in Excel / Google Sheets)
> - SQLite database backend
> - Colored CLI output (green/red/yellow)
> - Backup and restore system
> - GUI using GTK or ncurses