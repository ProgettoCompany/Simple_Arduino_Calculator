// Simple Arduino calculator made using an I2C LCD and the One Pin Keypad
// Created by John Wolf for Progetto Company
// Licensed under the MIT license.

#include <String.h>
#include <Wire.h> 
// Liquid Crystal Library can be found here: https://github.com/fdebrabander/Arduino-LiquidCrystal-I2C-library
#include <LiquidCrystal_I2C.h>
#include <OnePinKeypad.h>

// Define Analog Pin:
#define OPK_PIN A0

// Create a keypad object:
OnePinKeypad keypad(OPK_PIN);

// Set the LCD address to 0x27 for a 16 chars and 2 line display
LiquidCrystal_I2C lcd(0x27, 16, 2);

// String to store the equation
String equationStr = "";

// Variable to store the calculated result
double calcResult = 0.0;

// Used for clearing display for the next equation
bool clearDisplay = false;

// Declare buttons used for calculator
const char DIV_BTN = 'A';
const char MUL_BTN = 'B';
const char ADD_BTN = 'C';
const char SUB_BTN = 'D';
const char DEC_BTN = '*';
const char EQL_BTN = '#';


// Used to map the keypad buttons to the calculator operators
char keyValToCalcVal(char keyVal) {
  char calcVal = keyVal;
  switch (keyVal) {
    case DIV_BTN:
      calcVal = '/';
      break;
    case MUL_BTN:
      calcVal = '*';
      break;
    case ADD_BTN:
      calcVal = '+';
      break;
    case SUB_BTN:
      calcVal = '-';
      break;
    case DEC_BTN:
      calcVal = '.';
      break;
    case EQL_BTN:
      calcVal = '=';
      break;
  }
  return calcVal;
}

// Helper function to update the calculated result as the equationStr is parsed 
void updateCalcResult(int opInd, int i) {
  // Parse the number from the equationStr based on the operator index
  String numStr = equationStr.substring(opInd + 1, i);
  double num = numStr.toDouble();
  Serial.print("Retrieved number: ");
  Serial.println(num);
  // If the operator index is -1, this is the first operator and the first number
  if (opInd == -1) {
    calcResult = num;
  }
  // Otherwise update calcResult using the operator at opInd
  else {
    switch(equationStr[opInd]) {
      case '/':
        calcResult /= num;
        break;
      case '*':
        calcResult *= num;
        break;
      case '+':
        calcResult += num;
        break;
      case '-':
        calcResult -= num;
        break;
    }
  }
  Serial.print("Current result: ");
  Serial.println(calcResult);
}

// Function to set the calculated result, uses updateCalcResult()
void setCalcResult() {
  calcResult = 0.0;
  int opInd = -1;
  // Loop through the equationStr
  for (int i = 0; i < equationStr.length(); i++) {
    char equationChar = equationStr[i];
    Serial.print("Current character: ");
    Serial.println(equationStr[i]);

    // Check for operators
    if (isDigit(equationChar) == false && equationChar != '.') {
      // Update calcResult, then opInd
      updateCalcResult(opInd, i);   
      Serial.println("Updated opInd.");
      opInd = i;
    }
  }
  updateCalcResult(opInd, equationStr.length());
}

void setup() {
  // Begin Serial communication for debugging
  Serial.begin(9600);

  // Initialize the LCD
  lcd.begin();
  
  // Initialize the cursor
  lcd.cursor();

  // One Pin Keypad thresholds calibrated using the calibrateThresholds example sketch
  int myThresholds[16] = {10, 88, 155, 212, 253, 299, 339, 376, 400, 433, 459, 484, 503, 524, 544, 562};
  keypad.useCalibratedThresholds(myThresholds);
}

void loop() {
  // Variable to store button being pressed:
  char keyVal;

  // Run the readKeypadWithTimeout function to determine which button is pressed within no timeout
  // Store the button pressed in the variable keyVal
  keyVal = keypad.readKeypadWithTimeout(NO_TIMEOUT);

  // Check if the display should be cleared
  // Occurs after when another key has been pressed
  // after the equals sign operator has been used
  if (clearDisplay) {
    Serial.println("Time to clear the display!");
    equationStr = "";
    lcd.clear();
    lcd.setCursor(0, 0);
    clearDisplay = false;
  }

  // Retrieve the calculator operator and print it to 
  // the Serial Monitor
  char calcVal = keyValToCalcVal(keyVal);
  Serial.print("Pressed key: ");
  Serial.println(calcVal);

  // If the equals sign button has been pressed retrieve
  // the calculated result and print it on the second line
  // of the LCD
  if (calcVal == '=') {
    setCalcResult();
    lcd.setCursor(0, 1);
    lcd.noAutoscroll();
    lcd.print('=');
    lcd.print(calcResult);
    clearDisplay = true;
  }
  // Otherwise, add the pressed key to equationStr and print it
  // to the LCD, toggling autoscroll as necessary
  else {
    equationStr += calcVal;
    if (equationStr.length() > 16) {
      lcd.autoscroll();
    }
    else {
      lcd.noAutoscroll();
    }
    lcd.print(calcVal);
  }
}
