/*
 * WebServer.h
 *
 * Created: 04.12.2014 20:31:40
 *  Author: Rene
 */ 


#ifndef WEBSERVER_H_
#define WEBSERVER_H_


void WebServerSetup();
void WebServerLoop();

char StrContains(char *str, char *sfind);
void StrClear(char *str, char length);

#endif /* WEBSERVER_H_ */