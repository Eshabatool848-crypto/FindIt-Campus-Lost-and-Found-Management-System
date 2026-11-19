# FindIt – Campus Lost and Found Management System

## 1. Project Description

FindIt is a Campus Lost and Found Management System developed using the C programming language. The purpose of this project is to help students report lost items, register found items, search for belongings and identify possible matches.

The system provides a simple menu-based interface where users can manage lost and found records. It also includes an automatic matching system to make it easier to identify missing belongings.

## 2. Group Members

- Esha Batool – CT-26218
- Syeda Umme Abiha Zehra - CT-26220
- Shiza Kamran - CT-26206
- Maheen Ali - CT-26212

## 3. Features

The main features of FindIt include:

1. Register Lost Items – Add details of items that have been lost.
2. Register Found Items – Record items that have been found on campus.
3. Search Items – Search for items using their ID or person's name.
4. Automatic Matching – Compare lost and found items using their object, colour and location.
5. Update Records – Change the mobile number, location, object or colour of an open record.
6. Delete Records – Remove an item record from the system.
7. Ownership Verification – Verify the owner's mobile number before approving a claim.
8. Sorting – Sort records by name, ID or matching score.
9. Reports – Display lost, found, matched and returned items.
10. Location Tracker – Display a table showing items recorded at different campus locations.
11. File Handling – Save records and load previously saved records.

## 4. Campus Locations and Item Categories

The system includes the following predefined locations:

- Library
- Cafeteria
- Main Gate
- Computer Lab
- Sports Ground

Available item categories:

- Phone
- Wallet
- Bag
- Keys
- ID Card
- Laptop

Available colours:

- Black
- White
- Blue
- Red
- Green
- Brown

## 5. Automatic Matching System

The program compares lost and found items using three conditions:

- Object match: 50 points
- Colour match: 30 points
- Location match: 20 points

The maximum matching score is 100 points.

A match is accepted when the score is at least 70 points.

## 6. How to Compile and Run

The project requires a C compiler such as GCC.

First, open the terminal in the folder containing the source code.

Compile the program using:

```bash
gcc -std=c99 main.c -o findit
```

Run on Windows:

```bash
findit.exe
```

Run on Linux or macOS:

```bash
./findit
```

## 7. Sample Input and Output

Example:

A student reports a lost black phone at the Library.

Another student registers a found black phone at the Library.

Since the object, colour and location are the same, the system calculates the following score:

- Object: 50
- Colour: 30
- Location: 20
- Total: 100/100

Expected matching message:

```text
Lost ID 1001 matched with Found ID 1002 (score 100)
```

This is an illustrative example of the matching process.

## 8. Programming Concepts Used

The following C programming concepts are used in this project:

- Structures
- Functions
- Arrays and strings
- Two-dimensional arrays
- Pointers
- Dynamic memory allocation
- Conditional statements
- Switch statements
- Loops and nested loops
- Input validation
- Bubble sort
- Selection sort
- Linear search
- Recursive binary search
- File handling
- Recursion
- String manipulation

## 9. File Handling

The program uses file handling to save and load item records.

The data is stored in a binary file named:

```text
findit_data.dat
```

The program uses functions such as `fopen()`, `fwrite()`, `fread()` and `fclose()`.

## 10. Project Folder Structure

```text
FindIt-Campus-Lost-and-Found/
│
├── Source_Code/
│   └── main.c
│
├── Project_Report.pdf
│
├── README.md
│
├── Screenshots/
│   ├── output1.png
│   └── output2.png
│
└── GUI/
    └── (If applicable)
```

## 11. Conclusion

FindIt – Campus Lost and Found Management System provides a simple way to manage lost and found items within a campus. It demonstrates the practical use of C programming concepts, including structures, pointers, searching, sorting, dynamic memory allocation and file handling.

The project aims to make the process of reporting, finding and returning lost belongings more organized and efficient.
