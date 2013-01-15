// SonarDriver.ino
// Drives the Rover around and Polls the SONAR sensors
// Last updated 1/15/2013 - JLB

#include <Servo.h>

Servo driveServo;
#define DrivePin      8       // DON'T CHANGE!
#define DriveArm      1000
#define DriveDefault  1500    // Tare this for Drive Motor
#define DriveMax      1200
#define DriveMin      1800
#define DriveDiff     30
#define DriveDelay    5

Servo turnServo;
#define TurnPin       12      // DON'T CHANGE!
#define TurnDefault   1500    // Tare this for Turn Motor
#define TurnLeft      1200
#define TurnRight     1800
#define TurnDiff      30
#define TurnDelay     5

boolean debugOutput = false;
boolean autoUpdate = false;
boolean outputCh = false;
char cmd, amt, neg;
char tmp[12];
unsigned int y0, y1, y2, t0, t1, t2, i, aDrive, aTurn, nDrive, nTurn;

////#include <AP_Common.h>
//#include <Arduino_Mega_ISR_Registry.h>
//#include <APM_RC.h> // ArduPilot Mega RC Library
//
//Arduino_Mega_ISR_Registry isr_registry;
//APM_RC_APM2 APM_RC;

void setup()
{
  
//  isr_registry.init();
//  APM_RC.Init(&isr_registry);          // APM Radio initialization
//    
//  APM_RC.enable_out(CH_1);
//  APM_RC.enable_out(CH_2);
//  APM_RC.enable_out(CH_3);
//  APM_RC.enable_out(CH_4);
//  APM_RC.enable_out(CH_5);
//  APM_RC.enable_out(CH_6);
//  APM_RC.enable_out(CH_7);
//  APM_RC.enable_out(CH_8);
  
  digitalWrite(DrivePin, LOW);
  driveServo.attach(DrivePin);
  driveServo.writeMicroseconds(DriveArm);
  delay(1000);
  driveServo.writeMicroseconds(DriveDefault);
  
  turnServo.attach(TurnPin);
  turnServo.writeMicroseconds(TurnDefault);
  delay(1000);
  
    
  Serial.begin(57600);
  pinMode(A0, INPUT);
  pinMode(A1, INPUT);
  pinMode(A2, INPUT);
  
  tmp[0] = '\0';
  
  cmd = amt = neg = '\0';
  
  y0 = y1 = y2 = t0 = t1 = t2 = aDrive = aTurn = nDrive = nTurn = 0;
  
  while (!Serial){;}  // Wait for serial connection
  Serial.print("Welcome back!!!\r\n");
}

void loop()
{
  y0 = pulseIn(A0,HIGH);
  y1 = pulseIn(A1,HIGH);
  y2 = pulseIn(A2,HIGH);
  
  if (Serial.available() > 0)
  {
    cmd = char(Serial.read());
    switch (cmd)
    {
     case 'u':
       autoUpdate = !autoUpdate;
       break;
     case 'b':
       debugOutput = !debugOutput;
       break;
     case 'g':
       outputCh = !outputCh;
       break;
     case 'd':
       neg = char(Serial.read());
       aDrive = 1;        // Initialize aDrive
       switch (neg) {
         case '0':
           aDrive = 0;    // Set to zero
           break;
         case '+':  // NOT A MISTAKE - Positive Sign Here!
           aDrive = -1;   // Set negative
         case '-':    // And Negative Sign Here!
           amt = char(Serial.read());
           if (amt == 'T')
             aDrive *= 10;
           else
             aDrive *= (amt-'0');
           break;
         default:
           aDrive = 0;
           break;
       }
       nDrive = DriveDefault + aDrive*DriveDiff;
       driveServo.writeMicroseconds(nDrive);
       delay(DriveDelay);
       
       if (debugOutput) {
         sprintf(tmp, "DRIVE neg:%c amt:%c Rec:%i New:%i\n\r", neg, amt, aDrive, nDrive);
         Serial.print(tmp);
       }
       break;
     case 't':
       neg = char(Serial.read());
       aTurn = 1;         // Initialize aTurn
       switch (neg) {
         case '0':
           aTurn = 0;     // Set to zero
           break;
         case '-':
           aTurn = -1;    // Set negative
         case '+':
           amt = char(Serial.read());
           if (amt == 'T')
             aTurn *= 10;
           else
             aTurn *= (amt-'0');
           break;
         default:
           aTurn = 0;
           break;
       }
       nTurn = TurnDefault + aTurn*TurnDiff;
       turnServo.writeMicroseconds(nTurn);
       delay(TurnDelay);
       
       if (debugOutput) {
         sprintf(tmp, "TURN Rec: %i   New: %i\n\r", aTurn, nTurn);
         Serial.print(tmp);
       }
       break;
     
//     case 'f':              // Drive Forward
//       Serial.print("df\n");
//       break;
//     case 'b':              // Drive Back
//       Serial.print("db\n");
//       break;
//     case 'h':              // Halt
//       Serial.print("dh\n");
//       break;
//     case 'l':              // Turn Left
//       Serial.print("tl\n");
//       break;
//     case 'r':              // Turn Right
//       Serial.print("tr\n");
//       break;
//     case 's':              // Turn Straight
//       Serial.print("ts\n");
//       break;
     default:
         sprintf(tmp, "ERROR: Got %c\n\r", cmd);
         Serial.print(tmp);
       break;
    }
  }
  if (autoUpdate)
  {
    sprintf(tmp, "sf%u\r\n", y0);
    Serial.print(tmp);
    delay(20);
    
    sprintf(tmp, "sl%u\r\n", y1);
    Serial.print(tmp);
    delay(20);
    
    sprintf(tmp, "sr%u\r\n", y2);
    Serial.print(tmp);
    delay(20);
  }
  if (outputCh)
  {
//    for (i = 0; i < 5; i++){
//      sprintf(tmp, "C[%i] = %u, ", i, APM_RC.OutputCh_current(i));
//      Serial.print(tmp);
//    }
//    Serial.print("\r\n");
  }
}
