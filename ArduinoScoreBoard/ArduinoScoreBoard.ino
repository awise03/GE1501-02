/*----------------------------------*/
// Alexander Wise - 11/27/22
//
// General Controls:
// MAIN MENU:
//
// GREEN: Changes current menu item selection (Indicated by the flashing star)
// BLUE: Confirms the Selection
// RANDOM NUMBERS:
//    - GREEN will take you back to the main menu
//    - No other button will work here
//    - Can generate another set of numbers if reselected
//
// WHEN PLAYING: 
//
// GREEN: Moves to next player
// BLUE: Begins score input
// RED: Prompts user if they want to delete previous entry.
// ORANGE: Goes back a player
// DO NOT hold buttons down
// SCORE INPUT:
//    - 2 buttons max (GREEN, YELLOW, RED)
//    - Pressing BLUE at any point will end score input
//    - When prompted to Confirm, GREEN to confirm, RED to restart score input
// DELETE PREVIOUS:
//    - Only 2 buttons possible: RED or GREEN
//    - GREEN to confirm previous score removal
//    - RED to deny previous score removal
/*----------------------------------*/

#include <LiquidCrystal.h>
#include <ArduinoSort.h>
// Initialize the arrays
int scoreBoard[4] = {0, 0, 0, 0};
int colors[2], randomNums[12], cards[40];
// Initializing pins used by the buttons
int buttonPinGreen(6), buttonPinRed(7), buttonPinYellow(8), buttonPinBlue(9);
int count, count2, previous, rando, pointer(2), player(1), pointerM(4);
unsigned long previousMillis = 0;
const long interval = 500;
bool starStatus = false;
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

void setup(){
  Serial.begin(9600);
  pinMode(buttonPinGreen, INPUT_PULLUP);
  pinMode(buttonPinRed, INPUT_PULLUP);
  pinMode(buttonPinYellow, INPUT_PULLUP);
  pinMode(buttonPinBlue, INPUT_PULLUP);
  lcd.begin(16, 2); // Set up the number of columns and rows on the LCD.
  lcd.clear();
  startGame();
}

void loop(){
  printScore();
  buttonChecks();
  blink();
}

void startGame(){
  lcd.setCursor(0, 0);
  lcd.print("---   PAW    ---");
  lcd.setCursor(0,1);
  lcd.print("--   ISLAND   --");
  delay(2000);
  lcd.clear();
  promptAndCheck(); 
  delay(1000); 
  lcd.clear();
}

/***********************************************************************************************************************/
// Menu + Randomization
/***********************************************************************************************************************/

// Prompts the user to choose either the play button or the randomize button to determine which cards become the NPC cards
void promptAndCheck(){
  lcd.clear();
  count2 = 0;
  randomSeed(analogRead(0));
  fillAndEmpty();

  //Checks for the press of the blue button
  while(digitalRead(buttonPinBlue) == HIGH){
    lcd.setCursor(0, 0);
    lcd.print("Menu:");
    lcd.setCursor(0,1);
    lcd.print("PLAY");
    //lcd.setCursor(8, 1);
    //lcd.print("RANDOM");
	  blinkMenu();
    // Checks for the press of a green button indicating a change in menu selection
    /*if(digitalRead(buttonPinGreen) == LOW && pointerM == 14){
      delay(200);
      pointerM = 4;
      Serial.print(pointerM);
      lcd.clear();
    } 
    else if(digitalRead(buttonPinGreen) == LOW && pointerM == 4){
      delay(200); 
      pointerM = 14;
      Serial.print(pointerM);
      lcd.clear();
    }*/  
  }
  // Will clear the LCD and move on to loop() if pressed while star is on Play
  if(digitalRead(buttonPinBlue) == LOW && pointerM == 4){
    delay(1000);
    lcd.clear();
  }
  // Will clear the LCD and generate 12 random numbers when pressed while the star is on Random
  /*else if(digitalRead(buttonPinBlue) == LOW && pointerM == 14){
    delay(1000);
    lcd.clear();
    randomize();
    printRandom();
  }*/
}
// Determines when to turn the star on and off on the selection
void blinkMenu(){
  unsigned long currentMillis = millis();
  // Checks to see if 500ms have passed
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    // Will enter if the star is off
    if(starStatus == false) starOnMenu();
    // Will enter if the star is on
    else starOffMenu();
  }
}
// Generates 12 random numbers between 1 and 40
/*void randomize(){
  while(count2 < 4){
    rando = random(0,9);
    // If a card is set to 0, will not add to array
    if(cards[rando] != 0){
      randomNums[count2] = cards[rando];
      // Sets card value to 0 so the card isn't repeated
      cards[rando] = 0;
      count2++;
    }
  }
  while(count2 < 12){
    rando = random(11,39);
    // If a card is set to 0, will not add to array
    if(cards[rando] != 0){
      randomNums[count2] = cards[rando];
      // Sets card value to 0 so the card isn't repeated
      cards[rando] = 0;
      count2++;
    }
  }
  sortArray(randomNums, 12);
}
// Prints the numbers to the LCD
void printRandom(){
  lcd.setCursor(0, 0);
  for(int i = 0; i < 7; i++){
    lcd.print(randomNums[i]);
    lcd.print(" ");
  }
  lcd.setCursor(0, 1);
  for(int j = 7; j < 12; j++){
    lcd.print(randomNums[j]);
    lcd.print(" ");
  }
  // Will keep the numbers on screen until the green button is pressed
  while(digitalRead(buttonPinGreen) == HIGH);
  delay(500);
  lcd.clear();
  promptAndCheck();
}*/
// Turns the star on
void starOnMenu(){
  lcd.setCursor(pointerM, 1);
  lcd.print("*");
  starStatus = true;
}
// Turns the star off
void starOffMenu(){
  lcd.setCursor(pointerM, 1);
  lcd.print(" ");
  starStatus = false;
}
// Empties the randomNums array and fills the cards array
void fillAndEmpty(){
  for(int i = 1; i < 41; i++){
    cards[i-1] = i;
  }
  for(int j = 0; j < 12; j++){
    randomNums[j] = 0;
  }
}

/***********************************************************************************************************************/
// Scoreboard Code
/***********************************************************************************************************************/

void buttonChecks(){
  // Checks for a press of the blue button
  if(digitalRead(buttonPinBlue) == LOW){
    delay(200);
    lcd.clear();
    count = 0;
    // Clears the array of the colors
    emptyColors();
    // Calls the function where a user inputs the colors they want to score
    changeScore();
    lcd.clear();
  }
  // Checks for a press of the green button
  else if(digitalRead(buttonPinGreen) == LOW){
    delay(200);
    lcd.clear();
    // Increases the player value by one when a player signals the end of their turn
    player++;
    pointer += 4;
    // Resets all values to that of the first player after player 4's turn
    if(player > 4){
      player = 1;
      pointer = 2;
      previous = 0;
    }
  }
  // Checks for a press of the red button
  else if(digitalRead(buttonPinRed) == LOW){
    delay(200);
    lcd.clear();
    // Calls function prompting user if they want to delete the previous score
    removePrevious();
    lcd.clear();
  }
  // Checks for a press of the yellow button
  else if(digitalRead(buttonPinYellow) == LOW){
    delay(200);
    lcd.clear();
    // Decreases the player value by one 
    player--;
    pointer -= 4;
    // Checks to see if player one is moving back and sets the player to 4
    if(player == 0){
      player = 4;
      pointer = 14;
    }
  }
}
// Blinks the pointer/star to show who's turn it is
void blink(){
  unsigned long currentMillis = millis();
  // Checks to see if 500ms have passed
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    // Will enter if the star is off
    if(starStatus == false) starOn();
    // Will enter if the star is on
    else starOff();
  }
}
//  Prints the score to the LCD screen
void printScore(){
  for(int i = 3; i < 14; i+=4){
    for(int j = 0; j < 2; j++){
      lcd.setCursor(i, j);
      lcd.print("|");
    }
  }
  int player = 1;
  // Prints the player number and then the score of the player
  for(int z = 0; z < 14; z+=4){
    lcd.setCursor(z, 0);
    lcd.print("P");
    lcd.print(player);
    lcd.setCursor(z, 1);
    lcd.print(scoreBoard[z/4]);
    player++;
  }
}
// Takes a user color input and prints to screen and adds to colors array
void changeScore(){
  lcd.setCursor(0, 0);
  lcd.print("Input Colors...");
  delay(200);
  // Will continue to loop until the player hits a button
  while(count != 1){
    if(digitalRead(buttonPinBlue) == LOW){
      // Jumps to the end of the function to completely exit both loops
      goto tmp;
    }
    // All statements below will assign a value to colors[0] that corresponds to a color when the button is pressed
    if(digitalRead(buttonPinGreen) == LOW){
      colors[count] = 1;
      displayChoice(0);
      count++;
    }
    else if(digitalRead(buttonPinYellow) == LOW){
      colors[count] = 2;
      displayChoice(0);
      count++;
    }
    else if(digitalRead(buttonPinRed) == LOW){
      colors[count] = 3;
      displayChoice(0);
      count++;
    }
  }

  delay(500);
  // Does the same as the while loop above
  while(count != 2){
    if(digitalRead(buttonPinBlue) == LOW){
      goto tmp;
    }

    if(digitalRead(buttonPinGreen) == LOW){
      colors[count] = 1;
      displayChoice(8);
      count++;
      
    }
    else if(digitalRead(buttonPinYellow) == LOW){
      colors[count] = 2;
      displayChoice(9);
      count++;
     
    }
    else if(digitalRead(buttonPinRed) == LOW){
      colors[count] = 3;
      displayChoice(7);
      count++;
      
    }
  }
  delay(200);
  // When both choices have been displayed, asks the player to confirm
  if(count == 2){
    confirm();
  }
  // Goto statement comes here and skips rest of the function
  tmp: delay(200);
}
// Takes the values of the array and adds the score to the player's score
void addScore(){
  if(colors[0] == 1){
    switch(colors[1]){
      case 1:
        previous = 3;
        scoreBoard[player - 1] += 3;
        break;
      case 2:
        previous = 2;
        scoreBoard[player - 1] += 2;
        break;
      case 3:
        previous = 1;
        scoreBoard[player - 1] += 1;
        break;
    }
  }
  else if(colors[0] == 2){
    switch(colors[1]){
      case 1:
        previous = 2;
        scoreBoard[player - 1] += 2;
        break;
      case 2:
        previous = 3;       
        scoreBoard[player - 1] += 3;
        break;
      case 3:
        previous = 2;
        scoreBoard[player - 1] += 2;
        break;
    }
  }
  else if(colors[0] == 3){
    switch(colors[1]){
      case 1:
        previous = 1;
        scoreBoard[player - 1] += 1;
        break;
      case 2:
        previous = 2;
        scoreBoard[player - 1] += 2;
        break;
      case 3:
        scoreBoard[player - 1] += 3;
        break;
    }
  }
}
// Displays the user's color choice to the screen based on the value in the array
void displayChoice(int j){
  lcd.setCursor(j, 1);
  if(colors[count] == 1){
    lcd.print("GREEN");
  }
  else if(colors[count] == 2){
    lcd.print("YELLOW");
  }
  else if(colors[count] == 3){
    lcd.print("RED");
  }
}
// Prompts the user to confirm if they want to confirm the inputted score
void confirm(){
  lcd.setCursor(0, 0);
  for(int i = 0; i < 16; i++){
    lcd.setCursor(i, 0);
    lcd.print(" ");
  }
  // Loops until input detected
  while(digitalRead(buttonPinGreen) == HIGH && digitalRead(buttonPinRed) == HIGH && digitalRead(buttonPinBlue) == HIGH){
    lcd.setCursor(0, 0);
    lcd.print("Confirm?");
  }
  // If player declines their input, will loop through the changeScore() function again
  if(digitalRead(buttonPinRed) == LOW) {
      delay(200);
      lcd.clear();
      count = 0;
      emptyColors();
      changeScore();
  } 
  // Will add the score to the player's score when green button pressed
  else if(digitalRead(buttonPinGreen) == LOW) {
    addScore();
  } 
  // Will break out and take player back to the score screen
  else if(digitalRead(buttonPinBlue) == LOW) {
    delay(200);
  }
}
// Gives player the option to delete their previous score. Wiped when player is changed
void removePrevious(){
  // Will not exit until button press detected
  while(digitalRead(buttonPinGreen) == HIGH && digitalRead(buttonPinRed) == HIGH){
    lcd.setCursor(0, 0);
    lcd.print("Delete Previous?");

    lcd.setCursor(0, 1);
    lcd.print(previous);
  }
  // Will remove previous value from the player's score
  if(digitalRead(buttonPinGreen) == LOW){
    scoreBoard[player-1] -= previous;
    delay(200);
  }
  // Breaks out of the function and does not remove the player's score
  else if(digitalRead(buttonPinRed) == LOW){
    delay(200);
  }
  // Resets the value of previous
  previous = 0;
}
// Turns the star on next to the current player's name
void starOn(){
  lcd.setCursor(pointer, 0);
  lcd.print("*");
  starStatus = true;
}
// Turns off the star next to the current player's name
void starOff(){
  lcd.setCursor(pointer, 0);
  lcd.print(" ");
  starStatus = false;
}
// Empties the colors array
void emptyColors(){
  for(int p = 0; p < 2; p++){
    colors[p] = 0;
  }
}