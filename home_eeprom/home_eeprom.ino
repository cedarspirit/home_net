#include <EEPROM.h>




#define DEVICE_ADDRESS 2
#define HW_PROFILE 2       // what peripherals do we have? refer to G:\CloudDrives\OneDrive\W8_Cloud\Documents\Arduino\mastermind\docs\eeprom_guide.xlsx for info on hardware profile
#define MAX_SLAVES 3

enum EEPROM_STORE{
	EEP_DEVICE_ADDR,
	EEP_MAX_SLAVES,
	EEP_HW_PROFILE,
};


void setup ()
  {
	  
  if (EEPROM.read (EEP_DEVICE_ADDR) != DEVICE_ADDRESS)
    EEPROM.write (EEP_DEVICE_ADDR, DEVICE_ADDRESS);

  if (EEPROM.read (EEP_MAX_SLAVES) != MAX_SLAVES)
  EEPROM.write (EEP_MAX_SLAVES, MAX_SLAVES);

  if (EEPROM.read (EEP_HW_PROFILE) != HW_PROFILE)
    EEPROM.write (EEP_HW_PROFILE, HW_PROFILE);

  }  // end of setup

void loop () { }
