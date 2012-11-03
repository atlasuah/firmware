// -*- Mode: C++; c-basic-offset: 8; indent-tabs-mode: nil -*-

//
// Example code for the FastSerial driver.
//
// This code is placed into the public domain.
//

//
// Include the FastSerial library header.
//
// Note that this causes the standard Arduino Serial* driver to be
// disabled.
//
#include <FastSerial.h>
#include <AP_Common.h>
#include <AP_Math.h>
#include <Arduino_Mega_ISR_Registry.h>
#include <APM_RC.h> // ArduPilot Mega RC Library
#include <MINDSi.h>
#include <Servo.h>

//
// Create a FastSerial driver that looks just like the stock Arduino
// driver.
//
FastSerialPort0(Serial);

Arduino_Mega_ISR_Registry isr_registry;
APM_RC_APM2 APM_RC;

//
// To create a driver for a different serial port, on a board that
// supports more than one, use the appropriate macro:
//
//FastSerialPort2(Serial2);

#define MAX_INPUT_SIZE  63
int inIndex;
int inValue[MAX_INPUT_SIZE];
Servo drive, steer;

void setup(void)
{
        inIndex = 0;
        inValue[0] = 0;
        
        isr_registry.init();
        APM_RC.Init(&isr_registry);          // APM Radio initialization
    
        APM_RC.enable_out(CH_1);
        APM_RC.enable_out(CH_2);
        APM_RC.enable_out(CH_3);
        APM_RC.enable_out(CH_4);
        APM_RC.enable_out(CH_5);
        APM_RC.enable_out(CH_6);
        APM_RC.enable_out(CH_7);
        APM_RC.enable_out(CH_8);
        
        drive.attach(3); //set a pin for the ESC/steering servo to use
        steer.attach(5); 
          
        drive.write(90); //set the output for the ESC/servo
        steer.write(90);
        
        delay(1000); //delay 1 second for arming
        
        //
        // Set the speed for our replacement serial port.
        //
	Serial.begin(115200);
        
        //
        // Test printing things
        //
      /*  Serial.print("test");
        Serial.println(" begin");
        Serial.println(1000);
        Serial.println(1000, 8);
        Serial.println(1000, 10);
        Serial.println(1000, 16);
        Serial.println_P(PSTR("progmem"));
        Serial.printf("printf %d %u %#x %p %f %S\n", -1000, 1000, 1000, 1000, 1.2345, PSTR("progmem"));
        Serial.printf_P(PSTR("printf_P %d %u %#x %p %f %S\n"), -1000, 1000, 1000, 1000, 1.2345, PSTR("progmem"));
      */
        
        Serial.println("Ready.");
        
}

void loop(void)
{
    int    c;

    //
    // Perform a simple loopback operation.
    //
    c = Serial.read();
    if (-1 != c) {
      switch(c) {
        case 127:
        case 8:   if(inIndex == 0)
                    break;
                  else {
                    Serial.write(8);
                    inValue[--inIndex] = 0;
                  }
                  break;
        case 13:  Serial.print("\r\n");
                  if(inValue[0] == 126)
                    doWork();
                  else
                    outputData();
                  inIndex = 0;
                  inValue[inIndex] = 0;
                  break;
        default:  if(inIndex == MAX_INPUT_SIZE-1) {
                    Serial.println("ERROR: Too many items being entered!");
                  }
                  else {
                    Serial.write(c);
                    inValue[inIndex++] = c;
                    inValue[inIndex] = 0;
                  }
                  break;
      }
      
      //Serial.write(c);
    }
}

void outputData(void) {
  // DO STUFF HERE
  for(int i = 0; i < inIndex; i++) {
    Serial.write(inValue[i]);
  }
  Serial.print("\r\n");  // Carriage Return & Line Feed
  // DONE DOING STUFF
  
}

void doWork(void) {
  uint16_t tempWork = 0;
  int i;
  switch(inValue[1]) {    // Examine 2nd char position
    case 67:    // "C" or "c" for Channel
    case 99:  if (!isNumber(inValue[2]) || (isNumber(inValue[2]) && (inValue[2]-'0' > 10 || inValue[2]-'0' < 0))) {
                Serial.println("Error: Expects a valid integer after the C!");
                break;
              }
              i = 4;
              while (isNumber(inValue[i]) && i < inIndex) {
                tempWork = tempWork*10 + (inValue[i++]-'0');
              }
              if (i != inIndex) {
                Serial.println("Error: Expects only integers after the dash!");
                break;
              }
              else {
                APM_RC.OutputCh(inValue[2]-'0', tempWork);
              }
              Serial.printf("C[%c] <-- %i\r\n\r\n",inValue[2],tempWork);
              //Serial.write(inValue[2]);    // DEBUG: Output the channel we're adjusting
              //Serial.println(tempWork,10);
              //Serial.print("\r\n");
              break;
    
    case 71:    // "G" or "g" for Get
    case 103: 
              if (isNumber(inValue[2]) && 2 < inIndex && inValue[2]-'0' < 8) {    // When a valid channel is specified
                Serial.printf("C[%c] = %u\r\n", inValue[2], APM_RC.OutputCh_current(inValue[2]-'0'));
              }
              else {        // When a channel is not specified, get them all
                for (i = 0; i < 11; i++) {
                  Serial.printf("C[%i] = %u, ", i, APM_RC.OutputCh_current(i));
                  if (i == 5 || i == 10)
                    Serial.print("\r\n");
                }
                //Serial.print("\r\n");
              }
              
              break;
    
    case 68:    // "D" or "d" for Drive
    case 100: if (inIndex < 3) {
                Serial.println("Error: Enter the integer!");
                break;
              }
              i = 3;
              if (inValue[i] == 'f') {
                drive.write(100);    // drive forward one second
                delay(1000);
                drive.write(0);    // brake and wait
              }
              else if (inValue[i] == 'r') {
                drive.write(95);   // dissengage brakes
                drive.write(80);   // drive backward one second
                delay(1500);
              }
              else {
                while (isNumber(inValue[i]) && i < inIndex) {
                  tempWork = tempWork*10 + (inValue[i++]-'0');
                }
                // This will be for enting in the actual values... will work out in a bit here
              }
              
              break;
    
    default:
              Serial.println("?");      // ERROR: Uncaught parameter
              break;
  }
}

int isNumber(char val) {
  return (val >= '0' && val <= '9');
}
