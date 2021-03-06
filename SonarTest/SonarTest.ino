boolean autoUpdate = false;
char cmd;
double y0, y1, y2;

void setup()
{
  Serial.begin(9600);
  pinMode(A0, INPUT);
  pinMode(A1, INPUT);
  pinMode(A2, INPUT);
  
  while (!Serial){;}  // Wait for serial connection
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
    Serial.print("sf");
    Serial.print(y0/57.89);
    Serial.print("\n");
    
    Serial.print("sl");
    Serial.print(y1/57.89);
    Serial.print("\n");
    
    Serial.print("sr");
    Serial.print(y2/57.89);
    Serial.print("\n");
  }
}
