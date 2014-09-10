#pragma once
#include "stdafx.h"
#include <map>

namespace command
{
#define HELP "help"


	class cmdBase
	{
	public:
		cmdBase(){};
		~cmdBase(){};
		virtual bool receiveCmd(std::vector<std::string> args)
		{
			return false;
		}
	};

	class myCommnand
	{
	public:
		myCommnand(){};
		~myCommnand(){};
		myCommnand(std::string cm, std::string help, cmdBase* p);

		std::string strCommand;
		std::string strHelp;
		cmdBase* pObj;
	};


	typedef std::map<std::string, myCommnand> mapStrCmd;
	typedef std::pair<std::string, myCommnand> mapStrCmdPair;

	void print(const char *fmt, ...);
	void registerCommand(myCommnand cmd);

	void deregisterCommand(std::string strcmd);
	void printAndSave(const char *fmt, ...);
	void startThread();
	DWORD StartThread(LPVOID param);

	class logData : public cmdBase
	{
	public:
		
		~logData();

		void insertGetStringCommand(myCommnand cmd);
		void reciveCommand(std::string strCmd);
		virtual bool receiveCmd(std::vector<std::string> args);
		void deregisterCommand(std::string strcmd);
		static logData* getInstance();
		mapStrCmd commandList;
	private:
		logData(){};
		void operator = (logData& p);


	};
};