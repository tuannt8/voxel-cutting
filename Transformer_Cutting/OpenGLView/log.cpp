#include "stdafx.h"
#include "log.h"
#include <string>
#include <sstream>


using namespace std;

void command::print(const char *fmt, ...)
{
	// Get the text
	char text[256];
	va_list ap;

	if (fmt == NULL) {
		return;
	}

	va_start(ap, fmt);
	vsprintf(text, fmt, ap);
	va_end(ap);


	// Do what ever
	cout <<text;
}

void command::registerCommand(myCommnand cmd)
{

}

void command::startThread()
{
	logData::getInstance()->insertGetStringCommand(myCommnand( HELP, "Display all command lists", nullptr ));
	logData::getInstance()->thrd = AfxBeginThread((AFX_THREADPROC)StartThread, nullptr);
}

DWORD command::StartThread(LPVOID param)
{
// 	while (!cin.fail())
// 	{
// 		string s;
// 		getline(cin, s);
// 
// 		logData::getInstance()->reciveCommand(s);
// 
// 	}
	return 0;
}

void command::printAndSave(const char *fmt, ...)
{
	// Get the text
	char text[256];
	va_list ap;

	if (fmt == NULL) {
		return;
	}

	va_start(ap, fmt);
	vsprintf(text, fmt, ap);
	va_end(ap);


	// Do what ever
	cprintf(text);

	// Save to file
}

void command::deregisterCommand(std::string strcmd)
{
	logData::getInstance()->deregisterCommand(strcmd);
}

command::logData::~logData()
{

}

void command::logData::insertGetStringCommand(myCommnand cmd)
{
	if (commandList.find(cmd.strCommand) == commandList.end())
	{
		commandList.insert(mapStrCmdPair(cmd.strCommand, cmd));
	}
}

void command::logData::reciveCommand(std::string strCmd)
{
	// Parse the command
	std::stringstream ss(strCmd);
	std::istream_iterator<std::string> begin(ss);
	std::istream_iterator<std::string> end;
	std::vector<std::string> args(begin, end);
	std::copy(args.begin(), args.end(), std::ostream_iterator<std::string>(std::cout, "\n"));

	if (args.size() == 0)
	{
		cout << "\n>>";
		return;
	}

	// Find
	bool found = false;
	if (commandList.find(args[0]) != commandList.end())
	{
		myCommnand cm = commandList[args[0]];
		if (cm.pObj)
		{
			found = cm.pObj->receiveCmd(args);
		}
		else
			receiveCmd(args);
	}

	if (!found)
	{
		cout << "Command error";
	}
}

bool command::logData::receiveCmd(std::vector<std::string> args)
{
	// This has help command
	if (args[0] == HELP)
	{
		for (mapStrCmd::iterator it = commandList.begin(); it != commandList.end(); ++it)
		{
			myCommnand c = it->second;
			std::cout << c.strCommand << " : " << c.strHelp << endl;
		}
		return true;
	}


	return false;
}

void command::logData::deregisterCommand(std::string strcmd)
{
	if (commandList.find(strcmd) != commandList.end())
	{
		commandList.erase(strcmd);
	}
}

command::logData* command::logData::getInstance()
{
	static logData instance;
	return &instance;
}

void command::logData::setTerminate()
{
	terminating = true;

}

command::myCommnand::myCommnand(std::string cm, std::string help, cmdBase* p)
{
	strCommand = cm;
	strHelp = help;
	pObj = p;
}
