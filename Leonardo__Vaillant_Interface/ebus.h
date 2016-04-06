/*
 * ebus.h
 *
 * Created: 13.12.2014 13:56:52
 *  Author: Rene
 */ 


#ifndef EBUS_H_
#define EBUS_H_


void EbusSetup();
void EbusLoop();
void EbusTest();

void EvaluateDataTelegram(char indata);

void GetDataFromTelegram();
bool IsBroadCast();
void PrintData(char *data, int length);

void Extract_B503();
void Extract_B504();
void Extract_B509();
void Extract_0700();

char GetFromByte(char *indata, int startpos);
char GetFromBCD(char *indata, int startpos);
char GetFromData1B(char *indata, int startpos);
char GetFromData1C(char *indata, int startpos);
double GetFromData2C(char *indata, int startpos);
double GetFromData2B(char *indata, int startpos);
//String GetWeekDay(int day);
//void GetWeekDay(char* out, int day);
void GetWeekDay(String &out, int day);
void TranslateStatus();

#endif /* EBUS_H_ */