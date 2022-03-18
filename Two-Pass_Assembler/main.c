/* Jacob Wickline
 * 10/20/20
 * CS 325
 * Two-Pass Assembler
 */

#include <stdio.h> //for i/o
#include <string.h>
#include <stdlib.h> //for malloc

struct Node{
    char* symbol;
    int value;
    struct Node* next;
};

struct LinkedList{
    struct Node* head;
};

void fileClean();
char commentFinder(char*);
void firstPass();
char* breakBefore(char*, char);
char* breakAfter(char*, char);
char* labels(char*);
void printList(struct LinkedList* list);
void insertAtEnd(struct LinkedList* list, char*, int item);
void deleteList(struct LinkedList* list);
int symbolTableCheck(struct LinkedList* list, char*);

int main(){
    fileClean();
    firstPass();

    return 0;
}

void fileClean(){
    //Declaring files with pointers
    FILE* inFile = NULL;
    FILE* outFile = NULL;

    //This creates a buffer to limit the size of stuff in the program
    const int bufferSize = 256;
    //This limits the size of the input files name
    char inFileName[bufferSize];

    printf("Please enter the input file name: ");
    //This retrieves the file from the user's input
    fgets(inFileName, bufferSize, stdin);
    //This removes the new line from the file name
    inFileName[strcspn(inFileName, "\n")] = 0;

    //This opens the file in read only mode, "r" = read only
    inFile = fopen(inFileName, "r");
    //This creates a output file and allows it to be written to, "w" = write
    outFile = fopen("newFile.txt", "w");

    //This checks to see if the pointer is at NULL, if it's at NULL the file won't open
    if(inFile == NULL){
        puts("Error opening input file");
    }

    else if(outFile == NULL){
        puts("Error opening output file");
    }

    else{
        //This creates a buffer to read into the file with
        char readBuffer[bufferSize];
        char newBuffer[bufferSize];

        while(fgets(readBuffer, bufferSize, inFile) != NULL){
            //This removes the line feed
            readBuffer[strcspn(readBuffer, "\r\n")] = 0;

            /*This calls the commentFinder function, which will locate the comment and
             *set the comment to '\0' so it wont be printed
             */
            int commentPosition = commentFinder(readBuffer);
            if(strlen(readBuffer) > 0){
                readBuffer[commentPosition] = '\0';
            }

            //This filters out spaces and tabs in the file
            int adjustPosition = 0;
            if(readBuffer[adjustPosition] == ' ' || readBuffer[adjustPosition] == '\t'){
                while(readBuffer[adjustPosition] == ' ' || readBuffer[adjustPosition] == '\t'){
                    adjustPosition++;
                }
            }

            //This copies from the previous buffer position to a new buffer that starts from the beginning
            int buffPosition = 0;
            for(int i = 0; i < bufferSize; i++){
                newBuffer[buffPosition] = readBuffer[adjustPosition];
                buffPosition++;
                adjustPosition++;
            }

            fputs(newBuffer, outFile);

            //This writes a new line to the out file as long as there is text in it
            if(strlen(readBuffer) != 0){
                fputs("\n", outFile);
            }

        }

        //This closes the input and output file
        fclose(inFile);
        fclose(outFile);
    }
}

char commentFinder(char* string){
    int position = 0;
    while(string[position] != '/' && string[position] != '/' &&
          position < strlen(string)){
        position++;
    }

    return position;
}

void firstPass(){
    //Declaring files with pointers
    FILE* inFile = NULL;
    FILE* outFile = NULL;

    //Declaring my linked list
    struct LinkedList symbolTable;
    symbolTable.head = NULL;

    //Adding the default symbols to the symbol table
    insertAtEnd(&symbolTable, "R0", 0);
    insertAtEnd(&symbolTable, "R1", 1);
    insertAtEnd(&symbolTable, "R2", 2);
    insertAtEnd(&symbolTable, "R3", 3);
    insertAtEnd(&symbolTable, "R4", 4);
    insertAtEnd(&symbolTable, "R5", 5);
    insertAtEnd(&symbolTable, "R6", 6);
    insertAtEnd(&symbolTable, "R7", 7);
    insertAtEnd(&symbolTable, "R8", 8);
    insertAtEnd(&symbolTable, "R9", 9);
    insertAtEnd(&symbolTable, "R10", 10);
    insertAtEnd(&symbolTable, "R11", 11);
    insertAtEnd(&symbolTable, "R12", 12);
    insertAtEnd(&symbolTable, "R13", 13);
    insertAtEnd(&symbolTable, "R14", 14);
    insertAtEnd(&symbolTable, "R15", 15);
    insertAtEnd(&symbolTable, "SP", 0);
    insertAtEnd(&symbolTable, "LCL", 1);
    insertAtEnd(&symbolTable, "ARG", 2);
    insertAtEnd(&symbolTable, "THIS", 3);
    insertAtEnd(&symbolTable, "THAT", 4);
    insertAtEnd(&symbolTable, "SCREEN", 16384);
    insertAtEnd(&symbolTable, "KBD", 24576);
    insertAtEnd(&symbolTable, "OUTPUT_FIRST", 10);
    insertAtEnd(&symbolTable, "OUTPUT_D", 12);
    insertAtEnd(&symbolTable, "INFINITE_LOOP", 14);


    //This is for the ROM position
    int romPosition = 0;

    //This creates a buffer to limit the size of stuff in the program
    const int bufferSize = 256;
    //This gives the name of the new file that was sent through the fileClean function
    char inFileName[] = "newFile.txt";

    //This removes the new line from the file name
    inFileName[strcspn(inFileName, "\n")] = 0;

    //This opens the file in read only mode, "r" = read only
    inFile = fopen(inFileName, "r");
    //This creates a output file and allows it to be written to, "w" = write
    outFile = fopen("output.txt", "w");

    //This checks to see if the pointer is at NULL, if it's at NULL the file won't open
    if(inFile == NULL){
        puts("Error opening input file");
    }

    else if(outFile == NULL){
        puts("Error opening output file");
    }

    else{
        //This creates a buffer to read into the file with
        char readBuffer[bufferSize];

        while(fgets(readBuffer, bufferSize, inFile) != NULL){
            //This removes the line feed
            readBuffer[strcspn(readBuffer, "\r\n")] = 0;

            //This is testing the output file
            if(readBuffer[0] == '('){
                char* labelCheck = labels(readBuffer);
                int symbolCheck = symbolTableCheck(&symbolTable, labelCheck);
                if(symbolCheck != -1){
                    fputs(labelCheck, outFile);
                    insertAtEnd(&symbolTable, labelCheck, romPosition);
                }
                free(labelCheck);
            }

            //This will ignore the (whatever) and wont add to the counter
            if(readBuffer[0] != '('){
                romPosition++;
            }

            //This writes a new line to the out file as long as there is text in it
            if(strlen(readBuffer) != 0){
                fputs("\n", outFile);
            }
        }

        //This closes the input and output file
        fclose(inFile);
        fclose(outFile);

        //This is for printing and freeing up the memory for the symbol table
        printList(&symbolTable);
        deleteList(&symbolTable);
    }
}

char* breakBefore(char* string, char breakChar){
    int position = 0;

    while(string[position] != breakChar){
        position++;
    }

    int size = strlen(string) - position;
    char* result = malloc(sizeof(char) * (size + 1));

    strcpy(result, string);
    result[position] = '\0';

    return result;
}

char* breakAfter(char* string, char breakChar){
    int position = 0;

    while(string[position] != breakChar){
        position++;
    }

    int size = strlen(string) - position;
    char* result = malloc(sizeof(char) * (size + 1));

    position++;

    int start = 0;
    for(int i = position; i < strlen(string); i++){
        result[start] = string[i];
        start++;
    }

    result[start] = '\0';

    return result;
}


char* labels(char* string){
    char* labelArray = malloc(sizeof(char) * 8);
    char firstBreakChar = '(';
    char secondBreakChar = ')';

    char* firstPass = breakAfter(string, firstBreakChar);
    char* secondPass = breakBefore(firstPass, secondBreakChar);

    strcpy(labelArray, secondPass);

    return labelArray;
}

//This is used to print the Linked List
void printList(struct LinkedList* list)
{
    struct Node* temp = NULL;
    temp = list->head;
    while( temp != NULL )
    {
        printf("%s %d\n", temp->symbol, temp->value);
        temp = temp -> next;
    }
}

//This is used for inserting a char and value in the linked list
void insertAtEnd(struct LinkedList* list, char* symbol, int item)
{
    if(list->head == NULL)
    {
        list->head = malloc(sizeof(struct Node));
        list->head->symbol = malloc(sizeof(char) * strlen(symbol));

        strcpy(list->head->symbol, symbol);

        list->head->value = item;
        list->head->next = NULL;
    }
    else
    {
       struct Node* temp = list->head;
       while(temp->next != NULL){
           temp = temp->next;
       }

       temp->next = malloc(sizeof(struct Node));
       temp->next->symbol = malloc(sizeof(char) * strlen(symbol));
       strcpy(temp->next->symbol, symbol);
       temp->next->value = item;
       temp->next->next = NULL;

    }
}

//This is used for freeing up memory
void deleteList(struct LinkedList* list)
{
    struct Node* temp = list->head;
    while(temp != NULL){
        list->head = temp->next;
        free(temp->symbol);
        free(temp);
        temp = list->head;
    }
}


int symbolTableCheck(struct LinkedList* list, char* symbol){
    int symbolCheckStatus = 0;
    struct Node* temp = list->head;
    while(temp != NULL){
        if(temp->symbol == symbol){
            symbolCheckStatus = -1;
            puts("SYMBOL DETECTED");
            return symbolCheckStatus;
        }

        else{
            temp = temp->next;
        }
    }

    return symbolCheckStatus;

}
