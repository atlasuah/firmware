#include <Servo.h>

#define DrivePin    8       // Don't change
#define SteerPin    12      // Don't change

Servo driveservo;
int currentspeed;
int nospeed = 1500;
#define DriveArm    1000
#define MaxForward  1100    // Max Forward Speed
#define MaxReverse  1900    // Max Reverse Speed

Servo steerservo;
int currentdir;
int straight = 1500;
#define MaxLeft     1300    // Max Left Direction
#define MaxRight    1700    // Max Right Direction

boolean autoUpdate = false;
char cmd = '';
int y0, y1, y2;

void setup()
{
  currentspeed = nospeed;
  digitalWrite(DrivePin, LOW);
  driveservo.attach(DrivePin);
  driveservo.writeMicroseconds(DriveArm);
  delay(1000);
  driveservo.writeMicroseconds(currentspeed);
  
  currentdir = straight;
  steerservo.attach(SteerPin);
  steerservo.writeMicroseconds(currentdir);
  delay(1000);
  
  
  Serial.begin(57600);
  pinMode(A0, INPUT);
  pinMode(A1, INPUT);
  pinMode(A2, INPUT);
  
  while (!Serial){;}  // Wait for serial connection
  
  // Send initial data to software
  //Serial.print("dh\n");
  //Serial.print("ts\n");
}

void loop()
{
  y0 = pulseIn(A0,HIGH);
  y1 = pulseIn(A1,HIGH);
  y2 = pulseIn(A2,HIGH);
  
  if (Serial.available() > 0)
  {
    cmd = char(Serial.read());
  }
  
  switch (cmd)
  {
   case 'u':
     autoUpdate = !autoUpdate;
     break;
   case 'f':              // Drive Forward
     Serial.print("df\n");
     if( currentspeed > MaxForward )
     {
       currentspeed -= .1*(MaxForward-currentspeed);
       driveservo.writeMicroseconds(currentspeed);
       delay(120);
     }
     break;
   case 'b':              // Drive Back
     Serial.print("db\n");
     if( currentspeed < MaxReverse )
     {
       currentspeed += .1*(MaxReverse-currentspeed);
       driveservo.writeMicroseconds(currentspeed);
       delay(120);
     }
     break;
   case 'h':              // Halt
     Serial.print("dh\n");
     if(currentspeed == nospeed)
       break;
     else if(currentspeed > nospeed)
     {
       currentspeed -= .1*(nospeed-currentspeed);
       driveservo.writeMicroseconds(currentspeed);
       delay(120);
     }
     else if(currentspeed < nospeed)
     {
       currentspeed += .1*(nospeed-currentspeed);
       driveservo.writeMicroseconds(currentspeed);
       delay(120);
     }
     break;
   case 'l':              // Turn Left
     Serial.print("tl\n");
     delay(20);
     break;
   case 'r':              // Turn Right
     Serial.print("tr\n");
     delay(20);
     break;
   case 's':              // Turn Straight
     Serial.print("ts\n");
     delay(20);
     break;
   default:
     break;
  }
  
  
  if (autoUpdate)
  {
    Serial.print("sf" + String(y0, DEC) + "\n");
    delay(20);
    
    Serial.print("sl" + String(y1, DEC) + "\n");
    delay(20);
    
    Serial.print("sr" + String(y2, DEC) + "\n");
    delay(20);
  }
}
