/*
 * common.h
 *
 * Created: 3/12/2016 11:16:39 PM
 *  Author: tj
 */ 


#ifndef COMMON_H_
#define COMMON_H_


struct RGB {
	byte r;
	byte g;
	byte b;
};


enum op_modes {OFF, DAY, EVENING,NIGHT,MANUAL, ALL_OFF,RGB_SHOW, ALL_ON_HIGH, ALL_ON_DIM};



#endif /* COMMON_H_ */