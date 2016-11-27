/*
* keypad_class.cpp
*
* Created: 27/11/2016 3:11:39 PM
* Author: tj
*/


#include "keypad_class.h"


// default constructor
keypad_class::keypad_class()
{
	
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
					msg = " PRESSED.";
					if (kc.key[i].kchar=='*'){
						GLCD.ClearScreen();	
					}
					else
					{
						GLCD.print(kc.key[i].kchar);	
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
				}
				Serial.print("Key ");
				Serial.print(kc.key[i].kchar);
				Serial.println(msg);
				//
				
			}
		}

		
	}
	
	//	return (kc.getKey());
	
}

void keypad_class::init(){
	
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