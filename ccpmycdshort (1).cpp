// =====================================================================
// FindIt - Campus Lost-and-Found Management System
// ORDER OF PARTS IN THIS FILE (C needs things defined before use):
//   PART 2  : Includes, constants, structure, registration   (1st)
//   PART 3  : Matching                                       (2nd)
//   PART 4  : Sorting                                        (3rd)
//   PART 5  : Searching (recursive binary search)            (4th)
//   PART 6  : Update and delete                              (5th)
//   PART 7  : Claims                                         (6th)
//   PART 8  : Reports                                        (7th)
//   PART 9  : Location tracker (2D array)                    (8th)
//   PART 10 : File handling                                  (9th)
//   PART 1  : Menus and main()                               (LAST)
// =====================================================================
// =====================================================================
// PART 2: INCLUDES, CONSTANTS, STRUCTURE, REGISTRATION   (place 1st)
// =====================================================================
#include <stdio.h>
#include <stdlib.h>                // dynamic memory allocation and program control(realloc,exit)
#include <string.h>
#include <ctype.h>                 // checking characters (isalpha,isdigit)
//macros or symbolic constants(replaces name with it's assigned value)
#define NAME_LEN 50                // maxsize of array=50
#define MOBILE_LEN 12              // 11 digits + '\0'
#define INITIAL_CAPACITY 5         //initially,space for 5 item
#define DATA_FILE "findit_data.dat"// name of file to save data
#define LOCATION_COUNT 5           // total pre-defined locations
#define OBJECT_COUNT 6             
#define COLOUR_COUNT 6
#define LOST 1                     // item type
#define FOUND 2
#define STATUS_OPEN 0              // item status
#define STATUS_MATCHED 1
#define STATUS_RETURNED 2
// CONCEPT: 2D arrays + strings (each row is one predefined option)
const char locations[LOCATION_COUNT][20] = {"Library", "Cafeteria", "Main Gate", "Computer Lab", "Sports Ground"};
const char objects[OBJECT_COUNT][20]     = {"Phone", "Wallet", "Bag", "Keys", "ID Card", "Laptop"};
const char colours[COLOUR_COUNT][20]     = {"Black", "White", "Blue", "Red", "Green", "Brown"};
const char statusNames[3][20]            = {"Open", "Matched", "Returned"};
// CONCEPT: Structures (one record per lost/found item)
typedef struct {
    int id;
    int type;                      // LOST or FOUND
    char personName[NAME_LEN];
    char mobile[MOBILE_LEN];
    int locationIndex;             // position in locations[]
    int objectIndex;               // position in objects[]
    int colourIndex;               // position in colours[]
    int status;                    // STATUS_OPEN, STATUS_MATCHED or STATUS_RETURNED
    int matchedId;                 // ID of the matching item (0 if none)
    int matchScore;                // score of the match (0 if none)
} Item;                            //typedef name(alternative name for structure datatype
// Reads one line of text and removes the '\n'
// CONCEPT: Input/output + strings + error handling (stops if input ends)
void readLine(char *buffer, int size) {            //first: char pointer that recieves address of the char array 
    if (fgets(buffer, size, stdin) == NULL) {      // if fgets fails to recieve input then returns NULL,stdin-represnts standard input usually the keyboard 
        printf("\nInput ended. Exiting.\n");
        exit(0);                                   // terminates the entire program
    }
    buffer[strcspn(buffer, "\n")] = '\0';          // remove newlie and terminate string with null char
}
// CONCEPT: Input validation + loops (retry on bad input)
int readNumberInRange(const char *prompt, int min, int max) { // 1-message displayed to the user(prompt is a pointer to const char)2,3-min,max acceptable num
    char line[100];                  //stores the user-input as string(so that we can check input validation later)
    int value;                       //stores the converted integer
    while (1) {                      //creates an infinte loop until valid input is entered
        printf("%s", prompt);        //displays the message passed to the func 
        readLine(line, sizeof(line));//calls previously defined func(reads input and stores it in line)-sizeof:total size of array in bytes
        if (sscanf(line, "%d", &value) == 1 && value >= min && value <= max) {
            return value;
        }
        printf("Invalid input. Enter a number from %d to %d.\n", min, max);
    }
}
// Shows a predefined list and returns the chosen index
// CONCEPT: Functions with parameters and return values(func reuseability and modular programming)
int chooseFromList(const char *title, const char list[][20], int count) {   //1-heading above options,2-2D array containing options,3-no.of available options
    printf("\n%s\n", title);
    for (int i = 0; i < count; i++) {
        printf("  %d. %s\n", i + 1, list[i]);     //i+1:for menu options to start with 1,since array starts with 0
    }
    return readNumberInRange("Your choice: ", 1, count) - 1;   //calls above func to get valid input,and -1 to convert user's choice into valid input
}
// CONCEPT: Strings + input validation (letters and spaces only)-this func reads and validates user's name
void readName(char *name) {    //1-pointer to char array where valid name will be stored
    char line[100];
    while (1) {
        printf("Enter your name: ");
        readLine(line, sizeof(line));
        int isValid = (strlen(line) > 0 && strlen(line) < NAME_LEN);
        for (int i = 0; line[i] != '\0' && isValid; i++) {
            if (!isalpha(line[i]) && line[i] != ' ') isValid = 0;
        }
        if (isValid) {
            strcpy(name, line);
            return;
        }
        printf("Invalid name. Use letters and spaces only.\n");
    }
}
// CONCEPT: Strings + input validation (exactly 11 digits)
void readMobile(char *mobile) {
    char line[100];
    while (1) {
        printf("Enter mobile number (11 digits): ");
        readLine(line, sizeof(line));
        int isValid = (strlen(line) == 11);
        for (int i = 0; line[i] != '\0' && isValid; i++) {
            if (!isdigit(line[i])) isValid = 0;
        }
        if (isValid) {
            strcpy(mobile, line);
            return;
        }
        printf("Invalid number. It must be exactly 11 digits.\n");
    }
}
// Gives the next unique ID (highest existing ID + 1)
// CONCEPT: Loops + array of structs
int getNextId(Item *items, int count) {
    int highestId = 1000;
    for (int i = 0; i < count; i++) {
        if (items[i].id > highestId) highestId = items[i].id;
    }
    return highestId + 1;
}
// Doubles the array size when it is full
// CONCEPT: Dynamic memory (realloc) + pointers (Item **) + error handling
int growIfFull(Item **items, int count, int *capacity) {
    if (count < *capacity) return 1;                 // still space, nothing to do
    int newCapacity = (*capacity) * 2;
    Item *temp = (Item *)realloc(*items, newCapacity * sizeof(Item));
    if (temp == NULL) {
        printf("Error: out of memory.\n");
        return 0;
    }
    *items = temp;                                   // the array may have moved
    *capacity = newCapacity;
    return 1;
}
// Registers a lost or found item
// CONCEPT: Pointers / pass by reference (items, count are changed here)
int registerItem(Item **items, int *count, int *capacity, int type) {
    if (!growIfFull(items, *count, capacity)) return 0;
    Item *newItem = &(*items)[*count];
    printf("\n--- Register a %s item ---\n", type == LOST ? "LOST" : "FOUND");
    newItem->id = getNextId(*items, *count);
    newItem->type = type;
    readName(newItem->personName);
    readMobile(newItem->mobile);
    newItem->locationIndex = chooseFromList("Select location:", locations, LOCATION_COUNT);
    newItem->objectIndex   = chooseFromList("Select object:", objects, OBJECT_COUNT);
    newItem->colourIndex   = chooseFromList("Select colour:", colours, COLOUR_COUNT);
    newItem->status = STATUS_OPEN;
    newItem->matchedId = 0;
    newItem->matchScore = 0;
    (*count)++;
    printf("\nRegistered successfully. Item ID: %d\n", newItem->id);
    return 1;
}
// Prints one item on one line
// CONCEPT: Formatted output
void printItem(const Item *item) {
    printf("%d | %-5s | %s | %s | %s %s at %s | %s",
           item->id, item->type == LOST ? "LOST" : "FOUND",
           item->personName, item->mobile,
           colours[item->colourIndex], objects[item->objectIndex],
           locations[item->locationIndex], statusNames[item->status]);
    if (item->matchedId != 0) {
        printf(" (with ID %d, score %d)", item->matchedId, item->matchScore);
    }
    printf("\n");
}
// Prints every item
// CONCEPT: Loops (traversal of an array of structs)
void printItemList(Item *items, int count) {
    if (count == 0) {
        printf("No items registered yet.\n");
        return;
    }
    for (int i = 0; i < count; i++) {
        printItem(&items[i]);
    }
}
// =====================================================================
// PART 3: MATCHING   (place 2nd, after Part 2)
// =====================================================================
#define MATCH_THRESHOLD 70         // minimum score to accept a match
// Gives a score from 0 to 100 by comparing the three fields
// CONCEPT: if conditions + operators
int calculateScore(const Item *lostItem, const Item *foundItem) {
    int score = 0;
    if (lostItem->objectIndex == foundItem->objectIndex)     score += 50;
    if (lostItem->colourIndex == foundItem->colourIndex)     score += 30;
    if (lostItem->locationIndex == foundItem->locationIndex) score += 20;
    return score;
}
// Matches every open lost item with its best open found item
// CONCEPT: Nested loops + array of structs (items are edited in place)
void findMatches(Item *items, int count) {
    int matchesFound = 0;
    for (int i = 0; i < count; i++) {
        if (items[i].type != LOST || items[i].status != STATUS_OPEN) continue;
        int bestIndex = -1;
        int bestScore = 0;
        for (int j = 0; j < count; j++) {
            if (items[j].type != FOUND || items[j].status != STATUS_OPEN) continue;
            int score = calculateScore(&items[i], &items[j]);
            if (score > bestScore) {
                bestScore = score;
                bestIndex = j;
            }
        }
        if (bestIndex != -1 && bestScore >= MATCH_THRESHOLD) {
            items[i].status = STATUS_MATCHED;
            items[i].matchedId = items[bestIndex].id;
            items[i].matchScore = bestScore;
            items[bestIndex].status = STATUS_MATCHED;
            items[bestIndex].matchedId = items[i].id;
            items[bestIndex].matchScore = bestScore;
            printf("Lost ID %d matched with Found ID %d (score %d)\n",
                   items[i].id, items[bestIndex].id, bestScore);
            matchesFound++;
        }
    }
    if (matchesFound == 0) printf("No new matches found.\n");
}
// =====================================================================
// PART 4: SORTING   (place 3rd, after Part 3)
// =====================================================================
// Swaps two records
// CONCEPT: Pointers / pass by reference (swap)
void swapItems(Item *first, Item *second) {
    Item temp = *first;
    *first = *second;
    *second = temp;
}
// Copies text in lowercase (used for case-insensitive name sorting and search)
// CONCEPT: Strings
void toLowerCase(const char *source, char *result) {
    int i;
    for (i = 0; source[i] != '\0'; i++) {
        result[i] = tolower(source[i]);
    }
    result[i] = '\0';
}
// Bubble sort: smallest ID first
// CONCEPT: Sorting (bubble sort)
void sortById(Item *items, int count) {
    for (int pass = 0; pass < count - 1; pass++) {
        for (int i = 0; i < count - 1 - pass; i++) {
            if (items[i].id > items[i + 1].id) {
                swapItems(&items[i], &items[i + 1]);
            }
        }
    }
}
// Bubble sort: highest match score first
// CONCEPT: Sorting (bubble sort)
void sortByScore(Item *items, int count) {
    for (int pass = 0; pass < count - 1; pass++) {
        for (int i = 0; i < count - 1 - pass; i++) {
            if (items[i].matchScore < items[i + 1].matchScore) {
                swapItems(&items[i], &items[i + 1]);
            }
        }
    }
}
// Selection sort: A to Z by person name (ignoring capital letters)
// CONCEPT: Sorting (selection sort) + strings
void sortByName(Item *items, int count) {
    char nameA[NAME_LEN];
    char nameB[NAME_LEN];
    for (int i = 0; i < count - 1; i++) {
        int smallest = i;
        for (int j = i + 1; j < count; j++) {
            toLowerCase(items[j].personName, nameA);
            toLowerCase(items[smallest].personName, nameB);
            if (strcmp(nameA, nameB) < 0) {
                smallest = j;
            }
        }
        if (smallest != i) {
            swapItems(&items[i], &items[smallest]);
        }
    }
}
// =====================================================================
// PART 5: SEARCHING   (place 4th, after Part 4)
// =====================================================================
// Recursive binary search (the array MUST be sorted by ID)
// Returns the index of the item, or -1 if not found
// CONCEPT: Recursion + searching
int binarySearchById(Item *items, int low, int high, int targetId) {
    if (low > high) return -1;                       // base case: not found
    int middle = (low + high) / 2;
    if (items[middle].id == targetId) return middle; // base case: found
    if (targetId < items[middle].id) {
        return binarySearchById(items, low, middle - 1, targetId);   // search left half
    }
    return binarySearchById(items, middle + 1, high, targetId);      // search right half
}
// CONCEPT: Searching (binary search) + sorting before searching
void searchById(Item *items, int count) {
    int targetId = readNumberInRange("Enter item ID to search: ", 1, 999999);
    sortById(items, count);                          // binary search needs sorted IDs
    int index = binarySearchById(items, 0, count - 1, targetId);
    if (index == -1) {
        printf("No item found with ID %d.\n", targetId);
    } else {
        printItem(&items[index]);
    }
}
// Case-insensitive search: finds names that contain the typed text
// CONCEPT: Strings + searching (linear search)
void searchByName(Item *items, int count) {
    char typedText[100];
    char lowerTyped[100];
    char lowerName[NAME_LEN];
    int foundAny = 0;
    printf("Enter name (or part of it) to search: ");
    readLine(typedText, sizeof(typedText));
    toLowerCase(typedText, lowerTyped);
    for (int i = 0; i < count; i++) {
        toLowerCase(items[i].personName, lowerName);
        if (strstr(lowerName, lowerTyped) != NULL) {
            printItem(&items[i]);
            foundAny = 1;
        }
    }
    if (!foundAny) printf("No item found with that name.\n");
}
// =====================================================================
// PART 6: UPDATE AND DELETE   (place 5th, after Part 5)
// =====================================================================
// Linear search for an ID. Returns the index, or -1 if not found
// CONCEPT: Searching (linear search) + error handling (missing / duplicate IDs)
int findIndexById(Item *items, int count, int id) {
    for (int i = 0; i < count; i++) {
        if (items[i].id == id) return i;
    }
    return -1;
}
// Changes one detail of an open item through a pointer
// CONCEPT: Pointers / pass by reference + switch + error handling
void updateItem(Item *items, int count) {
    int id = readNumberInRange("Enter ID of the item to update: ", 1, 999999);
    int index = findIndexById(items, count, id);
    if (index == -1) {
        printf("Error: no item with ID %d.\n", id);
        return;
    }
    Item *item = &items[index];                      // pointer to the record
    if (item->status != STATUS_OPEN) {
        printf("Only open (unmatched) items can be updated.\n");
        return;
    }
    printf("\n1. Mobile number\n2. Location\n3. Object\n4. Colour\n");
    int choice = readNumberInRange("What do you want to change? ", 1, 4);
    switch (choice) {
        case 1: readMobile(item->mobile);                                                   break;
        case 2: item->locationIndex = chooseFromList("Select location:", locations, LOCATION_COUNT); break;
        case 3: item->objectIndex   = chooseFromList("Select object:", objects, OBJECT_COUNT);       break;
        case 4: item->colourIndex   = chooseFromList("Select colour:", colours, COLOUR_COUNT);       break;
    }
    printf("Item updated: ");
    printItem(item);
}
// Deletes an item by shifting the later items one place to the left
// CONCEPT: Pointers (count changed by reference) + loops + error handling
void deleteItem(Item *items, int *count) {
    int id = readNumberInRange("Enter ID of the item to delete: ", 1, 999999);
    int index = findIndexById(items, *count, id);
    if (index == -1) {
        printf("Error: no item with ID %d.\n", id);
        return;
    }
    // If it was matched, its partner becomes open again
    if (items[index].status == STATUS_MATCHED) {
        int partnerIndex = findIndexById(items, *count, items[index].matchedId);
        if (partnerIndex != -1) {
            items[partnerIndex].status = STATUS_OPEN;
            items[partnerIndex].matchedId = 0;
            items[partnerIndex].matchScore = 0;
        }
    }
    for (int i = index; i < *count - 1; i++) {
        items[i] = items[i + 1];
    }
    (*count)--;
    printf("Item %d deleted.\n", id);
}
// =====================================================================
// PART 7: CLAIMS   (place 6th, after Part 6)
// =====================================================================
// Checks that the claimant is the real owner, then marks both items as returned
// CONCEPT: if / else + strings (strcmp) + status update
void verifyAndClaim(Item *items, int count) {
    int lostId = readNumberInRange("Enter the LOST item ID being claimed: ", 1, 999999);
    int lostIndex = findIndexById(items, count, lostId);
    if (lostIndex == -1 || items[lostIndex].type != LOST) {
        printf("Error: no lost item with ID %d.\n", lostId);
        return;
    }
    if (items[lostIndex].status != STATUS_MATCHED) {
        printf("This item is not matched yet, or is already returned.\n");
        return;
    }
    // Ownership check: the mobile number must match the one on the lost report
    char typedMobile[MOBILE_LEN];
    printf("Ownership check - ");
    readMobile(typedMobile);
    if (strcmp(typedMobile, items[lostIndex].mobile) != 0) {
        printf("Claim REJECTED: mobile number does not match the owner's record.\n");
        return;
    }
    int foundIndex = findIndexById(items, count, items[lostIndex].matchedId);
    items[lostIndex].status = STATUS_RETURNED;
    if (foundIndex != -1) {
        items[foundIndex].status = STATUS_RETURNED;
    }
    printf("Claim APPROVED. Item %d is marked as Returned.\n", lostId);
}
// =====================================================================
// PART 8: REPORTS   (place 7th, after Part 7)
// =====================================================================
#define REPORT_LOST 1
#define REPORT_FOUND 2
#define REPORT_MATCHED 3
#define REPORT_RETURNED 4
// Prints only the items that belong to the chosen report
// CONCEPT: switch + loops + formatted output
void printReport(Item *items, int count, int reportType) {
    for (int i = 0; i < count; i++) {
        int belongsInReport = 0;
        switch (reportType) {
            case REPORT_LOST:     belongsInReport = (items[i].type == LOST);                   break;
            case REPORT_FOUND:    belongsInReport = (items[i].type == FOUND);                  break;
            case REPORT_MATCHED:  belongsInReport = (items[i].status == STATUS_MATCHED);       break;
            case REPORT_RETURNED: belongsInReport = (items[i].status == STATUS_RETURNED);      break;
        }
        if (belongsInReport) {
            printItem(&items[i]);
        }
    }
}
// =====================================================================
// PART 9: LOCATION TRACKER   (place 8th, after Part 8)
// =====================================================================
// Table: how many items of each object are linked with each campus location
// CONCEPT: 2D arrays (counting in a matrix) + nested loops
void showLocationTable(Item *items, int count) {
    int locationTracker[LOCATION_COUNT][OBJECT_COUNT] = {0};
    for (int i = 0; i < count; i++) {
        locationTracker[items[i].locationIndex][items[i].objectIndex]++;
    }
    printf("\n%-15s", "Location");
    for (int obj = 0; obj < OBJECT_COUNT; obj++) {
        printf("%-9s", objects[obj]);
    }
    printf("\n");
    for (int loc = 0; loc < LOCATION_COUNT; loc++) {
        printf("%-15s", locations[loc]);
        for (int obj = 0; obj < OBJECT_COUNT; obj++) {
            printf("%-9d", locationTracker[loc][obj]);
        }
        printf("\n");
    }
}
// =====================================================================
// PART 10: FILE HANDLING   (place 9th, after Part 9)
// =====================================================================
// Saves the item count first, then all records
// CONCEPT: File handling (fopen, fwrite, fclose) + error handling
void saveToFile(Item *items, int count) {
    FILE *file = fopen(DATA_FILE, "wb");
    if (file == NULL) {
        printf("Error: could not open the file for saving.\n");
        return;
    }
    fwrite(&count, sizeof(int), 1, file);
    fwrite(items, sizeof(Item), count, file);
    fclose(file);
    printf("%d record(s) saved to %s.\n", count, DATA_FILE);
}
// Adds records from the file to the list (skips IDs that already exist)
// CONCEPT: File handling (fopen, fread) + missing-file check + duplicate check
void loadFromFile(Item **items, int *count, int *capacity) {
    FILE *file = fopen(DATA_FILE, "rb");
    if (file == NULL) {
        printf("No saved file found yet. Save some records first.\n");
        return;
    }
    int savedCount;
    if (fread(&savedCount, sizeof(int), 1, file) != 1 || savedCount < 0) {
        printf("Error: the file is empty or damaged.\n");
        fclose(file);
        return;
    }
    int loaded = 0, skipped = 0;
    Item record;
    for (int i = 0; i < savedCount; i++) {
        if (fread(&record, sizeof(Item), 1, file) != 1) {
            printf("Error: the file ended early.\n");
            break;
        }
        if (findIndexById(*items, *count, record.id) != -1) {
            skipped++;                               // duplicate ID
            continue;
        }
        if (!growIfFull(items, *count, capacity)) break;
        (*items)[*count] = record;
        (*count)++;
        loaded++;
    }
    fclose(file);
    printf("Loaded %d record(s). Skipped %d duplicate(s).\n", loaded, skipped);
}
// =====================================================================
// PART 1: MENUS AND MAIN   (place LAST)
// =====================================================================
// CONCEPT: switch-based sub-menu
void registerMenu(Item **items, int *count, int *capacity) {
    int choice;
    do {
        printf("\n--- REGISTER ITEM ---\n");
        printf("1. Lost item\n");
        printf("2. Found item\n");
        printf("0. Back\n");
        choice = readNumberInRange("Your choice: ", 0, 2);
        switch (choice) {
            case 1: registerItem(items, count, capacity, LOST);  break;
            case 2: registerItem(items, count, capacity, FOUND); break;
        }
    } while (choice != 0);
}
// CONCEPT: switch-based sub-menu
void searchMenu(Item *items, int count) {
    int choice;
    do {
        printf("\n--- SEARCH ITEMS ---\n");
        printf("1. Search by ID\n");
        printf("2. Search by name\n");
        printf("0. Back\n");
        choice = readNumberInRange("Your choice: ", 0, 2);
        switch (choice) {
            case 1: searchById(items, count);   break;
            case 2: searchByName(items, count); break;
        }
    } while (choice != 0);
}
// CONCEPT: switch-based sub-menu
void manageMenu(Item *items, int *count) {
    int choice;
    do {
        printf("\n--- MANAGE RECORDS ---\n");
        printf("1. Update an item\n");
        printf("2. Delete an item\n");
        printf("0. Back\n");
        choice = readNumberInRange("Your choice: ", 0, 2);
        switch (choice) {
            case 1: updateItem(items, *count); break;
            case 2: deleteItem(items, count);  break;
        }
    } while (choice != 0);
}
// CONCEPT: switch-based sub-menu
void matchingMenu(Item *items, int count) {
    int choice;
    do {
        printf("\n--- MATCHING AND CLAIMS ---\n");
        printf("1. Find matches\n");
        printf("2. Verify and approve a claim\n");
        printf("0. Back\n");
        choice = readNumberInRange("Your choice: ", 0, 2);
        switch (choice) {
            case 1: findMatches(items, count);    break;
            case 2: verifyAndClaim(items, count); break;
        }
    } while (choice != 0);
}
// CONCEPT: switch-based sub-menu
void sortMenu(Item *items, int count) {
    int choice;
    do {
        printf("\n--- SORT ITEMS ---\n");
        printf("1. By name\n");
        printf("2. By ID\n");
        printf("3. By match score\n");
        printf("0. Back\n");
        choice = readNumberInRange("Your choice: ", 0, 3);
        switch (choice) {
            case 1: sortByName(items, count);  printItemList(items, count); break;
            case 2: sortById(items, count);    printItemList(items, count); break;
            case 3: sortByScore(items, count); printItemList(items, count); break;
        }
    } while (choice != 0);
}
// CONCEPT: switch-based sub-menu
void reportMenu(Item *items, int count) {
    int choice;
    do {
        printf("\n--- REPORTS ---\n");
        printf("1. All items\n");
        printf("2. Lost items\n");
        printf("3. Found items\n");
        printf("4. Matched items\n");
        printf("5. Returned items\n");
        printf("6. Location table\n");
        printf("0. Back\n");
        choice = readNumberInRange("Your choice: ", 0, 6);
        switch (choice) {
            case 1: printItemList(items, count);                    break;
            case 2: printReport(items, count, REPORT_LOST);         break;
            case 3: printReport(items, count, REPORT_FOUND);        break;
            case 4: printReport(items, count, REPORT_MATCHED);      break;
            case 5: printReport(items, count, REPORT_RETURNED);     break;
            case 6: showLocationTable(items, count);                break;
        }
    } while (choice != 0);
}
// CONCEPT: switch-based sub-menu
void fileMenu(Item **items, int *count, int *capacity) {
    int choice;
    do {
        printf("\n--- SAVE / LOAD ---\n");
        printf("1. Save records to file\n");
        printf("2. Load records from file\n");
        printf("0. Back\n");
        choice = readNumberInRange("Your choice: ", 0, 2);
        switch (choice) {
            case 1: saveToFile(*items, *count);                 break;
            case 2: loadFromFile(items, count, capacity);       break;
        }
    } while (choice != 0);
}
int main() {
    int capacity = INITIAL_CAPACITY;
    int count = 0;
    Item *items = (Item *)malloc(capacity * sizeof(Item));   // CONCEPT: dynamic memory
    if (items == NULL) {
        printf("Error: out of memory.\n");
        return 1;
    }
    int choice;
    do {
        printf("\n===== FindIt - Campus Lost and Found =====\n");
        printf("1. Register item\n");
        printf("2. Search items\n");
        printf("3. Manage records\n");
        printf("4. Matching and claims\n");
        printf("5. Sort items\n");
        printf("6. Reports\n");
        printf("7. Save / Load\n");
        printf("0. Exit\n");
        choice = readNumberInRange("Your choice: ", 0, 7);
        // CONCEPT: switch (main menu)
        switch (choice) {
            case 1: registerMenu(&items, &count, &capacity);  break;
            case 2: searchMenu(items, count);                 break;
            case 3: manageMenu(items, &count);                break;
            case 4: matchingMenu(items, count);               break;
            case 5: sortMenu(items, count);                   break;
            case 6: reportMenu(items, count);                 break;
            case 7: fileMenu(&items, &count, &capacity);      break;
            case 0: printf("Goodbye! (Unsaved records are lost.)\n"); break;
        }
    } while (choice != 0);
    free(items);                                              // CONCEPT: dynamic memory
    return 0;
}
