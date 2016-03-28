/* 
* ExternalTemperature.cpp
*
* Created: 10.01.2015 21:43:19
* Author: Rene
*/


#include "ExternalTemperature.h"
#include "vars.h"


#include "DS18S20.h"
//siehe http://www.bastelitis.de/temperaturen-messen-mit-dem-arduino/

DS18S20_List ds18s20(8); // pin 8 //PIN10 wird vom Ethernet Shield verwendet

//#define ID_OUTSIDE 0xABC5

unsigned long previousMillis = 0; // speichert wie viele Sekunden seit derletzten Änderung vergangen sind
unsigned long interval = 10000;    // Interval zwischen zwei Änderungen

void ExternalTemperatureSetup()
{
	for (int i=0;i<10;i++)
	{
		fTempExtern[i]=-1.99;
		nTempExternID[i]=0;
	}
	sprintf(sTempExternUnit,"C");

}


void ExternalTemperatureLoop()
{
	//delay(1000);
	if (millis() - previousMillis > interval) 
	{
		previousMillis = millis();   // aktuelle Zeit abspeichern
	
		ds18s20.update();

		nTempExternCount=ds18s20.count;
		for (int i=0;i<ds18s20.count;i++)
		{
			Serial.print("Sensor ");
			Serial.print(i,DEC);
			Serial.print(", T=");
			float value=ds18s20.get_temp(i);
			print_temperature(value);
			Serial.println("C");
			
			fTempExtern[i]=value;
			nTempExternID[i]=ds18s20.get_id(i);

		}
	}
/*
	float T_outside=ds18s20.get_temp_by_id(ID_OUTSIDE);

	Serial.print("Temperatur: ");
	print_temperature(T_outside);
	Serial.println("C");
	*/


/*
byte i;
byte present = 0;
byte data[12];
byte addr[8];
int HighByte, LowByte, TReading, SignBit, Tc_100, Whole, Fract;

ds.reset_search();
if ( !ds.search(addr)) {
	Serial.print("No more addresses.\n");
	ds.reset_search();
	return;
}

Serial.print("R=");
for( i = 0; i < 8; i++) {
	Serial.print(addr[i], HEX);
	Serial.print(" ");
}

if ( OneWire::crc8( addr, 7) != addr[7]) {
	Serial.print("CRC is not valid!\n");
	return;
}

if ( addr[0] == 0x10) {
	Serial.print("Device is a DS18S20 family device.\n");
}
else if ( addr[0] == 0x28) {
	Serial.print("Device is a DS18B20 family device.\n");
}
else {
	Serial.print("Device family is not recognized: 0x");
	Serial.println(addr[0],HEX);
	return;
}

ds.reset();
ds.select(addr);
ds.write(0x44,1);         // start conversion, with parasite power on at the end

delay(1000);     // maybe 750ms is enough, maybe not
// we might do a ds.depower() here, but the reset will take care of it.

present = ds.reset();
ds.select(addr);
ds.write(0xBE);         // Read Scratchpad

Serial.print("P=");
Serial.print(present,HEX);
Serial.print(" ");
for ( i = 0; i < 9; i++) {           // we need 9 bytes
	data[i] = ds.read();
	Serial.print(data[i], HEX);
	Serial.print(" ");
}
Serial.print(" CRC=");
Serial.print( OneWire::crc8( data, 8), HEX);
Serial.println();

LowByte = data[0];
HighByte = data[1];
TReading = (HighByte << 8) + LowByte;
SignBit = TReading & 0x8000;  // test most sig bit
if (SignBit) // negative
{
	TReading = (TReading ^ 0xffff) + 1; // 2's comp
}
Tc_100 = (6 * TReading) + TReading / 4;    // multiply by (100 * 0.0625) or 6.25

Whole = Tc_100 / 100;  // separate off the whole and fractional portions
Fract = Tc_100 % 100;


if (SignBit) // If its negative
{
	Serial.print("-");
}
Serial.print(Whole);
Serial.print(".");
if (Fract < 10)
{
	Serial.print("0");
}
Serial.print(Fract);

Serial.print("\n");

*/

}
