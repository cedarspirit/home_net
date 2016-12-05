/*
* rgb_class.h
*
* Created: 3/12/2016 11:03:51 PM
* Author: tj
*/


#ifndef __RGB_CLASS_H__
#define __RGB_CLASS_H__



#if defined(ARDUINO) && ARDUINO >= 100
#include "arduino.h"
#else
#include "WProgram.h"
#endif

#include "common.h"

#define ledPin 11

#define PIN_R0 9
#define PIN_G0 10
#define PIN_B0 11



struct TRANSITION_STRUCT {
	boolean active;
	RGB val_from;
	RGB val_to;
	
	float multiplier[3];
	int tick_steps_total;
	int tick_steps_cur;
	unsigned int tick_interval;  // how many ms delay between each tick
	unsigned long tick_ms_last;   // last ms a tick occured
};


//extern op_modes mode;
//extern int sensor_cur_band; //



class rgb_class
{




	private:
	
	#define SHOW_INTERVAL 5000
	#define SHOW_TICK 10
	unsigned long show_interval;
	RGB show_val;
	
	byte led_toggle ;
	
	struct RGB curRGB[2];
	//struct RGB led_intensity[5]={{0,0,0},{3,3,3},{20,20,20},{127,127,127},{254,254,254}}; // light intensity for low, dim, normal, and high
	struct RGB led_intensity[6]={{0,0,0},{254,254,254},{127,127,127},{20,20,20},{3,3,3},{255,255,255}};
	TRANSITION_STRUCT tranz ;
	boolean ActiveAlarm;
	int cur_sensor_zone;
	int btoi(bool b);
	//	unsigned long	_onInterval;
	//	unsigned long	_offInterval;
	//	uint8_t			_blinkingLedId;
	//	bool			_ledActivity;
	//	PATROL_DEF		_cfg;
	//	pt_mode_t		opMode;
	void trans_tick();
	void update_led(byte section);
	
	
	public:

	void init();
	void set_led_intensity(int zone);
	void trans_setup(RGB rFrom, RGB rTo, unsigned int total_ms);
	void update();
	void set_led_manual(byte r, byte g, byte b);
	void set_mode(op_modes activate_mode);
	String led_info();
	void sensor_capture_update(int sensor_zone);
	void alarm_capture_update( boolean Activate);
	void set_destination(RGB dest, unsigned int	 tick);
	void flicker(bool clr);  //toggle internal led for testing
	
	
	//	Patrol(unsigned long onInterval, unsigned long offInterval, uint8_t blinkingLedId);

	op_modes mode;
	int sensor_cur_band;


	//functions
	public:
	rgb_class();
	//~rgb_class();
	protected:
	private:
	rgb_class( const rgb_class &c );
	//rgb_class& operator=( const rgb_class &c );

}; //rgb_class

#endif //__RGB_CLASS_H__
