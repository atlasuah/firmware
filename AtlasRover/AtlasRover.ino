// SonarDriver.ino
// Drives the Rover around and Polls the SONAR sensors

#include <Servo.h>

#define DrivePin      8       // DON'T CHANGE!
#define TurnPin       12      // DON'T CHANGE!

Servo driveservo;
#define DriveArm      1000

#define DriveDefault  1500    // Tare this for Drive Motor
#define DriveMax      1100
#define DriveMin      1900
#define DriveDiff     40

#define TurnDefault   1500    // Tare this for Turn Motor
#define TurnLeft      1300
#define TurnRight     1700
#define TurnDiff      20

boolean autoUpdate = false;
boolean outputCh = false;
char cmd;
char tmp[12];
unsigned int y0, y1, y2, i;


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
        
  Serial.begin(57600);
  pinMode(A0, INPUT);
  pinMode(A1, INPUT);
  pinMode(A2, INPUT);
  
  tmp[0] = '\0';
  
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
     case 'g':
       outputCh = !outputCh;
       break;
     case 'd':
       // drive = DriveDefault + varDrive*DriveDiff;
       
       // turn = TurnDefault + varTurn*TurnDiff;     
       
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
    for (i = 0; i < 5; i++){
      sprintf(tmp, "C[%i] = %u, ", i, APM_RC.OutputCh_current(i));
      Serial.print(tmp);
    }
    Serial.print("\r\n");
  }
}
