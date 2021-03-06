#include <SimpleModbusSlave.h>
#include <EEPROM.h>
#include <Bounce2.h>
#include <OneWire.h>

#include <DallasTemperature.h>
#include <dht.h>
#include "common.h"
#include "rgb_class.h"


/* 
   SimpleModbusSlaveV10 supports function 3, 6 & 16.
   
   This example code will receive the adc ch0 value from the arduino master. 
   It will then use this value to adjust the brightness of the led on pin 9.
   The value received from the master will be stored in address 1 in its own
   address space namely holdingRegs[].
   
   In addition to this the slaves own adc ch0 value will be stored in 
   address 0 in its own address space holdingRegs[] for the master to
   be read. The master will use this value to alter the brightness of its
   own led connected to pin 9.
   
   The modbus_update() method updates the holdingRegs register array and checks
   communication.

   Note:  
   The Arduino serial ring buffer is 64 bytes or 32 registers.
   Most of the time you will connect the arduino to a master via serial
   using a MAX485 or similar.
 
   In a function 3 request the master will attempt to read from your
   slave and since 5 bytes is already used for ID, FUNCTION, NO OF BYTES
   and two BYTES CRC the master can only request 58 bytes or 29 registers.
 
   In a function 16 request the master will attempt to write to your 
   slave and since a 9 bytes is already used for ID, FUNCTION, ADDRESS, 
   NO OF REGISTERS, NO OF BYTES and two BYTES CRC the master can only write
   54 bytes or 27 registers.
 
   Using a USB to Serial converter the maximum bytes you can send is 
   limited to its internal buffer which differs between manufactures. 
*/

  
#define SLAVE_ID 3
#define DHTPIN 5     // what pin we're connected to
#define DHTTYPE DHT21    //DHT11   // DHT 11

#define RELAY_ON 1
#define RELAY_OFF 0
#define PIN_HP_SWITCH_1 12
#define PIN_PIR 7
#define RELAY_1_PIN 6
#define HP_TEMP_1_PIN 8
// How many bits to use for temperature values: 9, 10, 11 or 12
#define SENSOR_RESOLUTION 9
// Index of sensors connected to data pin, default: 0
#define SENSOR_INDEX 0

OneWire oneWire(HP_TEMP_1_PIN);
DallasTemperature sensors(&oneWire);
DeviceAddress sensorDeviceAddress;

union Pun {float f; uint32_t u;};  //http://stackoverflow.com/questions/28185196/modbus-floating-point-conversion

dht DHT;

rgb_class oRGB;


// Using the enum instruction allows for an easy method for adding and 
// removing registers. Doing it this way saves you #defining the size 
// of your slaves register array each time you want to add more registers
// and at a glimpse informs you of your slaves register layout.

#define RGB_R_PIN 9
#define RGB_G_PIN 10
#define RGB_B_PIN 11

//*** This block should be common between slave, master, and eeprom tools 
enum EEPROM_STORE_X{
	EEP_DEVICE_ADDR,
	EEP_MAX_SLAVES,
	EEP_HW_PROFILE,
};
/// **** EEPROM BLOCK ENDS



RGB curRGB;
byte curSwitch_1;

boolean last_switch_state = HIGH ; // for the toggle switch

enum EEPROM_STORE{     // build hardware info based on HW Profile from EEPROM
	HP_DEVICE_ADDR,
	HP_MAX_SLAVES,
	HP_TEMP_1,
	HP_RGB_1,
	HP_POT_1,
	HP_RELAY_1,
	HP_PIR_1,
	HP_SWITCH_1,
	HP_DHT11_1,
};


//////////////// registers of your slave ///////////////////
enum SLAVE_RESOURCES
{     
  // just add or remove registers and your good to go...
  // The first register starts at address 0
	HR_HW_PROFILE,
	HR_TEMP_1_MSB, 
	HR_TEMP_1_LSB, 
	HR_POT_1,
	HR_PIR_1,
	HR_SWITCH_1_TOGGLE,
	HR_SWITCH_1_STATUS,
	HR_RGB_1_R,   // OUTPUT PINS
	HR_RGB_1_G,   // OUTPUT PINS
	HR_RGB_1_B,   // OUTPUT PINS
	HR_RELAY_1, // OUTPUT PINS
	HR_DHT11_1_T_MSB,
	HR_DHT11_1_T_LSB,
	HR_DHT11_1_H_MSB,
	HR_DHT11_1_H_LSB,
	HR_CMD_DATA_1,
	HR_CMD_DATA_2,
	HR_CMD_DATA_3,
	HR_CMD_DATA_4,
	HR_CMD_DATA_5,
	HR_CMD_DATA_6,
	HR_CMD_DATA_7,
	HR_CMD_DATA_8,	
	HOLDING_REGS_SIZE // leave this one
  // total number of registers for function 3 and 16 share the same register array
  // i.e. the same address space
};

unsigned int holdingRegs[HOLDING_REGS_SIZE]; // function 3 and 16 register array
////////////////////////////////////////////////////////////
#define MAX_IO 10
boolean cfg_hw[MAX_IO];	

byte hw_address;
byte hw_profile;

unsigned long last_temperature_sample_time;
boolean request_temperature = true;
unsigned long last_dht11_sample_time;
byte curRelayStatus;

Bounce debouncer = Bounce(); 



void setup()
{
	
	oRGB.init();
	oRGB.set_mode(RGB_SHOW);
	
	hw_address =  EEPROM.read (EEP_DEVICE_ADDR);
	hw_profile =  EEPROM.read (EEP_HW_PROFILE);
	fncBuildHwConfiguration(hw_profile);
	
	pinMode(13, OUTPUT);  //LED test
	
	holdingRegs[HR_RGB_1_R] = 0xff;
	holdingRegs[HR_RGB_1_G] = 0xff;
	holdingRegs[HR_RGB_1_B] = 0xff;
	
 
	
	
	if (cfg_hw[HP_TEMP_1])
		{
		  sensors.begin();
		  sensors.getAddress(sensorDeviceAddress, 0);
		  sensors.setResolution(sensorDeviceAddress, SENSOR_RESOLUTION);
		  sensors.setWaitForConversion(false);  // makes it async
		  last_temperature_sample_time = millis();		
		  request_temperature = true; 	//for async operation
		}
	
	if (cfg_hw[HP_DHT11_1]) {
		//dht.begin();
		last_dht11_sample_time = millis();
	}
	
	
	if (cfg_hw[HP_SWITCH_1]){
		// Setup the button with an internal pull-up :
		pinMode(PIN_HP_SWITCH_1,INPUT_PULLUP);
		// After setting up the button, setup the Bounce instance :
		debouncer.attach(PIN_HP_SWITCH_1);
		debouncer.interval(5); // interval in ms
		
		curSwitch_1 = HIGH;
	}
	
	
	
	if (cfg_hw[HP_RGB_1]){
		pinMode(RGB_R_PIN, OUTPUT);
		pinMode(RGB_G_PIN, OUTPUT);
		pinMode(RGB_B_PIN, OUTPUT);	
	}
	
	if(cfg_hw[HP_PIR_1]){
		pinMode(PIN_PIR, INPUT);	
	}
	
	if(cfg_hw[HP_RELAY_1]){
		pinMode(RELAY_1_PIN, OUTPUT);	
		setRelay (RELAY_OFF);
	}
	
	holdingRegs[HR_HW_PROFILE] = hw_profile;		
	
  modbus_configure(&Serial, 38400 , SERIAL_8N1, hw_address, 4, HOLDING_REGS_SIZE, holdingRegs );

  // modbus_update_comms(baud, byteFormat, id) is not needed but allows for easy update of the
  // port variables and slave id dynamically in any function.
  modbus_update_comms(38400, SERIAL_8N1, hw_address);
  

   
}

void loop()
{
  // modbus_update() is the only method used in loop(). It returns the total error
  // count since the slave started. You don't have to use it but it's useful
  // for fault finding by the modbus master.
  
 

  
  
   oRGB.update();
  modbus_update();
  
  
  if (cfg_hw[HP_POT_1]){
	holdingRegs[HR_POT_1] = analogRead(A2); // update data to be read by the master to adjust the PWM
  }
  
  
  //analogWrite(LED, holdingRegs[PWM_VAL]>>2); // constrain adc value from the arduino master to 255//TJ

  /* Note:
     The use of the enum instruction is not needed. You could set a maximum allowable
     size for holdinRegs[] by defining HOLDING_REGS_SIZE using a constant and then access 
     holdingRegs[] by "Index" addressing. 
     I.e.
     holdingRegs[0] = analogRead(A0);
     analogWrite(LED, holdingRegs[1]/4);
  */


	if (cfg_hw[HP_TEMP_1])
	{
		if ((last_temperature_sample_time - millis() ) > 1000) {
			last_temperature_sample_time= millis();
		//holdingRegs[HR_TEMP_1_MSB] = millis();  //TJ TEST ONLY
		if (request_temperature) {
			sensors.requestTemperatures();
			request_temperature = false;
		}
		else {
			encodeFloat(&holdingRegs[HR_TEMP_1_MSB],sensors.getTempCByIndex(SENSOR_INDEX)) ;
			request_temperature = true;
		}
		
			
		}
	}


if (cfg_hw[HP_DHT11_1]) 
	{
		if ( (millis() - last_dht11_sample_time  ) > 2000) {
			
		int chk = DHT.read21(DHTPIN);
				float t = -99;
				float h = -99;		
		switch (chk)
		{
			case DHTLIB_OK:
				//Serial.print("OK,\t");
				t = DHT.temperature;
				h = DHT.humidity;
				encodeFloat(&holdingRegs[HR_DHT11_1_T_MSB],t) ;
				encodeFloat(&holdingRegs[HR_DHT11_1_H_MSB],h) ;				
				break;

//#if(0)
//			case DHTLIB_ERROR_CHECKSUM:
				//Serial.print("Checksum error,\t");
//				break;
//			case DHTLIB_ERROR_TIMEOUT:
				//Serial.print("Time out error,\t");
//				break;
//			case DHTLIB_ERROR_CONNECT:
				//Serial.print("Connect error,\t");
//				break;
//			case DHTLIB_ERROR_ACK_L:
				//Serial.print("Ack Low error,\t");
//				break;
//			case DHTLIB_ERROR_ACK_H:
				//Serial.print("Ack High error,\t");
//				break;
//#endif				
			default:
				//Serial.print("Unknown error,\t");
				encodeFloat(&holdingRegs[HR_DHT11_1_T_MSB],t+chk) ;
				encodeFloat(&holdingRegs[HR_DHT11_1_H_MSB],h+chk) ;				
				break;
		}			
			

		last_dht11_sample_time= millis();
			
		}
	}	


	if(cfg_hw[HP_PIR_1]){
		holdingRegs[HR_PIR_1]=  digitalRead(PIN_PIR);
	}
	
	
	
	update_input_switch_1();
	
	scan_for_command();  //check if  HR_CMD_DATA_1 registor > 0 
	
	

	oRGB.update();
 
}


	
void fncBuildHwConfiguration(byte thisProfile){
		
		for (int n=0;n<MAX_IO;n++)
		{
			cfg_hw[n]=false;
		}
		
		switch(thisProfile){
			case 0:
				break;
			case 1:
				break;
			case 2:
				cfg_hw[HP_RGB_1]=true;
				cfg_hw[HP_POT_1]=true;
				break;
			case 3:
				cfg_hw[HP_RGB_1]=true;
				cfg_hw[HP_SWITCH_1]=true;
				break;
			case 4:
				cfg_hw[HP_RGB_1]=true;
				cfg_hw[HP_TEMP_1]=true;
				cfg_hw[HP_RELAY_1]=true;
				cfg_hw[HP_PIR_1]=true;
				cfg_hw[HP_DHT11_1]=true;
				break;
			default:
			break;
		}
	}
	
	
void fncSetRGB(byte r, byte g, byte b)
{
	if (!cfg_hw[HP_RGB_1])
		return;
		
	if ((curRGB.r == r)	&& (curRGB.g == g)	&& (curRGB.b == b) ) 
		return;	

	curRGB.r = r;
	curRGB.g = g;
	curRGB.b = b;
	
	analogWrite(RGB_R_PIN, r);
	analogWrite(RGB_G_PIN, g);
	analogWrite(RGB_B_PIN, b);
	
}

void toggleRelay(){ //http://forum.arduino.cc/index.php?topic=41954.0
	digitalWrite(RELAY_1_PIN, !digitalRead(RELAY_1_PIN));
	curRelayStatus = digitalRead(RELAY_1_PIN);
}

void setRelay(byte val){
	
	if (curRelayStatus != val){
		curRelayStatus = val;
		digitalWrite(RELAY_1_PIN,val );
	}
	
	
}
void encodeFloat(uint16_t *regs, float x)
{
	union Pun pun;

	pun.f = x;
	regs[0] = (pun.u >> 16) & 0xFFFFU;
	regs[1] = pun.u & 0xFFFFU;
}

float decodeFloat(const uint16_t *regs)
{
	union Pun pun;

	pun.u = ((uint32_t)regs[0] << 16) | regs[1];
	return pun.f;
}

void update_input_switch_1()	{
		if (cfg_hw[HP_SWITCH_1])
		{
			debouncer.update(); // Update the Bounce instance :
			boolean curState = debouncer.read();
			holdingRegs[HR_SWITCH_1_STATUS] = curState;  // Get the updated value :
			if ((curState != last_switch_state) ){
				last_switch_state = curState;
				if (curState == LOW) 
				{
					++holdingRegs[HR_SWITCH_1_TOGGLE];
				}
			}
		}
		else
		{
			holdingRegs[HR_SWITCH_1_STATUS] =  HIGH;
		}
	}

void scan_for_command(){
	//check if  HR_CMD_DATA_1 registor > 0 
	byte cmd = holdingRegs[HR_CMD_DATA_1];
	holdingRegs[HR_CMD_DATA_1]=0;  // reset command
	switch (cmd) {
		case 0:
			break;
		case 1:
			
			if (holdingRegs[HR_CMD_DATA_2] == 0) {
				oRGB.set_mode( ALL_OFF);
			}	
			else {
				oRGB.set_mode( RGB_SHOW);
			}
				
			break;
		case 2: // RELAY CONTROL
			if (holdingRegs[HR_CMD_DATA_2] == 0) {
				toggleRelay();
				//setRelay (RELAY_OFF);
			}
			else 
			{
				setRelay (RELAY_ON);
			}
			break;
		case 3:  // ALL ON
			if (holdingRegs[HR_CMD_DATA_2] == 0) {
				oRGB.set_mode(ALL_ON_HIGH);
			}
			else
			{
				oRGB.set_mode(ALL_ON_DIM);
			}
			break;
		case 4: // toggle relay
			toggleRelay();
			break;
		default:
			break;
	}
	
	
}