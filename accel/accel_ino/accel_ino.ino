#include <SPI.h>

#define chip_select 53

#define SMPRT_DIV 0x19
#define DLPF_CFG 0x1A
#define FS_SEL 0x1B
#define AFS_SEL 0x1C
#define USER_CTRL 0x6A
#define PWR_MGMT_1 0x6B
#define WHO_AM_I 0x75

void regWrite(int reg, int data);
uint8_t regRead(int reg);
void initHardware();

void setup()
{
  Serial.begin(9600);
  
  Serial.println("Test Start");
  
  initHardware();
}


void loop()
{
  int id = regRead(WHO_AM_I);
  Serial.print("Id: ");
  Serial.println(id);
  
  delay(1000);
}

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

void initHardware()
{
  SPI.begin();  // start the SPI library
  SPI.setClockDivider(SPI_CLOCK_DIV2);  // setting SPI at 4Mhz
  SPI.setBitOrder(MSBFIRST);  // data delivered MSB first
  SPI.setDataMode(SPI_MODE0);  // latched on rising edge, transitioned on falling edge, active low
  
  pinMode(chip_select, OUTPUT);
  digitalWrite(chip_select, HIGH);
  pinMode(40, OUTPUT);
  digitalWrite(40, HIGH);
  delay(20);
  
  int tries;
  for(tries = 0; tries < 5; tries++)
  {
    regWrite(PWR_MGMT_1, 0x08);    //set the rest bit
    delay(100);                     //wait for reset
    
    regWrite(PWR_MGMT_1, 0x03);    //select gyro z as the clock source
    delay(5);
    
    if(regRead(PWR_MGMT_1) == 0x03) break;     //If reg is set, chip has reset and been set
  }
  regWrite(USER_CTRL, 0x10);
  regWrite(SMPRT_DIV, 0x00);
  regWrite(DLPF_CFG, 0x03);
  regWrite(FS_SEL, 0x00);
  regWrite(AFS_SEL, 0x00);
  
  
}
