#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>
#include <ctype.h>

#define NUM_CARDS 52

typedef struct card
{
    char suit;
    char rank;
    int rankValue;
    bool revealed;
} card;

typedef struct node
{
    struct card *card;
    struct node *prev;
    struct node *next;
} node;

typedef struct cardLocation
{
    node *foundNode;
    char location;
} cardLocation;

void startUp();
bool play();
struct card *createNewCard(char suit, char rank);
struct node *createNewNode(struct card *card);
int getRankValue(char rank);
void insertNode(node **headRef, card *card);
void createDeck(node **deck, char *filename);
void shuffleList(node **head);
void saveDeck(node *head, char *filename);
void setDeck(node **deck, node **C1, node **C2, node **C3, node **C4, node **C5, node **C6, node **C7);
void dealCards(node **deck, node **C1, node **C2, node **C3, node **C4, node **C5, node **C6, node **C7);
void setAllCardsRevealed(node *head, bool revealed);
void printNode(const struct node *node);
void printList(const struct node *list);
int countNodes(const struct node *list);
node *removeFirstNode(node **head);
void insertRandomPosition(node **head, node *newNode);
void insertListBack(node **head, node *newNode);
void returnCardsToDeck(node **deck, node **C1, node **C2, node **C3, node **C4, node **C5, node **C6, node **C7, node **F1, node **F2, node **F3, node **F4);
void printBoard(node **C1, node **C2, node **C3, node **C4, node **C5, node **C6, node **C7, node **F1, node **F2, node **F3, node **F4);
void moveCard(node **c1, node **c2, node **c3, node **c4, node **c5, node **c6, node **c7, node **f1, node **f2, node **f3, node **f4, const char *cardString, const char *destString);
void printNodeBoard(node **column, int i);
node *getLastNode(node *head);
void revealCard(node *C1, node *C2, node *C3, node *C4, node *C5, node *C6, node *C7);
cardLocation findCard(const char *cardString, node *C1, node *C2, node *C3, node *C4, node *C5, node *C6, node *C7);
void extractStrings(char *left, char *right);
node *RemoveCards(node **c1, node **c2, node **c3, node **c4, node **c5, node **c6, node **c7, cardLocation cardLocation, const char *cardString);
bool isMoveLegalToFoundation(node **c1, node **c2, node **c3, node **c4, node **c5, node **c6, node **c7, node **f1, node **f2, node **f3, node **f4, const char *cardString, const char *destString);
const char *parseCard(const struct node *cardNode);
void initializeRandom();
bool isMoveLegalToTableau(node **c1, node **c2, node **c3, node **c4, node **c5, node **c6, node **c7, node **f1, node **f2, node **f3, node **f4, const char *cardString, const char *destString);

node *C1 = NULL;
node *C2 = NULL;
node *C3 = NULL;
node *C4 = NULL;
node *C5 = NULL;
node *C6 = NULL;
node *C7 = NULL;
node *F1 = NULL;
node *F2 = NULL;
node *F3 = NULL;
node *F4 = NULL;
node *head = NULL;
bool gameWon = false;
bool playPhase = false;
bool startPhase = true;
char left[256] = {'\0'};
char right[256] = {'\0'};

int main()
{

    startUp();
    return 0;
}

void startUp()
{
    char input[256];
    char command[3], filename[256];
    strcpy(filename, "cardDeck.txt");
    while (startPhase)
    {
        printf("Enter a command: ");
        fgets(input, 256, stdin);
        sscanf(input, "%s %s", command, filename);

        // convert command to uppercase
        for (int i = 0; i < strlen(command); i++)
        {
            command[i] = toupper(command[i]);
        }

        switch (command[0])
        {
        case 'Q':
            if (command[1] == 'Q')
            {
                // Quit game
                startPhase = false;
                break;
            }
            // else fallthrough
        case 'P':

            // Perform action for command P
            if (head == NULL)
            {
                printf("please load a deck first\n");
            }
            else
            {
                setAllCardsRevealed(head, false);
                initializeRandom();
                printf("Shuffeling...\n");
                int numShuffles = rand() % 91 + 10;
                for (int i = 0; i < numShuffles; i++)
                {
                    shuffleList(&head);
                }
                dealCards(&head, &C1, &C2, &C3, &C4, &C5, &C6, &C7);
                printBoard(&C1, &C2, &C3, &C4, &C5, &C6, &C7, &F1, &F2, &F3, &F4);
                playPhase = true;
                while (playPhase)
                {
                    playPhase = play();
                }
            }

            break;
        case 'D':

            head = NULL;
            createDeck(&head, "debugDeck.txt");
            dealCards(&head, &C1, &C2, &C3, &C4, &C5, &C6, &C7);
            printBoard(&C1, &C2, &C3, &C4, &C5, &C6, &C7, &F1, &F2, &F3, &F4);
            playPhase = true;
            while (playPhase)
            {
                playPhase = play();
            }
            break;

        case 'L':
            if (command[1] == 'D')
            {
                // Load deck from file
                if (head == NULL && C1 != NULL && C2 != NULL && C3 != NULL && C4 != NULL && C5 != NULL && C6 != NULL && C7 != NULL)
                {
                    returnCardsToDeck(&head, &C1, &C2, &C3, &C4, &C5, &C6, &C7, &F1, &F2, &F3, &F4);
                    head = NULL;
                }
                printf("Loading deck from file: %s\n", filename);
                createDeck(&head, filename);
                setDeck(&head, &C1, &C2, &C3, &C4, &C5, &C6, &C7);
                printBoard(&C1, &C2, &C3, &C4, &C5, &C6, &C7, &F1, &F2, &F3, &F4);
                returnCardsToDeck(&head, &C1, &C2, &C3, &C4, &C5, &C6, &C7, &F1, &F2, &F3, &F4);
                break;
            }
            // else fallthrough
        case 'S':
            if (command[1] == 'W')
            {
                // Show cards in deck

                setAllCardsRevealed(head, true);
                setDeck(&head, &C1, &C2, &C3, &C4, &C5, &C6, &C7);
                printBoard(&C1, &C2, &C3, &C4, &C5, &C6, &C7, &F1, &F2, &F3, &F4);
                returnCardsToDeck(&head, &C1, &C2, &C3, &C4, &C5, &C6, &C7, &F1, &F2, &F3, &F4);
                break;
            }
            else if (command[1] == 'D')
            {
                // Save deck to file
                setAllCardsRevealed(head, false);
                printf("Saving deck to file: %s\n", filename);
                returnCardsToDeck(&head, &C1, &C2, &C3, &C4, &C5, &C6, &C7, &F1, &F2, &F3, &F4);
                saveDeck(head, filename);
                break;
            }
            else if (command[1] == 'R')
            {
                // Shuffle deck
                shuffleList(&head);
                setDeck(&head, &C1, &C2, &C3, &C4, &C5, &C6, &C7);
                printBoard(&C1, &C2, &C3, &C4, &C5, &C6, &C7, &F1, &F2, &F3, &F4);
                returnCardsToDeck(&head, &C1, &C2, &C3, &C4, &C5, &C6, &C7, &F1, &F2, &F3, &F4);
                break;
            }
            // else fallthrough
        default:
            printf("Invalid command\n");
        }
    }
}

bool play()
{
    char buffer[256];
    char left[256], right[256];

    printf("Type in a card move or command: ");
    if (fscanf(stdin, "%255s", buffer) == 1)
    {
        // Consume the newline character
        getchar();

        char *arrow = strstr(buffer, "->");
        if (arrow != NULL)
        {
            *arrow = '\0';
            strcpy(left, buffer);
            strcpy(right, arrow + 2);
            if (toupper(right[0]) == 'F')
            {
                if (isMoveLegalToFoundation(&C1, &C2, &C3, &C4, &C5, &C6, &C7, &F1, &F2, &F3, &F4, left, right))
                {
                    printf("OK\n");
                    moveCard(&C1, &C2, &C3, &C4, &C5, &C6, &C7, &F1, &F2, &F3, &F4, left, right);
        
                }else{
                     printf("Move not valid\n");
                }
            }else if (toupper(right[0]) == 'C')
            {
                 if (isMoveLegalToTableau(&C1, &C2, &C3, &C4, &C5, &C6, &C7, &F1, &F2, &F3, &F4, left, right))
                {
                    printf("OK\n");
                    moveCard(&C1, &C2, &C3, &C4, &C5, &C6, &C7, &F1, &F2, &F3, &F4, left, right);
                } else{
                     printf("Move not valid\n");
                }
            }
           

           
            revealCard(C1, C2, C3, C4, C5, C6, C7);
            printBoard(&C1, &C2, &C3, &C4, &C5, &C6, &C7, &F1, &F2, &F3, &F4);
        }
        else if (strcasecmp(buffer, "Q") == 0)
        {
            // Quit game
            return false;
        }
        else
        {
            printf("Invalid command\n");
        }
    }

    return true;
}

void initializeRandom()
{
    srand(time(NULL));
}

void setAllCardsRevealed(node *head, bool revealed)
{
    node *curr = head;
    while (curr != NULL)
    {
        curr->card->revealed = revealed;
        curr = curr->next;
    }
}

struct card *createNewCard(char suit, char rank)
{
    struct card *new_card = (struct card *)malloc(sizeof(struct card));
    new_card->suit = suit;
    new_card->rank = rank;
    new_card->revealed = false;
    new_card->rankValue = getRankValue(rank);
    return new_card;
};

int getRankValue(char rank){
    switch(rank){
        case 'A':
            return 1;
        case '2':
            return 2;
        case '3':
            return 3;
        case '4':
            return 4;
        case '5':
            return 5;
        case '6':
            return 6;
        case '7':
            return 7;
        case '8':
            return 8;
        case '9':
            return 9;
        case 'T':
            return 10;
        case 'J':
            return 11;
        case 'Q':
            return 12;
        case 'K':
            return 13;
        default:
            return 0;
    }
}

struct node *createNewNode(struct card *card)
{
    struct node *new_node = (struct node *)malloc(sizeof(struct node));
    new_node->card = card;
    new_node->next = NULL;
    new_node->prev = NULL;
    return new_node;
};

void insertNode(node **headRef, card *card)
{
    node *newNode = createNewNode(card);
    node *temp = *headRef;
    if (*headRef == NULL)
    {
        *headRef = newNode;
        return;
    }
    while (temp->next != NULL)
    {
        temp = temp->next;
    }
    temp->next = newNode;
    newNode->prev = temp; // assign the previous node for the new node
}

void saveDeck(node *head, char *filename)
{
    FILE *fp = fopen(filename, "w");
    if (fp == NULL)
    {
        printf("Error opening file %s\n", filename);
        return;
    }

    node *curr = head;
    while (curr != NULL)
    {
        fprintf(fp, "%c%c\n", curr->card->rank, curr->card->suit);
        curr = curr->next;
    }

    fprintf(fp, "\n"); // Add a new line after the last card

    fclose(fp);
}

void createDeck(node **deck, char *filename)
{
    card *curr_card, *prev_card;
    FILE *fp;
    if (filename == NULL)
    {
        fp = fopen("cardDeck.txt", "r");
    }
    else
    {
        fp = fopen(filename, "r");
    }

    if (fp == NULL)
    {
        printf("Error opening file\n");
        return;
    }
    else
    {
        printf("File opened successfully\n");
    }

    while (!feof(fp))
    {
        char line[4];
        fgets(line, 4, fp);
        if (strlen(line) == 2 || strlen(line) == 3)
        {
            line[strlen(line) - 1] = '\0';
            curr_card = createNewCard(line[1], line[0]);

            if (*deck == NULL)
            {
                *deck = createNewNode(curr_card);
                prev_card = curr_card;
            }
            else
            {
                insertNode(deck, curr_card);
                prev_card = curr_card;
            }
        }
    }

    fclose(fp);
}

void setDeck(node **deck, node **C1, node **C2, node **C3, node **C4, node **C5, node **C6, node **C7) 
{
    int i;
    int cardsDealt = 0;
    int currentColumn = 1;

    while (*deck != NULL)
    {
        node *node = removeFirstNode(deck);
        switch (currentColumn)
        {
        case 1:
            insertNode(C1, node->card);
            break;
        case 2:
            insertNode(C2, node->card);
            break;
        case 3:
            insertNode(C3, node->card);
            break;
        case 4:
            insertNode(C4, node->card);
            break;
        case 5:
            insertNode(C5, node->card);
            break;
        case 6:
            insertNode(C6, node->card);
            break;
        case 7:
            insertNode(C7, node->card);
            break;
        default:
            break;
        }
        cardsDealt++;

        // Increment the current column and reset to 1 if it exceeds 7
        currentColumn++;
        if (currentColumn > 7)
        {
            currentColumn = 1;
        }
    }
}


void printNode(const struct node *node)
{
    const char *card = parseCard(node);
    printf("%s\n", card);
}

void printList(const struct node *list)
{
    printf("Printing list:\n");
    const struct node *t;
    t = list;
    if (t == NULL)
    { // if the list is empty, print a message indicating that
        printf("list is empty\n");
    }
    else
    {
        while (t)
        {
            printNode(t); // print out the current person in line
            t = t->next;  // move to the next person in line
        }
    }
}

int countNodes(const struct node *list)
{
    const struct node *t = list;
    int count = 0;

    while (t != NULL)
    {
        count++;
        t = t->next;
    }

    // printf("The amount of nodes is %d\n", count);
    return count;
}

node *getLastNode(node *head)
{
    if (head == NULL)
    {
        // list is empty
        return NULL;
    }
    else
    {
        node *current = head;
        while (current->next != NULL)
        {
            current = current->next;
        }
        return current;
    }
}

void revealCard(node *C1, node *C2, node *C3, node *C4, node *C5, node *C6, node *C7)
{
    node *lastNode;

    for (int i = 1; i <= 7; i++)
    {
        switch (i)
        {
        case 1:
            lastNode = getLastNode(C1);
            break;
        case 2:
            lastNode = getLastNode(C2);
            break;
        case 3:
            lastNode = getLastNode(C3);
            break;
        case 4:
            lastNode = getLastNode(C4);
            break;
        case 5:
            lastNode = getLastNode(C5);
            break;
        case 6:
            lastNode = getLastNode(C6);
            break;
        case 7:
            lastNode = getLastNode(C7);
            break;
        default:
            break;
        }

        if (lastNode != NULL && lastNode->card->revealed == false)
        {
            lastNode->card->revealed = true;
        }
    }
}

node *removeFirstNode(node **head)
{
    if (*head == NULL)
    {
        // list is empty
        return NULL;
    }

    node *firstNode = *head;
    *head = (*head)->next;
    firstNode->next = NULL;
    return firstNode;
}

void insertRandomPosition(node **head, node *newNode)
{
    if (*head == NULL)
    {
        // list is empty, insert at head
        *head = newNode;
        return;
    }

    int listLength = 0;
    node *current = *head;
    while (current != NULL)
    {
        listLength++;
        current = current->next;
    }

    int randomIndex = rand() % listLength;
    if (randomIndex == 0)
    {
        // insert at head
        newNode->next = *head;
        *head = newNode;
        return;
    }

    current = *head;
    for (int i = 0; i < randomIndex - 1; i++)
    {
        current = current->next;
    }

    newNode->next = current->next;
    current->next = newNode;
}

void shuffleList(node **head)
{
    // Create a new empty list to hold shuffled nodes
    node *shuffledList = NULL;

    // Repeat 52 times
    for (int i = 0; i < NUM_CARDS; i++)
    {
        // Remove the first node from the original list
        node *removedNode = removeFirstNode(head);

        // Insert the removed node at a random position in the shuffled list
        insertRandomPosition(&shuffledList, removedNode);
    }

    // Update the original list with the shuffled nodes
    *head = shuffledList;
}

void dealCards(node **deck, node **C1, node **C2, node **C3, node **C4, node **C5, node **C6, node **C7)
{
    int i, j;
    int pileSizes[] = {1, 6, 7, 8, 9, 10, 11};
    int pileIndex = 0;
    int cardsDealt = 0;

    for (i = 0; i < 7; i++)
    {
        for (j = 0; j < pileSizes[i]; j++)
        {
            node *node = removeFirstNode(deck);
            switch (pileSizes[i])
            {
            case 1:
                node->card->revealed = true;
                insertNode(C1, node->card);
                break;
            case 6:
                if (j > 0)
                {
                    node->card->revealed = true;
                }
                insertNode(C2, node->card);
                break;
            case 7:
                if (j > 1)
                {
                    node->card->revealed = true;
                }
                insertNode(C3, node->card);
                break;
            case 8:
                if (j > 2)
                {
                    node->card->revealed = true;
                }
                insertNode(C4, node->card);
                break;
            case 9:
                if (j > 3)
                {
                    node->card->revealed = true;
                }
                insertNode(C5, node->card);
                break;

            case 10:
                if (j > 4)
                {
                    node->card->revealed = true;
                }
                insertNode(C6, node->card);
                break;
            case 11:
                if (j > 5)
                {
                    node->card->revealed = true;
                }
                insertNode(C7, node->card);
                break;

            default:
                break;
            }
            cardsDealt++;
        }
    }
}

void returnCardsToDeck(node **deck, node **C1, node **C2, node **C3, node **C4, node **C5, node **C6, node **C7, node **F1, node **F2, node **F3, node **F4)
{
    // Iterate through each column and insert the cards back into the deck
    insertListBack(deck, *C1);
    *C1 = NULL;

    insertListBack(deck, *C2);
    *C2 = NULL;

    insertListBack(deck, *C3);
    *C3 = NULL;

    insertListBack(deck, *C4);
    *C4 = NULL;

    insertListBack(deck, *C5);
    *C5 = NULL;

    insertListBack(deck, *C6);
    *C6 = NULL;

    insertListBack(deck, *C7);
    *C7 = NULL;

    insertListBack(deck, *F1);
    *F1 = NULL;

    insertListBack(deck, *F2);
    *F2 = NULL;

    insertListBack(deck, *F3);
    *F3 = NULL;

    insertListBack(deck, *F4);
    *F4 = NULL;
}

void insertListBack(node **head, node *newNode)
{
    // If the list is empty, make the new node the head
    if (*head == NULL)
    {
        *head = newNode;
        return;
    }

    if (newNode == NULL)
    {
        return;
    }

    // Traverse to the end of the list
    node *currentNode = *head;
    while (currentNode->next != NULL)
    {
        currentNode = currentNode->next;
    }

    // Insert the new node at the end of the list
    currentNode->next = newNode;
    newNode->prev = currentNode;
}

node *RemoveCards(node **c1, node **c2, node **c3, node **c4, node **c5, node **c6, node **c7, cardLocation cardLocation, const char *cardString)
{

    node *foundNode = cardLocation.foundNode;

    if (foundNode != NULL)
    {
        // Save the head of the list after the found node
        node *nextNode = foundNode->next;

        // Remove the found node and everything after it
        if (foundNode->prev == NULL)
        {
            // The found node is the head of the list
            if (nextNode != NULL)
            {
                // Set the head of the list to the next node
                nextNode->prev = NULL;
            }

            // Set the list pointer to the head of the list after the found node
            switch (cardLocation.location)
            {
            case '1':
                *c1 = NULL;
                break;
            case '2':
                *c2 = NULL;
                break;
            case '3':
                *c3 = NULL;
                break;
            case '4':
                *c4 = NULL;
                break;
            case '5':
                *c5 = NULL;
                break;
            case '6':
                *c6 = NULL;
                break;
            case '7':
                *c7 = NULL;
                break;
            }
        }
        else
        {
            // The found node is not the head of the list
            foundNode->prev->next = NULL;
            foundNode->prev = NULL;
        }

        return foundNode;
    }
    else
    {
        return NULL;
    }
}

bool isMoveLegalToFoundation(node **c1, node **c2, node **c3, node **c4, node **c5, node **c6, node **c7, node **f1, node **f2, node **f3, node **f4, const char *cardString, const char *destString)
{

    cardLocation mCardLocation = findCard(cardString, *c1, *c2, *c3, *c4, *c5, *c6, *c7);
    node *node = mCardLocation.foundNode;

    if (node->next == NULL)
    {
        switch (destString[1])
        {
            {
            case '1':
                if (*f1 == NULL)
                {
                    if (node->card->rank == 'A')
                    {
                        return true;
                    }
                    else
                    {
                      
                        return false;
                    }
                }
                else
                {
                    if (node->card->suit == (*f1)->card->suit && node->card->rankValue == getLastNode(*f1)->card->rankValue + 1)
                    {
                        return true;
                    }else{
                 
                        return false;
                    }
                }
                break;
            case '2':
                if (*f2 == NULL)
                {
                    if (node->card->rank == 'A')
                    {
                        return true;
                    }
                    else
                    {
       
                        return false;
                    }
                }
                else
                {
                    if (node->card->suit == (*f2)->card->suit && node->card->rankValue == getLastNode(*f2)->card->rankValue + 1)
                    {
                        return true;
                    }else{

                        return false;
                    }
                }
                break;
            case '3':
                if (*f3 == NULL)
                {
                    if (node->card->rank == 'A')
                    {
                        return true;
                    }
                    else
                    {
                        return false;
                    }
                }
                else
                {
                    if (node->card->suit == (*f3)->card->suit && node->card->rankValue == getLastNode(*f3)->card->rankValue + 1)
                    {
                        return true;
                    }else{

                        return false;
                    }
                }
                break;
            case '4':
                if (*f4 == NULL)
                {
                    if (node->card->rank == 'A')
                    {
                        return true;
                    }
                    else
                    {

                        return false;
                    }
                }
                else
                {
                    if (node->card->suit == (*f4)->card->suit && node->card->rankValue == getLastNode(*f4)->card->rankValue + 1)
                    {
                        return true;
                    }else{

                        return false;
                    }
                }
                break;
            default:
                break;
            }
        }
    }
    else
    {
        return false;
    }
    return false;
}

bool isMoveLegalToTableau(node **c1, node **c2, node **c3, node **c4, node **c5, node **c6, node **c7, node **f1, node **f2, node **f3, node **f4, const char *cardString, const char *destString)
{

    cardLocation mCardLocation = findCard(cardString, *c1, *c2, *c3, *c4, *c5, *c6, *c7);
    node *mNode = mCardLocation.foundNode;
    node *destTail = NULL;

    switch (destString[1])
    {
        case '1':
            destTail = getLastNode(*c1);
            break;
        case '2':
            destTail = getLastNode(*c2);
            break;
        case '3': 
            destTail = getLastNode(*c3);
            break;
        case '4':
            destTail = getLastNode(*c4);
            break;
        case '5':
            destTail = getLastNode(*c5);
            break;
        case '6':
            destTail = getLastNode(*c6);
            break;
        case '7':
            destTail = getLastNode(*c7);
            break;
        default:
            break;
    }

    if(destTail == NULL)
    {
        if(mNode->card->rank == 'K')
        {
            return true;
        }
        else
        {
            return false;
        }
    }
    if (mNode->card->rankValue == destTail->card->rankValue - 1 && mNode->card->suit != destTail->card->suit)
    {
        return true;
    }
    else
    {
        return false;
    }
    
}

void moveCard(node **c1, node **c2, node **c3, node **c4, node **c5, node **c6, node **c7, node **f1, node **f2, node **f3, node **f4, const char *cardString, const char *destString)
{

    cardLocation mCardLocation = findCard(cardString, *c1, *c2, *c3, *c4, *c5, *c6, *c7);

    // Call findAndRemoveCard() to search for the card
    node *foundNode = RemoveCards(c1, c2, c3, c4, c5, c6, c7, mCardLocation, cardString);
  //  printf("\nfoundNode: %c%c\n", foundNode->card->rank, foundNode->card->suit);
    node *destNode = NULL;

    if (foundNode != NULL)
    {
        if (toupper(destString[0]) == 'C')
        {
            switch (destString[1])
            {
            case '1':
                destNode = *c1;
                break;
            case '2':
                destNode = *c2;
                break;
            case '3':
                destNode = *c3;
                break;
            case '4':
                destNode = *c4;
                break;
            case '5':
                destNode = *c5;
                break;
            case '6':
                destNode = *c6;
                break;
            case '7':
                destNode = *c7;
                break;
            default:
                return; // This should not happen, but just in case
            }
        }
        else if (toupper(destString[0]) == 'F')
        {
            switch (destString[1])
            {
            case '1':
                destNode = *f1;
                break;
            case '2':
                destNode = *f2;
                break;
            case '3':
                destNode = *f3;
                break;
            case '4':
                destNode = *f4;
                break;
            default:
                return; // This should not happen, but just in case
            }
        }
        else
        {
            printf("\nInvalid destination\n");
            return;
        }

        // Insert the found node at the destination
        if (destNode == NULL)
        {
            // The destination list is empty
            if (toupper(destString[0]) == 'C')
            {
                switch (destString[1])
                {
                case '1':
                    *c1 = foundNode;
                    break;
                case '2':
                    *c2 = foundNode;
                    break;
                case '3':
                    *c3 = foundNode;
                    break;
                case '4':
                    *c4 = foundNode;
                    break;
                case '5':
                    *c5 = foundNode;
                    break;
                case '6':
                    *c6 = foundNode;
                    break;
                case '7':
                    *c7 = foundNode;
                    break;
                default:
                    return; // This should not happen, but just in case
                }
            }
            else if (toupper(destString[0]) == 'F')
            {
                switch (destString[1])
                {
                case '1':
                    *f1 = foundNode;
                    break;
                case '2':
                    *f2 = foundNode;
                    break;
                case '3':
                    *f3 = foundNode;
                    break;
                case '4':
                    *f4 = foundNode;
                    break;
                default:
                    return; // This should not happen, but just in case
                }

                printf("\nCard moved\n");
            }
        }
        else
        {
            // Add the found node to the destination list
            node *t = destNode;
            while (t->next != NULL)
            {
                t = t->next;
            }
            t->next = foundNode;
            foundNode->prev = t;

        }
    }
    else
    {
        printf("\nCard not found\n");
    }
}

cardLocation findCard(const char *cardString, node *C1, node *C2, node *C3, node *C4, node *C5, node *C6, node *C7)
{
    node *temp;

    // Loop through all columns
    for (int i = 1; i <= 7; i++)
    {
        switch (i)
        {
        case 1:
            temp = C1;
            break;
        case 2:
            temp = C2;
            break;
        case 3:
            temp = C3;
            break;
        case 4:
            temp = C4;
            break;
        case 5:
            temp = C5;
            break;
        case 6:
            temp = C6;
            break;
        case 7:
            temp = C7;
            break;
        default:
            return (cardLocation){NULL, '\0'};
        }
        // Loop through nodes in column
        while (temp != NULL)
        {
            // Check if card matches
            if (temp->card->suit == toupper(cardString[1]) && temp->card->rank == toupper(cardString[0]))
            {
                // printf("temp is found and is %c%c", temp->card->rank, temp->card->suit); // debugging
                return (cardLocation){temp, i + '0'};                                    // Return the found node and the location of the card
            }
            temp = temp->next;
        }
    }
    // Card not found
    return (cardLocation){NULL, '\0'};
}

void extractStrings(char *left, char *right)
{
    char buffer[256];
    if (fscanf(stdin, "%255s", buffer) == 1)
    {                                       // read up to 255 characters into buffer
        char *arrow = strstr(buffer, "->"); // find the arrow in the buffer
        if (arrow != NULL)
        {
            *arrow = '\0';            // terminate the left string at the arrow
            strcpy(left, buffer);     // copy the left string
            strcpy(right, arrow + 2); // copy the right string
        }
    }
}

const char *parseCard(const struct node *cardNode)
{
    char *cardString = malloc(3);

    if (cardNode == NULL || cardNode->card->revealed == false)
    {
        cardString[0] = '[';
        cardString[1] = ']';
        cardString[2] = '\0';
    }
    else
    {
        cardString[0] = cardNode->card->rank;
        cardString[1] = cardNode->card->suit;
        cardString[2] = '\0';
    }

    return cardString;
}

void printBoard(node **C1, node **C2, node **C3, node **C4, node **C5, node **C6, node **C7, node **F1, node **F2, node **F3, node **F4)
{

    const char *F1Last = parseCard(getLastNode(*F1));
    const char *F2Last = parseCard(getLastNode(*F2));
    const char *F3Last = parseCard(getLastNode(*F3));
    const char *F4Last = parseCard(getLastNode(*F4));

    printf("F1\tF2\tF3\tF4\n");
    printf("%s\t%s\t%s\t%s", F1Last, F2Last, F3Last, F4Last);
    printf("\n");
    printf("------------------------------------------------------------\n");

    node *columns[7] = {*C1, *C2, *C3, *C4, *C5, *C6, *C7};
    char *column_names[7] = {"C1", "C2", "C3", "C4", "C5", "C6", "C7"};
    int max_len = 0;
    for (int i = 0; i < 7; i++)
    {
        int len = countNodes(columns[i]);
        if (len > max_len)
        {
            max_len = len;
        }
    }

    // Print column names
    for (int i = 0; i < 7; i++)
    {
        printf("%s\t", column_names[i]);
    }
    printf("\n");
    printf("\n");

    // Print nodes in each column
    for (int i = 0; i < max_len; i++)
    {
        for (int j = 0; j < 7; j++)
        {
            if (i < countNodes(columns[j]))
            {
                printNodeBoard(&columns[j], i);
                printf("\t");
            }
            else
            {
                printf("\t");
            }
        }
        printf("\n");
    }
}

void printNodeBoard(node **column, int i)
{
    if (*column == NULL)
    {
        printf("[]");
    }
    else
    {
        node *curr = *column;
        for (int j = 0; j < i; j++)
        {
            if (curr == NULL)
            {
                printf("[]");
                return;
            }
            curr = curr->next;
        }
        if (curr == NULL || curr->card == NULL)
        {
            printf("[]");
        }
        else
        {
            if (curr->card->revealed == false)
            {
                printf("[]");
            }
            else
            {
                printf("%c%c", curr->card->rank, curr->card->suit);
            }
        }
    }
}
