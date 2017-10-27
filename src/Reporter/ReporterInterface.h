#ifndef REPORTERINTERFACE_H
#define REPORTERINTERFACE_H
#pragma warning(disable:4786)

#include "json.h"

/* 自定义消息 */
#define REP_TYPE		"ty"
#define REP_MSG			"Msg"
#define REP_APPDATE		"d"
#define REP_APPTIME		"Time"
#define REP_LOCALTIME	"lTime"

class CStatusReporter;
// key1=value1;key2=value2.....keyN=valueN;&&
class MsgBuilder
{
public:
	MsgBuilder();
	MsgBuilder(const MsgBuilder & src);
	~MsgBuilder();
	void Add(const char* key, const long val);
	void Add(const char* key, const char* val);
	void Add(const char* key, const double val);
	void Add(const char* key, const int val);
	void Add(const char* key, const unsigned long val);
	char* Get() const;
private:
	char* m_Buf;
	int m_AllocSize;
	int m_Size;
	bool Guarantee(int s);
	bool Append(const char * pMsg,...);

	friend class CStatusReporter;
};


class CStatusReporterInterface
{
public:
	static void Start(const char* ServerIP, const unsigned short ServerPort);
	static bool ChangeServer(const char* ServerIP, const unsigned short ServerPort);
	static void Stop();

	static void Send(const Json::Value& Msg);
	static void Send(const MsgBuilder& Msg);
	static void SendBySMS(const char* msg, const char* phone); 

	static const char* Ip();
	static const unsigned short Port();
};

#endif