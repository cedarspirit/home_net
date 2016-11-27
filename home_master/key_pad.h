/* 
* key_pad.h
*
* Created: 26/11/2016 4:29:58 PM
* Author: tj
*/


#ifndef __KEY_PAD_H__
#define __KEY_PAD_H__
#include <Keypad.h>
#include "Key.h"

#define ROWS  4; //four rows
#define COLS  4; //three columns

class key_pad
{
//variables
public:
protected:



private:

char keys[4][4] = {
	{'1','2','3','A'},
	{'4','5','6','B'},
	{'7','8','9','C'},
	{'*','0','#','D'}
};

byte rowPins[4] = {48, 49, 46, 47}; //connect to the row pinouts of the keypad
byte colPins[4] = {52, 53, 50,51}; //connect to the column pinouts of the keypad

Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, 4, 4 );







//functions
public:
	void get_key();
	key_pad();
	~key_pad();
protected:
private:
	key_pad( const key_pad &c );
	key_pad& operator=( const key_pad &c );
	
	
	
	void  my_listen(KeypadEvent eKey);
	
}; //key_pad

#endif //__KEY_PAD_H__
