/*
 * vars.h
 *
 * Created: 11.12.2014 21:08:29
 *  Author: Rene
 */ 


#ifndef VARS_H_
#define VARS_H_

extern bool bDebug;

extern char sName[20];
extern char sVersionName[20];
extern char sVersionNumber[20];

extern char sDate[20];
extern char sTime[20];
extern double fTempOut;
extern char sTempOutUnit[3];
extern double fTempVorlauf;
extern int nTempVorlaufStatus;
extern char sTempVorlaufUnit[3];
extern double fTempQuelle;
extern int nTempQuelleStatus;
extern char sTempQuelleUnit[3];
extern double fHeizLeistungMomentan;
extern char sHeizLeistungMomentanUnit[3];
extern int nHeizLeistungMomentanStatus;
extern double fTempExtern[10];
extern unsigned int nTempExternID[10];
extern unsigned int nTempExternCount;
extern char sTempExternUnit[3];
extern char sCurrentState[20];

extern int nSecond;
extern int nMinute;
extern int nHour;
extern int nDay;
extern int nMonth;
extern int nDayOfWeek;
extern int nYear;


#endif /* VARS_H_ */