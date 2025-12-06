#ifndef WIFI_MINILIB_H
#define WIFI_MINILIB_H

#include <nds.h>
#include <dswifi9.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>

#define SSID	"MES-NDS"


//WiFi initialization
int initWiFi();
//Socket initialization
int openSocket();
//Receive data trough the socket
int receiveData(int* msg[8], int bytes);
//Send data through the socket
int sendData(int* data_buff[8], int bytes);
//Close the Socket
void closeSocket();
//Disconnect the WiFi
void disconnectFromWiFi();


#endif // WIFI_MINILIB_H
