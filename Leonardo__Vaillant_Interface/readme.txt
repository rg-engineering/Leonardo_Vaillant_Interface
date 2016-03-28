/*
 * readme.txt
 *
 * Created: 29.12.2014 20:21:50
 *  Author: Rene
 */ 

 Versions
 1.0.0
 * first running version
 * just receives QuellTemp, OutTemp, VorlaufTemp und MomantanLeistung
 * send data not supported yet
 * fix IP address

 1.1.0

 1.2.0

 1.3.0

 1.4.0

 1.5.0

 1.5.1
 * Atmel studio 7.0 and Arduino IDE 1.6.5


 Hints
 * this project uses arduino libs
 * installation path of arduino is C:\Program Files (x86)\Arduino
 * path must be set under "Eigenschaften" "ToolChain" for C and C++
 * some sources from arduino are used as link here in project. (see folder "cores" and "Libs"). They point to the same arduino path
 * arduino version is 1.0.6
 * This project was created in Atmel Studio 6.2 SP1

 * download to Arduino Leonardo using USB via "externe Tools"
	Titel: "Arduino Lenardo Upload"
	Befehl: "python.exe"
	Argumente: "C:\Users\Rene\Documents\Atmel Studio\SerialUploader.py" "C:\Program Files (x86)\Arduino" leonardo "$(ProjectDir)Debug\$(ItemFileName).hex" COM9 COM12

	COM9 is used for BootLoader
	COM12 is used for application
	-> change it based on your system (check in "Gerätemanager" of Win)