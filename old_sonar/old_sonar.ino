void setup()
{
  Serial.begin(9600);
  pinMode(A0, INPUT);
  pinMode(A1, INPUT);
  pinMode(A2, INPUT);
}

void loop()
{
  double y0 = pulseIn(A0,HIGH);
  double y1 = pulseIn(A1,HIGH);
  double y2 = pulseIn(A2,HIGH);
  
  Serial.print("sf");
  Serial.println(y0/57.89);
  
  Serial.print("sl");
  Serial.println(y1/57.89);
  
  Serial.print("sr");
  Serial.println(y2/57.89);
}
