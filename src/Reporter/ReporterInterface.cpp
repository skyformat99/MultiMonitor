#include "ReporterInterface.h"
#include "SendReport.h"
#include "Utils.h"

MsgBuilder::MsgBuilder()
{
	m_AllocSize = m_Size = 0;
	m_Buf = NULL;
	this->Append("&&");
};

MsgBuilder::~MsgBuilder()
{
	SAFEDELETE(m_Buf);
	m_AllocSize = m_Size = 0;
}

MsgBuilder::MsgBuilder(const MsgBuilder & data)
{
	m_Buf = new char[data.m_Size+1];
	memcpy(m_Buf, data.m_Buf, data.m_Size+1);
	m_AllocSize = m_Size = data.m_Size;
}

bool MsgBuilder::Append(const char * pMsg,...)
{
	char buf[1024];
	va_list args;
	va_start(args, pMsg);
	int l = vsprintf(buf, pMsg, args);
	va_end(args);
	buf[1023] = '\0';
	
	this->Guarantee(l);
	if( m_Size > 0 )
		memcpy(m_Buf + l, m_Buf, m_Size);
	memcpy(m_Buf, buf, l);
	m_Size += l;
	m_Buf[m_Size] = '\0';
	return true;
}

bool MsgBuilder::Guarantee(int s)
{
	int ns = m_Size + s;
	if( ns < m_AllocSize )
		return true;

	int as = m_AllocSize;
	while( as < ns ) as += 1024;
	char* _Buf = new char[as];
	if( _Buf == NULL )
		return false;

	memcpy(_Buf, m_Buf, m_Size);
	SAFEDELETE(m_Buf);
	m_Buf = _Buf;
	m_AllocSize = as;
	return true;
}

void MsgBuilder::Add(const char* key, const char* val){
	if( strlen(key) >= 100 || strlen(val) >= 500 )
		return;
	this->Append("%s=%s;", key, val);
};
void MsgBuilder::Add(const char* key, const int val){
	if( strlen(key) >= 100)
		return;
	this->Append("%s=%d;", key, val);
};
void MsgBuilder::Add(const char* key, const double val){
	if( strlen(key) >= 100)
		return;
	this->Append("%s=%f;", key, val);
};
void MsgBuilder::Add(const char* key, const unsigned long val){
	if( strlen(key) >= 100)
		return;
	this->Append("%s=%u;", key, val);
};
void MsgBuilder::Add(const char* key, const long val){
	if( strlen(key) >= 100)
		return;
	this->Append("%s=%d;", key, val);
};

char* MsgBuilder::Get() const
{
	return m_Buf;
}
void CStatusReporterInterface::SendBySMS(const char* msg, const char* phone){
	//SendSMS(msg, phone);
};

bool CStatusReporterInterface::ChangeServer(const char* ServerIP, const unsigned short ServerPort){
	EM_TRY
	{
		return CStatusReporter::get().ChangeServer(ServerIP, ServerPort);
	}
	EM_CATCH(...)
	{
		return false;
	}
	EM_CATCHEND
}

const char* CStatusReporterInterface::Ip(){
	return CStatusReporter::get().m_Ip.c_str();
}
const unsigned short CStatusReporterInterface::Port(){
	return CStatusReporter::get().m_Port;
}
void CStatusReporterInterface::Start(const char* ServerIP, const unsigned short ServerPort){
	EM_TRY
	{
		CStatusReporter::get().Start(ServerIP, ServerPort);
	}
	EM_CATCH(...)
	{
		return;
	}
	EM_CATCHEND
};

void CStatusReporterInterface::Stop(){
	EM_TRY
	{
		CStatusReporter::get().Stop();
	}
	EM_CATCH(...)
	{
		return;
	}
	EM_CATCHEND
};


void CStatusReporterInterface::Send(const Json::Value& Msg){
	EM_TRY
	{
		Json::FastWriter writer;
		string strMsg = writer.write(Msg);
		char strLen[128];
		sprintf_s(strLen, 128, "%d", strMsg.length());
		strMsg = string(strLen) + ":" + strMsg + ",";
		CStatusReporter::get().Send(strMsg);
	}
	EM_CATCH(...)
	{
		return;
	}
	EM_CATCHEND
};

void CStatusReporterInterface::Send(const MsgBuilder& Msg){
	EM_TRY
	{
		string strMsg = Msg.Get();
		CStatusReporter::get().Send(strMsg);
	}
	EM_CATCH(...)
	{
		return;
	}
	EM_CATCHEND
};