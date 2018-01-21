#pragma once
#include <string>
#include <sstream>
#include "client.h"
#include "server.h"
#include "manual.h"

class Parser
{
public:
	Parser();
	~Parser();
	bool parseString(std::string inputString, char* recData);
	bool parseOp(std::string inputString, char* recData);
	bool parseCMD(std::string inputString, char* recData);
	bool parseScp(std::string inputString, char* recData);
	client currentConnection = client();


	server* managementServer;

};

