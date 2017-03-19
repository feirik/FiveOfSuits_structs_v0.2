//  Five of Suits v0.2
//
//  Console card game, player vs. computer
//
//  The goal of the game is to end your turn with five cards of the same suit.
//
//  Each player is dealt five cards at the start of the game.
//  During a player's turn, the player can decide to either draw a random card from the deck
//  or draw a known card from top of the pile.
//
//  After drawing a 6th card from the deck or the pile, the player has choose
//  which of the six cards in hand to discard, ending their turn with five cards.
//  After each turn, the hand is checked to see whether a winner has been decided.
//
//  Hint: Keep a close eye on what your opponent discards and draws from the pile.
//  You might be able to guess what suit your opponent collects and play accordingly!
//
//  -----------
//
//  New in v0.2
//
//  Redone the program with structs to simplify access of variables outside of functions
//  Implemented fgets instead scanf, making console inputs less prone to crashing
//  Updated AI. It now sees and acts on the same information as a human player, instead of cheating
//  Added replay functionality
//  Added a score tally after each game, keeping track of total wins

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define DECKSIZE 52
#define HANDSIZE 6
#define MAXSTRINGSIZE 20
#define MAXNAMESIZE 64

typedef struct
{
    char suit[MAXSTRINGSIZE];
    char cardName[MAXSTRINGSIZE];
    int cardValue;
}card;

typedef struct
{
    char name[MAXNAMESIZE];
    card hand[HANDSIZE];
    int winnerFlag;
    int playerDrewInFlag;
    int winCount;
    
    char playerDrewInSuit[MAXSTRINGSIZE];
}humanPlayer;

typedef struct
{
    char name[MAXNAMESIZE];
    card hand[HANDSIZE];
    int winnerFlag;
    int winCount;
    
    char drewFromPileName[MAXSTRINGSIZE];
    char drewFromPileSuit[MAXSTRINGSIZE];
    char suitToCollect[MAXSTRINGSIZE];
    
    int guessHearts;
    int guessDiamonds;
    int guessClubs;
    int guessSpades;
}computerPlayer;

// Input of a humanPlayer struct address
// Prompts user for a name, updates the name variable in struct
void getName(humanPlayer* input)
{
    printf("Welcome to Five of Suits!\n\n");
    char nameInput[MAXNAMESIZE];
    printf("Please enter your nickname and press ENTER: ");
    fgets(nameInput, sizeof(nameInput), stdin);
    
    // Fix to replace trailing \n with \0 from fgets
    char *pos;
    if ((pos=strchr(nameInput, '\n')) != NULL)
    {
        *pos = '\0';
    }
    
    strcpy(input->name, nameInput);
}

// Input of a humanPlayer struct address
// Initializes all variables other than winCount
void initHumanPlayer(humanPlayer* player)
{
    for(int i = 0; i < HANDSIZE; i++)
    {
        strcpy(player->hand[i].cardName,"empty");
        strcpy(player->hand[i].suit,"empty");
        player->hand[i].cardValue = 0;
    }
    
    //strcpy(player->name,"Placeholder");
    strcpy(player->playerDrewInSuit,"empty");
    
    player->winnerFlag = 0;
    player->playerDrewInFlag = 0;
}

// Input of a computerPlayer struct address
// Initializes all variables other than winCount
void initComputerPlayer(computerPlayer* player)
{
    strcpy(player->name,"Chuck Borris");
    
    for(int i = 0; i < HANDSIZE; i++)
    {
        strcpy(player->hand[i].cardName,"empty");
        strcpy(player->hand[i].suit,"empty");
        player->hand[i].cardValue = 0;
    }
    
    player->winnerFlag = 0;
    
    strcpy(player->drewFromPileName,"empty");
    strcpy(player->drewFromPileSuit,"empty");
    strcpy(player->suitToCollect,"empty");
    
    player->guessHearts = 1;
    player->guessDiamonds = 1;
    player->guessClubs = 1;
    player->guessSpades = 1;
}

// Inputs of humanPlayer and computerPlayer struct addresses
// Initializes win counts to 0
void initWinCounters(humanPlayer* player, computerPlayer* computer)
{
    player->winCount = 0;
    computer->winCount = 0;
}

// Input of a card struct array, needs to be of size 52
// Populates cardName and suit with strings similar to a 52 card deck
void populateDeck(card deck[DECKSIZE])
{
    char cardTemplate[13][MAXSTRINGSIZE] =
    {"two","three","four","five","six","seven","eight","nine","ten",
        "jack","queen","king","ace"};
    
    int j = 0;
    
    for(int i = 0; i < DECKSIZE; i++)
    {
        //Card names
        strcpy(deck[i].cardName,cardTemplate[j]);
        j++;
        if(j == 13)
        {
            j = 0;
        }
        //Card suits
        if(i <= 12)
        {
            strcpy(deck[i].suit,"Hearts");
        }
        if(13 <= i && i <= 25)
        {
            strcpy(deck[i].suit,"Diamonds");
        }
        if(26 <= i && i <= 38)
        {
            strcpy(deck[i].suit,"Clubs");
        }
        if(39 <= i && i <= 51)
        {
            strcpy(deck[i].suit,"Spades");
        }
    }
}

// Input of a card struct array
// Returns the array number of the top element
int topDeckArrayNumber(card deck[])
{
    int deckSize = 0;
    int i = 0;
    while(strcmp(deck[i].cardName,"empty"))
    {
        deckSize++;
        i++;
    }
    //Returning top deck array number one element before "empty"
    return deckSize - 1;
}

// Input of a card struct array
// Returns the first array number that consisting of "empty"
// Prints an error if no number found within pile/deck size
int firstEmptyArrayNumber(card handPileOrDeck[])
{
    int i = 0;
    int arrayCount = 0;
    while(strcmp(handPileOrDeck[i].cardName,"empty"))
    {
        arrayCount++;
        i++;
        if(arrayCount > 51)
        {
            printf("\nError, no available empty elements in pile!\n");
            break;
        }
    }
    return arrayCount;
}

// Input of struct card hand[6] and deck[52], integer number of cards
// Moves 'numberOfCards' elements from top of deck to hand and replaces the deck elements with "empty".
//
// This is a seperate function for dealing the first cards
// because no empty slots in the deck array has been assigned to "empty" yet.
void dealFirstCards(card hand[],card deck[],int numberOfCards)
{
    printf("Dealing first %i cards...\n\n",numberOfCards);
    for(int i = 0; i < numberOfCards; i++)
    {
        //Copying card name, replacing slots with "empty"
        strcpy(hand[i].cardName, deck[(DECKSIZE-1)-i].cardName);
        strcpy(deck[(DECKSIZE-1)-i].cardName, "empty");
        
        //Copying suit, replacing slots with "empty"
        strcpy(hand[i].suit, deck[(DECKSIZE-1)-i].suit);
        strcpy(deck[(DECKSIZE-1)-i].suit, "empty");
    }
}

// Input of struct card hand[6] and deck[52], integer number of cards
// Moves 'numberOfCards' elements from top of deck to hand and replaces the deck elements with "empty".
void dealCards(card hand[],card deck[],int numberOfCards)
{
    printf("Dealing %i cards...\n\n",numberOfCards);
    
    int topOfDeckElement = topDeckArrayNumber(deck);
    
    for(int i = 0; i < numberOfCards; i++)
    {
        //Copying card name, replacing slots with "empty"
        strcpy(hand[i].cardName, deck[topOfDeckElement-i].cardName);
        strcpy(deck[topOfDeckElement-i].cardName, "empty");
        
        //Copying suit, replacing slots with "empty"
        strcpy(hand[i].suit, deck[topOfDeckElement-i].suit);
        strcpy(deck[topOfDeckElement-i].suit, "empty");
    }
}

// Input of struct card hand[6] and deck[52]
// Moves the top elements in deck to empty slot in hand and replaces the deck elements with "empty".
void drawCard(card hand[], card deck[])
{
    int emptySlot = firstEmptyArrayNumber(hand);
    int topOfDeck = topDeckArrayNumber(deck);
    
    strcpy(hand[emptySlot].cardName, deck[topOfDeck].cardName);
    strcpy(deck[topOfDeck].cardName,"empty");
    
    strcpy(hand[emptySlot].suit, deck[topOfDeck].suit);
    strcpy(deck[topOfDeck].suit,"empty");
}

// Input of card struct deck[52] and an unsigned integer
// Shuffles the card elements randomly.
//
// Increase integer in rand()%6 to shuffle more thoroughly
// Foundations of the function found on stack overflow
void shuffleDeck(card deck[], size_t deckSize)
{
    printf("Shuffling...\n\n");
    srand((unsigned)time(NULL));
    int firstFlag = 0;
    if(deckSize > 1)
    {
        //Shuffling a random number of times, atleast 1
        while(rand()%6 > 1 || !firstFlag)
        {
            firstFlag = 1;
            size_t i;
            for(i = 0; i < deckSize-1; i++)
            {
                size_t j = i + rand() / (RAND_MAX / (deckSize - i) + 1);
                char tempName[MAXSTRINGSIZE];
                char tempSuit[MAXSTRINGSIZE];
                
                //Bug fix, otherwise crash in strcpy if i == j
                if(i != j)
                {
                    strcpy(tempName, deck[j].cardName);
                    strcpy(deck[j].cardName, deck[i].cardName);
                    strcpy(deck[i].cardName, tempName);
                    
                    strcpy(tempSuit, deck[j].suit);
                    strcpy(deck[j].suit, deck[i].suit);
                    strcpy(deck[i].suit, tempSuit);
                }
            }
        }
    }
}

// Input of a struct card pile[]
// Initializes variables cardName and suit to "empty"
void initEmptyPile(card pile[])
{
    for(int i = 0; i < DECKSIZE; i++)
    {
        strcpy(pile[i].cardName,"empty");
        strcpy(pile[i].suit,"empty");
        pile[i].cardValue = 0;
    }
}

// Input of a card struct deck[52] or pile[52]
// Prints an index number and the name of each card in the deck/pile
void printDeck(card deck[])
{
    printf("\nPrinting deck...\n");
    for(int i = 0; i < DECKSIZE; i++)
    {
        printf("Number: %2i %sOf%s\n",i+1,deck[i].cardName,deck[i].suit);
    }
}

// Input of a card struct hand[6]
// Prints an index number and the name of each card in the hand
void printHand(card hand[])
{
    printf("\nPrinting hand...\n");
    for(int i = 0; i < HANDSIZE; i++)
    {
        printf("Number: %2i %sOf%s\n",i+1,hand[i].cardName,hand[i].suit);
    }
}

// Input of a humanPlayer struct address, card struct pile[52] and deck[52]
// Asks user to either draw a random card from deck or a known card from top of the pile,
// then exectues the command
// In the first round, a card is drawn automatically as pile is empty
void drawFromPileOrDeck(humanPlayer* player1, card pile[], card deck[])
{
    int numberDecision = 0;
    
    int emptyLocationHand = firstEmptyArrayNumber(player1->hand);
    int emptyLocationPile = firstEmptyArrayNumber(pile);
    
    if(emptyLocationPile == 0)
    {
        printf("The goal of the game is to end your turn with five cards of the same suit.\n");
        printf("During a turn you need to make two decisions:\n");
        printf("1) Draw in a known card from top of pile, or a random card from the deck.\n");
        printf("2) Pick which card to discard from hand, ending your turn with five cards.\n");
        printf("Hint: Keep a close eye on what your opponent draws from the pile and discards!\n\n");
        
        printf("First round!\nYou drew a random card from the deck as the pile was empty.\n");
        drawCard(player1->hand, deck);
    }
    else
    {
        printf("\nYour hand is:\n");
        for(int i = 0; i < HANDSIZE; i++)
        {
            printf("(%i) %sOf%s\n",i+1,player1->hand[i].cardName, player1->hand[i].suit);
        }
        printf("-----------------------------------------\n");
        
        char *end;
        char buffer[10];
        
        while(numberDecision > 2 || numberDecision < 1)
        {
            printf("Do you want to:\n");
            printf("[1]Draw %sOf%s from the top of pile.\n",
                   pile[emptyLocationPile-1].cardName,pile[emptyLocationPile-1].suit);
            
            printf("[2]Draw a random card from the deck.\n");
            printf("Please input your choice and press ENTER: ");
            
            do
            {
                if (!fgets(buffer, sizeof buffer, stdin))
                    break;
                
                buffer[strlen(buffer) - 1] = 0;
                
                numberDecision = strtol(buffer, &end, 10);
            } while (end != buffer + strlen(buffer));
        }
        //Draw known card from top of pile
        if(numberDecision == 1)
        {
            player1->playerDrewInFlag = 1;
            strcpy(player1->playerDrewInSuit, pile[emptyLocationPile-1].suit);
            
            strcpy(player1->hand[emptyLocationHand].cardName, pile[emptyLocationPile-1].cardName);
            strcpy(pile[emptyLocationPile-1].cardName,"empty");
            
            strcpy(player1->hand[emptyLocationHand].suit, pile[emptyLocationPile-1].suit);
            strcpy(pile[emptyLocationPile-1].suit,"empty");
        }
        //Draw random card from deck
        if(numberDecision == 2)
        {
            drawCard(player1->hand, deck);
        }
    }
}

// Input of a humanPlayer struct address and card struct pile[52]
// Prompts user to discard one of the 6 cards in hand to pile
void discardCardToPile(humanPlayer* player1, card pile[])
{
    int emptyLocationPile = firstEmptyArrayNumber(pile);
    int numberDecision = 0;
    
    char *end;
    char buffer[10];
    
    while(1 > numberDecision || numberDecision > 6)
    {
        printf("\nDo you want to discard:\n");
        for(int i = 0; i < HANDSIZE; i++)
        {
            printf("[%i] %sOf%s\n", i+1, player1->hand[i].cardName, player1->hand[i].suit);
        }
        printf("Please input your choice and press ENTER: ");
        
        do
        {
            if (!fgets(buffer, sizeof buffer, stdin))
                break;
            
            buffer[strlen(buffer) - 1] = 0;
            
            numberDecision = strtol(buffer, &end, 10);
        } while (end != buffer + strlen(buffer));
    }
    for(int i = 0; i < HANDSIZE; i++)
    {
        if(i == (numberDecision - 1))
        {
            strcpy(pile[emptyLocationPile].cardName,player1->hand[i].cardName);
            strcpy(player1->hand[i].cardName,"empty");
            
            strcpy(pile[emptyLocationPile].suit,player1->hand[i].suit);
            strcpy(player1->hand[i].suit,"empty");
        }
    }
}

// Input of a card struct hand[6]
// Iterates through the suits of cards in hand
// Returns an integer of 1 if there are five or more cards of the same suit, 0 otherwise
int checkFiveSameSuit(card hand[])
{
    int returnValue = 0;
    
    int heartsCount = 0;
    int diamondsCount = 0;
    int clubsCount = 0;
    int spadesCount = 0;
    
    //Comparing cards in hand with suits
    for(int i = 0; i < HANDSIZE; i++)
    {
        if(strstr(hand[i].suit,"Hearts"))
        {
            heartsCount++;
        }
        if(strstr(hand[i].suit,"Diamonds"))
        {
            diamondsCount++;
        }
        if(strstr(hand[i].suit,"Clubs"))
        {
            clubsCount++;
        }
        if(strstr(hand[i].suit,"Spades"))
        {
            spadesCount++;
        }
    }
    //printf("\nHearts: %i, Diamonds: %i, Clubs: %i, Spades: %i\n",heartsCount,diamondsCount,clubsCount,spadesCount);
    
    if((heartsCount > 4) || (diamondsCount > 4) || (clubsCount > 4) || (spadesCount > 4))
    {
        returnValue = 1;
    }
    
    return returnValue;
}

// Input of a computerPlayer struct address and card struct pile[52]
// Counts the number of cards in each suit, adding in the card avaiable to be drawn from pile
// Updates suitToCollect with the best choice
// Is biased towards collecting hearts > diamonds > clubs > spades if count is the same
void pickSuitToCollectAI(computerPlayer* computer, card pile[])
{
    int heartsCount = 0;
    int diamondsCount = 0;
    int clubsCount = 0;
    int spadesCount = 0;
    
    char suitTopOfPile[MAXSTRINGSIZE];
    
    strcpy(suitTopOfPile,pile[topDeckArrayNumber(pile)].suit);
    
    for(int i = 0; i < HANDSIZE; i++)
    {
        if(strstr(computer->hand[i].suit,"Hearts"))
        {
            heartsCount++;
        }
        if(strstr(computer->hand[i].suit,"Diamonds"))
        {
            diamondsCount++;
        }
        if(strstr(computer->hand[i].suit,"Clubs"))
        {
            clubsCount++;
        }
        if(strstr(computer->hand[i].suit,"Spades"))
        {
            spadesCount++;
        }
    }
    if(strstr(suitTopOfPile,"Hearts"))
    {
        heartsCount++;
    }
    if(strstr(suitTopOfPile,"Diamonds"))
    {
        diamondsCount++;
    }
    if(strstr(suitTopOfPile,"Clubs"))
    {
        clubsCount++;
    }
    if(strstr(suitTopOfPile,"Spades"))
    {
        spadesCount++;
    }
    
    //Updating what suit to collect
    if(heartsCount > 0)
    {
        strcpy(computer->suitToCollect,"Hearts");
    }
    if(diamondsCount > heartsCount)
    {
        strcpy(computer->suitToCollect,"Diamonds");
    }
    if((clubsCount > diamondsCount)&&(clubsCount > heartsCount))
    {
        strcpy(computer->suitToCollect,"Clubs");
    }
    if((spadesCount > clubsCount)&&(spadesCount > diamondsCount)&&(spadesCount > heartsCount))
    {
        strcpy(computer->suitToCollect,"Spades");
    }
}

// Input of a computerPlayer struct adress, humanPlayer struct, card struct pile[52]
// Updates guesses of what suit player is collecting
// Prioritizes to avoid discarding a suit player has previously drawn in
// Then tries to discard a suit that player has previously discarded
// If none of the above conditions are met, discards a random card that computer doesn't collect
void discardCardToPileAI(computerPlayer* computer, humanPlayer player, card pile[])
{
    int firstEmptyInPile = firstEmptyArrayNumber(pile);
    char playerSuit[MAXSTRINGSIZE];
    
    int innerBreak = 0;
    int breakFlag = 0;
    
    //Updating guesses of what player is collecting by looking at player's last discard to pile
    //All guesses are initialized at 1
    if(strstr(pile[firstEmptyInPile-1].suit,"Hearts"))
    {
        computer->guessHearts = 0;
    }
    if(strstr(pile[firstEmptyInPile-1].suit,"Diamonds"))
    {
        computer->guessDiamonds = 0;
    }
    if(strstr(pile[firstEmptyInPile-1].suit,"Clubs"))
    {
        computer->guessClubs = 0;
    }
    if(strstr(pile[firstEmptyInPile-1].suit,"Spades"))
    {
        computer->guessSpades = 0;
    }
    
    //Player has drawn a suit from the pile, prioritized
    if(player.playerDrewInFlag == 1)
    {
        strcpy(playerSuit, player.playerDrewInSuit);
        
        for(int i = 0; i < HANDSIZE+1; i++)
        {
            //Looking to discard not what player drew in or what computer collects
            if(!(strstr(computer->hand[i].suit,playerSuit)) && !(strstr(computer->hand[i].suit,computer->suitToCollect)))
            {
                //Flag: End of first loop with two conditions
                if(i == HANDSIZE)
                {
                    //Searching in second loop, single condition
                    for(int j = 0; j < HANDSIZE; j++)
                    {
                        //Allowing to discard what player drew in
                        if(!(strstr(computer->hand[j].suit,computer->suitToCollect)))
                        {
                            strcpy(pile[firstEmptyInPile].cardName,computer->hand[j].cardName);
                            strcpy(computer->hand[j].cardName,"empty");
                            
                            strcpy(pile[firstEmptyInPile].suit,computer->hand[j].suit);
                            strcpy(computer->hand[j].suit,"empty");
                            
                            innerBreak = 1;
                            break;
                            
                        }
                    }
                }
                //Card has been discarded in inner loop, breaking
                if(innerBreak == 1)
                {
                    break;
                }
                strcpy(pile[firstEmptyInPile].cardName,computer->hand[i].cardName);
                strcpy(computer->hand[i].cardName,"empty");
                
                strcpy(pile[firstEmptyInPile].suit,computer->hand[i].suit);
                strcpy(computer->hand[i].suit,"empty");
                break;
            }
        }
    }
    //Player has not drawn in from pile yet, looking at what player has discarded to pile
    //to find a suit to discard that player does not collect
    else
    {
        if(!computer->guessHearts)
        {
            for(int i = 0; i < HANDSIZE; i++)
            {
                if(strstr(computer->hand[i].suit,"Hearts") && !strstr(computer->hand[i].suit,computer->suitToCollect))
                {
                    strcpy(pile[firstEmptyInPile].cardName,computer->hand[i].cardName);
                    strcpy(computer->hand[i].cardName,"empty");
                    
                    strcpy(pile[firstEmptyInPile].suit,computer->hand[i].suit);
                    strcpy(computer->hand[i].suit,"empty");
                    
                    breakFlag = 1;
                    break;
                }
            }
        }
        if(!computer->guessDiamonds && !breakFlag)
        {
            for(int i = 0; i < HANDSIZE; i++)
            {
                if(strstr(computer->hand[i].suit,"Diamonds") && !strstr(computer->hand[i].suit,computer->suitToCollect))
                {
                    strcpy(pile[firstEmptyInPile].cardName,computer->hand[i].cardName);
                    strcpy(computer->hand[i].cardName,"empty");
                    
                    strcpy(pile[firstEmptyInPile].suit,computer->hand[i].suit);
                    strcpy(computer->hand[i].suit,"empty");
                    
                    breakFlag = 1;
                    break;
                }
            }
        }
        if(!computer->guessClubs && !breakFlag)
        {
            for(int i = 0; i < HANDSIZE; i++)
            {
                if(strstr(computer->hand[i].suit,"Clubs") && !strstr(computer->hand[i].suit,computer->suitToCollect))
                {
                    strcpy(pile[firstEmptyInPile].cardName,computer->hand[i].cardName);
                    strcpy(computer->hand[i].cardName,"empty");
                    
                    strcpy(pile[firstEmptyInPile].suit,computer->hand[i].suit);
                    strcpy(computer->hand[i].suit,"empty");
                    
                    breakFlag = 1;
                    break;
                }
            }
        }
        if(!computer->guessSpades && !breakFlag)
        {
            for(int i = 0; i < HANDSIZE; i++)
            {
                if(strstr(computer->hand[i].suit,"Spades") && !strstr(computer->hand[i].suit,computer->suitToCollect))
                {
                    strcpy(pile[firstEmptyInPile].cardName,computer->hand[i].cardName);
                    strcpy(computer->hand[i].cardName,"empty");
                    
                    strcpy(pile[firstEmptyInPile].suit,computer->hand[i].suit);
                    strcpy(computer->hand[i].suit,"empty");
                    
                    breakFlag = 1;
                    break;
                }
            }
        }
        //If no ideal suit has been found to discard, discard something that computer does not collect
        if(!breakFlag)
        {
            for(int i = 0; i < HANDSIZE; i++)
            {
                if(!strstr(computer->hand[i].suit,computer->suitToCollect))
                {
                    strcpy(pile[firstEmptyInPile].cardName,computer->hand[i].cardName);
                    strcpy(computer->hand[i].cardName,"empty");
                    
                    strcpy(pile[firstEmptyInPile].suit,computer->hand[i].suit);
                    strcpy(computer->hand[i].suit,"empty");
                    
                    break;
                }
            }
        }
    }
}

// Input of computerPlayer struct address, card struct deck[52], card struct deck[52]
// If card at top of pile is what computer collects, draw it
// Else draw a random card from deck
// Prints a message to console to notify player about the choice
void drawFromPileOrDeckAI(computerPlayer* computer, card deck[], card pile[])
{
    int emptyLocationHand = firstEmptyArrayNumber(computer->hand);
    int topOfPile = topDeckArrayNumber(pile);
    
    char suitTopOfPile[MAXSTRINGSIZE];
    strcpy(suitTopOfPile, pile[topOfPile].suit);
    
    if(strstr(suitTopOfPile, computer->suitToCollect))
    {
        printf("\nALERT: %s drew in %sOf%s from pile!\n", computer->name, pile[topOfPile].cardName, pile[topOfPile].suit);
        
        strcpy(computer->hand[emptyLocationHand].cardName, pile[topOfPile].cardName);
        strcpy(pile[topOfPile].cardName,"empty");
        
        strcpy(computer->hand[emptyLocationHand].suit, pile[topOfPile].suit);
        strcpy(pile[topOfPile].suit,"empty");
    }
    else
    {
        printf("\n%s did not draw %sOf%s from pile.\n", computer->name, pile[topOfPile].cardName, pile[topOfPile].suit);
        drawCard(computer->hand, deck);
    }
}

// No input, returns integer 0 or 1
// Prompts player to either play again or exit the program
int playerWantToReplay(void)
{
    int numberDecision = 0;
    int returnValue = 0;
    
    char *end;
    char buffer[10];
    
    while(numberDecision > 2 || numberDecision < 1)
    {
        printf("\nDo you want to:\n");
        printf("[1] Play again\n");
        printf("[2] Exit the game\n");
        printf("Please input your choice and press ENTER: ");
        
        do
        {
            if (!fgets(buffer, sizeof buffer, stdin))
                break;
            
            buffer[strlen(buffer) - 1] = 0;
            
            numberDecision = strtol(buffer, &end, 10);
        } while (end != buffer + strlen(buffer));
    }
    if(numberDecision == 1)
    {
        returnValue = 1;
    }
    if(numberDecision == 2)
    {
        returnValue = 0;
    }
    
    return returnValue;
}

int main()
{
    card deck[DECKSIZE];
    card pile[DECKSIZE];
    
    humanPlayer player1;
    computerPlayer computer2;
    initWinCounters(&player1, &computer2);
    getName(&player1);
    
    int replayFlag = 1;
    while(replayFlag == 1)
    {
        populateDeck(deck);
        shuffleDeck(deck, DECKSIZE);
        initEmptyPile(pile);
        
        initHumanPlayer(&player1);
        initComputerPlayer(&computer2);
        
        dealFirstCards(player1.hand,deck,5);
        dealCards(computer2.hand,deck,5);
        
        int winFlag = 0;
        while(winFlag == 0)
        {
            drawFromPileOrDeck(&player1, pile, deck);
            discardCardToPile(&player1, pile);
            
            winFlag = checkFiveSameSuit(player1.hand);
            if(winFlag != 0)
            {
                player1.winCount++;
                printf("\nCongratulations %s, you won!",player1.name);
                printf(" Score is %i - %i\n", player1.winCount, computer2.winCount);
                break;
            }
            
            pickSuitToCollectAI(&computer2, pile);
            drawFromPileOrDeckAI(&computer2, deck, pile);
            discardCardToPileAI(&computer2, player1, pile);
            
            winFlag = checkFiveSameSuit(computer2.hand);
            if(winFlag != 0)
            {
                computer2.winCount++;
                printf("\nOh noes! %s was first to a full hand of %s! ",computer2.name, computer2.suitToCollect);
                printf(" Score is %i - %i\n", player1.winCount, computer2.winCount);
                break;
            }
        }
        replayFlag = playerWantToReplay();
    }
}