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

class keypad_class
{
	//variables
	public:
	protected:
	private:

	//functions
	public:
	keypad_class();
	~keypad_class();
	char read_kc();
	void mykeys(KeypadEvent key);
	void lcd_test();
	void init();
	protected:
	private:
	keypad_class( const keypad_class &c );
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
	
	String msg;
};

#endif //__KEYPAD_CLASS_H__
