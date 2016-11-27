/* 
* key_pad.cpp
*
* Created: 26/11/2016 4:29:58 PM
* Author: tj
*/


#include "key_pad.h"
#include "Key.h"
// default constructor
key_pad::key_pad()
{
	

    //keypad.addEventListener(keypadEvent); // Add an event listener for this keypad	
	
} //key_pad

// default destructor
key_pad::~key_pad()
{
} //~key_pad


void key_pad::get_key( ){
	char key = keypad.getKey();
	
	
	
    if (key) {
	   // Serial.println(key);	
	}
}


//take care of some special events
void key_pad::my_listen(KeypadEvent eKey) {
	switch (eKey) {
		case '*':
//		digitalWrite(ledPin, HIGH);
		break;
		case '#':
//		digitalWrite(ledPin, LOW);
		break;
		case '0': // KEY_RELEASED:
//		Serial.println("Key Released");
		break;
		default:
		if (eKey) {      // same as if(eKey != NO_KEY)
//			Serial.println(eKey);
		}
	}
}