/*
 * ebus.cpp
 *
 * Created: 13.12.2014 13:56:39
 *  Author: Rene
 */ 

#include <HardwareSerial.h>
#include <Arduino.h>
#include "vars.h"


#include "ebus.h"

int nState;
int nDataLengthCount;

char cSource;
char cDestination;
char cCommand;
char cSubCommand;
char cDataLength;
char cData[200] = {0x20, 0xFE, 0x5, 0x50, 0x14, 0x29, 0x12, 0x0, 0x14};
char cCRC;

char cAnswerDataLength;
char cAnswerData[200];
char cAnswerCRC;

void EbusSetup()
{
	Serial1.begin(2400);
}


void EbusLoop()
{	
	int nCount = Serial1.available();
	
	//bei jedem Durchlauf nur ein byte lesen und auswerten
	if (nCount>0)
	{
		char indata = Serial1.read();

		EvaluateDataTelegram(indata);

	}
}


void EvaluateDataTelegram(char indata)
{

	switch (nState)
	{
		case 0:
			if (indata != 0xAA) //Sync
			{
				cDestination = -1;
				cCommand =-1;
				cSubCommand = -1;
				cDataLength = -1;

				cCRC = -1;
				
				cAnswerDataLength = -1;
				
				cAnswerCRC = -1;
				
				nState++;
				
				cSource = indata;
//				Serial.print("source ");
//				Serial.println(cSource,HEX);
			}
			break;
		case 1: // Destination
			nState++;
			cDestination = indata;
//			Serial.print("destination ");
//			Serial.println(cDestination,HEX);
			break;
		case 2: // Command
			nState++;
			cCommand = indata;
//			Serial.print("command ");
//			Serial.println(cCommand,HEX);
			break;
		case 3: // SubCommand
			nState++;
			cSubCommand = indata;
//			Serial.print("sub command ");
//			Serial.println(cSubCommand,HEX);
			break;
		case 4: // Datalength
			cDataLength = indata;

			sprintf(cData,"");
			nDataLengthCount=0;
//			Serial.print("data length ");
//			Serial.println(cDataLength,DEC);
			if (cDataLength > 0)
			{
				nState++;
			}
			else
			{
				nState += 2;
			}
			break;
		case 5: // Data
			if (indata==0xAA)
			{
				nState = 0;
				Serial.println("missing data or sync lost");
			}
			else
			{
				cData[nDataLengthCount] = indata;
				
/*				Serial.print("data ");
				Serial.print(nDataLengthCount,DEC);
				Serial.print(" of ");
				Serial.print(cDataLength,DEC);
				Serial.print(" : ");
				Serial.println(cData[nDataLengthCount],HEX);
*/	
				nDataLengthCount++;
				if (nDataLengthCount == cDataLength || nDataLengthCount>199)
				{
					nState++;
	//				Serial.println("all data");
				}
			}
			

			
			break;
		case 6: // CRC
			cCRC = indata;
//			Serial.print("CRC ");
//			Serial.println(cCRC,HEX);
			
			if (IsBroadCast())
			{
				nState = 0;
//				Serial.println("Broadcast");
				GetDataFromTelegram();
			}
			else
			{
				nState++;
			}
			break;
		case 7: // Ack
			if (indata == 0x00)
			{
				nState++;
			}
			else if (indata == 0xAA)
			{
				nState = 0;
//				Serial.println("seems broadcast");
				GetDataFromTelegram();
			}
			else
			{
				nState = 0;
				Serial.print("ack missing... got ");
				Serial.println(indata,HEX);
				GetDataFromTelegram();
			}
			break;
		case 8: //answer datalength
			if (indata == 0xAA)
			{
				nState = 0;
//				Serial.println("sb(2)");
				GetDataFromTelegram();
			}
			else
			{
				cAnswerDataLength=indata;
				nDataLengthCount = 0;
				sprintf(cAnswerData,"");
				if (cAnswerDataLength > 0)
				{
					nState++;
				}
				else
				{
					nState += 2;
				}
			}
			break;
		case 9:
			if (indata==0xAA)
			{
				nState = 0;
				Serial.println("missing data or sync lost");
			}
			else
			{
				cAnswerData[nDataLengthCount] = indata;
				nDataLengthCount++;
				if (nDataLengthCount == cAnswerDataLength || nDataLengthCount>199)
				{
					nState++;
				}
			}
			break;
		case 10: //CRC
			nState++;
			cAnswerCRC = indata;
		    break;
		case 11:
			if (indata == 0x00)
			{
//				Serial.println("complete");
				GetDataFromTelegram();
			}
			else
			{
				Serial.println("answer ack missing");
			}
			nState = 0;
			GetDataFromTelegram();
			break;

		default:
			nState = 0;

			break;
		
	}
}

bool IsBroadCast()
{
	bool bRet = false;

    if (cDestination == 0xFE)
    {
		bRet = true;
    }
    else if (cCommand == 0x07 && cSubCommand == 0x00)
    {
	    bRet = true;
    }
    return bRet;
}


//siehe Spec_Prot_7_V1_6_3_D.pdf
void GetDataFromTelegram()
{
	if (cCommand == 0xB5 && cSubCommand == 0x03)
	{
		Extract_B503();
	}
	else if (cCommand == 0xB5 && cSubCommand == 0x04)
	{
		Extract_B504();
	}
	else if (cCommand == 0xB5 && cSubCommand == 0x09)
	{
		Extract_B509();
	}
	else if (cCommand == 0x07 && cSubCommand == 0x00)
	{
		Extract_0700();
	}
}

void Extract_B504()
{
}

void Extract_B503()
{
	if (cDataLength == 2)
	{
		if (cData[0]==0x00 && cData[1]==0x01) //aktueller Fehler
		{
			sCurrentError[0]=cAnswerData[0];
			sCurrentError[1]=cAnswerData[1];
			if (cAnswerData[0] != 0xFF && cAnswerData[1] != 0xFF)
			{
				nCurrentError=strtoul(cAnswerData,NULL,16);
			}
			else
			{
				nCurrentError=-1;
			}
		}
		else if (cData[0]==0x00 && cData[1]==0x02) //aktuelle Warnung
		{
			sCurrentWarning[0]=cAnswerData[0];
			sCurrentWarning[1]=cAnswerData[1];
			if (cAnswerData[0] != 0xFF && cAnswerData[1] != 0xFF)
			{
				nCurrentWarning=strtoul(cAnswerData,NULL,16);
			}
			else
			{
				nCurrentWarning=-1;
			}
		}
		else if (cData[0]==0x01 && cData[1]==0x01) //Fehlerhistorie
		{
		}
		else if (cData[0]==0x01 && cData[1]==0x02)  //Warnungshistorie
		{
		}
	 }
	 else if (cDataLength == 3)
	 {
		       
	 }
}

void Extract_B509()
{
//	Serial.println("cmd B5 09");
		
	if (cData[0]==0x29)
	{
		if (cData[1]==0x01 && cData[2]==0x00) //Vorlauftemperatur VF1
		{

		}
        else if (cData[1]==0x03 && cData[2]==0x00) //Vorlauftemperatur VF2
		{
           fTempVorlauf = GetFromData2C(cAnswerData, 2);
           nTempVorlaufStatus=GetFromBCD(cAnswerData,4);
           
           //PrintData(cAnswerData,9);
           
           Serial.print("VorlaufTemp ");
           Serial.println(fTempVorlauf);
		}
        else if (cData[1]==0x07 && cData[2]==0x00) //Rücklauftemperatur RF1
		{
		}
		else if (cData[1]==0x0F && cData[2]==0x00) //Quellentemperatur
        {
			fTempQuelle = GetFromData2C(cAnswerData, 2);
			nTempQuelleStatus = GetFromBCD(cAnswerData, 4);
			
			//PrintData(cAnswerData,9);
			
			Serial.print("Quellentemperatur ");
			Serial.println(fTempQuelle);
		}
//        else if (cData[1]==0xBB && cData[2]==0x00) //Hocheffizenzpumpenstatus
//        {
//		}
        else if (cData[1]==0xB9 && cData[2]==0x00) //Status Heizkreispumpe
		{
		}
		else if (cData[1]==0xBA && cData[2]==0x00)
		{
			fHeizLeistungMomentan = GetFromBCD(cAnswerData, 2);
			
			//PrintData(cAnswerData,9);
			
			Serial.print("Heizleistung Momentan ");
			Serial.println(fHeizLeistungMomentan);
		}
		else if (cData[1]==0xBB && cData[2]==0x00)
		{
			nHeizLeistungMomentanStatus = GetFromBCD(cAnswerData, 2);
			
			//PrintData(cAnswerData,9);
			
//			Serial.print("Heizstatus Momentan ");
//			Serial.println(nHeizLeistungMomentanStatus);
			
			TranslateStatus();
		}
	}
	else if (cData[0]==0x0D)
	{
		if (cData[1]==0x86 && cData[2]==0x00) //EnergyYieldDayTransfer
		{
		}
		else if (cData[1]==0x07 && cData[2]==0x00) //BufferNtcTo
		{
		} 
		else if (cData[1]==0x08 && cData[2]==0x00) //BufferNtcFrom
		{
		}
		else if (cData[1]==0x02 && cData[2]==0x00) //FlowTempSensor
		{
		}
		else if (cData[1]==0x06 && cData[2]==0x00) //SolarNtcTo
		{
		}
		else if (cData[1]==0x05 && cData[2]==0x00) //SolarNtcFrom
		{
		}
		else if (cData[1]==0x56 && cData[2]==0x00) //YieldSum
		{
		}
		else if (cData[1]==0x3B && cData[2]==0x00) //YieldDay
		{
		}
		else if (cData[1]==0x87 && cData[2]==0x00) //EnergyYieldSum
		{
		}
	}
}

void TranslateStatus()
{
	switch (nHeizLeistungMomentanStatus)
	{
		case 0:
			sprintf(sCurrentState,"Bereitschaft");
            break;
		case 42:
            sprintf(sCurrentState,"Warmwasser");
			break;
		case 50:
            sprintf(sCurrentState,"Heizen");
            break;
        case 64:
            sprintf(sCurrentState,"Heizen");
            break;
		default:
			sprintf(sCurrentState,"unknown %d",nHeizLeistungMomentanStatus);
			break;			
	}
	
	Serial.print("Heizstatus Momentan ");
	Serial.print(sCurrentState);
	Serial.print(" = ");
	Serial.println(nHeizLeistungMomentanStatus);

}

void PrintData(char *data, int length)
{
	for (int i=0; i<length;i++)
	{
		Serial.print(data[i],HEX);
		Serial.print(" ");
	}
	Serial.println();
}

void EbusTest()
{
	Extract_0700();
}


void Extract_0700()
{
	/*
	20 FE 5 50 14 29 12 0 14
	Second= 5
	Minute = 50
	Hour=14
	Day=29
	Month=12
	DayOfWeek=0
	Year=14
	cmd 07 00
	Temp Out -1.88
	41:18:2020 20:80:5
	*/
	
	//TempOutside and DateTime broadcast
	fTempOut = GetFromData2B(cData, 0);
	nSecond = GetFromBCD(cData, 2);
	nMinute = GetFromBCD(cData, 3);
	nHour = GetFromBCD(cData, 4);
	nDay = GetFromBCD(cData, 5);
	nMonth = GetFromBCD(cData, 6);
	nDayOfWeek = GetFromByte(cData, 7);
	nYear = GetFromBCD(cData, 8);
	if (nYear < 2000) 
		nYear += 2000;
	
	PrintData(cData,9);
	
	Serial.println("cmd 07 00");
	Serial.print("Temp Out ");
	Serial.println(fTempOut);
//	Serial.print(GetWeekDay(nDayOfWeek));
	
	
	//String sDay;
	//GetWeekDay(sDay,nDayOfWeek);
	
//	char *sday = new char();
//	GetWeekDay(sday,nDayOfWeek);

	//Serial.print("week day ");
	//Serial.print(sDay);
	//Serial.print(" ");
	Serial.print(nDayOfWeek);
	Serial.print(" , ");
	Serial.print(nDay,DEC);
	Serial.print(".");
	Serial.print(nMonth,DEC);
	Serial.print(".");
	Serial.print(nYear,DEC);	
	Serial.print(" ");
	Serial.print(nHour,DEC);
	Serial.print(":");	
	Serial.print(nMinute,DEC);
	Serial.print(":");
	Serial.println(nSecond,DEC);
	
	//char sTemp[5];
	//sDay.toCharArray(sTemp,3);
	sprintf(sDate,"%02d.%02d.%04d",nDay,nMonth,nYear);
	sprintf(sTime,"%02d:%02d:%02d",nHour,nMinute,nSecond);
	
	Serial.println(sDate);
	Serial.println(sTime);
	
/*
27.03.2015
ca. 21:28
VaillantInterface>
<sender IP="192.168.3.180" name="Arduino"> </sender>
<version name="Vaillant Interface" number="1.3.0"> </version>
<data>
<date date="Fri, 39.03.2021"> </date>
<time time="33:38:04"> </time>
<TempOut value="6.25" unit="C"> </TempOut>
<TempVorlauf value="24.81" unit="C"> </TempVorlauf>
<TempQuelle value="7.56" unit="C"> </TempQuelle>
<HeizLeistungMomentan value="0.00" unit="kW"/>
<Status value="Bereitschaft" unit=""/>
<extTemp0 value="23.50" unit="C" ID="1974"/>
</data>
</VaillantInterface>
*/
}


//Umrechnung siehe Spec_Prot_7_V1_6_3_D.pdf
//http://www.mycsharp.de/wbb2/thread.php?threadid=48671


//void GetWeekDay(char* out, int day)
void GetWeekDay(String &out, int day)
{
	if (day>=0 && day<7)
	{
		switch(day)
		{
			case 0:
				out="Mon";
				//sprintf(out,"Mon");
				break;
			case 1:
				out="Tue";
				//sprintf(out,"Tue");
				break;			
			case 2:
				out="Wed";
				//sprintf(out,"Wed");
				break;			
			case 3:
				out="Thu";
				//sprintf(out,"Thu");
				break;			
			case 4:
				out="Fri";
				//sprintf(out,"Fri");
				break;			
			case 5:
				out="Sat";
				//sprintf(out,"Sat");
				break;			
			case 6:
				out="Sun";
				//sprintf(out,"Sun");
				break;		
			default:	
				out="na";
				//sprintf(out,"na");
				break;
		}
	}
}

char GetFromByte(char *indata, int startpos)
{
	char nRet = (char) 0;
	
	//Serial.println("GetFromByte");
	
	byte nByte = indata[startpos];
	
	nRet=(char) (nByte);
	return nRet;
}

char GetFromBCD(char *indata, int startpos)
{
	//0..99
	char nRet = (char) 0;
	byte nByte = indata[startpos];
	byte nHighNibble = 0xF0;
	byte nLowNibble = 0x0F;

	byte nByteHighNibble = (byte)((nByte & nHighNibble) >> 4);
	byte nByteLowNibble = (byte)(nByte & nLowNibble);
	
	nRet = (char)(nByteHighNibble * 10 + nByteLowNibble);

	return nRet;
}
		
char GetFromData1B(char *indata, int startpos)
{
	//-127 ... 127
	char nRet = (char) 0;


	return nRet;
}
        
char GetFromData1C(char *indata, int startpos)
{
	//0 ..100
	char nRet = (char) 0;

	return nRet;
}
        
double GetFromData2C(char *indata, int startpos)
{
	//-127.99 ... 127.99
	double fRet = 0.0;

	byte nLowByte = indata[startpos];
	byte nHighByte = indata[startpos+1];

	byte nCheck = 0x80;

	byte nCheckResult = (byte)(nHighByte & nCheck);
	byte nHighNibble = 0xF0;
	byte nLowNibble = 0x0F;
		        
	if (nCheckResult != 0) //negativ
	{
		nHighByte = (byte)~nHighByte;
		nLowByte = (byte)~nLowByte;

		byte nLowByteHighNibble = (byte)((nLowByte & nHighNibble) >> 4);
		byte nLowByteLowNibble = (byte)(nLowByte & nLowNibble);

		fRet = -1.0* (double)nHighByte * 16.0 - (double)nLowByteHighNibble - (double)(nLowByteLowNibble +1.0) / 16.0;
	}
	else
	{
		byte nLowByteHighNibble = (byte)((nLowByte & nHighNibble) >> 4);
		byte nLowByteLowNibble = (byte)(nLowByte & nLowNibble);

		fRet = (double)nHighByte * 16.0 + (double)nLowByteHighNibble + (double) nLowByteLowNibble / 16.0;
	}

	return fRet;
}

double GetFromData2B(char *indata, int startpos)
{
	//-2047,99 .. 2047,99
	double fRet = 0.0;

	byte nLowByte = indata[startpos];
	byte nHighByte = indata[startpos+1];

	byte nCheck = 0x80;

	byte nCheckResult = (byte)(nHighByte & nCheck);

	if (nCheckResult != 0) //negativ
	{
		nHighByte = (byte)~nHighByte;
		nLowByte = (byte)~nLowByte;
		fRet = -1.0 * nHighByte - (double)(nLowByte + 1.0) / 256.0;
	}
	else
	{
		fRet = (double)nHighByte + (double)nLowByte / 256.0;
	}
	return fRet;
}

