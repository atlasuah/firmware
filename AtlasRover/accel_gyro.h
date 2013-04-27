//accel_gyro.h
#define chip_select 53

#define SMPRT_DIV 0x19
#define DLPF_CFG 0x1A
#define FS_SEL 0x1B
#define AFS_SEL 0x1C
#define USER_CTRL 0x6A
#define PWR_MGMT_1 0x6B
#define WHO_AM_I 0x75

void regWrite(int reg, int data)
{
  digitalWrite(chip_select, LOW);
  SPI.transfer(reg);
  SPI.transfer(data);
  digitalWrite(chip_select, HIGH);
}

uint8_t regRead(int reg)
{
  uint8_t dump, ret;
  uint8_t addr = reg | 0x80;
  digitalWrite(chip_select, LOW);
  dump = SPI.transfer(addr);
  ret = SPI.transfer(0x00);
  digitalWrite(chip_select, HIGH);
  return ret;
}

void initAccelGyro()
{
  SPI.begin();  
  SPI.setClockDivider(SPI_CLOCK_DIV2); 
  SPI.setBitOrder(MSBFIRST);  
  SPI.setDataMode(SPI_MODE0);
  
  pinMode(chip_select, OUTPUT);
  digitalWrite(chip_select, HIGH);
  pinMode(40, OUTPUT);
  digitalWrite(40, HIGH);
  delay(20);
  
  int tries;
  for(tries = 0; tries < 5; tries++)
  {
    regWrite(PWR_MGMT_1, 0x08);     //set the rest bit
    delay(100);                     //wait for reset
    
    regWrite(PWR_MGMT_1, 0x03);     //select gyro z as the clock source
    delay(5);
    
    if(regRead(PWR_MGMT_1) == 0x03) break;     //If reg is set, chip has reset and been set
  }
  regWrite(USER_CTRL, 0x10);
  regWrite(SMPRT_DIV, 0x00);
  regWrite(DLPF_CFG, 0x03);
  regWrite(FS_SEL, 0x00);
  regWrite(AFS_SEL, 0x00); 
}

int accelx()
{
  uint8_t accelx_H = regRead(0x3B);
  uint8_t accelx_L = regRead(0x3C);
  int16_t accelx = accelx_H << 8 | accelx_L;
  return(accelx);
}

int accely()
{
  uint8_t accely_H = regRead(0x3D);
  uint8_t accely_L = regRead(0x3E);
  int16_t accely = accely_H << 8 | accely_L;
  return(accely);
}

int accelz()
{
  uint8_t accelz_H = regRead(0x3F);
  uint8_t accelz_L = regRead(0x40);
  int16_t accelz = accelz_H << 8 | accelz_L;
  return(accelz);
}

int gyrox()
{
  uint8_t gyrox_H = regRead(0x43);
  uint8_t gyrox_L = regRead(0x44);
  int16_t gyrox = gyrox_H << 8 | gyrox_L;
  return gyrox;
}

int gyroy()
{
  uint8_t gyroy_H = regRead(0x45);
  uint8_t gyroy_L = regRead(0x46);
  int16_t gyroy = gyroy_H << 8 | gyroy_L;
  return gyroy;
}

int gyroz()
{
  uint8_t gyroz_H = regRead(0x47);
  uint8_t gyroz_L = regRead(0x48);
  int16_t gyroz = gyroz_H << 8 | gyroz_L;
  return gyroz;
}
