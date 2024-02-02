/**********************/
// Alexander Wise - 4/16/23
/**********************/


#include <LiquidCrystal.h>

LiquidCrystal lcd(7, 6, 5, 4, 3, 2);

int scoreSum = 0;
int threshold = 650;
int interval = 5000;
int timer;
int photoresistorValue[3], ledPins[3] = {13, 12, 11};

unsigned long currentMillis, previousMillis;
unsigned long currentMillisPhoto[3], previousMillisPhoto[3] = {0,0,0};

bool newDataReceived = false;
bool canBeHit[3] = {true, true, true};

byte photoresPins[3] = {A0, A2, A4};

char receivedScore[10];

/************************************************************/
// Functions
/************************************************************/

void setup() {
  Serial.begin(9600);
  lcd.begin(16, 2);
  pinMode(10, INPUT_PULLUP);
  for(int i = 0; i < 3; i++){
    pinMode(ledPins[i], OUTPUT);
    digitalWrite(ledPins[i], HIGH);
  }   
}

void loop() {
  startGame();
}

void startGame(){
  while(digitalRead(10) == HIGH){
    lcd.setCursor(0,0);
    lcd.print("Press to start!");
    lcd.setCursor(0,1);
    lcd.print("Highscore: 2500");
  }
  scoreSum = 0;
  timer = 60;
  lcd.clear();
  game();
}

void game(){
  while(changeTime() == true){
    scoring();
  }
  gameOver();
  delay(6000);
}

// Reads incoming data from the Photoresistor controller
void scoring(){
  
  if(Serial.available() > 0){
    Serial.readBytes(receivedScore, 2);
    // Increases score based on received value
    int received = atoi(receivedScore);
    if(received == 25){
      scoreSum += 25;
    }
    else if(received == 50){
      scoreSum += 50;
    }
  }
  photoresistors();

  // Checks to see if there is time left 
  if(changeTime()){
    lcd.setCursor(0,0);
    lcd.print("Score: ");
    lcd.print(scoreSum);
    newDataReceived = false;
  }
  else{
    gameOver();
  }
}

void gameOver(){
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Final: ");
  lcd.print(scoreSum);
}

bool changeTime(){
  
  currentMillis = millis();
  if(currentMillis - previousMillis >= 1000){
    previousMillis = currentMillis;
    
    timer--;
    if(timer == 9){
      lcd.clear();
    }
    lcd.setCursor(0,1);
    lcd.print("Time Left: ");
    lcd.print(timer);
  }
  if(timer >= 0){
    return true;
  } else {
    return false;
  }
}

// Updates which photoresistor can register a hit from the middle row
void photoresistors(){
  for(int i = 0; i < 3; i++){
    currentMillisPhoto[i] = millis();
    photoresistorValue[i] = analogRead(photoresPins[i]);
  }
  for(int target = 0; target < 3; target++){
    if(photoresistorValue[target] > threshold && canBeHit[target] == true){
      canBeHit[target] = false;
      previousMillisPhoto[target] = millis();
      digitalWrite(ledPins[target], LOW);  
      // Checks to see if special 100 point target can be hit 
      if(target == 1){
        scoreSum += 100;
      }
      else{
        scoreSum += 25;
      }
    }  
  }
  turnOnLED();
  changeStatus(); 
  blinkTwo();
}

// Blinks the special 100 point target in the center
void blinkTwo(){
  static unsigned long previousBlinkMillis = 0;
  unsigned long interval = 10000;
  if(!canBeHit[1]){
    if(millis() - previousBlinkMillis >= interval){
      previousBlinkMillis = millis();
      canBeHit[1] = true;
      digitalWrite(ledPins[1], HIGH);
    }
  }
}

// Changes the status of the photo resistor if enough time has passed
void changeStatus(){
  for(int target = 0; target < 3; target += 2){
    if(currentMillisPhoto[target] - previousMillisPhoto[target] >= interval){
      previousMillisPhoto[target] = currentMillisPhoto[target];
      canBeHit[target] = true;
    }
  }
  if(currentMillisPhoto[1] - previousMillisPhoto[1] >= 1500){
    previousMillisPhoto[1] = currentMillisPhoto[1];
    canBeHit[1] = false;
  }
}

// Turns on LED if a target can be hit
void turnOnLED(){
  for(int target = 0; target < 3; target++){
    if(canBeHit[target] == false){
      digitalWrite(ledPins[target], LOW);
    } else {
      digitalWrite(ledPins[target], HIGH);
    }
  }
}