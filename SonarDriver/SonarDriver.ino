boolean autoUpdate = false;
char cmd;
int y0, y1, y2;

void setup()
{
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
    switch (cmd)
    {
     case 'u':
       autoUpdate = !autoUpdate;
       break;
     case 'f':              // Drive Forward
       Serial.print("df\n");
       break;
     case 'b':              // Drive Back
       Serial.print("db\n");
       break;
     case 'h':              // Halt
       Serial.print("dh\n");
       break;
     case 'l':              // Turn Left
       Serial.print("tl\n");
       break;
     case 'r':              // Turn Right
       Serial.print("tr\n");
       break;
     case 's':              // Turn Straight
       Serial.print("ts\n");
       break;
     default:
       break;
    }
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
