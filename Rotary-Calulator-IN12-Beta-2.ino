//
#define VERSION "1.5-beta-IN-12/15"
// #define DEBUG
#ifdef DEBUG
#define DEBUG_PRINT(x) Serial.print(x)
#define DEBUG_PRINTLN(x) Serial.println(x)
#define DEBUGDISPLAYTIME 5000
#else
#define DEBUG_PRINT(x) 
#define DEBUG_PRINTLN(x)
#define DEBUGDISPLAYTIME 9999999
#endif  


#define HOOK A3     // hook pin
#define ROTARY A2   // rotary pin
#define LED A1      // LED pin 

// State
#define HOME 0      // Current State is HOME
#define SLEEP 1
#define ENTRY1 2
#define ENTRY2 3
#define TOTAL 4
#define FINAL 5
#define OVERFLOW 6  // Overflow state 

// Operations
#define ADD 0       // Operation value is an Add 
#define SUBTRACT 1
#define MULTIPLY 2
#define DIVIDE 3
#define PERCENT 4

// CHaracters 
// Note that these values represent the Display NUMBERS (not pins) of the iN-12 as mapped to the IN-15. 
// The #define name description matches an IN-15A and the value is the NUMERIC value of the IN-12B

#define MICRONCHAR 0    // Micron Character is mapped to IN-12 number 0
#define NCHAR 1        // n Character is mapped to IN-12 number 1
#define PERCENTCHAR 2    // % Character is mapped to IN-12 pin 2
#define PICHAR 3      // Pi Character is mapped to IN-12 pin 3
#define KCHAR 4       // K Character is mapped to IN-12 pin 4
#define MULTIPLYCHAR 5       // M Character is mapped to IN-12 pin 5
#define MCHAR 6       // m Character is mapped to IN-12 pin 6
#define PLUSCHAR 7    // + Character is mapped to IN-12 pin 7
#define MINUSCHAR 8    // - Character is mapped to IN-12 pin 8
#define PCHAR 9       // P Character is mapped to IN-12 pin 9
#define DOTCHAR 10    // . Character is mapped to IN-12 pin 10

#define DIVIDECHAR PICHAR
#define OVERFLOWCHAR MICRONCHAR // Overflow is Micron character
#define resetArray NumberArray[0]=0; NumberArray[1]=0; NumberArray[2]=0; NumberArray[3]=0; 
#define bumpState lastCalcState=calcState; ++calcState;


const int debounceDelay = 10; // this is for rotary debounce - old value was 10
const int fdebounceDelay = 15; // this is for flash hook
const long hookSleep = 120000; // 2 minute idle goes to sleep 
const int hookLong = 1400;
const int hookMedium = 700;
const int hookShort = 10;
 
const int ledOff = 0;
const int ledLow = 100;
const int ledMedium = 150;
const int ledHigh = 200;
const int blinkDelay = 1000;  // sleep led blink length
const int dialHasFinishedRotatingAfterMs = 140; // was 100
const int sleepDisplaySpeed = 30;
const int flickerPulse = 3;

// note: Becuase there are only pins A0-A5 available, the actual dimming will not work. 
// I just left the code in in case at some point, I will free up some other pins for PWM stealing from the arduinix. 

int sleeper = 0; // pulsing led value
boolean trigger = false;
boolean needToPrint = false;
int count;

int lastState = LOW;
int trueState = LOW;
long lastStateChangeTime = 0;
int lastflash = LOW;
int flastState = LOW;
int ftrueState = LOW;
long flastStateChangeTime = 0;
long sleepTimer = millis();
int flastflash = LOW;
int cleared = 0; 

//int debounceDelay = 15; //was 10 for rotary
//int fdebounceDelay = 10; // for flash hook
int operation = ADD;
int calcState = ENTRY1; // Set to beginning state - HOME, ENTRY1, ENTRY2, TOTAL are current states
int lastCalcState = HOME;  // Was set to SLEEP
int operand1 = 0;
int operand2 = 0;
int result = 0;
long debugTimer = 0;

long millNow = millis();

long hookTimer = 0;

  int NumberArray[6] = {0, 0, 0, 0, 0, 0};

// Arduinix 4 tub board
// v1.0
//
// This code runs a 4 bulb tube board setup and displays a prototype clock setup.
// NOTE: the delay is setup for IN-17 nixie bulbs.
//
// by Jeremy Howa - Heavily modified by M. Keith Moore for testing 4 character nixies.
// www.robotpirate.com
// www.arduinix.com
// 2009
//
// Note: Anod pin 3 is not used with this tube board but the code sets it up.
//
// Anod to number diagram
//
//
//          num array position
//            0   1   2   3
// Anod 0     #           #
// Anod 1         #   #
//
// Anod 1  Array #0 Colon 1
// Anod 0  Array #0 Colon 2

// SN74141 : Truth Table
//D C B A #
//L,L,L,L 0
//L,L,L,H 1
//L,L,H,L 2
//L,L,H,H 3
//L,H,L,L 4
//L,H,L,H 5
//L,H,H,L 6
//L,H,H,H 7
//H,L,L,L 8
//H,L,L,H 9

// SN74141 (1)


int ledPin_0_a = 2;
int ledPin_0_b = 3;
int ledPin_0_c = 4;
int ledPin_0_d = 5;
// SN74141 (2)
int ledPin_1_a = 6;
int ledPin_1_b = 7;
int ledPin_1_c = 8;
int ledPin_1_d = 9;

// anod pins
int ledPin_a_1 = 10;
int ledPin_a_2 = 11;
int ledPin_a_3 = 12;
int ledPin_a_4 = 13;
boolean randomFlag = false;

void setup()
{
  pinMode(ledPin_0_a, OUTPUT);
  pinMode(ledPin_0_b, OUTPUT);
  pinMode(ledPin_0_c, OUTPUT);
  pinMode(ledPin_0_d, OUTPUT);

  pinMode(ledPin_1_a, OUTPUT);
  pinMode(ledPin_1_b, OUTPUT);
  pinMode(ledPin_1_c, OUTPUT);
  pinMode(ledPin_1_d, OUTPUT);

  pinMode(ledPin_a_1, OUTPUT);
  pinMode(ledPin_a_2, OUTPUT);
  pinMode(ledPin_a_3, OUTPUT);
  pinMode(ledPin_a_4, OUTPUT);


  Serial.begin(9600); // OPen debug terminal
  Serial.print("Arduinix rotary phone dialer toy version - "); Serial.println(VERSION);
  // NOTE:
  // Grounding on pints 14 and 15 will set the Hour and Mins.
  pinMode( 14, INPUT ); // set the vertual pin 14 (pin 0 on the analog inputs )
  digitalWrite(14, HIGH); // set pin 14 as a pull up resistor.

  pinMode( 15, INPUT ); // set the vertual pin 15 (pin 1 on the analog inputs )
  digitalWrite(15, HIGH); // set pin 15 as a pull up resistor.
  pinMode(ROTARY, INPUT);

  analogWrite (LED,ledOff);

  sleepTimer = millis();  // prime the sleep timer to current
  
}

////////////////////////////////////////////////////////////////////////
//
// DisplayNumberSet
// Use: Passing anod number, and number for bulb 1 and bulb 2, this function
//      looks up the truth table and opens the correct outs from the arduino
//      to light the numbers given to this funciton (num1,num2).
//      On a 6 nixie bulb setup.
//
// Change to handle only one number at a time for testing purposes - MKM
//
////////////////////////////////////////////////////////////////////////
// void DisplayNumberSet( int anod, int num1, int num2 )
void DisplayNumberSet( int anod, int num1, int num2 )
{
  int anodPin;
  int a, b, c, d;

  // set defaults.
  a = 0; b = 0; c = 0; d = 0; // will display a zero.
  anodPin =  ledPin_a_1;     // default on first anod.

  // Select what anod to fire.
  switch ( anod )
  {
    case 0:    anodPin =  ledPin_a_1;    break;
    case 1:    anodPin =  ledPin_a_2;    break;
    case 2:    anodPin =  ledPin_a_3;    break;
    case 3:    anodPin =  ledPin_a_4;    break;
  }

  // Load the a,b,c,d.. to send to the SN74141 IC (1)
  switch ( num1 )
  {
    case 0: a = 0; b = 0; c = 0; d = 0; break;
    case 1: a = 1; b = 0; c = 0; d = 0; break;
    case 2: a = 0; b = 1; c = 0; d = 0; break;
    case 3: a = 1; b = 1; c = 0; d = 0; break;
    case 4: a = 0; b = 0; c = 1; d = 0; break;
    case 5: a = 1; b = 0; c = 1; d = 0; break;
    case 6: a = 0; b = 1; c = 1; d = 0; break;
    case 7: a = 1; b = 1; c = 1; d = 0; break;
    case 8: a = 0; b = 0; c = 0; d = 1; break;
    case 9: a = 1; b = 0; c = 0; d = 1; break;
    default: break;  // used to no-op the number in the array
  }

  // Write to output pins.
  digitalWrite(ledPin_0_d, d);
  digitalWrite(ledPin_0_c, c);
  digitalWrite(ledPin_0_b, b);
  digitalWrite(ledPin_0_a, a);

  // Load the a,b,c,d.. to send to the SN74141 IC (2)
  switch ( num2 )
  {
    case 0: a = 0; b = 0; c = 0; d = 0; break;
    case 1: a = 1; b = 0; c = 0; d = 0; break;
    case 2: a = 0; b = 1; c = 0; d = 0; break;
    case 3: a = 1; b = 1; c = 0; d = 0; break;
    case 4: a = 0; b = 0; c = 1; d = 0; break;
    case 5: a = 1; b = 0; c = 1; d = 0; break;
    case 6: a = 0; b = 1; c = 1; d = 0; break;
    case 7: a = 1; b = 1; c = 1; d = 0; break;
    case 8: a = 0; b = 0; c = 0; d = 1; break;
    case 9: a = 1; b = 0; c = 0; d = 1; break;
    default: break;
  }

  // Write to output pins
  digitalWrite(ledPin_1_d, d);
  digitalWrite(ledPin_1_c, c);
  digitalWrite(ledPin_1_b, b);
  digitalWrite(ledPin_1_a, a);

  // Turn on this anod.
  digitalWrite(anodPin, HIGH);

  // Delay
  // NOTE: With the difference in Nixie bulbs you may have to change
  //       this delay to set the update speed of the bulbs. If you
  //       dont wait long enough the bulb will be dim or not light at all
  //       you want to set this delay just right so that you have
  //       nice bright output yet quick enough so that you can multiplex with
  //       more bulbs.
  delay(3);

  // Shut off this anod.
  digitalWrite(anodPin, LOW);
}


////////////////////////////////////////////////////////////////////////
//
// DisplayNumberString
// Use: passing an array that is 8 elements long will display numbers
//      on a 6 nixie bulb setup.
//
////////////////////////////////////////////////////////////////////////
void DisplayNumberStringSingle( int* array )
{
 // bank 1 (bulb 0,3)
  DisplayNumberSet(0,array[0],array[3]);   
  // bank 2 (bulb 1,2)
  DisplayNumberSet(1,array[1],array[2]);
  if (array[4]<1) DisplayNumberSet(3,array[4],11);  // 11 is a dummy value to ignore in the code (fall-thru case/switch value)
  if (array[5]<1) DisplayNumberSet(2,11,array[5]); // this colon is turned on if 0  

}
////////////////////////////////////////////////////////////////////////
void DisplayNumberString( int* array )
{
  // bank 1 (bulb 0,3)
  DisplayNumberSet(0, array[0], array[3]);
  // bank 2 (bulb 1,2)
  DisplayNumberSet(1, array[1], array[2]);
  if (array[4] < 1) DisplayNumberSet(3, array[4], 11); // 11 is a dummy value to ignore in the code (fall-thru case/switch value)
  if (array[5] < 1) DisplayNumberSet(2, 11, array[5]); // this colon is turned on if 0
}

// DisplayNumberSet(2,0,1); // colon 1
// DisplayNumberSet(3,0,1); // colon 2


// Defines
long MINS = 60;         // 60 Seconds in a Min.
long HOURS = 60 * MINS; // 60 Mins in an hour.
long DAYS = 24 * HOURS; // 24 Hours in a day. > Note: change the 24 to a 12 for non military time.

long runTime = 0;       // Time from when we started.
//************************************
long previous = 0;
long interval = 1000;
int colon = 1;

//************************************

// default time sets. clock will start at 12:59:00
// NOTE: We start seconds at 0 so we dont need a clock set
//       The values you see here would be what you change
//       if you added a set clock inputs to the board.
long clockHourSet = 23;
long clockMinSet  = 59;

int HourButtonPressed = false;
int MinButtonPressed = false;
/********************************************
   SIngle Digit DIsplays
*/

/********************************************
   Shift Digit DIsplays
*/
void shiftDigits(int digit){  
  long StartDuration = (millis()) / 1000;
  long EachSec = StartDuration;

  long NowTime = (millis() / 1000);
  
//      NumberArray[0] = NumberArray[1];
      NumberArray[1] = NumberArray[2];
      NumberArray[2] = NumberArray[3];
      NumberArray[3] = digit;
      switch(operation){
          case ADD: {  // 0/0
            NumberArray[4] = 1;          //Digit 4, wire 0 (value 1 is off)
            NumberArray[5] = 1;          //Digit 4, wire 0 
            NumberArray[0] = PLUSCHAR;          //Digit 4, wire 0 
            break;
          }
          case SUBTRACT: {  //0/1
            NumberArray[4] = 1;          //Digit 4, wire 0
            NumberArray[5] = 1;          //Digit 4, wire 0
            NumberArray[0] = MINUSCHAR;          //Digit 4, wire 0 
            break; 
          }
          case MULTIPLY: {   // 1/0
            NumberArray[4] = 1;          //Digit 4, wire 0
            NumberArray[5] = 1;          //Digit 4, wire 0
             NumberArray[0] = MULTIPLYCHAR;          //Digit 4, wire 0 
             break; 
          }
          case DIVIDE: {   // 1/1
            NumberArray[4] = 1;          //Digit 4, wire 0
            NumberArray[5] = 1;          //Digit 4, wire 0
            NumberArray[0] = DIVIDECHAR;          //Digit 4, wire 0 
            break;
          }
          case PERCENT: {   // 1/1
            NumberArray[4] = 1;          //Digit 4, wire 0
            NumberArray[5] = 1;          //Digit 4, wire 0
            NumberArray[0] = PERCENTCHAR;          //Digit 4, wire 0 
            break;
          }
          case OVERFLOW: {   // 1/1
            NumberArray[4] = 0;          //Digit 4, wire 0
            NumberArray[5] = 0;          //Digit 4, wire 0
            NumberArray[0] = OVERFLOWCHAR;          //Digit 4, wire 0 
            break;
          }
          
          default: {   // 0/0 
            NumberArray[4] = 1;          //Digit 4, wire 0
            NumberArray[5] = 1;          //Digit 4, wire 0 
            NumberArray[0] = DOTCHAR;          //Digit 4, wire 0 
            break;       
        }; // end switch 
#if debug
      DEBUG_PRINT("All-Array0=");DEBUG_PRINT(NumberArray[0]);DEBUG_PRINT(" - ");
      DEBUG_PRINT("Array1=");DEBUG_PRINT(NumberArray[1]);DEBUG_PRINT(" - ");
      DEBUG_PRINT("Array2=");DEBUG_PRINT(NumberArray[2]);DEBUG_PRINT(" - ");
      DEBUG_PRINT("Array3=");DEBUG_PRINT(NumberArray[3]);DEBUG_PRINT(" -");
      DEBUG_PRINT("Top=");DEBUG_PRINT(NumberArray[4]);DEBUG_PRINT(" - ");
      DEBUG_PRINT("Bottom=");DEBUG_PRINT(NumberArray[5]);DEBUG_PRINTLN(". ");
#endif debug
      while ((EachSec=(millis()/1000) == NowTime)){ 
        DisplayNumberStringSingle( NumberArray );
 //       delay(250); // dummy for test
        };
      };
}

/********************************************
   Shift Digit DIsplays
*/
int enumerate (){  
    
    int d3, d2, d1, d0 = 0;
 //     d3 = (NumberArray[0] * 1000);
      d2 = (NumberArray[1] * 100);
      d1 = d2 + (NumberArray[2] * 10);
      d0 = d1 + NumberArray[3];
//     DEBUG_PRINT("------------------->Enumerated digits="); DEBUG_PRINTLN(d2);DEBUG_PRINTLN(d1);DEBUG_PRINTLN(d0);
      return d0;
} // end of enumerate
void unenumerate(int number){  
 //     int micro = number / 1000 ;
 //     DEBUG_PRINTLN(micro);
      int milli = number  / 100;
 //      DEBUG_PRINTLN(milli);
       
      int deca = (number % 100) / 10;
 //           DEBUG_PRINTLN(deca);
      int digit = (number % 10);
  //          DEBUG_PRINTLN(digit);
            

  // Fill in the Number array used to display on the tubes.
  //int NumberArray[6]={0,0,0,0,0,0};
 // NumberArray[0] = micro;

 
  NumberArray[1] = milli;
  NumberArray[2] = deca;
  NumberArray[3] = digit;
  

   DEBUG_PRINT("All-Array0=");DEBUG_PRINT(NumberArray[0]);DEBUG_PRINT(" - ");
   DEBUG_PRINT("Array1=");DEBUG_PRINT(NumberArray[1]);DEBUG_PRINT(" - ");
   DEBUG_PRINT("Array2=");DEBUG_PRINT(NumberArray[2]);DEBUG_PRINT(" - ");
   DEBUG_PRINT("Array3=");DEBUG_PRINT(NumberArray[3]);DEBUG_PRINT(" -");
   DEBUG_PRINT("Top=");DEBUG_PRINT(NumberArray[4]);DEBUG_PRINT(" - ");
   DEBUG_PRINT("Bottom=");DEBUG_PRINT(NumberArray[5]);DEBUG_PRINTLN(". ");

} // end of unenumerate
int calculate (int operand1,int operand2,int operation){
  int val; 
  
  DEBUG_PRINT("Operation="); DEBUG_PRINT(operation); DEBUG_PRINT(" Operand1="); DEBUG_PRINT(operand1); DEBUG_PRINT(" Operand2="); DEBUG_PRINT(operand2);
  switch (operation){
    case ADD: {
      DEBUG_PRINT("Trying to ADD and I get ->");
      val = (operand1 + operand2);
//      DEBUG_PRINTLN(val);
      if (val > 999) {
        val=-1;};
//      DEBUG_PRINTLN(val);  
      break;
    }
    case SUBTRACT: {
//  DEBUG_PRINTLN("Trying to SUBTRACT");
  if (operand1 >= operand2) {
        val=operand1-operand2;
      } else
        val = -1; // negative number
      break;
    }
    case DIVIDE: {
 // DEBUG_PRINTLN("Trying to DIVIDE");
  if ((operand2 <1) || (operand2 > operand1)) {
        val = -1;
      } else
        val = round(operand1/operand2);
      break;
    }
    case MULTIPLY: {
 //       DEBUG_PRINTLN("Trying to MULTIPLY");
      val=operand1*operand2;
      if (val > 999) val = -1;
      break;
    }
    case PERCENT: {
 // DEBUG_PRINTLN("Trying to PERCENT");
  float result = 0;
  if ((operand2 <1)) {
        val = -1;
      } else 
      {
   //     result = operand2/100;
   //     Serial.print(result);Serial.print(" percent=");
        result = ((operand1*operand2)/100);
        Serial.print(result);
        val = round(result);
      };
  //    if ((val < 1) || (val > 999)) val = -1; // Check for over/underflow. 
      break;
    }
    default : {
 //     DEBUG_PRINTLN("DEFAULTING in calculate!");
      val=000;
      break;
      }

  };

  DEBUG_PRINT("------------------> Result=>"); DEBUG_PRINTLN(val);

    return val; 
}; // end of calculate
// SLeep 
void sleep(){
  int  jump1, jump2 = 0;
  int freading = digitalRead(HOOK);
 // freading = digitalRead(HOOK);
  lastState = freading;
  DEBUG_PRINTLN("Inside of Sleep.");
  DEBUG_PRINT("LastState="); DEBUG_PRINT(lastState);DEBUG_PRINT(" freading="); DEBUG_PRINTLN(freading);
  
  while (lastState == freading){
//    DEBUG_PRINTLN("In the display loop.");
//    freading = digitalRead(HOOK);
    jump1 = jump2 = 0;
    resetArray;
    for (jump2=0 ; jump2 <= 10 ; ++jump2){
 //   if (freading=digitalRead(HOOK) != lastState) {jump2 = 14; jump1 = 11; Serial.println("I see a hook!");};
    for (jump1=0 ; jump1 <= 13 ; ++jump1){
      NumberArray[0]=jump2;
      NumberArray[1]=jump1%10;
      NumberArray[2]=(jump1+1)%10;
      NumberArray[3]=(jump1+2)%10;
      for (int ii=0; ii < sleepDisplaySpeed; ii++) {
        DisplayNumberString(NumberArray);
 //       if (freading != lastState) {jump2 = 14; jump1 = 11; Serial.println("I still see a hook!");};
      } 
      if (freading=digitalRead(HOOK) != lastState) return; 
      delay (flickerPulse);  // element flicker is bigger the biggger the value
      //freading = digitalRead(HOOK); 
    }  
    } // end of jump2 increment
  }
  calcState=HOME;
  trigger = true;
  lastState = freading;
}
////////////////////////////////////////////////////////////////////////
void loop()
{
  int reading = digitalRead(ROTARY);
  int freading = digitalRead(HOOK);
  int digit = 0; // holds the current end-state decimal 0-9 digit received from rotary
  boolean flash = false;


  if (reading != lastState) {
      lastState=reading; // Set last state to current state  
//      sleepTimer = try with and without ************************
      lastStateChangeTime = millis();
      delay (debounceDelay);  // debounce 
//      DEBUG_PRINT("tick..");
     }
 
  if ((millis() - lastStateChangeTime) > dialHasFinishedRotatingAfterMs) {// the dial isn't being dialed, or has just finished being dialed.
    if (needToPrint) {// if it's only just finished being dialed, we need to send the number down the serial
      // line and reset the count. We mod the count by 10 because '0' will send 10 pulses.
        if (calcState == TOTAL || calcState == HOME){  /// we finished the last calulation so we assume a new one
          analogWrite(LED,ledLow);
          resetArray;
          operand1 = operand2 = 0;
          calcState = ENTRY1;
        };
        needToPrint = false;
        digit=count;
        count = 0;
        cleared = 0;
        shiftDigits(digit);  // shift and display the current value. 
      }; // end need to print
    }; // end finished rotary dial read
  
  if (reading != lastState) {
      lastState = reading; // lastState is reset upon a change
      sleepTimer = lastStateChangeTime = millis();
      delay (debounceDelay*2);
  //    DEBUG_PRINTLN("time change");
     }
  
  if ((millis() - lastStateChangeTime) > debounceDelay) {// debounce - this happens once it's stablized
    if (reading != trueState) {// this means that the switch has either just gone from closed->open or vice versa.
      trueState = reading;

      if (trueState == HIGH) {// increment the count of pulses if it's gone high.
        count++;needToPrint = true; // we'll need to print this number (once the dial has finished rotating)
        count = count % 10;
      DEBUG_PRINTLN(count);
      }
    }
// Check the flash button now. 
    if (freading != flastState) { 
          flastState = freading; // lastState is reset upon a change
          sleepTimer = flastStateChangeTime = millis();
    //      DEBUG_PRINT("ZReset.");
          delay (fdebounceDelay);  // debounce 
  //        DEBUG_PRINT("hick..");
    };
    if ((millis() - flastStateChangeTime) > fdebounceDelay) {// the dial isn't being dialed, or has just finished being dialed.
      if (freading != ftrueState) {// this means that the switch has either just gone from closed->open or vice versa.
          ftrueState = freading;
      if (ftrueState == HIGH) {// increment the count of pulses if it's gone high.
  //       DEBUG_PRINTLN("Hook+");
         hookTimer = millis();
         if (calcState == ENTRY1){
          analogWrite(LED,ledLow);
         };
      } else
      {   // then it must be a rest back to off. Flash was on but now it is off. 
  //      if (operation == OVERFLOW) operation = ADD; // Reset operation if we overflowed. 
        if ((flastStateChangeTime - hookTimer) > hookLong){
   //       DEBUG_PRINTLN("Long Hook");
          calcState = HOME;
          analogWrite(LED,ledLow);
          trigger = true; 
        } else
        if ((flastStateChangeTime - hookTimer) > hookMedium){
  //        DEBUG_PRINT("Medium hook - changed operation to ");
            if (++operation > DIVIDE) { // shift to the next operation
              operation = ADD; // rest to ADD    
            };
  //          DEBUG_PRINTLN(operation);
        } else
        if ((flastStateChangeTime - hookTimer) > hookShort){
  //        DEBUG_PRINTLN("Short hook");

          if (calcState == ENTRY1) {
            operand1 = enumerate();
            bumpState;
            resetArray;
  //          analogWrite(LED,ledMedium);
            operand2 = 0; 
   //         DEBUG_PRINT("Entry1 operand1="); DEBUG_PRINTLN(operand1);   
          } else
          if (calcState == ENTRY2) {
              operand2 = enumerate();
              bumpState;
              result = 0;
              trigger = true;
  //            DEBUG_PRINT("Entry2 operand2="); DEBUG_PRINTLN(operand2);     
          };
        }; // short hook
      } // else flash length logic
    }
    } // flash was detected
  };
 //  End of flash analysis
 //       DEBUG_PRINT("Sleepy time="); DEBUG_PRINT(sleepTimer); DEBUG_PRINT(" Timer val="); DEBUG_PRINTLN(millis()-sleepTimer); DEBUG_PRINT(" hookSleep val="); DEBUG_PRINTLN(hookSleep);delay(500);
       
        if ((millis() - sleepTimer) > hookSleep){  
//          DEBUG_PRINT("We will sleep"); DEBUG_PRINT(sleepTimer); DEBUG_PRINT(" Timer val="); DEBUG_PRINTLN(millis()-sleepTimer);   
            calcState=SLEEP; 
            trigger = true; 
        };

  if (trigger){
    trigger = false; 
//    DEBUG_PRINTLN("trigger reset");
    switch (calcState) {
      default: {
        break;
      }
      case SLEEP: { // Sleep
        sleep();
        DEBUG_PRINTLN("Waken!");
        sleepTimer = millis();
        calcState = HOME;
        trigger = true;
        break;
      } // sleep case
      case TOTAL: {
//        DEBUG_PRINT("Totals executed...");
        if ((result = calculate (operand1,operand2,operation)) >= 0){  // this is a good result
        DEBUG_PRINTLN(result);
        unenumerate (result);                                              
  //      calcState = ENTRY1; 
        analogWrite(LED,ledHigh);
        break;
        } else   // this is an overflow result
        {
          DEBUG_PRINTLN(">Total value overflow.<");
          operand1 = operand2 = operation = result = 0;
          operation = OVERFLOW;
          trigger = true;
          unenumerate (result);  // Will be zero 
    //      calcState = ENTRY1; // try again?
          break;
        }
      } // end of totals
      case HOME: {
//        DEBUG_PRINTLN("WE ARE HOME");
        operand1 = operand2 = operation = result = 0;
        resetArray;
        calcState = ENTRY1;
        analogWrite(LED,ledLow);
        trigger = false;
        break;
      }
      case OVERFLOW: {
        operand1 = operand2 = operation = result = 0;
        unenumerate(result);
        analogWrite(LED,ledLow);
//        DEBUG_PRINTLN("Overflow reset.");
        calcState = HOME;
        break;
      }
  
    }; // switch calcState
  }; // trigger
    switch(operation){
          case ADD: {  // 0/0
            NumberArray[4] = 1;          //Digit 4, wire 0 (value 1 is off)
            NumberArray[5] = 1;          //Digit 4, wire 0
            NumberArray[0] = PLUSCHAR;          //Digit 4, wire 0             
            break;
          }
          case SUBTRACT: {  //0/1
            NumberArray[4] = 1;          //Digit 4, wire 0
            NumberArray[5] = 1;          //Digit 4, wire 0
            NumberArray[0] = MINUSCHAR;          //Digit 4, wire 0            
            break; 
          }
          case MULTIPLY: {   // 1/0
            NumberArray[4] = 1;          //Digit 4, wire 0
            NumberArray[5] = 1;          //Digit 4, wire 0
            NumberArray[0] = MULTIPLYCHAR;          //Digit 4, wire 0            
            break; 
          }
          case DIVIDE: {   // 1/1
            NumberArray[4] = 1;          //Digit 4, wire 0
            NumberArray[5] = 1;          //Digit 4, wire 0
            NumberArray[0] = DIVIDECHAR;          //Digit 4, wire 0            
            break;
          }
          case PERCENT: {   // 1/1
            NumberArray[4] = 1;          //Digit 4, wire 0
            NumberArray[5] = 1;          //Digit 4, wire 0
            NumberArray[0] = PERCENTCHAR;          //Digit 4, wire 0 
            break;
          }
          case OVERFLOW: {   // 1/1
            NumberArray[4] = 0;          //Digit 4, wire 0
            NumberArray[5] = 0;          //Digit 4, wire 0
            NumberArray[0] = OVERFLOWCHAR;          //Digit 4, wire 0 
 //           DEBUG_PRINTLN("Overflow display set");  
  //          calcState = HOME;       operand1 = operand2 = operation = result = 0;
            break;
          }
          default: {   // 0/0 
            NumberArray[4] = 1;          //Digit 4, wire 0
            NumberArray[5] = 1;          //Digit 4, wire 0 
            NumberArray[0] = DOTCHAR;          //Digit 4, wire 0 
            break;
          }        
    }; // end switch 

    DisplayNumberString(NumberArray);
  #ifdef DEBUG  
  if (millis() - debugTimer > DEBUGDISPLAYTIME ){   // every 10 seconds
    debugTimer = millis();
    DEBUG_PRINTLN("++++++++++++++++");
    DEBUG_PRINTLN("Debug Display");
    DEBUG_PRINTLN("=============");
    DEBUG_PRINT("Operation="); DEBUG_PRINTLN(operation);
    DEBUG_PRINT("Operand1="); DEBUG_PRINTLN(operand1);
    DEBUG_PRINT("Operand2="); DEBUG_PRINTLN(operand2);
    DEBUG_PRINT("Result="); DEBUG_PRINTLN(result);
    DEBUG_PRINT("calcState="); DEBUG_PRINTLN(calcState);
    DEBUG_PRINT("lastCalcState="); DEBUG_PRINTLN(lastCalcState);
    DEBUG_PRINT("Array 0=");DEBUG_PRINT(NumberArray[0]);DEBUG_PRINT(" - ");
    DEBUG_PRINT("1=");DEBUG_PRINT(NumberArray[1]);DEBUG_PRINT(" - ");
    DEBUG_PRINT("2=");DEBUG_PRINT(NumberArray[2]);DEBUG_PRINT(" - ");
    DEBUG_PRINT("3=");DEBUG_PRINT(NumberArray[3]);DEBUG_PRINT(" -");
    DEBUG_PRINT("Top=");DEBUG_PRINT(NumberArray[4]);DEBUG_PRINT(" - ");
    DEBUG_PRINT("Bottom=");DEBUG_PRINT(NumberArray[5]);DEBUG_PRINTLN(". ");
    DEBUG_PRINTLN("++++++++++++++++");
  };  // end of debug display
#endif DEBUG
} // End of loop
