// Left arm arduino control

char receivedChar;
int receivedInt;
boolean newData = false;

//number of total pins
const int numOfPins = 20;

int pinMax[numOfPins] = {255, 135, 255, 255, 255, 255};
int pinMin[numOfPins] = {125,  0,   100,   160,   0,   0};
int pinRest[numOfPins] = {155, 105, 180, 170, 0, 0};

int pinVal[numOfPins] = {50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50};

//offset that the pins have on the board starting from 0
int actualPinOffset = 8;

//how much is added to the pins by default each loop
int baseRate = 1;

/****************************
 *  Pin Variables
************************** */

int ARM_ROTATE_PIN_1 = 0;
int ARM_ROTATE_PIN_2 = 1;

int ARM_RAISE_PIN_1 = 3;
int ARM_RAISE_PIN_2 = 2;

int BICEP_RAISE_PIN_1 = 4;
int BICEP_RAISE_PIN_2 = 5;

/****************************
 *    Prototypes
 * 
 ************************/
 void moveArm();
 void recInfo();
 void changeMovement(int mov);
 void wave();
 void point();
 void welcome();
 void resetPosition(float modifier = 1);
 bool raiseArm(int val, float modifier = 1);
 bool raiseBicep(int val, float modifier = 1);
 bool turnArm(int val, float modifier = 1);
 bool restTurnArm(float modifier = 1);
 bool restRaiseArm(float modifier = 1);
 bool restRaiseBicep(float modifier = 1);
 bool rotatePinGroup(int pinNum1, int pinNum2, int val, float modifier = 1);
 bool changePinGroup(int pinNum1, int pinNum2, int val, float modifier = 1);
 bool changePinValue(int pinNum, int val, float modifier = 1);
 bool writeToPin(int pinNum, int value);
 bool normalizePin(int pinNum);


void debug()
{
   if (!newData) return; // if no new command recieved since last run, do not execute any case

  int blueprint = receivedInt; // store recieved int into variable for switch statements
  Serial.println("Number received: " + String(blueprint)); // print int recieved for communication confirmation
  rotatePinGroup(2, 3, blueprint);

  newData = false;

}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  for(int i=2; i<= 3; i++){
    analogWrite(i, OUTPUT);
  }
  
  resetPosition(4);
}

void loop() {
  recInfo();
  //point();
  //wave();
  //welcome();
  moveArm();
  //debug();
}

// switch statements for preprogrammed motions based on recieved int
void moveArm(){

  if (!newData) return; // if no new command recieved since last run, do not execute any case

  int blueprint = receivedInt; // store recieved int into variable for switch statements
  Serial.println("Number received: " + String(blueprint)); // print int recieved for communication confirmation
  switch (blueprint){
  case 1: // Wave case
    Serial.println("Case 1 start");
    wave();
    Serial.println("Case 1 end");
    break;
  case 2: // welcome case
    Serial.println("Case  2 start"); 
    welcome();
    Serial.println("Case 2 end");
    break;
  case 3: // point case
    Serial.println("Case 3 start");
    point();
    Serial.println("Case 3 end");
    break;
  case 4:
  resetPosition();
    break;
  }

  newData = false;

}


//wave
//raise arm
//raise bicep
//move arm back and forth

//point
//raise arm

//welcome
//move arms back
//raise arms
//raise biceps

void recInfo() {
  if (Serial.available()) {
    Serial.println("recInfo\n"); // print method name for run confirmation
    
    changeMovement(atoi(Serial.readString().c_str()));
  }
}

void changeMovement(int mov)
{
  if(newData == true)
  {
    resetPosition();
  }

  newData = true;
  receivedInt = mov; 
}

void wave()
{
  while(!raiseArm(255, 3) & !raiseBicep(255, 4));
  while(!raiseBicep(255, 4));
  
  for(int i = 0; i < 3; i++)
  {
    while(!turnArm(0));
    while(!turnArm(255));
  }

  resetPosition(6);
}

void point()
{
  while(!raiseArm(255, 4));
  for(int i=0; i<3; i++)
    {
      while(!raiseBicep(255, 3));
      while(!raiseBicep(0, 3));
    }

  resetPosition(4);
}

void welcome()
{
  while(!turnArm(-220, 3) && !raiseArm(220, 3) && !raiseBicep(220, 3))
  {

  }
  while(!raiseArm(255, 3) && !raiseBicep(255, 3))
{
  }
  while(!raiseBicep(255, 2))
  {
  }

  delay(2500);

  resetPosition(6);
}

void resetPosition(float modifier)
{
  //wait until the entire machine has reached the resting values
  while(!restTurnArm(modifier) && !restRaiseArm(modifier) && !restRaiseBicep(modifier));
  while(!restTurnArm(modifier));
  while(!restRaiseArm(modifier));
  while(!restRaiseBicep(modifier));
Serial.print("reset finished");
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//changes the arm raise pin 1 to the sepcified value while inversely changing arm raise pin 2
//returns true when the arm has reached the desired value or has been normalized
bool raiseArm(int val, float modifier)
{
  int firstPin = ARM_RAISE_PIN_1; //10
  int secondPin = ARM_RAISE_PIN_2; //9

  if(val < 0)
  {
    val *= -1;
    firstPin = ARM_RAISE_PIN_2; //9
    secondPin = ARM_RAISE_PIN_1; //10
  }
  
  return rotatePinGroup(firstPin, secondPin, val, modifier);
}

//changes the bicep raise pins at the same rate
//returns true when the bicep has raised to the desired value or has been normalized
bool raiseBicep(int val, float modifier)
{
  return changePinGroup(BICEP_RAISE_PIN_1, BICEP_RAISE_PIN_2, val, modifier);
  //return true;
}

//changes the arm rotate pin 1 to the specified value while inversely changing arm rotate pin2
//returns true when the arm has reached the desired value or has been normalized
bool turnArm(int val, float modifier)
{
  int firstPin = ARM_ROTATE_PIN_1;
  int secondPin = ARM_ROTATE_PIN_2;

  if(val < 0)
  {
    val *= -1;
    firstPin = ARM_ROTATE_PIN_2;
    secondPin = ARM_ROTATE_PIN_1;
  }
  return rotatePinGroup(firstPin, secondPin, val, modifier);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//goes towards the resting value of the turning arm
bool restTurnArm(float modifier)
{
  if(turnArm(pinRest[ARM_ROTATE_PIN_1], modifier))
  {
    Serial.println("inside restTurnarm");
    return changePinValue(ARM_ROTATE_PIN_2, pinRest[ARM_ROTATE_PIN_2], modifier);
  }
  return false;
}

//goes towards the resting value of the turning arm
bool restRaiseArm(float modifier)
{
  if(raiseArm(pinRest[ARM_RAISE_PIN_1], modifier))
  {
    Serial.println("inside restraise arm");
    return changePinValue(ARM_RAISE_PIN_2, pinRest[ARM_RAISE_PIN_2], modifier);
  }
  return false;
}

//goes towards the resting value of the turning arm
bool restRaiseBicep(float modifier)
{
  Serial.println("inside restRaiseBicep");
  if(raiseBicep(pinRest[BICEP_RAISE_PIN_1], modifier))
  {
    bool test = changePinValue(BICEP_RAISE_PIN_2, pinRest[BICEP_RAISE_PIN_2], modifier);

     Serial.println(test);
     Serial.println("");
     Serial.println("Exiting rest raise bicep");
    return test;
    
  }
  Serial.println("Exiting rest raise bicep");
  return false;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//increases one pin at the same rate the other pin decreases
//the given val is the desired val for pinNum1, pinNum2 will do inverse operation
//returns true when the first pin has reached the desired value or has been normalized
bool rotatePinGroup(int pinNum1, int pinNum2, int val, float modifier)
{
  //store the current values of the two pins
  int pin1Val = pinVal[pinNum1];
  int pin2Val = pinVal[pinNum2];

  //get the value to add to the current pin
  int difference = val - pin1Val;

  if(difference == 0)
  {
    //Serial.println(pinVal[pinNum1]);
    //Serial.println("Already at value");
    return true;
  }

  //change the values of the pins
  changePinValue(pinNum2, pin2Val - difference, modifier);
  return changePinValue(pinNum1, pin1Val + difference, modifier);
}

//changes the two given pins at the same rate
//returns true when both pin groups have reached the desired value or has been normalized
bool changePinGroup(int pinNum1, int pinNum2, int val, float modifier)
{
  //change both pins by the given parameters
  bool val1 = changePinValue(pinNum1, val, modifier);
  bool val2 = changePinValue(pinNum2, val, modifier);

 

  return val1 && val2;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//changes the pin to the specified modifier
//returns true when the pin has reached the desired value or has been normalized
bool changePinValue(int pinNum, int val, float modifier)
{
  int currentVal = pinVal[pinNum];
  bool returnVal = false;
  
  //check if the destination value is below or aboove the current value
  if(val < currentVal)
  {
    //subtract by the base rate and multiply by the modifier
    currentVal -= baseRate * modifier;

    //if the step went past the value, just set it to the value
    if(currentVal <= val)
    {
      pinVal[pinNum] = val;
      currentVal = val;
      returnVal = true;
    }
  }
  else if (val > currentVal)
  {
    //add by the base rate and multiply by the modifier
    currentVal += baseRate * modifier;
     //if the step went past the value, just set it to the value
    if(currentVal >= val)
    {
      pinVal[pinNum] = val;
      currentVal = val;
      returnVal = true;
    }
  }
  else
  {
    return true;
  }

  //output to the pin
  //if either of these values is true, true should be returned
  returnVal = writeToPin(pinNum, currentVal) || returnVal;

  Serial.println(returnVal);
  Serial.println("");

  return returnVal;
}

//does an analog write, but checks the max and min nums for the pin
//returns true when the value has been normalized
bool writeToPin(int pinNum, int value)
{
  bool returnVal = false;
  
  //update the value outputted to the pin
  pinVal[pinNum] = value; 

  //make sure the pin is not above or below the values
  returnVal = normalizePin(pinNum);
  
  //write to the pin
  analogWrite(pinNum + actualPinOffset, pinVal[pinNum]);
  Serial.println(pinNum+actualPinOffset, 10);
  Serial.println(pinVal[pinNum], 10);
  
  return returnVal;
}

//corrects any values that are below the min of the pin and above the max of the pin
//returns true when the pin has been normalized
bool normalizePin(int pinNum)
{
  //make sure the output does not go past the extremes for the pin
  if(pinVal[pinNum] >= pinMax[pinNum])
  {
    pinVal[pinNum] = pinMax[pinNum];
    return true;
  }
  else if(pinVal[pinNum] <= pinMin[pinNum])
  {
    pinVal[pinNum] = pinMin[pinNum];
    return true;
  }
  return false;
}
