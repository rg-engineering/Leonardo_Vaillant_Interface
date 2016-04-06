
/*
  Web Server


 */

#include <SPI.h>
#include <Ethernet.h>
//#include <DHCP.h>

#include "vars.h"
#include "WebServer.h"

// Enter a MAC address and IP address for your controller below.
// The IP address will be dependent on your local network:
byte mac[] = {  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
IPAddress ip(192, 168, 3, 180);
//byte mac[] = {  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xEE };
//IPAddress ip(192, 168, 3, 181);

// Initialize the Ethernet server library
// with the IP address and port you want to use
// (port 80 is default for HTTP):
EthernetServer server(80);

#define REQ_BUF_SZ   50
char HTTP_req[REQ_BUF_SZ] = {0};
char req_index = 0;

void WebServerSetup() 
{
	// start the Ethernet connection and the server:
	Ethernet.begin(mac, ip);
	//Serial.print("try to get IP from DHCP server ");
	//Ethernet.begin(mac);
	//if (Ethernet.begin(mac) == 0)
	//{
	//	Serial.println("Failed to configure Ethernet using DHCP");
	//}
 
	server.begin();
	
	Serial.print("server is at ");
	Serial.println(Ethernet.localIP());
}


void WebServerLoop() 
{
	//Ethernet.maintain(); 
	
	// listen for incoming clients
	EthernetClient client = server.available();
	if (client) 
	{
		//Serial.println("new client");
		// an http request ends with a blank line
		boolean currentLineIsBlank = true;
		while (client.connected()) 
		{
			if (client.available()) 
			{
				char c = client.read();
				if (req_index < (REQ_BUF_SZ - 1)) 
				{
					HTTP_req[req_index] = c;
					req_index++;
				}
				//Serial.write(c);
				// if you've gotten to the end of the line (received a newline
				// character) and the line is blank, the http request has ended,
				// so you can send a reply
				if (c == '\n' && currentLineIsBlank) 
				{

					client.println("HTTP/1.1 200 OK");
					//client.println("Content-Type: text/html"); //auskommentiert für browser? was macht QT?
					client.println("Connection: keep-alive");
					client.println();
					
					client.println("<?xml version='1.0' encoding='utf-8' ?>");
					client.println("<VaillantInterface>");
					client.print("<sender IP = '");
					client.print(Ethernet.localIP());
					client.print("' name= '");
					client.print(sName);
					client.println("'> </sender>");
					
					client.print("<version name = '");
					client.print(sVersionName);
					client.print("' number= '");
					client.print(sVersionNumber);
					client.println("'> </version>");
					
					client.println("<data>");
					client.print("<date date='");
					client.print(sDate);
					client.println("'> </date>");
					client.print("<time time='");
					client.print(sTime);
					client.println("'> </time>");
					
					client.print("<TempOut value='");
					client.print(fTempOut);
					client.print("' unit='");
					client.print(sTempOutUnit);
					client.println("'> </TempOut>");
					
					client.print("<TempVorlauf value='");
					client.print(fTempVorlauf);
					client.print("' unit='");
					client.print(sTempVorlaufUnit);
					client.println("'> </TempVorlauf>");
					client.print("<TempQuelle value ='");
					client.print(fTempQuelle);
					client.print("' unit='");
					client.print(sTempQuelleUnit);
					client.println("'> </TempQuelle>");
					
					client.print("<HeizLeistungMomentan value ='");
					client.print(fHeizLeistungMomentan);
					client.print("' unit='");
					client.print(sHeizLeistungMomentanUnit);
					client.println("'></HeizLeistungMomentan>");
					
					client.print("<Status value ='");
					client.print(sCurrentState);
					client.println("' unit=''></Status>");

					client.print("<Error value ='");
					client.print(nCurrentError);
					client.print(" (");
					client.print(sCurrentError);
					client.println(") ' unit=''></Error>");
					
					client.print("<Warning value ='");
					client.print(nCurrentWarning);
					client.print(" (");
					client.print(sCurrentWarning);
					client.println(") ' unit=''></Warning>");					
					
					
					for (int i=0; i<nTempExternCount;i++)
					{
						client.print("<extTemp");
						client.print(i);
						client.print(" value ='");
						client.print(fTempExtern[i]);
						client.print("' unit='");
						client.print(sTempExternUnit);
						client.print("' ID='");
						client.print(nTempExternID[i]);
						client.print("'></extTemp");
						client.print(i);
						client.println(">");
					}
					client.println("</data>");
					client.println("</VaillantInterface>");
					
					/*
					// send a standard http response header
					client.println("HTTP/1.1 200 OK");
					client.println("Content-Type: text/html");
					client.println("Connection: close");  // the connection will be closed after completion of the response
					client.println("Refresh: 5");  // refresh the page automatically every 5 sec
					client.println();
					client.println("<!DOCTYPE HTML>");
					client.println("<html>");
					// output the value of each analog input pin
					for (int analogChannel = 0; analogChannel < 6; analogChannel++) 
					{
						//int sensorReading = analogRead(analogChannel);
						int sensorReading = analogChannel;
						client.print("analog input ");
						client.print(analogChannel);
						client.print(" is ");
						client.print(sensorReading);
						client.println("<br />");
					}
					client.println("</html>");
				*/
					Serial.print(HTTP_req);
					req_index = 0;
					StrClear(HTTP_req, REQ_BUF_SZ);
					break;
				}
				if (c == '\n') 
				{
					// you're starting a new line
					currentLineIsBlank = true;
				}
				else if (c != '\r') 
				{
					// you've gotten a character on the current line
					currentLineIsBlank = false;
				}
			}
		}
		// give the web browser time to receive the data
		delay(1);
		// close the connection:
		client.stop();
		//Serial.println("client disconnected");
	}
}

void StrClear(char *str, char length)
{
	for (int i = 0; i < length; i++) 
	{
		str[i] = 0;
	}
}

char StrContains(char *str, char *sfind)
{
	char found = 0;
	char index = 0;
	char len;
	len = strlen(str);
	if (strlen(sfind) > len) 
	{
		return 0;
	}
	while (index < len) 
	{
		if (str[index] == sfind[found]) 
		{
			found++;
			if (strlen(sfind) == found) 
			{
				return 1;
			}
		}
		else 
		{
			found = 0;
		}
		index++;
	}
	return 0;
}