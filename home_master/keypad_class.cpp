/*
* keypad_class.cpp
*
* Created: 27/11/2016 3:11:39 PM
* Author: tj
*/


#include "keypad_class.h"
 
 
// default constructor
keypad_class::keypad_class(DateTime *curTimePtrPar)
{
	curTimePtr = curTimePtrPar;
} //keypad_class

// default destructor
keypad_class::~keypad_class()
{
} //~keypad_class
char keypad_class::read_kc() {
	
	if (kc.getKeys()){

		for (int i=0; i<LIST_MAX; i++)   // Scan the whole key list.
		{
			if ( kc.key[i].stateChanged )   // Only find keys that have changed state.
			{
				switch (kc.key[i].kstate) {  // Report active key state : IDLE, PRESSED, HOLD, or RELEASED
					case PRESSED:

						if (kc.key[i].kchar == '#' )	{
							foo(key_buffer);  //TEST
							key_buffer_clear();
							break;
						}
						key_buffer[key_buffer_ndx++] = kc.key[i].kchar;  
						key_buffer[key_buffer_ndx]= '\0';  
						
						if ( (key_buffer_ndx >= MAX_KEY_BUFFER) || (kc.key[i].kchar == '*' )) {
								key_buffer_clear();
							} 
						else
							{
								lcd_main_cmd_box.print(kc.key[i].kchar);
							}
						break;
					case HOLD:
						msg = " HOLD.";
						break;
					case RELEASED:
						msg = " RELEASED.";
						break;
					case IDLE:
						msg = " IDLE.";
						break;
				}
				
				Serial.print("Key ");
				Serial.print(kc.key[i].kchar);
				Serial.println(msg);
				
			}
		}

		
	}
	
	if ((millis() -last_clock_update ) > 3000) {
		//curTime = rtc.now();
		
		
		
		plot_temperature( sense_temperature[lcd_next_sensor].value,sense_temperature[lcd_next_sensor].location  );
		++lcd_next_sensor;
		if (lcd_next_sensor==MAX_SENSE_TEMPERATURE) {
			lcd_next_sensor= 0;
		}
		last_clock_update = millis() ;
		plot_clock();
		plot_day();
		
	}
	
}

void keypad_class::init(){

 
 for (int n = 0;n<MAX_SENSE_TEMPERATURE-1;n++)
 {
	 sense_temperature[n].value = -99;
 }
 
 

 
 
 	area_temperature_val.DefineArea(0, 13, GLCD.Width-1	, 40, DEFAULT_gTEXTMODE);
 	area_temperature_val.SelectFont(CalBlk36);
 	
 	area_temperature_location.DefineArea  (0, 42, GLCD.Width-1	, 48,DEFAULT_gTEXTMODE);
 	area_temperature_location.SelectFont(Callibri11_bold); // select the system font for the text area name textTop

	area_time.DefineArea(64 ,  0, GLCD.Width-1	, 9,DEFAULT_gTEXTMODE);
	area_time.SelectFont(System5x7);

	area_day.DefineArea(0 ,  0, GLCD.Width/2-1	, 9,DEFAULT_gTEXTMODE);
	area_day.SelectFont(System5x7);

	lcd_main_cmd_box.DefineArea(0, GLCD.Height - 8 ,  GLCD.Width-1	, GLCD.Height-1 ,DEFAULT_gTEXTMODE);
	lcd_main_cmd_box.SelectFont(System5x7);

	scrPaint_Main();
 
	key_buffer_clear();

 	
}


void keypad_class::key_buffer_clear(){
	lcd_main_cmd_box.ClearArea();
	lcd_main_cmd_box.print (">");
	key_buffer_ndx = 0;
	key_buffer[key_buffer_ndx]= '\0';
}

void keypad_class::lcd_test(){
	GLCD.Init();
	
 // Select the font for the default text area
 //GLCD.SelectFont(Verdana12_bold); // (System5x7);
GLCD.SelectFont(System5x7); // (System5x7);
 //  GLCD.print(F("hello, world!")); // keep string in flash on AVR boards with IDE 1.x
 //  GLCD.Puts(F("hello, world!")); // Puts() supports F() with any version of IDE

 // print() below uses RAM on AVR boards but works
 // on any version of IDE with any processor
 // note: Same is true for Puts()
 GLCD.print("hello, world!");
 //GLCD.println("");
 
 GLCD.DrawHLine(20,15,45);
  GLCD.DrawRoundRect(3,20,30,10,2);
 //GLCD.CursorTo(5, 5);
// GLCD.println("hello, world!5");
	
	
}


// Taking care of some special events.
void keypad_class::mykeys(KeypadEvent key){
	switch (kc.getState()){
		case     PRESSED :
		if (key == '#') {
			//digitalWrite(ledPin,!digitalRead(ledPin));
			//ledPin_state = digitalRead(ledPin);        // Remember LED state, lit or unlit.
		}
		break;

		case RELEASED:
		if (key == '*') {
			//digitalWrite(ledPin,ledPin_state);    // Restore LED state from before it started blinking.
			//blink = false;
		}
		break;

		case HOLD:
		if (key == '*') {
			//blink = true;    // Blink the LED when holding the * key.
		}
		break;
	}
}

void keypad_class::callme(void (*ptr)()){
	foo = ptr;
}


void keypad_class::scrPaint_Main(){

	GLCD.ClearScreen();
	GLCD.DrawHLine(0,10,GLCD.Width);
	GLCD.DrawHLine(0,GLCD.Height-10,GLCD.Width);
	GLCD.DrawString_P(PSTR("Mon 28 Nov"), gTextfmt_left, gTextfmt_top);  
	GLCD.DrawString_P(PSTR("11:12 PM"), gTextfmt_right, gTextfmt_top); 
	plot_temperature(-99,"");
	plot_clock();

}


void keypad_class::set_sense_temperature(int ndx, float val, String location){
	sense_temperature[ndx].value = val;
	sense_temperature[ndx].location = location;
	
}

void keypad_class::plot_temperature(float val, String location){
	char str_temp[8];
	
	if (val > -99) {
		dtostrf(val, 4, 2, str_temp); //http://stackoverflow.com/questions/27651012/arduino-sprintf-float-not-formatting
		area_temperature_val.DrawString(str_temp, gTextfmt_center, gTextfmt_current,eraseFULL_LINE );
		area_temperature_location.DrawString(location.c_str(), gTextfmt_center, gTextfmt_current,eraseFULL_LINE );		
	}
	else
	{
		area_temperature_val.DrawString("?", gTextfmt_center, gTextfmt_current,eraseFULL_LINE );
		area_temperature_location.DrawString("", gTextfmt_center, gTextfmt_current,eraseFULL_LINE );		
	}

}


void keypad_class::plot_clock(){
	
	int hr = curTimePtr->hour();
	int min = curTimePtr->minute();
	
	String s;
	String m;
	String ampm = " AM" ;
	
	if (min < 10) {
		m = "0" + String(min);
	}
	else
	{
		m =  String(min);
	}
	
	if (hr > 11) {
		ampm = " PM";
	}
	
	if (hr > 12) {
		hr = hr-12;
	}
		
	s =   String(hr) + ":" + m + ampm;
	
	
	
	
	area_time.DrawString( s.c_str(), gTextfmt_right, gTextfmt_current,eraseFROM_BOL );
	
	
}

void keypad_class::plot_day(){
	
	String s =  String(curTimePtr->day()) + " " + String(months[curTimePtr->month()-1]) + " " + String(curTimePtr->year())   ;
	// String(curTimePtr->day()) + " " + String(months[curTimePtr->month()-1] ) + " " + String(curTimePtr->year() - 2000 )
	area_day.DrawString(   s.c_str(), gTextfmt_left, gTextfmt_current,eraseFULL_LINE );
	
	
}

