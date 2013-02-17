// SonarDriver.ino
// Drives the Rover around and Polls the SONAR sensors
// Last updated 1/15/2013 - JLB

#include <Servo.h>

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
#define TurnDefault   1500    // Tare this for Turn Motor
#define TurnLeft      1100
#define TurnRight     1900
#define TurnDiff      40
#define TurnDelay     0

boolean debugOutput = true;
boolean autoUpdate = false;
boolean outputCh = false;
char cmd, amt, neg;
char tmp[12];
String cmdRead[10];
String readInStr;
String command;
char readInChar;
unsigned int y0, y1, y2, t0, t1, t2, i, aDrive, aTurn, nDrive, nTurn;
const int CMD_MIN_SIM = 3;
const int CMD_MIN_CNT = 6;

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

String DetermineCmd(String pD)
{
  //int startTime = millis();
  //int endTime = 0;
  String ppD = pD;
  
  
  
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
    return "FAIL - Not enough commands received (" + String(numCmds) + ")" + ppD;
  
  
  //endTime = millis();
  
  return retcmd;
}

void ManageCmd()
{
  cmd = char(Serial.read());
  if (cmd == '<')
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
           aDrive *= (amt-'0');
           break;
         default:
           aDrive = 0;
           break;
       }
       nDrive = DriveDefault + aDrive*DriveDiff;
       //driveServo.writeMicroseconds(nDrive);
       //delay(DriveDelay);
       
       if (debugOutput) {
         sprintf(tmp, "DRIVE neg:%c amt:%c Rec:%i New:%i\n\r", neg, amt, aDrive, nDrive);
         Serial.print(tmp);
       }
       
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
       //driveServo.writeMicroseconds(nDrive);
       //turnServo.writeMicroseconds(nTurn);
       //delay(TurnDelay);
       
       if (debugOutput) {
         sprintf(tmp, "TURN Rec: %i   New: %i\n\r", aTurn, nTurn);
         Serial.print(tmp);
       }
       break;
     default:
         sprintf(tmp, "ERROR: Got %c\n\r", cmd);
         Serial.print(tmp);
       break;
    }
  }
  else
  {
    
  }
}

void loop()
{
  y0 = pulseIn(A0,HIGH);
  y1 = pulseIn(A1,HIGH);
  y2 = pulseIn(A2,HIGH);
  
  if (Serial.available() > 63)
  {
    //cmd = char(Serial.read());
    readInStr = "";
    delay(10);    // Without this, it will read in one character then exit while loop
    while (Serial.available() > 0)
    {
      readInChar = Serial.read();
      readInStr.concat(readInChar);
    }
    command = DetermineCmd(readInStr);
    //command = readInStr;
    delay(20);
    Serial.println(command);
    //ReadSerial();
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
