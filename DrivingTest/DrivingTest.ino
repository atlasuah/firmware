void setup()  
{
 // Open serial communications and wait for port to open:
  Serial.begin(9600);
   while (!Serial) {
    ; // wait for serial port to connect.
  }
}

void loop()
{
  char cmd;
  
  if (Serial.available() > 0)
  {
    cmd = char(Serial.read());
    Serial.println(cmd);
    
    switch (cmd)
    {
     case 'F':
       // Drive forward
       break;
       
     case 'H':
       // Halt
       break;
     
     case 'B':
       // Drive back
       break;
     
     case 'L':
       // Turn Left
       break;
     
     case 'S':
       // Turn Straight
       break;
     
     case 'R':
       // Turn Right
       break;
       
     default:
       break;
    }
  }
}
