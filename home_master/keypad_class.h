/*
* keypad_class.h
*
* Created: 27/11/2016 3:11:39 PM
* Author: tj
*/


#ifndef __KEYPAD_CLASS_H__
#define __KEYPAD_CLASS_H__

#include <Keypad.h>
#include <openGLCD.h>
#include "RTClib.h"

class keypad_class
{
	//variables
	public:
	protected:
	private:
	#define MAX_SENSE_TEMPERATURE 3
	//functions
	public:
	keypad_class(DateTime *curTime);
	~keypad_class();
	char read_kc();
	void mykeys(KeypadEvent key);
	void lcd_test();
	void init();
	void set_sense_temperature(int ndx, float val, String location);
	void callme(void (*ptr)());
	void (*foo)(String msg);
	protected:
	
	
	private:
	//	keypad_class( const keypad_class &c );
		keypad_class& operator=( const keypad_class &c );
	
		char keys[4][4] = {
			{'1','2','3','A'},
			{'4','5','6','B'},
			{'7','8','9','C'},
			{'*','0','#','D'}
		};

		byte rowPins[4] = {48, 49, 46, 47};		//connect to the row pinouts of the keypad
		byte colPins[4] = {52, 53, 50,51};		//connect to the column pinouts of the keypad
		Keypad kc = Keypad( makeKeymap(keys), rowPins, colPins, 4, 4 );


		unsigned long last_clock_update;
	
		String msg;
		
		byte lcd_next_sensor = 0;
		
		struct TEMPERATURE_INFO {
			String location;
			float value;
			};
		
		TEMPERATURE_INFO sense_temperature[MAX_SENSE_TEMPERATURE];
		
		
		DateTime *curTimePtr;
		
		
		String months[12] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};		
		
		
		#define MAX_KEY_BUFFER 10	
		char key_buffer[MAX_KEY_BUFFER];
		byte key_buffer_ndx = 0;
		
	//	gText txt ;
		
		gText area_temperature_val;
		gText area_temperature_location;
		gText area_time;
		gText area_day;
		gText lcd_main_cmd_box;
				
		void scrPaint_Main();
		void plot_temperature(float val,  String location);
		void plot_clock();
		void plot_day();
		void key_buffer_clear();
};	

#endif //__KEYPAD_CLASS_H__
