// -*- Mode: C++; c-basic-offset: 8; indent-tabs-mode: nil -*-



#define THISFIRMWARE "ArduRover v2.20b" //New version of the APMrover for the APM v1 or APM v2 and magnetometer + SONAR


// AVR runtime
#include <avr/io.h>
#include <avr/eeprom.h>
#include <avr/pgmspace.h>
#include <math.h>



// Libraries
#include <FastSerial.h>
#include <AP_Common.h>
#include <AP_Menu.h>
#include <Arduino_Mega_ISR_Registry.h>
#include <APM_RC.h>         // ArduPilot Mega RC Library
//#include <AP_GPS.h>         // ArduPilot GPS library
//#include <I2C.h>			// Wayne Truchsess I2C lib
#include <SPI.h>			// Arduino SPI lib
#include <AP_Semaphore.h>   // for removing conflict between optical flow and dataflash on SPI3 bus
#include <DataFlash.h>      // ArduPilot Mega Flash Memory Library
#include <AP_ADC.h>         // ArduPilot Mega Analog to Digital Converter Library
#include <AP_AnalogSource.h>// ArduPilot Mega polymorphic analog getter
#include <AP_PeriodicProcess.h> // ArduPilot Mega TimerProcess
//#include <AP_Baro.h>        // ArduPilot barometer library
//#include <AP_Compass.h>     // ArduPilot Mega Magnetometer Library
#include <AP_Math.h>        // ArduPilot Mega Vector/Matrix math Library
//#include <AP_InertialSensor.h> // Inertial Sensor (uncalibated IMU) Library
//#include <AP_AHRS.h>         // ArduPilot Mega DCM Library
#include <PID.h>            // PID library
#include <RC_Channel.h>     // RC Channel Library
//#include <AP_RangeFinder.h>	// Range finder library
//#include <Filter.h>			// Filter library
//#include <AP_Buffer.h>      // FIFO buffer library
//#include <ModeFilter.h>		// Mode Filter from Filter library
//#include <AverageFilter.h>	// Mode Filter from Filter library
//#include <AP_Relay.h>       // APM relay
//#include <AP_Mount.h>		// Camera/Antenna mount
//#include <GCS_MAVLink.h>    // MAVLink GCS definitions
//#include <AP_Airspeed.h>    // needed for AHRS build
#include <memcheck.h>


//
// Example code for the FastSerial driver.
//
// This code is placed into the public domain.
//

//
// Include the FastSerial library header.
//
// Note that this causes the standard Arduino Serial* driver to be
// disabled.
//
/*
#include <FastSerial.h>
#include <AP_Common.h>
#include <AP_Math.h>
#include <Arduino_Mega_ISR_Registry.h>
#include <APM_RC.h> // ArduPilot Mega RC Library
#include <MINDSi.h>
//#include <Servo.h>
#include <RC_Channel.h>
#include <AP_TimerProcess.h>
#include <AP_ADC.h>
*/

// Configuration
#include "config.h"

// Local modules
#include "defines.h"
#include "Parameters.h"
#include "GCS.h"

//#include "Parameters.h"
//#include "APM_Config_Rover.h"
//#include "GCS.h"


// this sets up the parameter table, and sets the default values. This
// must be the first AP_Param variable declared to ensure its
// constructor runs before the constructors of the other AP_Param
// variables
AP_Param param_loader(var_info, WP_START_BYTE);

//AP_Param::show_all();

Arduino_Mega_ISR_Registry isr_registry;

#if CONFIG_APM_HARDWARE == APM_HARDWARE_APM2
    APM_RC_APM2 APM_RC;
#else
    APM_RC_APM1 APM_RC;
#endif

/*
#if CONFIG_APM_HARDWARE == APM_HARDWARE_APM2
AP_Semaphore spi3_semaphore;
DataFlash_APM2 DataFlash(&spi3_semaphore);
#else
DataFlash_APM1 DataFlash;
#endif
*/

// Global parameters are all contained within the 'g' class.
//
// static Parameters      g;


////////////////////////////////////////////////////////////////////////////////
// prototypes
static void update_events(void);



static AP_ADC_ADS7844          adc;

// we always have a timer scheduler
AP_TimerProcess timer_scheduler;



#if CONFIG_PITOT_SOURCE == PITOT_SOURCE_ADC
AP_AnalogSource_ADC pitot_analog_source( &adc,
                        CONFIG_PITOT_SOURCE_ADC_CHANNEL, 1.0);
#elif CONFIG_PITOT_SOURCE == PITOT_SOURCE_ANALOG_PIN
AP_AnalogSource_Arduino pitot_analog_source(CONFIG_PITOT_SOURCE_ANALOG_PIN, 4.0);
#endif


///////////////////////////////////////////////////////////////////////////////
// Global variables
////////////////////////////////////////////////////////////////////////////////

// APM2 only
#if USB_MUX_PIN > 0
static bool usb_connected;
#endif

static const char *comma = ",";

static const char* flight_mode_strings[] = {
	"Manual",
	"Circle",
	"Learning",
	"",
	"",
	"FBW_A",
	"FBW_B",
	"",
	"",
	"",
	"Auto",
	"RTL",
	"Loiter",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	""};

/* Radio values
		Channel assignments
			1   Ailerons (rudder if no ailerons)
			2   Elevator
			3   Throttle
			4   Rudder (if we have ailerons)
			5   Aux5
			6   Aux6
			7   Aux7
			8   Aux8/Mode
		Each Aux channel can be configured to have any of the available auxiliary functions assigned to it.
		See libraries/RC_Channel/RC_Channel_aux.h for more information
*/


static int16_t rc_override[8] = {0,0,0,0,0,0,0,0};
static bool rc_override_active = false;
//
// Create a FastSerial driver that looks just like the stock Arduino
// driver.
//
FastSerialPort0(Serial);


//APM_RC_APM2 APM_RC;

//
// To create a driver for a different serial port, on a board that
// supports more than one, use the appropriate macro:
//
//FastSerialPort2(Serial2);

#define MAX_INPUT_SIZE  63
int inIndex;
int inValue[MAX_INPUT_SIZE];

void setup(void)
{
        inIndex = 0;
        inValue[0] = 0;
        
        
        //
	// Initialize the ISR registry.
	//
    isr_registry.init();

    //
	// Initialize the timer scheduler to use the ISR registry.
	//

    timer_scheduler.init( & isr_registry );

	//
	// Check the EEPROM format version before loading any parameters from EEPROM.
	//


        
        
        
        
      adc.Init(&timer_scheduler);      // APM ADC library initialization
        
        
        
        
        rc_override_active = APM_RC.setHIL(rc_override);		// Set initial values for no override

        RC_Channel::set_apm_rc( &APM_RC ); // Provide reference to RC outputs.
        isr_registry.init();
        
        APM_RC.Init(&isr_registry);          // APM Radio initialization
    
        APM_RC.enable_out(CH_1);
        APM_RC.enable_out(CH_2);
        APM_RC.enable_out(CH_3);
        APM_RC.enable_out(CH_4);
        APM_RC.enable_out(CH_5);
        APM_RC.enable_out(CH_6);
        APM_RC.enable_out(CH_7);
        APM_RC.enable_out(CH_8);

        APM_RC.OutputCh(CH_1, 	1500);					// Initialization of servo outputs
	APM_RC.OutputCh(CH_2, 	1500);
	APM_RC.OutputCh(CH_3, 	1000);
	APM_RC.OutputCh(CH_4, 	1500);
	APM_RC.OutputCh(CH_5, 	1500);
	APM_RC.OutputCh(CH_6, 	1500);
	APM_RC.OutputCh(CH_7,   1500);
        APM_RC.OutputCh(CH_8,   2000);

        //timer_scheduler.set_failsafe(failsafe_check);
        
        //drive.attach(3); //set a pin for the ESC/steering servo to use
        //steer.attach(5); 
          /*
        drive.write(90); //set the output for the ESC/servo
        steer.write(90);
        */
        delay(1000); //delay 1 second for arming
        
        //
        // Set the speed for our replacement serial port.
        //
	Serial.begin(115200);
        
        //
        // Test printing things
        //
      /*  Serial.print("test");
        Serial.println(" begin");
        Serial.println(1000);
        Serial.println(1000, 8);
        Serial.println(1000, 10);
        Serial.println(1000, 16);
        Serial.println_P(PSTR("progmem"));
        Serial.printf("printf %d %u %#x %p %f %S\n", -1000, 1000, 1000, 1000, 1.2345, PSTR("progmem"));
        Serial.printf_P(PSTR("printf_P %d %u %#x %p %f %S\n"), -1000, 1000, 1000, 1000, 1.2345, PSTR("progmem"));
      */
        
        Serial.println("Ready.");
        
}

void loop(void)
{
    int    c;

    //
    // Perform a simple loopback operation.
    //
    c = Serial.read();
    if (-1 != c) {
      switch(c) {
        case 127:
        case 8:   if(inIndex == 0)
                    break;
                  else {
                    Serial.write(8);
                    inValue[--inIndex] = 0;
                  }
                  break;
        case 13:  Serial.print("\r\n");
                  if(inValue[0] == 126)
                    doWork();
                  else
                    outputData();
                  inIndex = 0;
                  inValue[inIndex] = 0;
                  break;
		case 'u': updateSonar();
				  inIndex = 0;
				  inValue[inIndex] = 0;
				  break;
        default:  if(inIndex == MAX_INPUT_SIZE-1) {
                    Serial.println("ERROR: Too many items being entered!");
                  }
                  else {
                    Serial.write(c);
                    inValue[inIndex++] = c;
                    inValue[inIndex] = 0;
                  }
                  break;
      }
      
      //Serial.write(c);
    }
}

void outputData(void) {
  // DO STUFF HERE
  for(int i = 0; i < inIndex; i++) {
    Serial.write(inValue[i]);
  }
  Serial.print("\r\n");  // Carriage Return & Line Feed
  // DONE DOING STUFF
  
}

void updateSonar(void) {
  
  
  
}

void doWork(void) {
  uint16_t tempWork = 0;
  int i;
  switch(inValue[1]) {    // Examine 2nd char position
    case 67:    // "C" or "c" for Channel
    case 99:  if (!isNumber(inValue[2]) || (isNumber(inValue[2]) && (inValue[2]-'0' > 10 || inValue[2]-'0' < 0))) {
                Serial.println("Error: Expects a valid integer after the C!");
                break;
              }
              i = 4;
              while (isNumber(inValue[i]) && i < inIndex) {
                tempWork = tempWork*10 + (inValue[i++]-'0');
              }
              if (i != inIndex) {
                Serial.println("Error: Expects only integers after the dash!");
                break;
              }
              else {
                APM_RC.OutputCh(inValue[2]-'0', tempWork);
              }
              Serial.printf("C[%c] <-- %i\r\n\r\n",inValue[2],tempWork);
              //Serial.write(inValue[2]);    // DEBUG: Output the channel we're adjusting
              //Serial.println(tempWork,10);
              //Serial.print("\r\n");
              break;
    
    case 71:    // "G" or "g" for Get
    case 103: 
              if (isNumber(inValue[2]) && 2 < inIndex && inValue[2]-'0' < 8) {    // When a valid channel is specified
                Serial.printf("C[%c] = %u\r\n", inValue[2], APM_RC.OutputCh_current(inValue[2]-'0'));
              }
              else {        // When a channel is not specified, get them all
                for (i = 0; i < 10; i++) {
                  Serial.printf("C[%i] = %u, ", i, APM_RC.OutputCh_current(i));
                  if (i == 5 || i == 10)
                    Serial.print("\r\n");
                }
                //Serial.print("\r\n");
              }
              
              break;
     case 73:
     case 105:
             if (isNumber(inValue[2]) && 2 < inIndex && inValue[2]-'0' < 8) {    // When a valid channel is specified
                Serial.printf("Ch[%c] = %u\r\n", inValue[2], APM_RC.InputCh(inValue[2]-'0'));
              }
              else {        // When a channel is not specified, get them all
                Serial.print("Radio Inputs:\r\n");
                for (i = 0; i < 10; i++) {
                  Serial.printf("Ch[%i] = %u, ", i, APM_RC.InputCh(i));
                  if (i == 5 || i == 10)
                    Serial.print("\r\n");
                }
                //Serial.print("\r\n");
              }
              
              break;
    
/*    case 68:    // "D" or "d" for Drive
    case 100: if (inIndex < 3) {
                Serial.println("Error: Enter the integer!");
                break;
              }
              i = 3;
              if (inValue[i] == 'f') {
                drive.write(100);    // drive forward one second
                delay(1000);
                drive.write(0);    // brake and wait
              }
              else if (inValue[i] == 'r') {
                drive.write(95);   // dissengage brakes
                drive.write(80);   // drive backward one second
                delay(1500);
              }
              else {
                while (isNumber(inValue[i]) && i < inIndex) {
                  tempWork = tempWork*10 + (inValue[i++]-'0');
                }
                // This will be for enting in the actual values... will work out in a bit here
              }
              
              break;
    */
    default:
              Serial.println("?");      // ERROR: Uncaught parameter
              break;
  }
}

int isNumber(char val) {
  return (val >= '0' && val <= '9');
}
