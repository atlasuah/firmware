void setup()
{
  Serial.begin(9600);
  pinMode(A0, INPUT);
}

void loop()
{
  double y = pulseIn(A0,LOW);
  Serial.println(y/57.89);
}
