//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//                                                                              //
// Joystick Computer Control with Scheme Bank                                          //
// Created by: Aaron Weinstein                                                  //
// Company: BrightLogic                                                         //
// Date: 7/9/2012                                                               //
// Description: This program enables control of a computer mouse and keyboard   //
// from two 4-directional joysticks, with the ability to adjust control schemes //
// and cycle between these schemes easily                                       //
// Hardware: Two 4-direction joysticks, Arduino Leonardo, USB cable             //
//                                                                              // 
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//

// All arrays stored as:
//{lUp, lDown, lLeft, lRight, rUp, rDown, rLeft, rRight}

const int joyStickPins[8] = {2, 3, 4, 5, 6, 7, 8, 9};


// Bank of arrays to store 8 joyStrokes 
// Four types of joyStrokes: MouseX(x), MouseY(y), Keyboard(k), Setup(s)
// Every Scheme corresponds to a type array and val array

// Vals are either increment values or ASCII key numbers
// Types are: s for scheme changes, k for keyboard, y for mouse movement in y-axis
// x for mouse movement in x-axis, and c for clicks
// CAPITAL TYPES ARE FOR REPEATED ACTIONS i.e. HOLDING THE KEY OR BUTTON
// lower case types are for single actions, if held they will repeat but with
// pauses in between, like clicking really fast
// recommend capital for clicks and lower for keyboard, but both will work

// Mouse directions: Y: + is down, - is up. X: - is left, + is right
// Click Types: l = left, r = right, m = middle

// Any joystick motion can correspond to any action

char  joyBankTypes1[8]   = {'s', 'k', 'k', 'k', 'y', 'y', 'x', 'x'};
int   joyBankVals1[9]    = {  1, 'a', 'b', 'c',  1,  -1,  -1,   1,  1}; //red
char  joyBankTypes2[8]   = {'s', 'k', 'k', 'k', 'y', 'y', 'x', 'x'};
int   joyBankVals2[9]    = {  1, 'A', 'B', 'C',  1,  -1,  -1,   1,  5}; //purple
char  joyBankTypes3[8]   = {'s', 'k', 'k', 'k', 'y', 'y', 'x', 'x'};
int   joyBankVals3[9]    = {  1,  217, 216, 215,  1,  -1,  -1,  1,  10}; //yellow
char  joyBankTypes4[8]   = {'s', 'c', 'c', 'c', 'y', 'y', 'x', 'x'};
int   joyBankVals4[9]    = {  1, 'l', 'r', 'm',   5,  -5,  -5,  5,  20}; //green


// Used to store which scheme is in use, in reference to bank

int currentScheme = 1; 
char currentSchemeTypes[8];
int currentSchemeVals[9];


int mouseToMove[3]; // Temporary storage before mouse movements, used for diagonals


// Use to see button state and change of state

int oldReading[8];
int reading[8];

void setup()
{
  for(int a = 0; a <= 8; a++)
  {
    pinMode(joyStickPins[a], INPUT_PULLUP); //setup buttons as inputs
    currentSchemeTypes[a] = joyBankTypes1[a]; //set initial scheme to 1
    currentSchemeVals[a] = joyBankVals1[a];
  }
  Mouse.begin();
  Keyboard.begin();
  Serial.begin(9600);
  pinMode(11, OUTPUT); // set up LEDs
  pinMode(12, OUTPUT);
  pinMode(13, OUTPUT);
  digitalWrite(13, 0); // turn on initial scheme color
  digitalWrite(12, 1);
  digitalWrite(11, 1);
}

void loop()
{
  if(digitalRead(10) == LOW)
  {
    readStick(); //reads and updates reading memory
    findType(); //determines action to be performed
    moveMouse(); //moves mouse
  }
  if(digitalRead(10) == HIGH) //fail safe, in case leonardo goes rogue
  {
    Serial.println("waiting");
    delay(100);
  }
}

void readStick()
{
  for(int b = 0; b <= 7; b++)
  {
    oldReading[b] = reading[b]; //send past values back
    reading[b] = digitalRead(joyStickPins[b]); //update with new ones
  }
}

void findType() //figures out what to do and sends to that function
{
  for(int c = 0; c <= 7; c++)
  {
    if((currentSchemeTypes[c] > 90) && (reading[c] == 1)) //lowercase letter, and on
    {
      switch (currentSchemeTypes[c])
      {
        case 's':  //setup
          if(oldReading[c] == 0) //only goes once when the button is pressed, won't 
          {                      //cycle if held
            doSetup(c);          //changes the scheme
          }
          break;
        case 'y':  //mouse y
          addMouseY(c);  //adds to the y portion of mouseToMove
          break;
        case 'x':  //mouse x
          addMouseX(c);  //adds to the x portion of mouseToMove
          break;
        case 'c':  // click
          doClick(c);  //clicks once
        case 'k':  // keyboard
          doKeyboard(c);  //presses the right key once
          break;
      }
    }
    if((currentSchemeTypes[c] <= 90) && (reading[c] == 1) && (oldReading[c] == 0))
    {  //capital letter and switched on
      switch (currentSchemeTypes[c])
      {
        case 'K': //keyboard
          startKey(c); //presses key
          break;
        case 'C':  //click
          startClick(c); //presses mouse
          break;
      }
    }
    if((currentSchemeTypes[c] <= 90) && (reading[c] == 0) && (oldReading[c] == 1))
    { //capital letter and switched off
      switch (currentSchemeTypes[c])
      {
        case 'K':  //keyboard
          stopKey(c);  //releases key
          break;
        case 'C':
          stopClick(c);  //releases mouse
          break;
      }
    }
  }
}

void doSetup(int valLocation)  //changes scheme
{
  currentScheme++;  // change scheme reference number
  switch (currentScheme)
  {
    case 1:  
      for(int d = 0; d <= 8; d++)  //update all parts of both "current" arrays
      {
        currentSchemeTypes[d] = joyBankTypes1[d];
        currentSchemeVals[d] = joyBankVals1[d];
      }
      digitalWrite(13, 1); //change LED color
      digitalWrite(12, 0);
      digitalWrite(11, 0);
      break;
    case 2:  
      for(int d = 0; d <= 8; d++)  //update all parts of both "current" arrays
      {
        currentSchemeTypes[d] = joyBankTypes2[d];
        currentSchemeVals[d] = joyBankVals2[d];
      }
      digitalWrite(13, 0); //change LED color
      digitalWrite(12, 1);
      digitalWrite(11, 0);
      break;
    case 3:  
      for(int d = 0; d <= 8; d++)  //update all parts of both "current" arrays
      {
        currentSchemeTypes[d] = joyBankTypes3[d];
        currentSchemeVals[d] = joyBankVals3[d];
      }
      digitalWrite(13, 0); //change LED color
      digitalWrite(12, 0);
      digitalWrite(11, 1);
      break;
    case 4:  
      for(int d = 0; d <= 8; d++)  //update all parts of both "current" arrays
      {
        currentSchemeTypes[d] = joyBankTypes4[d];
        currentSchemeVals[d] = joyBankVals4[d];
      }
      digitalWrite(13, 1); //change LED color
      digitalWrite(12, 0);
      digitalWrite(11, 1);
      break;
    case 5:  //identical to 1, for use with 4 schemes, if more then add this on the end
      currentScheme = 1;  //cycle back to 1 
      for(int d = 0; d <= 8; d++)  //perform the function of 1 as well
      {
        currentSchemeTypes[d] = joyBankTypes1[d];
        currentSchemeVals[d] = joyBankVals1[d];
      }
      digitalWrite(13, 0); //change LED color
      digitalWrite(12, 1);
      digitalWrite(11, 1);
      break;   
  }
}

void addMouseY(int valLocation) // add to mouseToMove array in Y axis
{
  mouseToMove[1] += currentSchemeVals[valLocation];  //checks val from scheme
}

void addMouseX(int valLocation)// add to mouseToMove array in X axis
{
  mouseToMove[0] += currentSchemeVals[valLocation]; //checks val from scheme
}

void moveMouse()  // moves mouse based on increments in mouseToMove
{
  if ((mouseToMove[0] != 0) || (mouseToMove[1] != 0)) //check to see that the mouse should move
  {
    Mouse.move(mouseToMove[0], mouseToMove[1], mouseToMove[2]); //moves mouse
  }
  mouseToMove[0] = 0; //reset all values in mouseToMove, or else mouse will run away
  mouseToMove[1] = 0;
  mouseToMove[2] = 0;
}

void doClick(int valLocation) //clicks and releases mouse, for lowercase type
{
  int type = currentSchemeVals[valLocation]; //checks val from scheme
  switch (type)
  {
    case 'l':
      Mouse.click(MOUSE_LEFT);
      break;
    case 'r':
      Mouse.click(MOUSE_RIGHT);
      break;
    case 'm':
      Mouse.click(MOUSE_MIDDLE);
      break;
  }
}

void startClick(int valLocation) //presses mouse, for CAPITAL type
{
  int type = currentSchemeVals[valLocation]; //checks val from scheme
  switch (type) //clicks appropriate button
  {
    case 'l':
      Mouse.press(MOUSE_LEFT);
      break;
    case 'r':
      Mouse.press(MOUSE_RIGHT);
      break;
    case 'm':
      Mouse.press(MOUSE_MIDDLE);
      break;
  }
}

void stopClick(int valLocation) //releases mouse, for CAPITAL type
{
  int type = currentSchemeVals[valLocation]; //checks val from scheme
  switch (type)
  {
    case 'l':
      Mouse.release(MOUSE_LEFT);
      break;
    case 'r':
      Mouse.release(MOUSE_RIGHT);
      break;
    case 'm':
      Mouse.release(MOUSE_MIDDLE);
      break;
  }
}

void doKeyboard(int valLocation)  //clicks and releases key, for lowercase type
{
  Keyboard.write(currentSchemeVals[valLocation]); //checks val from scheme
}

void startKey(int valLocation) //presses key, for CAPITAL type
{
  Keyboard.press(currentSchemeVals[valLocation]); //checks val from scheme
}

void stopKey(int valLocation)  //releases key, for CAPITAL type
{
  Keyboard.release(currentSchemeVals[valLocation]); //checks val from scheme
}
