// AtlasRover.ino
// Main firmware for rover
// Team 4 | CPE 496-01 | Spring 2013

#include <Servo.h>
#include <Wire.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>
#include <SPI.h>
#include "compassFunctions.h"
#include "accel_gyro.h"

Servo driveServo;
#define DrivePin      8       // DON'T CHANGE!
#define DriveArm      1000
#define DriveDefault  1500    // Tare this for Drive Motor
#define DriveMax      1100
#define DriveMin      1900
#define DriveDiff     40
#define DriveDelay    0

Servo turnServo;
#define TurnPin       12      // DON'T CHANGE!
#define TurnDefault   1460    // Tare this for Turn Motor   (was 1500)
#define TurnLeft      1100
#define TurnRight     1900
#define TurnDiff      40
#define TurnDelay     0

#define SonarFront    A0
#define SonarLeft     A1
#define SonarRight    A2

#define EncoderPin    3

boolean sendHeartbeat = true;    // Default to always sending heartbeat
char cmd, amt, neg;
char tmp[42];
String cmdRead[10];
String readInStr;
String command;
char readInChar;
int encoderCount;

unsigned int i, aDrive, aTurn, nDrive, nTurn;
int t0, t1, t2;
int driveCount = 10;
int driveDir = 0;
float heading;

const int CMD_MIN_SIM = 3;
const int CMD_MIN_CNT = 6;

void setup()
{
  
  cli();                                // Disable all interrupts.
  MCUSR &= ~(1<<WDRF);			// Clear WDRF if it has been unintentionally set. 
  WDTCSR = (1<<WDCE )|(1<<WDE );	// Enable configuration change. 
  WDTCSR = (1<<WDIF)|(1<<WDIE)|	        // Enable Watchdog Interrupt Mode. 
    (1<<WDCE )|(0<<WDE )|	        // Disable Watchdog System Reset Mode if unintentionally enabled. 

        // Set Watchdog Timeout period to 32 ms.
        //(0<<WDP3 )|(0<<WDP2 )|(0<<WDP1)|(1<<WDP0);
        
        // Set Watchdog Timeout period to 64 ms.
        //(0<<WDP3 )|(0<<WDP2 )|(1<<WDP1)|(0<<WDP0);
        
        // Set Watchdog Timeout period to 128 ms.
        //(0<<WDP3 )|(0<<WDP2 )|(1<<WDP1)|(1<<WDP0);
        
        // Set Watchdog Timeout period to 256 ms or ~.25 sec.
        (0<<WDP3 )|(1<<WDP2 )|(0<<WDP1)|(0<<WDP0);
	
  
  sei();                                // Enable all interrupts.
  
  digitalWrite(DrivePin, LOW);
  driveServo.attach(DrivePin);
  driveServo.writeMicroseconds(DriveArm);
  delay(1000);
  driveServo.writeMicroseconds(DriveDefault);
  
  turnServo.attach(TurnPin);
  turnServo.writeMicroseconds(TurnDefault);
  delay(1000);

  encoderCount = 0;  
    
  Serial.begin(57600);
  pinMode(SonarFront, INPUT);    // Front sonar
  pinMode(SonarLeft, INPUT);     // Left sonar
  pinMode(SonarRight, INPUT);    // Right sonar
  pinMode(A4, OUTPUT);           // Initial sonar trigger
  pinMode(EncoderPin, INPUT);    // Encoder
  
  attachInterrupt(0, encoderTick, RISING);
  
  tmp[0] = '\0';
  cmd = amt = neg = '\0';
  aDrive = aTurn = nDrive = nTurn = 0;
  t0 = t1 = t2 = 0;
  
  CompassSetup();
  initAccelGyro();
  heading = getHeading();
  
  while (!Serial){;}  // Wait for serial connection
  
  digitalWrite(A4, HIGH);  // Trigger sonar's to begin polling
  delay(20);
  pinMode(A4, INPUT);      // Return to High Impedence state
}

String DetermineCmd(String pD)
{
  String retcmd = "";
  String tmpcmd = "";
  String tstr   = "";
  String basecmd = "";
  String cmdlist[10];
  char c = pD[0];
  int start = 0,
      cmdindex = 0,
      difcount = 0,
      checkindex = 0,
      maxCmdIndex = 0,
      numCmds = 0;
  int simcnt = 0;
  
  bool checkDone = false;

  // Get list of cmds
  while (pD.length() >= 8)
  {
    while (start < (pD.length() - 1) && pD[start] != '<')
      start++;

    // go ahead and trim the front of the string (it was garbage)
    tstr = pD.substring(start);
    pD = tstr;
    start = 0;
    		
    if ((start + 8) <= pD.length())
    {
      tmpcmd = pD.substring(start, 8);
      if (tmpcmd.indexOf('<', 1) == -1)
      {
        if (tmpcmd.indexOf('>', 7) != -1)
        {
          cmdlist[cmdindex] = tmpcmd;
          cmdindex++;
          tstr = pD.substring(start + 8);
          pD = tstr;
        }
        else
        {
          tstr = pD.substring(start + 1);
          pD = tstr;
        }
      }
      else
      {
        tstr = pD.substring(start + 1);
        pD = tstr;
      }
      start = 0;
    }
  }
  numCmds = cmdindex;
  maxCmdIndex = cmdindex - 1;
  
  // Check each cmd
  if (numCmds >= CMD_MIN_CNT)
  {
    basecmd = cmdlist[0];
    while (!checkDone)
    {
      for (int i = 0; i <= maxCmdIndex; i++)
      {
        if (basecmd == cmdlist[i])
          simcnt++;
      }
      
      if (simcnt >= CMD_MIN_SIM)      // If not enough other commands are similar
      {
        retcmd = basecmd;
        checkDone = true;
      }
      else if (checkindex <= maxCmdIndex)
      {
        basecmd = cmdlist[checkindex];
        checkindex++;
        simcnt = 0;
      }
      else
        return "FAIL - No commands passed similarity test";
    }
  }
  else
    return "FAIL - Not enough commands received (" + String(numCmds) + ")";
  
  return retcmd;
}

void loop()
{
  if (Serial.available() > 0)
  {
    readInStr = "";
    delay(10);    // Without this, it will read in one character then exit while loop
    while (Serial.available() > 0)
    {
      readInChar = Serial.read();
      readInStr.concat(readInChar);
    }
    command = DetermineCmd(readInStr);
    if (command[0] == 'F') {
      // It failed!
    }
    else {
      wdt_reset();              //Call this to reset the timer's value.
      
      aDrive = (command[3]-'0');
      if (command[2] == '+')    // NOT A MISTAKE!!!
        aDrive *= -1;
      
      aTurn = (command[6]-'0');
      if (command[5] == '-')
        aTurn *= -1;
      
      nDrive = DriveDefault + aDrive*DriveDiff;
      nTurn = TurnDefault + aTurn*TurnDiff;
      driveServo.writeMicroseconds(nDrive);    // Set speed servo
      turnServo.writeMicroseconds(nTurn);      // Set turn servo
      delay(TurnDelay);
    }
    
    if (sendHeartbeat) {
      t0 = analogRead(SonarFront);  // Read front sonar
      t1 = analogRead(SonarLeft);   // Read left sonar
      t2 = analogRead(SonarRight);  // Read right sonar
      driveDir = (int)getHeading(); // Get compass heading
      cli();    // Disable all interrupts
      int encoderDelta = encoderCount;  // Copy current encoder value
      encoderCount = 0;                 // Reset current encoder value
      sei();    // Enable all interrupts
      // fwd left right encoder direction accelX accelY accelZ gyroX gyroY gyroZ
      sprintf(tmp, "%i_%i_%i_%i_%d_%i_%i_%i_%i_%i_%i\r\n",
              t0, t1, t2, encoderDelta, driveDir, accelx(), accely(),accelz(),
	      gyrox(), gyroy(), gyroz());
      Serial.print(tmp);    // Send string to GUI
    }
  }
}

// Encoder interrupt function
void encoderTick()
{
  if(digitalRead(EncoderPin))
    encoderCount++;    // Forward motion, increment counter
  else
    encoderCount--;    // Reverse motion, decrement counter
}

// Watchdog timer function
ISR(WDT_vect)
{
  driveServo.writeMicroseconds(DriveDefault);
  turnServo.writeMicroseconds(TurnDefault);
  wdt_reset();              //Call this to reset the timer's value.
}
