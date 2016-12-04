/*
* rgb_class.cpp
*
* Created: 3/12/2016 11:03:51 PM
* Author: tj
*/


#include "rgb_class.h"






void rgb_class::set_led_intensity(int zone){
	
	RGB x1;
	RGB x2;
	
	
	//	curRGB[0].r =  led_intensity[zone].r ;
	//	curRGB[0].g =  led_intensity[zone].g ;
	//	curRGB[0].b =  led_intensity[zone].b ;
	
	trans_setup(curRGB[0],led_intensity[zone], 20 ) ;
	//trans_setup(curRGB[0], curRGB[1], 100 ) ;
	//	update_led(0);
	
}


void rgb_class::set_mode(op_modes activate_mode){
	if (mode== activate_mode) {
		return;
	}
	
	mode= activate_mode;
	switch(mode){
		case OFF:
			trans_setup(curRGB[0],led_intensity[5], 10 ) ;  // Turn OFF LEDs
			break;
		case DAY:
			trans_setup(curRGB[0],led_intensity[0], 20 ) ; // Turn OFF LEDs during the day
			break;
		case EVENING:
			trans_setup(curRGB[0],led_intensity[sensor_cur_band], 20 ) ; // highest
			break;
		case NIGHT:
			trans_setup(curRGB[0],led_intensity[4], 20 ) ;  // TODO change to highest if Alarm is active
			break;
		case ALL_ON_HIGH:
			trans_setup(curRGB[0],led_intensity[0], 10 ) ;  // Turn OFF LEDs
			break;
		case ALL_ON_DIM:
			trans_setup(curRGB[0],led_intensity[1], 10 ) ;  // Turn OFF LEDs
			break;	
		case MANUAL:
			break;
		case ALL_OFF:
			set_led_manual(255,255,255);
			tranz.active = false;
			break;
		case RGB_SHOW:
			show_interval = millis() - SHOW_INTERVAL - 1;
		
		break;
	}
}


void rgb_class::set_destination(RGB dest, unsigned int  tick){
	trans_setup(curRGB[0], dest, tick);
}

void rgb_class::set_led_manual(byte r, byte g, byte b){
	curRGB[0].r =  r ;
	curRGB[0].g =  g ;
	curRGB[0].b =  b ;
	update_led(0);
}

void rgb_class::alarm_capture_update( boolean Activate){
	ActiveAlarm = Activate;
	if(Activate  ){
		if (mode== NIGHT) {
			trans_setup(curRGB[0],led_intensity[1], 10 ) ; // highest
		}
		
	}
	else {
		if (mode== NIGHT) {
			trans_setup(curRGB[0],led_intensity[4], 20 );
		}
	}
	
}



void rgb_class::sensor_capture_update(int sensor_zone){
	cur_sensor_zone = sensor_zone;
	switch(mode){
		case OFF:
			trans_setup(curRGB[0],led_intensity[0], 20 ) ;  // Turn OFF LEDs
		break;
		case DAY:
		trans_setup(curRGB[0],led_intensity[0], 20 ) ; // Turn OFF LEDs during the day
		break;
		case EVENING:
		trans_setup(curRGB[0],led_intensity[cur_sensor_zone], 20 ) ; // highest
		break;
		case NIGHT:
		if (ActiveAlarm){
			trans_setup(curRGB[0],led_intensity[1], 10 ) ; // highest
		}
		else {
			trans_setup(curRGB[0],led_intensity[4], 20 ) ;  // lowest
		}
		
		break;
		case MANUAL:
		break;
	}
}


rgb_class::rgb_class(){
	//set_led_manual(0,255,255);
	//init();
}

void  rgb_class::update_led(byte section){
	if (section==0){
		analogWrite(PIN_R0, curRGB[0].r );
		analogWrite(PIN_G0, curRGB[0].g );
		analogWrite(PIN_B0, curRGB[0].b );
	}
}

void  rgb_class::init(){
	pinMode(PIN_R0, INPUT_PULLUP);
	pinMode(PIN_G0, INPUT_PULLUP);
	pinMode(PIN_B0, INPUT_PULLUP);
	set_led_manual(255,255,255);
	randomSeed(analogRead(0));
}

void  rgb_class::trans_setup(RGB rFrom, RGB rTo, unsigned int tick_ms)
{
	
	byte hi_val;
	
	
	
	tranz.val_from= rFrom;
	tranz.val_to = rTo;
	
	hi_val = abs(rTo.r - rFrom.r);
	
	if ( abs(rTo.g - rFrom.g) > hi_val) {
		hi_val =  abs(rTo.g - rFrom.g);
	}
		
	if ( abs(rTo.b - rFrom.b) > hi_val) {
		hi_val =  abs(rTo.b - rFrom.b);
	}
	
	if (hi_val == 0 ) {
		tranz.active=false ;
		return; 
	}
	
	tranz.tick_steps_total = hi_val;
	tranz.tick_steps_cur=0;
	
	
	tranz.multiplier[0] =(float) (rTo.r - rFrom.r )/hi_val ;
	tranz.multiplier[1] =(float) (rTo.g - rFrom.g )/hi_val ;
	tranz.multiplier[2] =(float) (rTo.b - rFrom.b )/hi_val ;
	
	tranz.tick_interval = tick_ms;
	tranz.tick_ms_last = millis();
	
	tranz.active=true;
	
	
	
	//	update_led(0);
}
String rgb_class::led_info(){

	char infoMsg[50];
	if(ActiveAlarm){
		sprintf(infoMsg, "current mode %d. ACTIVE Alarm state:%d", mode,ActiveAlarm);	 //http://forum.arduino.cc/index.php?topic=158901.0
		} else {
		sprintf(infoMsg, "current mode %d. Alarm state:%d", mode,ActiveAlarm);	 //http://forum.arduino.cc/index.php?topic=158901.0
	}
	
	
	return infoMsg;
}

void rgb_class::update(){
	if (mode == RGB_SHOW) {
		
		if ( (millis()-show_interval)> SHOW_INTERVAL)		{
			flicker(false);
			show_interval = millis();
			show_val.r = random(255);
			show_val.g = random(255);
			show_val.b = random(255);
			trans_setup (curRGB[0],show_val,SHOW_TICK);
		}
	} else
	{
		flicker(true);
	}
	
	
	
	trans_tick();
}

void rgb_class::flicker(boolean clr ) {
	if (clr) {
	  led_toggle = LOW;
	  digitalWrite(13, LOW);
	}
	else
	{
	  led_toggle = !led_toggle;
	  digitalWrite(13, led_toggle);
		
	}
}

void  rgb_class::trans_tick(){
	
	
	
	
	
	
	
	if (!tranz.active){
		return;
	}
	
	if ((millis()-tranz.tick_ms_last) > tranz.tick_interval) {
		++tranz.tick_steps_cur;
		curRGB[0].r = tranz.val_from.r  + tranz.multiplier[0] * tranz.tick_steps_cur;
		curRGB[0].g = tranz.val_from.g  + tranz.multiplier[1] * tranz.tick_steps_cur;
		curRGB[0].b = tranz.val_from.b  + tranz.multiplier[2] * tranz.tick_steps_cur;
		update_led(0);
		tranz.tick_ms_last= millis();
		if(tranz.tick_steps_cur==tranz.tick_steps_total){
			tranz.active=false;
		}
	}
	
	

	
}

int btoi(bool b){
	return b ? 1 : 0;
}
