/*
 * master.h
 *
 * Created: 28/11/2016 6:27:25 PM
 *  Author: tj
 */ 


#ifndef MASTER_H_
#define MASTER_H_

//////////////////// Port information ///////////////////
#define baud 38400
#define timeout 1500  // Original was set to 1000 but we are receivig xx3
#define polling 200 // the scan rate
#define retry_count 4

// used to toggle the receive/transmit pin on the driver
#define TxEnablePin 4

#define LED 9






#endif /* MASTER_H_ */