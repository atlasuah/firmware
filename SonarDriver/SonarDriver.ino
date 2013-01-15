boolean autoUpdate = false;
char cmd;
char tmp[12];
unsigned int y0, y1, y2;

void setup()
{
  Serial.begin(57600);
  pinMode(A0, INPUT);
  pinMode(A1, INPUT);
  pinMode(A2, INPUT);
  
  tmp[0] = '\0';
  
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
}
