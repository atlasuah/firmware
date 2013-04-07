//#include <math.h>
#include <Wire.h>

#define mpu_address 0x68
#define mpu_address_high 0x69

#define PWR_MGMT_1 0x6B
#define WHO_AM_I 0x75

int x_accel, y_accel, z_accel;
int x_gyro, y_gyro, z_gyro;

void setup()
{
  Serial.begin(9600);
  Serial.println("Test Start");
  
  Wire.begin();
  
 /* Wire.begin(mpu_address);
  Wire.write(PWR_MGMT_1);
  Wire.write(0, 1);  // write data bytes
  int error = Wire.endTransmission();
  if(error != 0)
  {
    Serial.print("ERROR on wake: ");
    Serial.println(error);
  }*/
}

void loop()
{
  int id;
  
  Wire.begin(mpu_address);
  Wire.write(WHO_AM_I);
  int n = Wire.endTransmission();
  if(n != 0)
  {
    Serial.print("ERROR on end: ");
    Serial.println(n);
  }
  
  Wire.requestFrom(mpu_address, 1);
  if(1<=Wire.available())
  {
    id = Wire.read();
  }
  
  Serial.println(id);
  delay(500);
}
