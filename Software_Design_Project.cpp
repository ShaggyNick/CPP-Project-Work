#include "FEHLCD.h"
#include "FEHRandom.h"
#include "FEHUtility.h"
 //prototypes
void Menu();
void PlayGame();
void Credits();
void Stats();
void Instructions();
void DrawPenguins();
void makeCards();
void KillPenguin(int a);
//playerWins and computerWins are for the ROUNDS
int playerWins = 0;
int computerWins = 0;
//These int numbers only go up when the computer has won 5 rounds or the player has won 5 rounds
int totalWins = 0;
int totalLosses = 0;
using namespace std;
/*
 * Entry point to the application
 */
int main() {
    //Window size: 320 – x, 240 – y
    Menu(); //Call the Menu function, the first thing that will be seen
    while (1) {
        LCD.Update();
        // Never end
    }
    return 0;
}
//The Main Menu
void Menu(){
    playerWins = 0;
    computerWins = 0;
    LCD.SetBackgroundColor(BLACK);
    LCD.Clear();
    LCD.SetFontColor(WHITESMOKE);
    LCD.WriteAt("Welcome To Club Penguin!", 25, 25);
    LCD.WriteAt("Menu", 140, 70);
    LCD.DrawRectangle(30,100,120,50); //First box: play game
    LCD.DrawRectangle(170, 100, 120, 50); //2nd box: instructions
    LCD.DrawRectangle(30,175,120,50); //3rd box: stats
    LCD.DrawRectangle(170, 175, 120, 50); //4th box: credits
    LCD.WriteAt("PLAY!",65,120);
    LCD.WriteAt("How2Play", 180, 120);
    LCD.WriteAt("Stats", 60, 195);
    LCD.WriteAt("Credits", 190, 195);
    float xPos, yPos;
    while (1) { //never end
        LCD.Update();
        while(!LCD.Touch(&xPos, &yPos)){};
        while(LCD.Touch(&xPos, &yPos)){};
        if(xPos >= 30 && xPos <= 150 && yPos >= 100 && yPos <= 150) { //the Play Game box
            PlayGame();
        }
        else if(xPos >= 170 && xPos <= 290 && yPos >= 100 && yPos <= 150) { //the Instructions box
            Instructions();
        }
        else if (xPos >= 30 && xPos <= 150 && yPos >= 175 && yPos <= 295) { //the Stats box
            Stats();
        }
        else if (xPos >= 170 && xPos <= 290 && yPos >= 175 && yPos <= 295) { //the Credits box
            Credits();
        }
    }
}
void PlayGame(){        //the logic behind the game
    LCD.SetBackgroundColor(DIMGRAY);
    LCD.Clear();
    float x, y;
    LCD.SetFontColor(WHITESMOKE); //create the menu box as well as the overall border
    LCD.DrawRectangle(0,0,50,20);
    LCD.WriteAt("Menu",0,0);
    LCD.DrawRectangle(10,40,300,138);
    DrawPenguins(); //this function creates 2 penguins
    makeCards(); //this function creates 10 Card objects and then assigns random elements and numbers to each
     while (1) { //never end
        LCD.Update();
        while(!LCD.Touch(&x, &y)){};
        while(LCD.Touch(&x, &y)){};
        if(x > 0 && x < 50 && y > 0 && y < 20){ //if the player goes back to menu
            Menu();
        }
    }
}
//define the class card
class Card {
    public:
        Card(int x1 = 0, int x2 = 0);   //constructor for the x and y coordinates
        //prototypes
        void DisplayCards();        //the blue or red filled rectangles
        void DragCards();           //the function controlling movement of the card
        void CardInfo();            //the letter corresponding to element and number shown through this function
        void MakeRandom();      //assigns random element and number to the card
        int getX();
        int getY();
        char getElement();
        int getNumber();
    private:
        char element;               //element stored in a single letter
        int number;                 //number corresponding to the "level" of each card
        int x;                      //x-coordinate of the card
        int y;                      //y-coordinate of the card
};
//prototype for Battle();
void Battle(Card c1, Card c2);
//Card Constructor
Card::Card(int x1, int y1) {
    x = x1;
    y = y1;
}
int Card::getX() {
    return x;
}
int Card::getY() {
    return y;
}
char Card::getElement() {
    return element;
}
int Card::getNumber() {
    return number;
}
//Creates filled rectangles for the card (color specified before the function is called)
void Card::DisplayCards() {
    LCD.FillRectangle(x,y,24,50);
}
//Prints the element and the number within the card, using the x and y coordinates to make sure it's always stuck on the card
void Card::CardInfo(){
    LCD.SetFontColor(WHITESMOKE);
    LCD.WriteAt(element,x+2,y+2);
    LCD.WriteAt(number,x+2,y+20);
}
//This function controls the dragging of the card into the main playing area
void Card::DragCards() {
    float a, b;
    //starts the timer
    ResetTime();
    TimeNow();
    while(!LCD.Touch(&a,&b) && TimeNow() <= 10.0){};        //the time CAN go past 10 seconds, but the moment the
    while(LCD.Touch(&a,&b) && TimeNow() <= 10.0){            //while the card is being touched (touched and held down)
        LCD.SetFontColor(DIMGRAY);
        LCD.FillRectangle(x,y,24,50);       //removes "old" cards by drawing background-colored rectangles behind where the card is being dragged
        x = a;      //x and y coordinates being set by the touch
        y = b;
        LCD.SetFontColor(BLUE);       //creates new blue rectangle for where the card is being moved by player
        LCD.FillRectangle(x,y,24,50);
    }
    DrawPenguins();         //recreate the drawing of the penguins so that if they were destroyed by the dragging, they are now back
    LCD.SetFontColor(WHITESMOKE);       //recreate the menu button and border of the game in case it was destroyed by dragging
    LCD.DrawRectangle(0,0,50,20);
    LCD.WriteAt("Menu",0,0);
    LCD.DrawRectangle(10,40,300,138);
}
//Assigns a random element and number value to the Card for which the function is being called
void Card::MakeRandom(){
    int elem = (Random.RandInt() % 3) + 1;
    int num = (Random.RandInt() % 9) + 1;
    if (elem == 1)
        element = 'W'; //Water
    if (elem == 2)
        element = 'F'; //Fire
    if (elem == 3)
        element = 'S'; //Snow
    number = num;
}
void makeCards() {      // creates 10 cards, also includes code for replacing erased parts of the screen and also calling the
                        //battle() method which will compare a random computer's card to the player's card which was dragged to the screen
    Card card1(10,180);
    Card card2(35,180);
    Card card3(60,180);
    Card card4(85,180);
    Card card5(110,180);
    Card card6(300-10,180);
    Card card7(300-35,180);
    Card card8(300-60,180);
    Card card9(300-85,180);
    Card card10(300-110,180);
    //temporary cards for what is played by computer
    Card computerCard;
    //assigns random values to each card using the MakeRandom() function
    Random.Seed();
    card1.MakeRandom();
    card2.MakeRandom();
    card3.MakeRandom();
    card4.MakeRandom();
    card5.MakeRandom();
    card6.MakeRandom();
    card7.MakeRandom();
    card8.MakeRandom();
    card9.MakeRandom();
    card10.MakeRandom();
    LCD.SetFontColor(BLUE);     //BLUE cards for the player
    card1.DisplayCards();
    card2.DisplayCards();
    card3.DisplayCards();
    card4.DisplayCards();
    card5.DisplayCards();
    LCD.SetFontColor(SCARLET); //SCARLET cards for the computer
    card6.DisplayCards();
    card7.DisplayCards();
    card8.DisplayCards();
    card9.DisplayCards();
    card10.DisplayCards();
    //The element and numbers are only visible for the player's cards
    card1.CardInfo();
    card2.CardInfo();
    card3.CardInfo();
    card4.CardInfo();
    card5.CardInfo();
    while(playerWins < 5 && computerWins < 5){
        LCD.SetFontColor(WHITESMOKE);
        LCD.WriteAt(playerWins,140,45);
        LCD.WriteAt("|",150,45);
        LCD.WriteAt(computerWins,160,45);
        float x1, y1;
        //player picks which card they want to drag
        while(!LCD.Touch(&x1,&y1)){};
        while(LCD.Touch(&x1,&y1)) {};
        int something = (Random.RandInt() % 5) + 1;
        if(something == 1)
            computerCard = card6;
        if (something == 2)
            computerCard = card7;
        if (something == 3)
            computerCard = card8;
        if (something == 4)
            computerCard = card9;
        if (something == 5)
            computerCard = card10;
        if(x1 >= 10 && x1 <= 34 && y1 >= 180 && y1 <= 230) {        //if the player moves card 1
            card1.DragCards();
            //replace destroyed cards
            //in case the dragging of the cards damaged the other cards, the following 2 chunks of code replaces the
            //rectangles for the player and the computer
            LCD.SetFontColor(BLUE);
            card1.DisplayCards();
            card2.DisplayCards();
            card3.DisplayCards();
            card4.DisplayCards();
            card5.DisplayCards();
            LCD.SetFontColor(SCARLET);
            card6.DisplayCards();
            card7.DisplayCards();
            card8.DisplayCards();
            card9.DisplayCards();
            card10.DisplayCards();
            //the info presented on the player's cards are replaced if they were destroyed by the dragging
            card1.CardInfo();
            card2.CardInfo();
            card3.CardInfo();
            card4.CardInfo();
            card5.CardInfo();
            computerCard.CardInfo();
            Battle(card1,computerCard);
            Sleep(2.0);
            LCD.SetFontColor(DIMGRAY);
            LCD.FillRectangle(135, 43, 40, 20);
            LCD.FillRectangle(card1.getX(), card1.getY(), 24, 50);   //create a gray rectangle over wherever card 1 is
            LCD.SetFontColor(SCARLET);  //reset the computer card
            computerCard.MakeRandom();
            computerCard.DisplayCards();
            //Card 1's previous slot is replaced by a new card
            LCD.SetFontColor(BLUE);
            Card newcard1(10,180);
            newcard1.MakeRandom();
            newcard1.DisplayCards();
            newcard1.CardInfo();
            card1 = newcard1;
        }
        else if(x1 >= 35 && x1 <= 59 && y1 >= 180 && y1 <= 230) {       //if the player moves card 2
            card2.DragCards();
            //replace destroyed cards
            //in case the dragging of the cards damaged the other cards, the following 2 chunks of code replaces the
            //rectangles for the player and the computer
            LCD.SetFontColor(BLUE);
            card1.DisplayCards();
            card2.DisplayCards();
            card3.DisplayCards();
            card4.DisplayCards();
            card5.DisplayCards();
            LCD.SetFontColor(SCARLET);
            card6.DisplayCards();
            card7.DisplayCards();
            card8.DisplayCards();
            card9.DisplayCards();
            card10.DisplayCards();
        //the info presented on the player's cards are replaced if they were destroyed by the dragging
            card1.CardInfo();
            card2.CardInfo();
            card3.CardInfo();
            card4.CardInfo();
            card5.CardInfo();
            card2.CardInfo();
            computerCard.CardInfo();
            Battle(card2,computerCard);
            Sleep(2.0);
            LCD.SetFontColor(DIMGRAY);
            LCD.FillRectangle(135, 43, 40, 20);
            LCD.FillRectangle(card2.getX(), card2.getY(), 24, 50);
            LCD.SetFontColor(SCARLET);  //reset the computer card
            computerCard.MakeRandom();
            computerCard.DisplayCards();
            //Card 2's previous slot is replaced by a new card
            LCD.SetFontColor(BLUE);
            Card newcard2(35,180);
            newcard2.MakeRandom();
            newcard2.DisplayCards();
            newcard2.CardInfo();
            card2 = newcard2;
        }
        else if(x1 >= 60 && x1 <= 84 && y1 >= 180 && y1 <= 230){        //if the player moves card 3
            card3.DragCards();
            //replace destroyed cards
            //in case the dragging of the cards damaged the other cards, the following 2 chunks of code replaces the
            //rectangles for the player and the computer
            LCD.SetFontColor(BLUE);
            card1.DisplayCards();
            card2.DisplayCards();
            card3.DisplayCards();
            card4.DisplayCards();
            card5.DisplayCards();
            LCD.SetFontColor(SCARLET);
            card6.DisplayCards();
            card7.DisplayCards();
            card8.DisplayCards();
            card9.DisplayCards();
            card10.DisplayCards();
            //the info presented on the player's cards are replaced if they were destroyed by the dragging
            card1.CardInfo();
            card2.CardInfo();
            card3.CardInfo();
            card4.CardInfo();
            card5.CardInfo();
            card3.CardInfo();
            computerCard.CardInfo();
            Battle(card3,computerCard);
            Sleep(2.0);
            LCD.SetFontColor(DIMGRAY);
            LCD.FillRectangle(135, 43, 40, 20);
            LCD.FillRectangle(card3.getX(), card3.getY(), 24, 50);
            LCD.SetFontColor(SCARLET);  //reset the computer card
            computerCard.MakeRandom();
            computerCard.DisplayCards();
            //Card 3's previous slot is replaced by a new card
            LCD.SetFontColor(BLUE);
            Card newcard3(60,180);
            newcard3.MakeRandom();
            newcard3.DisplayCards();
            newcard3.CardInfo();
            card3 = newcard3;
        }
        else if(x1 >= 85 && x1 <= 109 && y1 >= 180 && y1 <= 230) {      //if the player moves card 4
            card4.DragCards();
            //replace destroyed cards
            //in case the dragging of the cards damaged the other cards, the following 2 chunks of code replaces the
            //rectangles for the player and the computer
            LCD.SetFontColor(BLUE);
            card1.DisplayCards();
            card2.DisplayCards();
            card3.DisplayCards();
            card4.DisplayCards();
            card5.DisplayCards();
            LCD.SetFontColor(SCARLET);
            card6.DisplayCards();
            card7.DisplayCards();
            card8.DisplayCards();
            card9.DisplayCards();
            card10.DisplayCards();
            //the info presented on the player's cards are replaced if they were destroyed by the dragging
            card1.CardInfo();
            card2.CardInfo();
            card3.CardInfo();
            card4.CardInfo();
            card5.CardInfo();
            card4.CardInfo();
            computerCard.CardInfo();
            Battle(card4,computerCard);
            Sleep(2.0);
            LCD.SetFontColor(DIMGRAY);
            LCD.FillRectangle(135, 43, 40, 20);
            LCD.FillRectangle(card4.getX(), card4.getY(), 24, 50);
            LCD.SetFontColor(SCARLET);  //reset the computer card
            computerCard.MakeRandom();
            computerCard.DisplayCards();
            //Card 4's previous slot is replaced by a new card
            LCD.SetFontColor(BLUE);
            Card newcard4(85,180);
            newcard4.MakeRandom();
            newcard4.DisplayCards();
            newcard4.CardInfo();
            card4 = newcard4;
        }
        else if(x1 >= 110 && x1 <= 134 && y1 >= 180 && y1 <= 230) {     //if the player moves card 5
            card5.DragCards();
            //replace destroyed cards
            //in case the dragging of the cards damaged the other cards, the following 2 chunks of code replaces the
            //rectangles for the player and the computer
            LCD.SetFontColor(BLUE);
            card1.DisplayCards();
            card2.DisplayCards();
            card3.DisplayCards();
            card4.DisplayCards();
            card5.DisplayCards();
            LCD.SetFontColor(SCARLET);
            card6.DisplayCards();
            card7.DisplayCards();
            card8.DisplayCards();
            card9.DisplayCards();
            card10.DisplayCards();
            //the info presented on the player's cards are replaced if they were destroyed by the dragging
            card1.CardInfo();
            card2.CardInfo();
            card3.CardInfo();
            card4.CardInfo();
            card5.CardInfo();
            card5.CardInfo();
            computerCard.CardInfo();
            Battle(card5,computerCard);
            Sleep(2.0);
            LCD.SetFontColor(DIMGRAY);
            LCD.FillRectangle(135, 43, 40, 20);
            LCD.FillRectangle(card5.getX(), card5.getY(), 24, 50);
            LCD.SetFontColor(SCARLET);  //reset the computer card
            computerCard.MakeRandom();
            computerCard.DisplayCards();
            //Card 5's previous slot is replaced by a new card
            LCD.SetFontColor(BLUE);
            Card newcard5(110,180);
            newcard5.MakeRandom();
            newcard5.DisplayCards();
            newcard5.CardInfo();
            card5 = newcard5;
        }
        else if (x1 > 0 && x1 < 50 && y1 > 0 && y1 < 20){ //if the player goes back to menu
            Menu();
        }
        else {
            LCD.Clear();
            LCD.WriteAt("Click once on a card,",0,0);
            LCD.WriteAt("then drag it. Try again",0,20);
            LCD.WriteAt("Going back to Menu",0,60);
            Sleep(5.0);
            Menu();
        }
    }
    if(playerWins == 5){
        LCD.Clear();
        LCD.SetBackgroundColor(ROYALBLUE);
        LCD.SetFontColor(GREENYELLOW);
        LCD.WriteAt("You win!",0,0);
        Sleep(3.0);
        totalWins++;
        Menu();
    }
    else {
        LCD.SetBackgroundColor(PALEVIOLETRED);
        LCD.SetFontColor(SCARLET);
        LCD.Clear();
        LCD.WriteAt("You lose!", 0,0);
        Sleep(3.0);
        totalLosses++;
        Menu();
    }
}
//Checks who is the winner of the game, the computer or the player
void Battle(Card player, Card computer){
    //Check if player's coordinates are in the box with the penguins
    if (player.getX() >=10 && player.getX() <= 310 && player.getY() >= 40 && player.getY() <= 178) {
        //Compare player's elem first. if elem is the same, compare number. determine winner, have some kind of animation, call makeCards();
        char playerElem = player.getElement();
        char computerElem = computer.getElement();
        int playerNum = player.getNumber();
        int computerNum = computer.getNumber();
        if(playerElem != computerElem){     //if elements are not the same, S beats W, W beats F, and F beats S
            if(playerElem == 'W' && computerElem == 'S'){
                computerWins++;
                KillPenguin(0);
            }
            else if(playerElem == 'F' && computerElem == 'S'){
                playerWins++;
                KillPenguin(1);
            }
            else if(playerElem == 'S' && computerElem == 'W'){
                playerWins++;
                KillPenguin(1);
            }
            else if(playerElem == 'F' && computerElem == 'W'){
                computerWins++;
                KillPenguin(0);
            }
            else if(playerElem == 'W' && computerElem == 'F'){
                playerWins++;
                KillPenguin(1);
            }
            else if(playerElem == 'S' && computerElem == 'F'){
                computerWins++;
                KillPenguin(0);
            }
        }
        else if(playerElem == computerElem){    //if element is the same, the greater number wins
            if(playerNum > computerNum){
                playerWins++;
                KillPenguin(1);
            }
            else if(computerNum > playerNum){
                computerWins++;
                KillPenguin(0);
            }
        }
    }
    else {
        //if the player somehow dragged the card somewhere else, just make computer the winner. or if player took too long
        LCD.Clear();
        LCD.WriteAt("You should have dragged",0,0);
        LCD.WriteAt("The card in the box,",0,20);
        LCD.WriteAt("You may have",0,40);
        LCD.WriteAt("taken too long",0,60);
        LCD.WriteAt("YOU LOSE THIS ROUND",0,100);
        Sleep(5.0);
        LCD.Clear();
        computerWins++;
        DrawPenguins();         //recreate the drawing of the penguins
        LCD.SetFontColor(WHITESMOKE);       //recreate the menu button and border of the game
        LCD.DrawRectangle(0,0,50,20);
        LCD.WriteAt("Menu",0,0);
        LCD.DrawRectangle(10,40,300,138);
        //recreate the cards
        makeCards();
    }
}
//Creates 2 penguins looking at each other
void DrawPenguins(){
    //Penguin's feet and beak
    LCD.SetFontColor(GOLDENROD);
    LCD.FillRectangle(85, 170, 20, 7);
    LCD.FillRectangle(45, 170, 20, 7);
    LCD.FillRectangle(90,105,10,5);
    //Penguin's body and head
    LCD.SetFontColor(ROYALBLUE);
    LCD.FillCircle(75, 145, 30);
    LCD.FillCircle(75, 105, 15);
    //mask on player 1
    LCD.SetFontColor(WHITE);
    LCD.FillRectangle(60,105,30,2);
    //Penguin's eye, size depends on who's in the lead
    if(computerWins > playerWins){
        LCD.SetFontColor(WHITE);
        LCD.FillCircle(83,105,8);
        LCD.SetFontColor(BLACK);
        LCD.FillCircle(85,105,2);
    }
    else {
        LCD.SetFontColor(WHITE);
        LCD.FillCircle(85,105,4);
        LCD.SetFontColor(BLACK);
        LCD.FillCircle(87,105,2);
    }
    //Penguin's feet and beak
    LCD.SetFontColor(GOLDENROD);
    LCD.FillRectangle(255, 170, 20, 7);
    LCD.FillRectangle(215, 170, 20, 7);
    LCD.FillRectangle(220,105,10,5);
    //Penguin's body and head
    LCD.SetFontColor(PALEVIOLETRED);
    LCD.FillCircle(245, 145, 30);
    LCD.FillCircle(245, 105, 15);
    //mask on computer
    LCD.SetFontColor(WHITE);
    LCD.FillRectangle(230,105,30,2);
    //Penguin's eye, size depends on who's in the lead
    if(playerWins > computerWins) {
        LCD.SetFontColor(WHITE);
        LCD.FillCircle(236,105,8);
        LCD.SetFontColor(BLACK);
        LCD.FillCircle(234,105,2);
    }
    else {
        LCD.SetFontColor(WHITE);
        LCD.FillCircle(236,105,4);
        LCD.SetFontColor(BLACK);
        LCD.FillCircle(234,105,2);
    }
}
//The instructions page with the instructions. Clicking anywhere on the screen goes back to Menu.
void Instructions() {
    LCD.SetBackgroundColor(BLACK);
    LCD.Clear();
    LCD.WriteAt("Get random cards and", 0, 0);
    LCD.WriteAt("play against a computer", 0, 20);
    LCD.WriteAt("which has random cards.",0, 40);
    LCD.WriteAt("Drag your card to the box",0,60);
    LCD.WriteAt("Fire(F) beats Snow(S)", 0, 80);
    LCD.WriteAt("Snow(S) beats Water(W)", 0, 100);
    LCD.WriteAt("And Water(W) beats Fire(F)", 0, 120);
    LCD.WriteAt("If same element,",0,140);
    LCD.WriteAt("Greater number wins", 0, 160);
    LCD.WriteAt("Click anywhere to go back",0,200);
    float x,y;
     while (1) { //never end
        LCD.Update();
        while(!LCD.Touch(&x, &y)){};
        while(LCD.Touch(&x, &y)){};
        if(x > 0 && y > 0){
            Menu();
        }
    }
}
//The Statistics page with the wins and losses. Clicking anywhere on the screen goes back to Menu.
void Stats() {
   LCD.SetBackgroundColor(BLACK);
   LCD.Clear();
   LCD.WriteAt("Wins: ", 0, 0);
   LCD.WriteAt(totalWins,150,0);
   LCD.WriteAt("Losses: ", 0, 20);
   LCD.WriteAt(totalLosses, 150, 20);
   LCD.WriteAt("Click anywhere to go back",0,60);
    float x, y;
    while (1) {  //never end
        LCD.Update();
        while(!LCD.Touch(&x, &y)){};
        while(LCD.Touch(&x, &y)){};
        if(x > 0 && y > 0){
            Menu();
        }
    }
}
//The credits page with the names of Shagnik and Maitreya. Clicking anywhere on the screen goes back to Menu.
void Credits(){
   LCD.SetBackgroundColor(BLACK);
   LCD.Clear();
   LCD.WriteAt("Creators:", 0, 0);
   LCD.WriteAt("Maitreya Dixit", 0, 20);
   LCD.WriteAt("Shagnik Sarkar", 0, 40);
   LCD.WriteAt("Click anywhere to go back",0,80);
    float x,y;
    while (1) { //never end
        LCD.Update();
        while(!LCD.Touch(&x, &y)){};
        while(LCD.Touch(&x, &y)){};
        if(x > 0 && y > 0){
            Menu();
        }
    }
}
//This method puts a giant red cross on the penguin who lost a round
void KillPenguin(int check){
    Sleep(1.0);
    if(check == 1){
        LCD.SetFontColor(RED);
        LCD.DrawCircle(245, 145, 30);
        LCD.FillRectangle(240, 115, 10, 58);
        LCD.FillRectangle(215, 140, 58, 10);
    }
    else if(check == 0){
        LCD.SetFontColor(RED);
        LCD.DrawCircle(75, 145, 30);
        LCD.FillRectangle(70, 115, 10, 58);
        LCD.FillRectangle(45, 140, 58, 10);
    }
    Sleep(1.0);
    DrawPenguins();
}