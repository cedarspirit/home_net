#include <SimpleModbusMaster.h>
#include <Keypad.h>
#include <Wire.h>
#include "RTClib.h"
#include "Key.h"
#include <openGLCD.h>
#include "keypad_class.h"

#include "master.h"
/*
   The example will use packet1 to read a register from address 0 (the adc ch0 value)
   from the arduino slave (id=1). It will then use this value to adjust the brightness
   of an led on pin 9 using PWM.
   It will then use packet2 to write a register (its own adc ch0 value) to address 1 
   on the arduino slave (id=1) adjusting the brightness of an led on pin 9 using PWM.
*/


// This is the easiest way to create new packets
// Add as many as you want. TOTAL_NO_OF_PACKETS
// is automatically updated.


		RTC_DS3231 rtc;
		char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
		
		DateTime curTime;


//////////////// registers of your slave ///////////////////
enum SLAVE_RESOURCES
{
	// just add or remove registers and your good to go...
	// The first register starts at address 0

	SR_HW_PROFILE,
	SR_TEMP_1_MSB,
	SR_TEMP_1_LSB,
	SR_POT_1,
	SR_PIR_1,
	SR_SWITCH_1,
	SR_RGB_1_R,   // OUTPUT PINS
	SR_RGB_1_G,   // OUTPUT PINS
	SR_RGB_1_B,   // OUTPUT PINS
	SR_RELAY_1, // OUTPUT PINS
	SR_DHT11_1_T_MSB,
	SR_DHT11_1_T_LSB,
	SR_DHT11_1_H_MSB,
	SR_DHT11_1_H_LSB,
};


enum LOCAL_REGISTERS
{
	LR_POT_ON2,
	LR_PIR_ON4,
	LR_TEMP_MSB_ON4,	
	LR_TEMP_LSB_ON4,
	
	LR_SET_RGB_R_ON4,
	LR_SET_RGB_G_ON4,
	LR_SET_RGB_B_ON4,
	
	LR_SET_RGB_R_ON3,
	LR_SET_RGB_G_ON3,
	LR_SET_RGB_B_ON3,

	LR_SET_RGB_R_ON2,
	LR_SET_RGB_G_ON2,
	LR_SET_RGB_B_ON2,

	LR_GET_SWITCH_ON3,
	LR_DHT11_T_MSB_ON4,
	LR_DHT11_T_LSB_ON4,	
	LR_DHT11_H_MSB_ON4,
	LR_DHT11_H_LSB_ON4,
	HOLDING_REGS_SIZE // leave this one
	// total number of registers for function 3 and 16 share the same register array
	// i.e. the same address space	
};


// The total amount of available memory on the master to store data
#define TOTAL_NO_OF_REGISTERS HOLDING_REGS_SIZE

unsigned char cur_pir;
byte curSwitchOn4;

enum
{
	PACKET_POT_ON2,
	PACKET_TEMP_ON_SLAVE4,
	PACKET_PIR_ON_SLAVE_4,
	PACKET_GET_SWITCH_ON3,
	PACKET_SET_RGB_ON2,	
	PACKET_SET_RGB_ON3,	
	PACKET_SET_RGB_ON4,	
	PACKET_GET_RGB_ON4,	
	PACKET_DHT11_ON_SLAVE4,
	TOTAL_NO_OF_PACKETS // leave this last entry
};

// Create an array of Packets to be configured
Packet packets[TOTAL_NO_OF_PACKETS];

// Masters register array
unsigned int regs[TOTAL_NO_OF_REGISTERS];

unsigned long tick;
union Pun {float f; uint32_t u;}; 

keypad_class  ky(&curTime);
	
void setup()
{
	
	Serial.begin(38400);
  
	// Initialize each packet

	Serial.println("hello");
  
	modbus_construct(&packets[PACKET_POT_ON2],   2, READ_HOLDING_REGISTERS, SR_POT_1,1, LR_POT_ON2,false);
 
	modbus_construct(&packets[PACKET_TEMP_ON_SLAVE4], 4, READ_HOLDING_REGISTERS, SR_TEMP_1_MSB ,2,  LR_TEMP_MSB_ON4,false); 
	
	modbus_construct(&packets[PACKET_DHT11_ON_SLAVE4], 4, READ_HOLDING_REGISTERS, SR_DHT11_1_T_MSB ,4,  LR_DHT11_T_MSB_ON4,false); 

	modbus_construct(&packets[PACKET_PIR_ON_SLAVE_4], 4, READ_HOLDING_REGISTERS, SR_PIR_1,1,  LR_PIR_ON4,false); 
	
	modbus_construct(&packets[PACKET_SET_RGB_ON4],   4, PRESET_MULTIPLE_REGISTERS,SR_RGB_1_R ,3, LR_SET_RGB_R_ON4,true);

	modbus_construct(&packets[PACKET_SET_RGB_ON3],   3, PRESET_MULTIPLE_REGISTERS,SR_RGB_1_R ,3, LR_SET_RGB_R_ON3,true);
	
	modbus_construct(&packets[PACKET_SET_RGB_ON2],   2, PRESET_MULTIPLE_REGISTERS,SR_RGB_1_R ,3, LR_SET_RGB_R_ON2,true);	
	
	modbus_construct(&packets[PACKET_GET_RGB_ON4],   4, READ_HOLDING_REGISTERS,SR_RGB_1_R ,3, LR_SET_RGB_R_ON4,true);
	
	modbus_construct(&packets[PACKET_GET_SWITCH_ON3],   3, READ_HOLDING_REGISTERS,SR_SWITCH_1 ,1, LR_GET_SWITCH_ON3,false);
	
	
 
	
	regs[LR_SET_RGB_R_ON4] = 0xff;
	regs[LR_SET_RGB_G_ON4] = 0xff;
	regs[LR_SET_RGB_B_ON4] = 0xff;
	
 //  packets[PACKET6].connection=0;
//   packets[PACKET5].connection=0;
   
//   packets[PACKET3].connection=0;
//   packets[PACKET4].connection=0;   
   
   
  modbus_configure(&Serial3, baud, SERIAL_8N1, timeout, polling, retry_count, TxEnablePin, packets, TOTAL_NO_OF_PACKETS, regs);
  
  pinMode(LED, OUTPUT);
  
  tick = millis();
  
  
  gloabl_RGB(12,232,1);
  
   ky.lcd_test();
   
   ky.callme(&keyTrigger)  ;
   
   ky.init();

 if (! rtc.begin()) {
	 Serial.println("Couldn't find RTC");
	 while (1);
 }

	curTime = rtc.now();
	
}

void loop()
{
  modbus_update();
  
  //regs[0] = 0x66 ; // analogRead(0); // update data to be written to arduino slave
  
  //analogWrite(LED, regs[0]>>2); // constrain adc value from the arduino slave to 255
   analogWrite(LED, 123); // constrain adc value from the arduino slave to 255
   
   if ((millis()-tick) > 1000){
		tick = millis();
		
		curTime = rtc.now();
		
		
		//Serial.print("REG HR_POT_1: ");
		//Serial.println(regs[LR_POT_ON2]);
	
		ky.set_sense_temperature(0,decodeFloat(&regs[ LR_TEMP_MSB_ON4]),"INSIDE");
		ky.set_sense_temperature(1,decodeFloat(&regs[ LR_DHT11_T_MSB_ON4]),"DHT11_TEMPERATURE");
		ky.set_sense_temperature(2,decodeFloat(&regs[ LR_DHT11_H_MSB_ON4]),"DHT11_HUMIDITY");

		Serial.print("DHT11_ON_SLAVE4: ");
		Serial.println(decodeFloat(&regs[LR_DHT11_T_MSB_ON4]));

			
		Serial.print("REG HR_PIR_1: ");
		Serial.println(regs[LR_PIR_ON4]);	

		Serial.print("REG PACKET_GET_SWITCH_ON3: ");
		Serial.println(regs[LR_GET_SWITCH_ON3]);
		
		
		
		
		
		//regs[LR_SET_RGB_R_ON4] = regs[LR_SET_RGB_R_ON4] +10;  //TJ_PLAY
		//if (regs[LR_SET_RGB_R_ON4] > 256) {
		//	regs[LR_SET_RGB_R_ON4]=0;
		//}
		//gloabl_RGB	(regs[LR_SET_RGB_R_ON4],regs[LR_SET_RGB_R_ON4],regs[LR_SET_RGB_R_ON4]); //TJ_PLAY	
   }
   
   
   if(curSwitchOn4!= regs[LR_GET_SWITCH_ON3]){
	   curSwitchOn4= regs[LR_GET_SWITCH_ON3];
	   if (curSwitchOn4 == 1){
		   gloabl_RGB (12,232,1);
		} 
		else 
		{
		   gloabl_RGB (0,255,255);
		}
   }
   
   if (cur_pir != regs[LR_PIR_ON4])
	{
		cur_pir = regs[LR_PIR_ON4];
		//update_RGB_ON4(cur_pir);
	}
   
   ky.read_kc();
   
}


void update_RGB_ON4(unsigned char val)
{
	if (val==0)	{
		regs[LR_SET_RGB_R_ON4] = 0xff;
		regs[LR_SET_RGB_G_ON4] = 0xff;
		regs[LR_SET_RGB_B_ON4] = 0xff;	
		
		regs[LR_SET_RGB_R_ON3] = 0xff;
		regs[LR_SET_RGB_G_ON3] = 0xff;
		regs[LR_SET_RGB_B_ON3] = 0xff;
		
		regs[LR_SET_RGB_R_ON2] = 0xff;
		regs[LR_SET_RGB_G_ON2] = 0xff;
		regs[LR_SET_RGB_B_ON2] = 0xff;
		
		
			
	} else {
		regs[LR_SET_RGB_R_ON4] = 0xff;
		regs[LR_SET_RGB_G_ON4] = 0x0;
		regs[LR_SET_RGB_B_ON4] = 0xff;		

		regs[LR_SET_RGB_R_ON3] = 0xff;
		regs[LR_SET_RGB_G_ON3] = 0x0;
		regs[LR_SET_RGB_B_ON3] = 0xff;
		
		regs[LR_SET_RGB_R_ON2] = 0xff;
		regs[LR_SET_RGB_G_ON2] = 0x0;
		regs[LR_SET_RGB_B_ON2] = 0xff;
		


	}
	packets[PACKET_SET_RGB_ON4].id=0;
	packets[PACKET_SET_RGB_ON4].run_countown=1;
//	packets[PACKET_SET_RGB_ON3].run_countown=1;
//	packets[PACKET_SET_RGB_ON2].run_countown=1;
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

float gloabl_RGB(byte r, byte g, byte b)
{
	regs[LR_SET_RGB_R_ON4] = r;
	regs[LR_SET_RGB_G_ON4] = g;
	regs[LR_SET_RGB_B_ON4] = b;	
	packets[PACKET_SET_RGB_ON4].id=0;
	packets[PACKET_SET_RGB_ON4].run_countown=1;	
	
}

void keyTrigger(String msg){
	Serial.println("=======================" + msg + "===========");
}