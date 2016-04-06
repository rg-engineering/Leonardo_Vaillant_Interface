#define ARDUINO_MAIN

// Disable some warnings for the Arduino files
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wsign-compare"
#pragma GCC diagnostic ignored "-Wattributes"
#pragma GCC diagnostic ignored "-Wunused-variable"
#pragma GCC diagnostic ignored "-Wuninitialized"

#include <Arduino.h>
extern "C"
{
	#include <pins_arduino.h>
}

#include "vars.h"
#include "WebServer.h"
#include "ebus.h"
#include "ExternalTemperature.h"

/*
* Libraries
*/
// Arduino Leonardo source files for serial:
#define USB_VID 0x2341
#define USB_PID 0x8036
#include <CDC.cpp>
#include <USBCore.cpp>
//#include <HID.cpp>
#include <PluggableUSB.cpp>

// Other source files, depends on your program which you need
#include <Print.cpp>
#include <New.cpp>
#include <wiring.c>
#include <wiring_digital.c>
//#include <wiring_analog.c> //analog read/write functions
#include <WString.cpp>
#include <WMath.cpp>
#include <Stream.cpp>

// Unused source files:
//#include <WInterrupts.c>
//#include <wiring_pulse.c>
//#include <wiring_shift.c>
//#include <IPAddress.cpp>
//#include <Tone.cpp>



// Restore original warnings configuration
#pragma GCC diagnostic pop


char sName[20];
char sVersionName[20];
char sVersionNumber[20];

char sDate[20];
char sTime[20];
double fTempOut;
char sTempOutUnit[3];
double fTempVorlauf;
char sTempExternUnit[3];
int nTempVorlaufStatus;
char sTempVorlaufUnit[3];
double fTempQuelle;
int nTempQuelleStatus;
char sTempQuelleUnit[3];
double fHeizLeistungMomentan;
char sHeizLeistungMomentanUnit[3];
int nHeizLeistungMomentanStatus;
unsigned int nTempExternID[10];
double fTempExtern[10];
unsigned int nTempExternCount;
char sCurrentState[20];
char sCurrentError[2];
char sCurrentWarning[2];
int nCurrentError;
int nCurrentWarning;

bool bDebug=false;

int nSecond;
int nMinute;
int nHour;
int nDay;
int nMonth;
int nDayOfWeek;
int nYear;

/*
* Main harness for sketch.
*/
int main(void)
{
	init();
	
	#if defined(USBCON)
	USBDevice.attach();
	#endif
	
	sprintf (sName,"Arduino");
	sprintf (sVersionName, "Vaillant Interface");
	sprintf ( sVersionNumber,"1.5.2");
	
	sprintf(sDate,"1.1.1970");
	sprintf(sTime,"7:30:00");
	fTempOut=-1.2;
	sprintf(sTempOutUnit,"C");
	fTempVorlauf=-3.4;
	sprintf(sTempVorlaufUnit,"C");
	fTempQuelle=-2.3;
	sprintf(sTempQuelleUnit,"C");
	fHeizLeistungMomentan=-1;
	sprintf(sHeizLeistungMomentanUnit,"kW");
	
	sCurrentError[0]=0xff;
	sCurrentError[1]=0xff;
	sCurrentWarning[0]=0xff;
	sCurrentWarning[1]=0xff;
	int nCurrentError=-1;
	int nCurrentWarning=-1;

	
	sprintf (sCurrentState,"unknown");
	// Open serial communications and wait for port to open:
	//Serial.begin(57600);
	Serial.begin(115200);
	
	if (bDebug)
	{
		while (!Serial)
		{
			; // wait for serial port to connect. Needed for Leonardo only
		}
	}
	else
	{
		
	}
	WebServerSetup();
	EbusSetup();
	ExternalTemperatureSetup();
	
	if (Serial)
	{
		Serial.print(sVersionName);
		Serial.print(" ");
			
		Serial.println(sVersionNumber);
	}

	
	for (;;) 
	{
		WebServerLoop();
		EbusLoop();
		ExternalTemperatureLoop();
		if (serialEventRun) serialEventRun();
		//EbusTest();
		//while(1); //muss wieder weg
	}
	return 0;
}