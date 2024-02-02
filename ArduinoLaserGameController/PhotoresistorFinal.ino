/******************/
// Alexander Wise - 4/6/23
/*****************/


int threshold = 600;
int interval = 5000;
int numTargets = 6;
int scoreValues[6] = {50, 50, 50, 25, 25, 25};
int photoresistorValue[6], ledPins[6] = {13, 12, 11, 10, 9, 8}; 

unsigned long currentMillis[6], previousMillis[6] = {0, 0, 0, 0, 0, 0};

char scoreSend[16];
bool canBeHit[6] = {true, true, true, true, true, true};
byte photoresPins[6] = {A0, A1, A2, A3, A4, A5};

/************************************************************/
// Functions
/************************************************************/

void setup() {
  Serial.begin(9600);
  randomSeed(analogRead(0));
  for(int i = 0; i < 6; i++){
    pinMode(ledPins[i], OUTPUT);
  }   
}

void loop() {
  // Cycles through each photoresistor and reads the value at it
  for(int i = 0; i < numTargets; i++){
    currentMillis[i] = millis();
    photoresistorValue[i] = analogRead(photoresPins[i]);
  }
  // Cycles through and checks to see if the value is > threshold and target can be hit
  for(int target = 0; target < numTargets; target++){
    if(photoresistorValue[target] > threshold && canBeHit[target] == true){
      canBeHit[target] = false;
      previousMillis[target] = millis();
      digitalWrite(ledPins[target], LOW);
      // Sends data to the other arduino
      sprintf(scoreSend, "%d", scoreValues[target]);
      Serial.write(scoreSend, 2);
    }
  }
  randomBlinkTopRow();
  turnOnLED();
  changeStatus();

  delay(100);
}

int randomBlinkTopRow() {
  static unsigned long previousBlinkMillis = 0;
  // Checks to see if there are any targets that can be turned off
  if(canBeHit[0] == false && canBeHit[1] == false && canBeHit[2] == false){
        return -1;
  } else {
    // Checks to see if 2 seconds have passed
    if (millis() - previousBlinkMillis >= 2000) {
      previousBlinkMillis = millis();
      // Selects a random target
      int target = random(numTargets) % 3;
      // Will keep randomly selecting a target until one that isn't turned off is selected
      while (!canBeHit[target]) {
        target = random(numTargets) % 3;
      }
      // Deactivates the target
      canBeHit[target] = false;
      digitalWrite(ledPins[target], LOW);
    }
  }
  return 0;
}

void turnOnLED(){
  for(int target = 0; target < numTargets; target++){
    if(canBeHit[target] == false){
      digitalWrite(ledPins[target], LOW);
    } else {
      digitalWrite(ledPins[target], HIGH);
    }
  }
}

void changeStatus(){
  for(int target = 0; target < numTargets; target++){
    if(currentMillis[target] - previousMillis[target] >= interval){
      previousMillis[target] = currentMillis[target];
      canBeHit[target] = true;
    }
  }
}

