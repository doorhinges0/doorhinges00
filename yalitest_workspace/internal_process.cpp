


#include <assert.h>

#include <string.h>
#include <string>
#include <vector>
#include <sstream>
#include <stdexcept>

#include "log.h"
#include "msglog.h"
#include "UserData.h"
#include "internal_process.h"

#include <ctype.h>

#include "Shm_Queue.h"

#include "oi_aes.h"
#include "Options.h"

#include "Common.h"

#include "code_conv.h"
#include "donsonim.pb.h"
#include "Status_Api.h"
#include "rb_query.h"
#include "Http_Handle.h"
using namespace std;
using namespace donson::im;

#include <google/protobuf/text_format.h>
using namespace google::protobuf;

#include "Options.h"
//#define ACCOUNTSYSTEM_URL "http://42.121.119.66/Api/Login"
//#define GROUPMEMBER_URL "http://42.121.119.66/Api/SchoolFellowMember"
//#define OFFLINEPUSH_URL "http://42.121.119.66/Api/offlinePush"
//#define ShortMsg_URL "http://api.dongxintong.com/MessageService.asmx/SendSms?username=SOLOMOxm&password=123456&companycode=82851&phonelist=15013640810&msg=IM_Server_Send"
#if 0
//move to head file
#define MAX_MSG_LEN 3072
#define MAX_MSG_NUM 10
#define MAX_CHKGRP_NUM  150
#endif

#if 0
//add by caijunbo,used to get group member ids by groupid and user id
void GetGroupMemberIDs(uint32_t grpid, uint32_t uid, set<uint32_t>& memberIDs)
{
	memberIDs.clear();
	ostringstream ostr;
	ostr << grpid;
	std::string strGrpID(ostr.str());

	ostringstream ostr2;
	ostr2 <<uid;
	std::string strUID(ostr2.str());

	//靠API靠靠靠靠?
	string strBaseIP(ini.apiip);
	string GROUPMEMBER_URL="http://"+strBaseIP+"/Api/SchoolFellowMember";
	Http_Handle httpHandle;
	httpHandle.setURL(GROUPMEMBER_URL);
	httpHandle.addPostParam("a",strGrpID);//group id
	httpHandle.addPostParam("b",strUID);//user id
	httpHandle.addPostParam("c",0);//c靠靠
	httpHandle.addPostParam("d",2);//d靠靠
	string strZtoken="23b71dae-a270-4a36-9492-9d28c0217e2f";
	httpHandle.addPostParam("z",strZtoken);//need to change to dynamic ztoken
	httpHandle.performRequest();

	Json::Value result;
	result=httpHandle.getJsonResult();
	if (result["z"]["a"].asInt()!=200) //靠靠靠?
	{
		LOG( LOG_TRACE, "****error GET Member IDS");
		return;
	}
	int isize=result["a"].size();

//	std::vector<int>::iterator it;
//	it=memberIDs.begin();
	for (int i=0;i<isize;i++)
	{
		uint32_t uid=atoi(result["a"][i]["e"].asString().c_str());
		memberIDs.insert(uid);
	}
}
#endif

void BUILDPACKET( uint32_t ret , const ReqHeader& req , const char* msg , char* buffer, int& length)
{
	RspHeader header;
	string s("");
	header.set_seq(req.seq());
	header.set_cmd(req.cmd());
	header.set_ret(ret);
	header.set_message(msg);
	header.set_ispush(0);
	CommonPkt ack;
	header.SerializeToString(&ack.header);
	ack >> s;
	memcpy( buffer , s.c_str() , s.size() );
	length = s.size();
}

void BUILDACK( uint32_t ret , const ReqHeader& req , const char* msg , const string& pkt, char* buffer, int& length )
{
	RspHeader header;
	string s("");
	header.set_seq(req.seq());
	header.set_cmd(req.cmd());
	header.set_ret(ret);
	header.set_message(msg);
	header.set_ispush(0);
	CommonPkt ack;
	ack.packet = pkt;
	header.SerializeToString(&ack.header);
	ack >> s;
	memcpy( buffer , s.c_str() , s.size() );
	length = s.size();
}

int KEY2SID( const string& session , int uin , int& time )
{
	int iUin = 0;
	int iRet = Key2SID( session.c_str() , iUin , time );
	if ( iRet != 0 || iUin != uin )
	{
		LOG( LOG_ERROR , "SESSION[%s] KEY2SID FAIL RESULT[%d] UIN[%d|%d]" , session.c_str() , iRet , iUin , uin );
		return 1;
	}
	return 0;
}



#define VERIFYPACKET( FD , UIN )	\
{\
	int64_t cid = 0;\
	uint32_t uin = 0;\
	int iRet = g_pstFdInfo->Get( FD , cid , uin );\
	if( iRet != 0 )\
	{\
		LOG( LOG_ERROR , "FD[%d] UIN[%u] VERIFYPACKET GETFD FAIL RESULT[%d]" , FD , UIN , iRet );\
		BUILDPACKET( 1 , header , "fd error" , buffer, length );\
		return 0;\
	}\
	if ( uin != UIN )\
	{\
		LOG( LOG_ERROR , "FD[%d] UIN[%u] PACKET UIN[%u] VERIFYPACKET UIN NOT MATCH" , FD , uin , UIN , iRet );\
		BUILDPACKET( 1 , header , "uin dismatch" , buffer, length );\
		return 0;\
	}\
}

#define VERIFYSESSION( UIN )	\
{\
	User_Session ss;\
	int iRet = g_pstUserInfo->GetSession( UIN , ss );\
	if( iRet != 0 )\
	{\
		LOG( LOG_ERROR , "UIN[%u] VERIFYSESSION FAIL RESULT[%d]" , UIN , iRet );\
		BUILDPACKET( 1 , header , "session error" , buffer, length );\
		return 0;\
	}\
}


std::vector<long> &split(const std::string &s, char delim, std::vector<long> &elems)
{
	std::stringstream ss(s);
	std::string item;
	while (std::getline(ss, item, delim))
	{
		elems.push_back(atol(item.c_str()));
	}
	return elems;
}


std::vector<long> split(const std::string &s, char delim)
{
	std::vector<long> elems;
	return split(s, delim, elems);
}

//查询用户状态
bool getStatus(uint32_t uin, uint32_t tNow)
{
	char asUserStat[32];
	int iRet = GetRBUser(uin, asUserStat);
	if (iRet == 0)	//Maybe Online..
	{
		uint32_t time = *((uint32_t*)&asUserStat[4]);
		LOG(LOG_DEBUG, "USER[%u] STATUS CURTIME[%u] LASTTIME[%u]" , uin, tNow , time );
		if ((int)(tNow - time) <  SESSION_TIMEOUT )
			return true;	//Online
	}
	return false;
}

int getC2CID(uint32_t uin, uint32_t& readid, uint32_t& msgid)
{
	ostringstream rstr, mstr;
	rstr << uin << ".readid";
	mstr << uin << ".msgid";
	const char *key[2];
	char ** val;
	string rs = rstr.str();
	string ms = mstr.str();
	key[0] = rs.c_str();
	key[1] = ms.c_str();
	LOG(LOG_TRACE,"key[%s][%s]", key[0], key[1]);
	int iRet = credis_mget(g_stRedis, 2, key, &val);
	if ( iRet < 0)
	{
		LOG(LOG_ERROR, "getC2CID UIN[%u] Ret[%d]", uin, iRet);
		return iRet;
	}
	readid = (val[0] == NULL) ? 0 : atoi(val[0]);
	msgid = (val[1] == NULL) ? 0 : atoi(val[1]);
	LOG(LOG_DEBUG, "getC2CID[%u][%u][%u]", uin, msgid, readid);
	return 0;
}

int setMsgID(uint32_t uin, uint32_t& msgid, int type)	//type 0:C2C, 1:Grp, 2:notice
{
	ostringstream str;
	if (type == 0)
		str << uin << ".msgid";
	else if (type == 1)
		str << uin << ".grpmsgid";
	else
		str << uin << ".noticeid";
	int iRet = credis_incr(g_stRedis, str.str().c_str(), (int*)&msgid);
	LOG(LOG_DEBUG, "Set UIN[%s][%u]", str.str().c_str(), msgid);
	return iRet;
}

////Notice
int getNoticeID(uint32_t uin, uint32_t& readid, uint32_t& msgid)
{
	ostringstream rstr, mstr;
	rstr << uin << ".noticereadid";
	mstr << uin << ".noticeid";
	const char *key[2];
	char ** val;
	string rs = rstr.str();
	string ms = mstr.str();
	key[0] = rs.c_str();
	key[1] = ms.c_str();
	LOG(LOG_TRACE,"key[%s][%s]", key[0], key[1]);
	int iRet = credis_mget(g_stRedis, 2, key, &val);
	if ( iRet < 0)
	{
		LOG(LOG_ERROR, "getNoticeID UIN[%u] Ret[%d]", uin, iRet);
		return iRet;
	}
	readid = (val[0] == NULL) ? 0 : atoi(val[0]);
	msgid = (val[1] == NULL) ? 0 : atoi(val[1]);
	LOG(LOG_DEBUG, "getNoticeID[%u][%u][%u]", uin, msgid, readid);
	return 0;
}

#if 0
//非事务版本
int setReadID(uint32_t uin, uint32_t msgid, int type = 0)	//type: 0:C2C, 2:notice
{
	uint32_t readid_ =0, msgid_ = 0;
	if ( type == 0)
		getC2CID(uin,readid_, msgid_);
	else
		getNoticeID(uin, readid_, msgid_);
	if ( msgid > msgid_ || msgid <= readid_ )
	{
		LOG(LOG_ERROR, "setReadID ERROR ReqID[%u]|DB MsgID[%u] ReadID[%u]", msgid, msgid_, readid_);
		if (msgid > msgid_)
			msgid = msgid_;
		else
			return -1;
	}
	ostringstream str, keystr;
	if (type == 0)
		str << uin << ".readid";
	else
		str << uin << ".noticereadid";
	keystr << msgid;
	int iRet = credis_set(g_stRedis, str.str().c_str(), keystr.str().c_str());
	LOG(LOG_DEBUG, "Set UIN[%s][%u]", str.str().c_str(), msgid);
	return iRet;
}
#else
int setReadID(uint32_t uin, uint32_t msgid, int type = 0)	//type: 0:C2C, 2:notice
{
	uint32_t readid =0;
	ostringstream str, mstr;
	char* val;
	if( type == 0)
	{
		str << "local item = redis.call('mget', '" << uin << ".readid','"
		<< uin << ".msgid');if (item[1] == false or tonumber(KEYS[1]) > tonumber(item[1])) and"
		<< "(item[2] == false or tonumber(KEYS[1]) <= tonumber(item[2])) then redis.call('set','"
		<< uin << ".readid', KEYS[1]);return KEYS[1]; else return item[1] end;";
	}
	else
	{
		str << "local item = redis.call('mget', '" << uin << ".noticereadid','"
		<< uin << ".noticeid');if (item[1] == false or tonumber(KEYS[1]) > tonumber(item[1])) and"
		<< "(item[2] == false or tonumber(KEYS[1]) <= tonumber(item[2])) then redis.call('set','"
		<< uin << ".noticereadid', KEYS[1]);return KEYS[1]; else return item[1] end;";
	}
	mstr << msgid;
	int iRet = credis_eval1(g_stRedis, str.str().c_str(), mstr.str().c_str(), &val);
	if ( iRet < 0)
	{
		LOG(LOG_ERROR, "setReadID UIN[%u] Type[%d] MsgID[%u] Ret[%d]", uin, type, msgid, iRet);
		return iRet;
	}
	readid = (val == NULL) ? 0 : atoi(val);
	if(msgid != readid)
	{
		LOG(LOG_ERROR, "setReadID UIN[%u] Type[%d] ReqID[%u] ReadID[%u]", uin, type, msgid, readid);
		return -1;
	}
	LOG(LOG_DEBUG, "setReadID|%u|%d|%u", uin, type, msgid);
	return iRet;
}
#endif

int setToken(uint32_t uin, string& token)
{
	ostringstream str;
	str << uin << ".token";
	int iRet = credis_set(g_stRedis, str.str().c_str(), token.c_str());
	LOG(LOG_DEBUG, "SetToken|%d|%u|%s", iRet, uin, token.c_str());
	return iRet;
}

int delToken(uint32_t uin)
{
	ostringstream str;
	str << uin << ".token";
	int iRet = credis_del(g_stRedis, str.str().c_str());
	LOG(LOG_DEBUG, "delToken|%d|%u", iRet, uin);
	return iRet;
}

int getToken(uint32_t uin, string& token)
{
	ostringstream str;
	str << uin << ".token";
	char *val;
	int iRet = credis_get(g_stRedis, str.str().c_str(), &val);
	if( val == NULL)
		token = "";	//无则设空
	else
		token = val;	
	LOG(LOG_DEBUG, "getToken|%d|%u|%s", iRet, uin, val);
	return iRet;
}

int getGrpReadID(uint32_t uin, const ::google::protobuf::RepeatedField< ::google::protobuf::uint32 >& grpids,
                 map<uint32_t,uint32_t>& mapReadId)
{
	ostringstream str, keystr;
	str << uin << ".grpreadid";
	const char* key[MAX_CHKGRP_NUM];
	for ( ::google::protobuf::RepeatedField< ::google::protobuf::uint32 >::const_iterator it = grpids.begin();
	        it != grpids.end(); it++)
	{
		keystr << *it << '\0';
	}
	string s = keystr.str();
	size_t pos = 0;
	for ( int i = 0; i< grpids.size(); i++)
	{
		key[i] = &s[pos];
		pos = s.find('\0', pos) + 1 ;
	}
	LOG(LOG_DEBUG, "getGrpReadID UIN[%u] Count[%u]", uin, grpids.size());
	char** val;
	int iRet = credis_hmget(g_stRedis, str.str().c_str(), grpids.size(), key, &val);
	if ( iRet < 0)
	{
		LOG(LOG_ERROR, "getGrpReadID UIN[%u] Ret[%d]", uin, iRet);
		return iRet;
	}

	pos = 0;
	for (::google::protobuf::RepeatedField< ::google::protobuf::uint32 >::const_iterator it = grpids.begin();
	        it != grpids.end(); it++)
	{
		uint32_t readid = (val[pos] == NULL) ? 0 : atoi(val[pos]);
		mapReadId.insert(pair<uint32_t, uint32_t>(*it, readid));
		LOG(LOG_DEBUG, "getGrpReadID|%u|%u|%u", uin, *it, readid);
		pos++;
	}
	return 0;
}

int getGrpMsgID(uint32_t uin, const ::google::protobuf::RepeatedField< ::google::protobuf::uint32 >& grpids,
                map<uint32_t,uint32_t>& mapMsgId)
{
	ostringstream keystr;
	const char* key[MAX_CHKGRP_NUM];
	for ( ::google::protobuf::RepeatedField< ::google::protobuf::uint32 >::const_iterator it = grpids.begin();
	        it != grpids.end(); it++)
	{
		keystr << *it << ".grpmsgid" << '\0';
	}
	string s = keystr.str();
	size_t pos = 0;
	for ( int i = 0; i< grpids.size(); i++)
	{
		key[i] = &s[pos];
		pos = s.find('\0', pos) + 1 ;
	}
	LOG(LOG_DEBUG, "getGrpMsgID UIN[%u] Count[%u]", uin, grpids.size());
	char** val;
	int iRet = credis_mget(g_stRedis, grpids.size(), key, &val);
	if ( iRet < 0)
	{
		LOG(LOG_ERROR, "getGrpMsgID UIN[%u] Ret[%d]", uin, iRet);
		return iRet;
	}

	pos = 0;
	for (::google::protobuf::RepeatedField< ::google::protobuf::uint32 >::const_iterator it = grpids.begin();
	        it != grpids.end(); it++)
	{
		uint32_t msgid = (val[pos] == NULL) ? 0 : atoi(val[pos]);
		mapMsgId.insert(pair<uint32_t, uint32_t>(*it, msgid));
		LOG(LOG_DEBUG, "getGrpMsgID|%u|%u|%u", uin, *it, msgid);
		pos++;
	}
	return 0;
}

#if 0
int setGrpReadID(uint32_t uin, uint32_t grpid, uint32_t msgid)
{
	ostringstream str, keystr, valstr;
	str << uin << ".grpreadid";
	keystr << grpid;
	valstr << msgid;
	int iRet = credis_hset(g_stRedis, str.str().c_str(), keystr.str().c_str(), valstr.str().c_str());
	LOG(LOG_DEBUG, "Set UIN[%s] GrpID[%u][%u] Ret[%d]", str.str().c_str(), grpid, msgid, iRet);
	return iRet;
}
#else
int setGrpReadID(uint32_t uin, uint32_t grpid, uint32_t msgid)
{
	uint32_t readid =0;
	ostringstream str, mstr;
	char* val;
	
	str << "local rid = redis.call('hget', '" << uin << ".grpreadid', '" << grpid << "');"
		<< "local mid = redis.call('get', '" << grpid << ".grpmsgid');"
		<< "if (rid == false or tonumber(KEYS[1]) > tonumber(rid)) and"
		<< "(mid == false or tonumber(KEYS[1]) <= tonumber(mid)) then redis.call('hset','"
		<< uin << ".grpreadid', '" << grpid << "', KEYS[1]);return KEYS[1]; else return rid end;";
	
	mstr << msgid;
	int iRet = credis_eval1(g_stRedis, str.str().c_str(), mstr.str().c_str(), &val);
	if ( iRet < 0)
	{
		LOG(LOG_ERROR, "setGrpReadID UIN[%u] GrpID[%u] MsgID[%u] Ret[%d]", uin, grpid, msgid, iRet);
		return iRet;
	}
	readid = (val == NULL) ? 0 : atoi(val);
	if(msgid != readid)
	{
		LOG(LOG_ERROR, "setGrpReadID UIN[%u] GrpID[%u] ReqID[%u] ReadID[%u]", uin, grpid, msgid, readid);
		return -1;
	}
	LOG(LOG_DEBUG, "setGrpReadID|%u|%u|%u", uin, grpid, msgid);
	return iRet;
}
#endif

int setMsg(uint32_t uin, uint32_t msgid,  const char* msg, int type)	//type 0:C2C, 1:Grp, 2:Notice
{
	ostringstream str, keystr;
	if (type == 0)
		str << uin << ".msg";
	else if (type == 1)
		str << uin << ".grpmsg";
	else
		str << uin << ".notice";
	keystr << msgid;
	int iRet = credis_hset(g_stRedis, str.str().c_str(), keystr.str().c_str(), msg);
	if ( type == 0 )
	{
		LOG(LOG_DEBUG, "setMsg UIN[%u] MsgID[%u] Msg[%s] Ret[%d]", uin, msgid, msg, iRet);
	}
	else if ( type == 1)
	{
		LOG(LOG_DEBUG, "setGrpMsg GrpID[%u] MsgID[%u] Msg[%s] Ret[%d]", uin, msgid, msg, iRet);
	}
	else
	{
		LOG(LOG_DEBUG, "setNotice UIN[%u] MsgID[%u] Msg[%s] Ret[%d]", uin, msgid, msg, iRet);
	}
	return iRet;
}

int getMsg(uint32_t uin, uint32_t msgid, uint32_t& count, RspOfflineMsg& offmsg )
{
	ostringstream str, mstr;
	str << uin << ".msg";
	const char* key[MAX_MSG_NUM];
	for ( uint32_t i = 0; i< count; i++)
	{
		uint32_t mid = msgid - i;
		mstr << mid << '\0';
		if (mid == 1)
		{
			count = i + 1;
			break;
		}
	}
	string s = mstr.str();
	size_t pos = 0;
	for (uint32_t i = 0; i< count; i++)
	{
		key[i] = &s[pos];

		pos = s.find('\0', pos) + 1 ;
	}
	LOG(LOG_DEBUG, "getMsg UIN[%u] MsgID[%u] Count[%u]", uin, msgid, count);
	char** val;
	int iRet = credis_hmget(g_stRedis, str.str().c_str(), count, key, &val);
	if ( iRet < 0)
	{
		LOG(LOG_ERROR, "getMsg UIN[%u] Ret[%d]", uin, iRet);
		return iRet;
	}
	for (uint32_t i = 0; i < count; i ++)
	{
		if (val[i]!= NULL)
		{
//			LOG(LOG_TRACE, "getMsg UIN[%u] MSGID[%s] Msg[%s]", uin, key[i], val[i]);
			char sPacket[MAX_MSG_LEN];
			int iLen = MAX_MSG_LEN;

			iRet = base64_decode(val[i], strlen(val[i]), sPacket, &iLen);
			if (iRet != 0 || iLen == 0)
			{
				LOG(LOG_ERROR, "getMsg base64 decode error UIN[%u] MSGID[%s] Msg[%s]", uin , key[i] , val[i]);
				continue;
			}
			PushMsg msg;
			bool bRet = msg.ParseFromArray(sPacket, iLen);
			if (!bRet)
			{
				string sHex = HexShow(sPacket, iLen, 0);
				LOG(LOG_ERROR, "getMsg Parse error UIN[%u] MSGID[%s] Msg[%s] Hex[%s]", uin , key[i] , val[i], sHex.c_str());
				continue;
			}
			//		string sMsg;
			//		TextFormat::PrintToString(msg, &sMsg);
			//		LOG(LOG_DEBUG, "getMsg UIN[%u] MSGID[%s]|%s", uin, key[i], sMsg.c_str());
			PushMsg* pushmsg = offmsg.add_messages();
			pushmsg->CopyFrom(msg);
		}
	}
	if (msgid - count > 0)
		offmsg.set_msgid(msgid - count);
	else
		offmsg.set_msgid(0);
	return 0;
}

int getGrpMsg(uint32_t uin, uint32_t grpid, uint32_t msgid, uint32_t& count, RspGetGrpMsg& grpmsg )
{
	ostringstream str, mstr;
	char uinkey[64] = {0};
	char uinfield[32] = {0};
	//char *minval = NULL;
	//int minmsgid = 0;
	sprintf(uinkey,"%u.grpusermsgid",grpid);
	sprintf(uinfield,"%u",uin);
	str << grpid << ".grpmsg";
	const char* key[MAX_MSG_NUM];
	/*int rc = credis_hget(g_stRedis,uinkey,uinfield,&minval);
	if(!rc){
		minmsgid = atoi(minval);
		LOG(LOG_DEBUG, "getminMsg uin=%d,minmsgid=%d,success ",uin,minmsgid);
	}
	else{
	    LOG(LOG_DEBUG, "getminMsg uin=%d,fail ,minmsgid=0",uin);
	}
	LOG(LOG_DEBUG, "Uin[%u],getminMsg %s=%dvs%d",uin,uinkey,msgid,minmsgid);
	if(msgid- minmsgid <=0 ){
		LOG(LOG_DEBUG, "No Offline message");
		return 0;
	}*/
	
 	//LOG(LOG_DEBUG, "getminMsg %s=%dvs%d",uinkey,msgid,minmsgid);
	for ( uint32_t i = 0; i< count; i++)
	{
		uint32_t mid = msgid - i;
		mstr << mid << '\0';
		if (mid ==1/* minmsgid*/)
		{
			count = i + 1;
			break;
		}
	}
	
	string s = mstr.str();
	size_t pos = 0;
	for (uint32_t i = 0; i< count; i++)
	{
		key[i] = &s[pos];

		pos = s.find('\0', pos) + 1 ;
	}
	LOG(LOG_DEBUG, "getGrpMsg UIN[%u] GrpID[%u] MsgID[%u] Count[%u]", uin, grpid, msgid, count);
	char** val;
	int iRet = credis_hmget(g_stRedis, str.str().c_str(), count, key, &val);
	if ( iRet < 0)
	{
		LOG(LOG_ERROR, "getGrpMsg UIN[%u] GrpID[%u] Ret[%d]", uin, grpid, iRet);
		return iRet;
	}
	for (uint32_t i = 0; i < count; i ++)
	{
		if (val[i]!= NULL)
		{
//			LOG(LOG_TRACE, "getMsg UIN[%u] MSGID[%s] Msg[%s]", uin, key[i], val[i]);
			char sPacket[MAX_MSG_LEN];
			int iLen = MAX_MSG_LEN;

			iRet = base64_decode(val[i], strlen(val[i]), sPacket, &iLen);
			if (iRet != 0 || iLen == 0)
			{
				LOG(LOG_ERROR, "getMsg base64 decode error GrpID[%u] MSGID[%s] Msg[%s]", grpid , key[i] , val[i]);
				continue;
			}
			PushGrpMsg msg;
			bool bRet = msg.ParseFromArray(sPacket, iLen);
			if (!bRet)
			{
				string sHex = HexShow(sPacket, iLen, 0);
				LOG(LOG_ERROR, "getGrpMsg Parse error GrpID[%u] MSGID[%s] Msg[%s] Hex[%s]", grpid , key[i] , val[i], sHex.c_str());
				continue;
			}
			//		string sMsg;
			//		TextFormat::PrintToString(msg, &sMsg);
			//		LOG(LOG_DEBUG, "getMsg UIN[%u] MSGID[%s]|%s", uin, key[i], sMsg.c_str());
			PushGrpMsg* pushmsg = grpmsg.add_messages();
			pushmsg->CopyFrom(msg);
		}
	}
	if (msgid - count > 0)
		grpmsg.set_msgid(msgid - count);
	else
		grpmsg.set_msgid(0);
	return 0;
}


int getNotice(uint32_t uin, uint32_t msgid, int count, RspNotice& notice )
{
	LOG(LOG_DEBUG,"getNotice|%u|%u|%d",uin, msgid, count);
	if (count <= 0)
	{
		//无下发数据
		return 0;
	}
	ostringstream str, mstr;
	str << uin << ".notice";
	const char* key[MAX_MSG_NUM];
	for ( int i = 0; i< count; i++)
	{
		uint32_t mid = msgid + i;
		mstr << mid << '\0';
	}
	string s = mstr.str();
	size_t pos = 0;
	for ( int i = 0; i< count; i++)
	{
		key[i] = &s[pos];

		pos = s.find('\0', pos) + 1 ;
	}
	char** val;
	int iRet = credis_hmget(g_stRedis, str.str().c_str(), count, key, &val);
	if ( iRet < 0)
	{
		LOG(LOG_ERROR, "getNotice UIN[%u] Ret[%d]", uin, iRet);
		return iRet;
	}
	for ( int i = 0; i < count; i ++)
	{
		if (val[i]!= NULL)
		{
//			LOG(LOG_TRACE, "getMsg UIN[%u] MSGID[%s] Msg[%s]", uin, key[i], val[i]);
			char sPacket[MAX_MSG_LEN];
			int iLen = MAX_MSG_LEN;

			iRet = base64_decode(val[i], strlen(val[i]), sPacket, &iLen);
			if (iRet != 0 || iLen == 0)
			{
				LOG(LOG_ERROR, "getMsg base64 decode error UIN[%u] MSGID[%s] Msg[%s]", uin , key[i] , val[i]);
				continue;
			}
			PushMsg msg;
			bool bRet = msg.ParseFromArray(sPacket, iLen);
			if (!bRet)
			{
				string sHex = HexShow(sPacket, iLen, 0);
				LOG(LOG_ERROR, "getMsg Parse error UIN[%u] MSGID[%s] Msg[%s] Hex[%s]", uin , key[i] , val[i], sHex.c_str());
				continue;
			}
			//		string sMsg;
			//		TextFormat::PrintToString(msg, &sMsg);
			//		LOG(LOG_DEBUG, "getMsg UIN[%u] MSGID[%s]|%s", uin, key[i], sMsg.c_str());
			PushMsg* pushmsg = notice.add_messages();
			pushmsg->CopyFrom(msg);
		}
	}
	return 0;
}


//static char sRespBuf[10240];

string  apiresponse(/* int */ uint32_t UIN )
{
        char buff[32] = {0};
        sprintf(buff,"%d",UIN);
        char buff2[32] = {0};
        sprintf(buff2,"%d",UIN);
        char buff3[32] = {0};
        sprintf(buff3,"%ld",13800010001);

        char buffi[32] = {0};
        sprintf(buffi,"%d",0);
        char buffa[32] = {0};
        sprintf(buffa,"%d",200);



        string sret = "{ \"a\":";
        sret.append(buffi);
        sret.append(",\"b\":\"");
        sret.append(buff2);
        sret.append("\",\"c\":\"");
        sret.append(buff2);
        sret.append("\",\"d\":");
        sret.append(buff3);
        sret.append(",\"e\":");
        sret.append(buffi);

        sret.append(",\"f\":[");
        sret.append("false, false");
        sret.append("]");

        sret.append(",\"g\":\"");
        sret.append("test1");
        sret.append("\",\"h\":\"");
        sret.append("test2");
        sret.append("\",\"i\":");
        sret.append(buffi);
        sret.append(",\"z\":{\"a\":");
        sret.append(buffa);
        sret.append("}}");

        return sret;

}


int ProcessLogin( char *buffer , int &length , int64_t id , ReqHeader& header, string& packet )
{


//	 Http_Handle httpHandle;
//	 httpHandle.setURL(ShortMsg_URL);
//	httpHandle.performRequest();
	int iRet = 0;

	//get apibaseip

	//get ztoken begins
	ReqLogin req;
	bool bRet = req.ParseFromString(packet);

	if (!bRet)
	{
		string sHex = HexShow(packet.c_str(), packet.size(), 0);
		LOG(LOG_ERROR, "Parse Login MSG[%s]", sHex.c_str());

		BUILDPACKET( 1 , header , "parse packet error" , buffer, length );
		return HTTP_SUCCESS;
	}

	//uint32_t UIN = 0;
	static uint32_t UIN = 800000;
	Json::Value result;
	//add by caijunbo
	//verify user name and password
	std::string strUserName=req.username();
	std::string strPasswd=req.password();
	std::string strDeviceToken=req.devicetoken();
	int iDeviceType=req.devicetype();

	{
		LOG( LOG_TRACE, "**login [%s][%s][%d]" , strUserName.c_str(),
		     strDeviceToken.c_str(), iDeviceType);

	//	ostringstream ostr;
	//	ostr << iDeviceType;
	//	std::string strDeviceType(ostr.str());
#if 0
		string strBaseIP(ini.apiip);
		string ACCOUNTSYSTEM_URL="http://"+strBaseIP+"/Api/Login";
		
		 LOG( LOG_TRACE, "**ip address is[%s]" ,ACCOUNTSYSTEM_URL.c_str() );
		
		Http_Handle httpHandle;
		httpHandle.setURL(ACCOUNTSYSTEM_URL);
		httpHandle.addPostParam("a",strUserName);//user name
		httpHandle.addPostParam("b",strPasswd);//passwd
		httpHandle.addPostParam("c",strDeviceToken);//Device Token
		httpHandle.addPostParam("d",iDeviceType);//Device type
		httpHandle.performRequest();


		result=httpHandle.getJsonResult();
		LOG( LOG_TRACE, "**result %s" , result.toStyledString().c_str());
		//fail to login
		if (result["a"].asInt()!=0)
		{
			LOG( LOG_ERROR, "USER[%s] FAIL AUTHENTICATION" , strUserName.c_str());
			BUILDPACKET(result["a"].asInt(), header , "FAIL AUTHENTICATION" , buffer , length );
			return HTTP_SESSION_ERROR;
		}

		LOG( LOG_DEBUG, "CheckLogin Ret[%d] USER[%s] UIN[%s]" , result["a"].asInt(), strUserName.c_str(), result["b"].asString().c_str());
		UIN = (uint32_t)atoi(result["b"].asString().c_str());
		if (UIN == 0)
		{
			LOG( LOG_ERROR, "USER[%s] UIN[%s] GetUin Fail", strUserName.c_str(), result["b"].asString().c_str());
			BUILDPACKET(1, header, "FAIL AUTHENTICATION" , buffer , length );
			return HTTP_SESSION_ERROR;
		}
		//end adding by caijunbo
		//get ztoken ends
#endif
	}


	//查询用户是否在线，在线则推送下线消息
	User_Session s;
	string sPacket;
	iRet = g_pstUserInfo->GetSession(UIN, s);
	if (iRet == 0)
	{
		string sToken;
		iRet = getToken(UIN, sToken);
#if 0
		if (iRet == 0 && sToken != strDeviceToken)
		{
			//下发离线通知
			LOG(LOG_DEBUG, "Notify Offline Msg UIN[%u] Token[%s] New[%s]", UIN,
			    sToken.c_str(),strDeviceToken.c_str());
			PushMsg offnotice;
			offnotice.set_fromuin(UIN);
			offnotice.mutable_msg()->set_msgid(0);
			offnotice.mutable_msg()->set_time(time(NULL));
			RichMsg* offrm = offnotice.mutable_msg()->add_messages();
			offrm->set_msg("");
			offrm->set_type( Msg_Offline );
			offnotice.SerializeToString(&sPacket);

			RspHeader rspHeader;
			string sPushMsg("");

			rspHeader.set_cmd(CMD_PUSHMSG);
			rspHeader.set_ispush(1);
			rspHeader.set_ret(0);
			rspHeader.set_message("");
			rspHeader.set_seq(header.seq());

			CommonPkt ack;
			ack.packet = sPacket;
			rspHeader.SerializeToString(&ack.header);
			ack >> sPushMsg;


			iRet = g_pstUserInfo->SendSessionMessage(s, sPushMsg);
			if ( iRet != 0 )
			{
				LOG(LOG_ERROR,"Notify Offline Msg UIN[%u] RET[%d]", UIN, iRet);
			}
			//g_pstFdInfo->Set(CONNECTION_FD(s.cid),CONNECTION_FD(s.cid), 0);
		}
#endif
		g_pstFdInfo->Set(CONNECTION_FD(s.cid),s.cid, 0);
	}
	LOG(LOG_TRACE,"*******LOGIN SUCESSFULL");
	//设置状态
	static char szErrMsg[ 1024 ] = { 0 };
	iRet = Status_Set( UIN , 1 , 500 , szErrMsg , sizeof( szErrMsg ) );
	LOG( LOG_DEBUG , "LOGIN UIN[%d] STATUS RET[%d]" , UIN , iRet );
	//现在失败，后续心跳包会继续维护在线状态
	if ( iRet != 0 )
	{
		LOG( LOG_ERROR , "LOGIN UIN[%d] STATUS FAIL RET[%d] MSG[%s]" , UIN , iRet , szErrMsg );
	}

	iRet = g_pstUserInfo->InsertSession( UIN , id ,  s  );

	if ( iRet != 0 )
	{
		LOG( LOG_ERROR, "UIN[%u] CREATE SESSION FAIL RESULT[%d]" , UIN , iRet );
		BUILDPACKET( 3 , header , "Insert Session error" , buffer , length );
		return HTTP_SESSION_ERROR;
	}

	iRet = setToken(UIN, strDeviceToken);
	if ( iRet != 0 )
	{
		LOG( LOG_ERROR, "UIN[%u] SET TOKEN FAIL RESULT[%d]" , UIN , iRet );
	}

	//set fd
	iRet = g_pstFdInfo->Set( CONNECTION_FD( id ), s.cid, s.uin);


	RspLogin rsp;
	rsp.set_token(s.sKey);
	rsp.set_apiresponse(apiresponse(UIN).c_str());
	
	//rsp.set_apiresponse(result.toStyledString().c_str());


	//拉离线
	uint32_t readid = 0, msgid = 0;
	rsp.mutable_msg()->set_readid(readid);
	rsp.mutable_msg()->set_msgid(msgid);
#if 0	
	if ((iRet = getC2CID(UIN, readid, msgid)) == 0)
	{
		rsp.mutable_msg()->set_readid(readid);
		if (msgid > readid)	//需要下发
		{
			uint32_t count = msgid - readid;
			if (count >MAX_MSG_NUM)
				count = MAX_MSG_NUM;
			getMsg( UIN, msgid, count, *rsp.mutable_msg());
		}
		else if ( msgid == readid )
		{
			rsp.mutable_msg()->set_msgid(msgid);
#if 0
			//测试下发消息
			uint32_t count = MAX_MSG_NUM;
			getMsg( UIN, msgid, count, *rsp.mutable_msg());
#endif
		}
	}
#endif
	rsp.SerializeToString(&sPacket);
	BUILDACK( 0 , header , "" , sPacket, buffer, length );

	//5 send buffered messages
#if 0
	int len = ini.socket_bufsize;
	g_pstSessionData->Get( uin , buffer , len );
	length = len;
#endif

	UIN++;
	return HTTP_LONGCONN_SUCCESS;
}

int ProcessHeartBeat( char *buffer , int &length , int64_t id , ReqHeader& header, string& packet )
{
//	int iRet = 0;
	uint32_t UIN = header.uin();
	VERIFYPACKET( CONNECTION_FD( id ) , UIN );
	//Session续期
	VERIFYSESSION( UIN );

	static char szErrMsg[ 1024 ] = { 0 };
	int iRet = Status_Set( UIN , 1 , 500 , szErrMsg , sizeof( szErrMsg ) );
	LOG( LOG_DEBUG , "HB UIN[%d] STATUS RET[%d]" , UIN , iRet );
	//现在失败，后续心跳包会继续维护在线状态
	if ( iRet != 0 )
	{
		LOG( LOG_ERROR , "HB UIN[%d] STATUS FAIL RET[%d] MSG[%s]" , UIN , iRet , szErrMsg );
	}
	RspHeartBeat rsp;
	string sPacket;
	rsp.SerializeToString(&sPacket);
	BUILDACK( 0 , header , "" , sPacket, buffer, length );
	return HTTP_LONGCONN_SUCCESS;
}

int ProcessReLogin( char *buffer , int &length , int64_t id , ReqHeader& header, string& packet )
{
	uint32_t UIN = header.uin();
	//Session续期
	User_Session ss;
	int iRet = g_pstUserInfo->GetSession( UIN , ss );
	if ( iRet != 0 )
	{
		LOG( LOG_ERROR , "UIN[%u] VERIFYSESSION FAIL RESULT[%d]" , UIN , iRet );
		BUILDPACKET( 1 , header , "session error" , buffer, length );
		return 0;
	}

	ReqReLogin req;
	bool bRet = req.ParseFromString(packet);

	if (!bRet)
	{
		string sHex = HexShow(packet.c_str(), packet.size(), 0);
		LOG(LOG_ERROR, "Parse ReLogin MSG[%s]", sHex.c_str());
		BUILDPACKET( 1 , header , "parse packet error" , buffer, length );
		return HTTP_SUCCESS;
	}

	if ( strlen(ss.sKey) != req.token().size() || strncmp( req.token().c_str(), ss.sKey , req.token().size()) != 0)
	{
		LOG(LOG_ERROR, "Parse ReLogin SKEY[%s][%s]", req.token().c_str(), ss.sKey);
		BUILDPACKET( 1 , header , "ReLogin error" , buffer, length );
		return HTTP_SUCCESS;
	}

	static char szErrMsg[ 1024 ] = { 0 };
	iRet = Status_Set( UIN , 1 , 500 , szErrMsg , sizeof( szErrMsg ) );
	LOG( LOG_DEBUG , "HB UIN[%d] STATUS RET[%d]" , UIN , iRet );
	//现在失败，后续心跳包会继续维护在线状态
	if ( iRet != 0 )
	{
		LOG( LOG_ERROR , "HB UIN[%d] STATUS FAIL RET[%d] MSG[%s]" , UIN , iRet , szErrMsg );
	}
	RspReLogin rsp;
	string sPacket;
	rsp.SerializeToString(&sPacket);
	BUILDACK( 0 , header , "" , sPacket, buffer, length );
	return HTTP_LONGCONN_SUCCESS;

}




int ProcessUpMsg( char *buffer , int &length , int64_t id , ReqHeader& header, string& packet )
{
	int iRet = 0;
	uint32_t UIN = header.uin();
	VERIFYPACKET( CONNECTION_FD( id ) , UIN );
	//Session续期
	VERIFYSESSION( UIN );

	ReqUpMsg req;
	bool bRet = req.ParseFromString(packet);

	if (!bRet)
	{
		string sHex = HexShow(packet.c_str(), packet.size(), 0);
		LOG(LOG_ERROR, "Parse UpMsg UIN[%u] MSG[%s]", UIN, sHex.c_str());

		BUILDPACKET( 1 , header , "parse packet error" , buffer, length );
		return HTTP_SUCCESS;
	}

	PushMsg msg;
	msg.set_fromuin(UIN);
	msg.mutable_msg()->CopyFrom(req.msg());

	//判断是否重试请求
	//...

	//生成MsgID
	uint32_t msgid = 0;
	if ( (iRet = setMsgID(req.touin(), msgid)) != 0 )	//touin的msgid
	{
		BUILDPACKET( 1 , header , "msgid error" , buffer , length );
		return HTTP_LONGCONN_SUCCESS;
	}

	time_t tNow = time(NULL);
	msg.mutable_msg()->set_msgid(msgid);
	msg.mutable_msg()->set_time((uint32_t)tNow);

	//存储消息
	string sPacket;
//	TextFormat::PrintToString(msg, &sPacket);
//	LOG(LOG_DEBUG, "UpMsg TO[%u]|%s", req.touin(), sPacket.c_str());
	LOG(LOG_DEBUG, "UpMsg FROM[%u] TO[%u] MSGID[%u]", UIN, req.touin(), msgid);

	msg.SerializeToString(&sPacket);

	if ( sPacket.size() > MAX_MSG_LEN ) 	//2048
	{
		//考虑放弃该消息
		LOG(LOG_ERROR, "FROM[%u] TO[%u] MSG LEN[%u]", UIN, req.touin(), sPacket.size());
		BUILDPACKET( 2 , header , "msg too long" , buffer , length );
		return HTTP_LONGCONN_SUCCESS;
	}

	static char b64Packet[MAX_MSG_LEN * 2];
	int iLen = MAX_MSG_LEN * 2;
	string sHex = HexShow(sPacket.c_str(), sPacket.size(), 0);
	base64_encode(sPacket.c_str(), sPacket.size(), b64Packet, &iLen);
	LOG(LOG_DEBUG, "UpMsg Len[%d] Msg[%s] Hex[%s]", iLen, b64Packet, sHex.c_str());

	if ( (iRet = setMsg( req.touin(), msgid, b64Packet )) != 0 )	//touin的msgid
	{
		BUILDPACKET( 1 , header , "msgdb error" , buffer , length );
		return HTTP_LONGCONN_SUCCESS;
	}

	if (req.msg().messages_size() > 0)
	{
		MSGLOG("0|%u|%u|%u|%u|%s", UIN ,req.touin(), msgid, req.msg().messages(0).type(),
		       req.msg().messages(0).msg().c_str());
	}

	//查用户是否在线
	bool bOnline = getStatus(req.touin(), tNow);

	if ( bOnline )
	{
		//在线下发
		//暂时只有一台接入，就直接下发本地
		//后续失败也当正常
		//modify by caijunbo
		RspHeader rspHeader;
		string sPushMsg("");

		rspHeader.set_cmd(CMD_PUSHMSG);
		rspHeader.set_ispush(1);
		rspHeader.set_ret(0);
		rspHeader.set_message("");
		rspHeader.set_seq(header.seq());

		CommonPkt ack;
		ack.packet = sPacket;
		rspHeader.SerializeToString(&ack.header);
		ack >> sPushMsg;

		iRet = g_pstUserInfo->SendUserMessage(req.touin(), sPushMsg);
		//end modifying
		if (iRet != 0)
		{
			LOG(LOG_ERROR, "UIN[%u] SENDMSG ERROR[%d]", req.touin(), iRet);
		}
	}
	else
	{
		string sToken;
		if ((iRet = getToken(UIN, sToken)) == 0 && sToken != "")
		{
			//走iphonepush通道

			ostringstream ostr1;
			ostr1<< UIN;
			std::string fromID(ostr1.str());

			ostringstream ostr2;
			ostr2<< req.touin();
			std::string strToID(ostr2.str());

			//获取消息类型和内容
			enum MsgType msgType=req.mutable_msg()->mutable_messages(0)->type();
			string strContent;
			int iMsgType=4;
			//文本消息类型
			if (msgType==Msg_Text)
			{
				strContent=req.mutable_msg()->mutable_messages(0)->msg();
			}
			//非文本消息类型
			else
			{
				switch (msgType)
				{
				case Msg_Pic:
					strContent="给你发送了图片";
					iMsgType=2;
					break;
				case Msg_Audio:
					strContent="给你发送了音频";
					iMsgType=3;
					break;
				case Msg_File:
					strContent="给你发送了文件";
					break;
				case Msg_Offline:
					break;
				default:
					break;
				}
			}

			Json::Value toIDArray;
			toIDArray.append(strToID);

			string strBaseIP(ini.apiip);
			string OFFLINEPUSH_URL="http://"+strBaseIP+"/Api/offlinePush";
			LOG(LOG_TRACE,"********ABOUT TO DO APPLE PUSH***");
			Http_Handle httpHandle;
			httpHandle.setURL(OFFLINEPUSH_URL);
			httpHandle.addPostParam("a",fromID);//user name
			httpHandle.addPostParam("b",toIDArray);//passwd
			httpHandle.addPostParam("c",strContent);//Device Token
			httpHandle.addPostParam("d",1);// 1表示单聊
			string strGrpID="0";
			httpHandle.addPostParam( "e", strGrpID);//不需要群id
			httpHandle.addPostParam( "f", 1);// 1靠靠靠?

			httpHandle.addPostParam( "g", iMsgType);// 1.靠2.靠3.靠4.靠
			string strzToken="23b71dae-a270-4a36-9492-9d28c0217e2f";
			httpHandle.addPostParam("z",strzToken);//z token 临时使用
			httpHandle.performRequest();

			Json::Value result;
			result=httpHandle.getJsonResult();

			LOG( LOG_TRACE, "****server offline rsp:[%s]",result.toStyledString().c_str());
			if (result["a"].asInt()!=0) //靠靠靠?
			{
				LOG( LOG_TRACE, "****error send offline message");
			}
		}
	}

	//回包
	RspUpMsg rsp;
	rsp.set_msgid(msgid);
	rsp.set_touin(req.touin());
	rsp.set_time((uint32_t)tNow);
	rsp.SerializeToString(&sPacket);
	BUILDACK( 0 , header , "" , sPacket, buffer, length );

	return HTTP_LONGCONN_SUCCESS;
}

#if 0
int ProcessUpGrpMsg( char *buffer , int &length , int64_t id , ReqHeader& header, string& packet )
{
	int iRet = 0;
	uint32_t UIN = header.uin();
	VERIFYPACKET( CONNECTION_FD( id ) , UIN );
	//Session续期
	VERIFYSESSION( UIN );

	ReqUpGrpMsg req;
	bool bRet = req.ParseFromString(packet);

	if (!bRet)
	{
		LOG(LOG_ERROR, "Parse UpGrpMsg UIN[%u]", UIN);

		BUILDPACKET( 1 , header , "parse packet error" , buffer, length );
		return HTTP_SUCCESS;
	}

	uint32_t GRPID=req.grpid();

	PushGrpMsg msg;
	msg.set_fromuin(UIN);
	msg.set_grpid(GRPID);
	msg.mutable_msg()->CopyFrom(req.msg());

	//判断是否重试请求
	//...

	//生成MsgID
	uint32_t msgid = 0;
	if ( (iRet = setMsgID(req.grpid(), msgid, 1)) != 0 )	//groupid的msgid
	{
		LOG(LOG_ERROR, "UpGrpMsg Set MSGID[%u] UIN[%u] GRPID[%u]", msgid, UIN, req.grpid()); 
		BUILDPACKET( 1 , header , "msgid error" , buffer , length );
		return HTTP_LONGCONN_SUCCESS;
	}
	if ( (iRet = setGrpReadID(UIN, req.grpid(), msgid)) != 0)
	{
		LOG(LOG_ERROR, "UpGrpMsg Set GrpReadID[%d] UIN[%u] GRPID[%u]", msgid, UIN, req.grpid());
		//continue even readid set error...
	}
	
	time_t tNow = time(NULL);
	msg.mutable_msg()->set_msgid(msgid);
	msg.mutable_msg()->set_time((uint32_t)tNow);

	//存储消息
	string sPacket;
//	TextFormat::PrintToString(msg, &sPacket);
//	LOG(LOG_DEBUG, "UpMsg TO[%u]|%s", req.touin(), sPacket.c_str());
	LOG(LOG_DEBUG, "GrpUpMsg FROM[%u] TO[%u] MSGID[%u]", UIN, req.grpid(), msgid);

	msg.SerializeToString(&sPacket);

	if ( sPacket.size() > MAX_MSG_LEN ) 	//2048
	{
		//考虑放弃该消息
		LOG(LOG_ERROR, "FROM[%u] TO[%u] MSG LEN[%u]", UIN, req.grpid(), sPacket.size());
		BUILDPACKET( 2 , header , "msg too long" , buffer , length );
		return HTTP_LONGCONN_SUCCESS;
	}

	static char b64Packet[MAX_MSG_LEN * 2];
	int iLen = MAX_MSG_LEN * 2;
	base64_encode(sPacket.c_str(), sPacket.size(), b64Packet, &iLen);

	if ( (iRet = setMsg( req.grpid(), msgid, b64Packet , 1)) != 0 )	//groupid的msgid
	{
		BUILDPACKET( 1 , header , "msgdb error" , buffer , length );
		return HTTP_MSGDB_ERROR;
	}

	if (req.msg().messages_size() > 0)
	{
		MSGLOG("0|%u|%u|%u|%u|%s", UIN ,req.grpid(), msgid, req.msg().messages(0).type(),
		       req.msg().messages(0).msg().c_str());
	}

	RspHeader rspHeader;
	string sPushMsg("");

	rspHeader.set_cmd(CMD_PUSHGRPMSG);
	rspHeader.set_ispush(1);
	rspHeader.set_ret(0);
	rspHeader.set_message("");
	rspHeader.set_seq(header.seq());

	CommonPkt ack;
	ack.packet = sPacket;
	rspHeader.SerializeToString(&ack.header);
	ack >> sPushMsg;

	//send group message to group member
	set<uint32_t> memberIDs;
	GetGroupMemberIDs(GRPID,UIN, memberIDs);
	for (set<uint32_t>::iterator it=memberIDs.begin();it!=memberIDs.end();it++)
	{
		if (*it == UIN)
			continue;
		//查用户是否在线
		bool bOnline = getStatus(*it, tNow);

		if ( bOnline )
		{
			iRet = g_pstUserInfo->SendUserMessage(*it, sPushMsg);
			if (iRet != 0)
			{
				LOG(LOG_ERROR, "UIN[%u] SEND GRPMSG ERROR[%d]", req.grpid(), iRet);
			}
		}
		else { //add by cdjlove @20130520 start
			//1  collect all off-line id
			//2 call push msg function  
		}//add by cdjlove @20130520 end
#if 0
		//offline push
		//add by caijunbo
		else
		{
			enum MsgType msgType=req.mutable_msg()->mutable_messages(0)->type();
			string strContent;
			int iMsgType=4;
			//??±???】àDí
			if (msgType==Msg_Text)
			{
				strContent=req.mutable_msg()->mutable_messages(0)->msg();
			}
			//·???±???】àDí
			else
			{
				switch (msgType)
				{
				case Msg_Pic:
					strContent="给你发送了图片";
					iMsgType=2;
					break;
				case Msg_Audio:
					strContent="给你发送了音频";
					iMsgType=3;
					break;
				case Msg_File:
					strContent="给你发送了文件";
					break;
				case Msg_Offline:
					break;
				default:
					break;
				}

			}

			ostringstream ostr1;
			ostr1<< UIN;
			std::string fromID(ostr1.str());

			ostringstream ostr2;
			ostr2<< *it;
			std::string strToID(ostr2.str());

			Json::Value toIDArray;
			toIDArray.append(strToID);

			string strBaseIP(ini.apiip);
			string OFFLINEPUSH_URL="http://"+strBaseIP+"/Api/offlinePush";

			Http_Handle httpHandle;
			httpHandle.setURL(OFFLINEPUSH_URL);
			httpHandle.addPostParam("a",fromID);//user name
			httpHandle.addPostParam("b",toIDArray);//passwd
			httpHandle.addPostParam("c",strContent);//Device Token
			httpHandle.addPostParam("d",1);// 1??え?
			string strGrpID="0";
			httpHandle.addPostParam( "e", strGrpID);//2?Dèòaèoid
			httpHandle.addPostParam( "f", 1);// 1??ê?á?ììí??í
			httpHandle.addPostParam( "g", iMsgType);// 1.??é2.í???3.ó?ò?4.??×
			string strzToken="23b71dae-a270-4a36-9492-9d28c0217e2f";
			httpHandle.addPostParam("z",strzToken);//z token áùê??ó?
			httpHandle.performRequest();

			Json::Value result;
			result=httpHandle.getJsonResult();

			LOG( LOG_TRACE, "****server offline rsp:[%s]",result.toStyledString().c_str());
			if (result["a"].asInt()!=0) //???????
			{
				LOG( LOG_TRACE, "****error send offline message");
			}

			LOG( LOG_TRACE, "****send offline message successfully");		
		}
		//end add by caijunbo
#endif
	}
	

	//回包
	RspUpGrpMsg rsp;
	rsp.set_msgid(msgid);
	rsp.set_grpid(req.grpid());
	rsp.set_time((uint32_t)tNow);
	rsp.SerializeToString(&sPacket);
	BUILDACK( 0 , header , "" , sPacket, buffer, length );
	return HTTP_LONGCONN_SUCCESS;
}
#else

int ProcessUpGrpMsg( char *buffer , int &length , int64_t id , ReqHeader& header, string& packet )
{
	int iRet = 0;
	uint32_t UIN = header.uin();
	VERIFYPACKET( CONNECTION_FD( id ) , UIN );
	//Session续期
	VERIFYSESSION( UIN );

	ReqUpGrpMsg req;
	bool bRet = req.ParseFromString(packet);

	if (!bRet)
	{
		LOG(LOG_ERROR, "Parse UpGrpMsg UIN[%u]", UIN);

		BUILDPACKET( 1 , header , "parse packet error" , buffer, length );
		return HTTP_SUCCESS;
	}

	uint32_t GRPID=req.grpid();

	PushGrpMsg msg;
	msg.set_fromuin(UIN);
	msg.set_grpid(GRPID);
	msg.mutable_msg()->CopyFrom(req.msg());

	//判断是否重试请求
	//...

	//生成MsgID
	uint32_t msgid = 0;
	if ( (iRet = setMsgID(req.grpid(), msgid, 1)) != 0 )	//groupid的msgid
	{
		LOG(LOG_ERROR, "UpGrpMsg Set MSGID[%u] UIN[%u] GRPID[%u]", msgid, UIN, req.grpid()); 
		BUILDPACKET( 1 , header , "msgid error" , buffer , length );
		return HTTP_LONGCONN_SUCCESS;
	}
	if ( (iRet = setGrpReadID(UIN, req.grpid(), msgid)) != 0)
	{
		LOG(LOG_ERROR, "UpGrpMsg Set GrpReadID[%d] UIN[%u] GRPID[%u]", msgid, UIN, req.grpid());
		//continue even readid set error...
	}
	
	time_t tNow = time(NULL);
	msg.mutable_msg()->set_msgid(msgid);
	msg.mutable_msg()->set_time((uint32_t)tNow);

	//存储消息
	string sPacket;
//	TextFormat::PrintToString(msg, &sPacket);
//	LOG(LOG_DEBUG, "UpMsg TO[%u]|%s", req.touin(), sPacket.c_str());
	LOG(LOG_DEBUG, "GrpUpMsg FROM[%u] TO[%u] MSGID[%u]", UIN, req.grpid(), msgid);

	msg.SerializeToString(&sPacket);

	if ( sPacket.size() > MAX_MSG_LEN ) 	//2048
	{
		//考虑放弃该消息
		LOG(LOG_ERROR, "FROM[%u] TO[%u] MSG LEN[%u]", UIN, req.grpid(), sPacket.size());
		BUILDPACKET( 2 , header , "msg too long" , buffer , length );
		return HTTP_LONGCONN_SUCCESS;
	}

	static char b64Packet[MAX_MSG_LEN * 2];
	int iLen = MAX_MSG_LEN * 2;
	base64_encode(sPacket.c_str(), sPacket.size(), b64Packet, &iLen);

	if ( (iRet = setMsg( req.grpid(), msgid, b64Packet , 1)) != 0 )	//groupid的msgid
	{
		BUILDPACKET( 1 , header , "msgdb error" , buffer , length );
		return HTTP_MSGDB_ERROR;
	}

	if (req.msg().messages_size() > 0)
	{
		MSGLOG("0|%u|%u|%u|%u|%s", UIN ,req.grpid(), msgid, req.msg().messages(0).type(),
		       req.msg().messages(0).msg().c_str());
	}

	RspHeader rspHeader;
	string sPushMsg("");

	rspHeader.set_cmd(CMD_PUSHGRPMSG);
	rspHeader.set_ispush(1);
	rspHeader.set_ret(0);
	rspHeader.set_message("");
	rspHeader.set_seq(header.seq());

	CommonPkt ack;
	ack.packet = sPacket;
	rspHeader.SerializeToString(&ack.header);
	ack >> sPushMsg;

	//send group message to group member
	Json::Value toIDArray;
	int num = 0;
	char uid_tmp[UID_LEN_MAX] = {0};
	//set<uint32_t> memberIDs;
	uint32_t memberIDs[GRP_MEMBERS_MAX+1] = {0};
	int count = 0;
	//GetGroupMemberIDs(GRPID,UIN, memberIDs);
	count = dsnGetGrpMembersFromIM(GRPID, UIN, memberIDs);
	
	for (int i =0; i < count; ++i)
	{
		if (memberIDs[i] == UIN)
			continue;
		//查用户是否在线
		bool bOnline = getStatus(memberIDs[i], tNow);

		if ( bOnline )
		{
			iRet = g_pstUserInfo->SendUserMessage(memberIDs[i], sPushMsg);
			if (iRet != 0)
			{
				LOG(LOG_ERROR, "UIN[%u] SEND GRPMSG ERROR[%d]", req.grpid(), iRet);
			}
		}
		else 
		{ 
			sprintf(uid_tmp, "%u", memberIDs[i]);
			toIDArray.append(uid_tmp);
			num++;
		}
	}

	enum MsgType msgType=req.mutable_msg()->mutable_messages(0)->type();
	string strContent;
	int iMsgType = MSG_TYPE_TEXT;	
	if (msgType == Msg_Text)
	{
		strContent = req.mutable_msg()->mutable_messages(0)->msg();
	}
	else
	{
		switch (msgType)
		{
		case Msg_Pic:
			strContent="给你发送了图片";
			iMsgType = MSG_TYPE_PIC;
			break;
		case Msg_Audio:
			strContent="给你发送了音频";
			iMsgType = MSG_TYPE_VOICE;
			break;
		case Msg_File:
			strContent="给你发送了文件";
			break;
		case Msg_Offline:
			break;
		default:
			break;
		}
	}
	if (num > 0)
		dsnPushMsg2NetSrv(UIN, req.grpid(), toIDArray, strContent, iMsgType);

	//回包
	RspUpGrpMsg rsp;
	rsp.set_msgid(msgid);
	rsp.set_grpid(req.grpid());
	rsp.set_time((uint32_t)tNow);
	rsp.SerializeToString(&sPacket);
	BUILDACK( 0 , header , "" , sPacket, buffer, length );
	return HTTP_LONGCONN_SUCCESS;
}

#endif

int ProcessPushMsgAck( char *buffer , int &length , int64_t id , ReqHeader& header, string& packet )
{
//客户端需尽量保证不会上报比当前已读更小的msgid
	int iRet = 0;
	uint32_t UIN = header.uin();
	VERIFYPACKET( CONNECTION_FD( id ) , UIN );
	//Session续期
	VERIFYSESSION( UIN );

	ReqPushMsgAck req;
	bool bRet = req.ParseFromString(packet);

	if (!bRet)
	{
		LOG(LOG_ERROR, "Parse MsgAck UIN[%u]", UIN);

		BUILDPACKET( 1 , header , "parse packet error" , buffer, length );
		return HTTP_SUCCESS;
	}

	if ( (iRet = setReadID(UIN, req.msgid())) != 0)
	{
		BUILDPACKET( 1 , header , "msgdb error" , buffer , length );
		return HTTP_LONGCONN_SUCCESS;
	}

	//回包为空包包体
	BUILDPACKET( 0 , header , "" , buffer, length );
	return HTTP_LONGCONN_SUCCESS;
}

int ProcessPushGrpMsgAck( char *buffer , int &length , int64_t id , ReqHeader& header, string& packet )
{
//客户端需尽量保证不会上报比当前已读更小的msgid
	int iRet = 0;
	uint32_t UIN = header.uin();
	VERIFYPACKET( CONNECTION_FD( id ) , UIN );
	//Session续期
	VERIFYSESSION( UIN );

	ReqPushGrpMsgAck req;
	bool bRet = req.ParseFromString(packet);

	if (!bRet)
	{
		LOG(LOG_ERROR, "Parse GrpMsgAck UIN[%u]", UIN);

		BUILDPACKET( 1 , header , "parse packet error" , buffer, length );
		return HTTP_SUCCESS;
	}

	if ( (iRet = setGrpReadID(UIN, req.grpid(), req.msgid())) != 0)
	{
		BUILDPACKET( 1 , header , "msgdb error" , buffer , length );
		return HTTP_LONGCONN_SUCCESS;
	}

	//回包为空包包体
	BUILDPACKET( 0 , header , "" , buffer, length );
	return HTTP_LONGCONN_SUCCESS;
}

int ProcessOfflineMsg( char *buffer , int &length , int64_t id , ReqHeader& header, string& packet )
{
	uint32_t UIN = header.uin();
	VERIFYPACKET( CONNECTION_FD( id ) , UIN );
	//Session续期
	VERIFYSESSION( UIN );

	ReqOfflineMsg req;
	bool bRet = req.ParseFromString(packet);

	if (!bRet || req.msgid() == 0 )
	{
		LOG(LOG_ERROR, "Parse OfflineMsg UIN[%u] MsgID[%u]", UIN, req.msgid());

		BUILDPACKET( 1 , header , "parse packet error" , buffer, length );
		return HTTP_SUCCESS;
	}

	RspOfflineMsg rsp;
	uint32_t count = (req.count()>MAX_MSG_NUM) ? MAX_MSG_NUM:req.count();
	uint32_t readid = 0, msgid =0;
	int iRet;
	if ((iRet = getC2CID(UIN, readid, msgid)) == 0)
	{
		rsp.set_readid(readid);
		getMsg( UIN, req.msgid(), count, rsp);
	}

	string sPacket;
	rsp.SerializeToString(&sPacket);
	BUILDACK( 0 , header , "" , sPacket, buffer, length );
	return HTTP_LONGCONN_SUCCESS;
}

int ProcessCheckGrpMsg(  char *buffer , int &length , int64_t id , ReqHeader& header, string& packet )
{
	uint32_t UIN = header.uin();
	VERIFYPACKET( CONNECTION_FD( id ) , UIN );
	//Session续期
	VERIFYSESSION( UIN );
	ReqCheckGrpMsg req;
	bool bRet = req.ParseFromString(packet);

	if (!bRet || req.grpid().size() > MAX_CHKGRP_NUM )
	{
		LOG(LOG_ERROR, "Parse ChecGrpMsg UIN[%u] SIZE[%u]", UIN, req.grpid().size());

		BUILDPACKET( 1 , header , "parse packet error" , buffer, length );
		return HTTP_LONGCONN_SUCCESS;
	}

	int iRet = 0;
	map<uint32_t, uint32_t> mapReadId;
	map<uint32_t, uint32_t> mapMsgId;

	RspCheckGrpMsg rsp;
	if ((iRet = getGrpReadID(UIN, req.grpid(), mapReadId)) == 0 &&
	        (iRet = getGrpMsgID(UIN, req.grpid(), mapMsgId)) == 0)
	{
		for ( ::google::protobuf::RepeatedField< ::google::protobuf::uint32 >::const_iterator it = req.grpid().begin();
		        it != req.grpid().end(); it++)
		{
			map<uint32_t,uint32_t>::iterator itread, itmsg;
			if ( (itread = mapReadId.find(*it)) != mapReadId.end() &&
			        (itmsg = mapMsgId.find(*it)) != mapMsgId.end() &&
			        itread->second != itmsg->second)	//一样的不下发
			{
				GrpInfo* info = rsp.add_grpinfo();
				info->set_grpid(*it);
				info->set_readid(itread->second);
				info->set_msgid(itmsg->second);
				LOG(LOG_DEBUG, "ChkGrpMsg|%u|%u|%u|%u", UIN, *it, itread->second, itmsg->second);
			}
		}
	}

	string sPacket;
	rsp.SerializeToString(&sPacket);
	BUILDACK( 0 , header , "" , sPacket, buffer, length );
	return HTTP_LONGCONN_SUCCESS;
}

int ProcessGetGrpMsg(  char *buffer , int &length , int64_t id , ReqHeader& header, string& packet )
{
	uint32_t UIN = header.uin();
	VERIFYPACKET( CONNECTION_FD( id ) , UIN );
	//Session续期
	VERIFYSESSION( UIN );
	ReqGetGrpMsg req;
	bool bRet = req.ParseFromString(packet);

	if (!bRet || req.msgid() == 0 )
	{
		LOG(LOG_ERROR, "Parse GetGrpMsg UIN[%u] MsgID[%u]", UIN, req.msgid());

		BUILDPACKET( 1 , header , "parse packet error" , buffer, length );
		return HTTP_SUCCESS;
	}

	RspGetGrpMsg rsp;
	uint32_t count = (req.count()>MAX_MSG_NUM) ? MAX_MSG_NUM:req.count();
	getGrpMsg( UIN, req.grpid(), req.msgid(), count, rsp);

	string sPacket;
	rsp.SerializeToString(&sPacket);
	BUILDACK( 0 , header , "" , sPacket, buffer, length );
	return HTTP_LONGCONN_SUCCESS;
}

int ProcessNotice( char *buffer , int &length , int64_t id , ReqHeader& header, string& packet )
{
	uint32_t UIN = header.uin();
	VERIFYPACKET( CONNECTION_FD( id ) , UIN );
	//Session续期
	VERIFYSESSION( UIN );

	ReqNotice req;
	bool bRet = req.ParseFromString(packet);

	if (!bRet)
	{
		LOG(LOG_ERROR, "Parse Notice UIN[%u]", UIN);

		BUILDPACKET( 1 , header , "parse packet error" , buffer, length );
		return HTTP_SUCCESS;
	}


	RspNotice rsp;
	int count = (req.count()>MAX_MSG_NUM) ? MAX_MSG_NUM:req.count();
	uint32_t readid = 0, msgid =0;
	int iRet;
	if ((iRet = getNoticeID(UIN, readid, msgid)) == 0)
	{
		rsp.set_readid(readid);
		if ( req.noticeid() == 0)
		{
			if ( readid + count > msgid)	//过界
			{
				count = msgid - readid;
				rsp.set_noticeid(0);
			}
			else
				rsp.set_noticeid(readid + count + 1);
			getNotice( UIN, readid +1, count, rsp);	//注意是从前往后给
		}
		else
		{
			if ( req.noticeid() + count > msgid)	//过界
			{
				count = msgid - req.noticeid() + 1;
				rsp.set_noticeid(0);
			}
			else
				rsp.set_noticeid(req.noticeid()+count);
			getNotice( UIN, req.noticeid(), count, rsp);	//注意是从前往后给
		}
	}

	//回包为空包包体
	string sPacket;
	rsp.SerializeToString(&sPacket);
	BUILDACK( 0 , header , "" , sPacket, buffer, length );
	return HTTP_LONGCONN_SUCCESS;
}


int ProcessNoticeAck( char *buffer , int &length , int64_t id , ReqHeader& header, string& packet )
{
	//客户端需尽量保证不会上报比当前已读更小的noticeid
	int iRet = 0;
	uint32_t UIN = header.uin();
	VERIFYPACKET( CONNECTION_FD( id ) , UIN );
	//Session续期
	VERIFYSESSION( UIN );

	ReqNoticeAck req;
	bool bRet = req.ParseFromString(packet);

	if (!bRet)
	{
		LOG(LOG_ERROR, "Parse NoticeAck UIN[%u]", UIN);

		BUILDPACKET( 1 , header , "parse packet error" , buffer, length );
		return HTTP_SUCCESS;
	}

	if ( (iRet = setReadID(UIN, req.noticeid(), 2)) != 0)
	{
		BUILDPACKET( 1 , header , "msgdb error" , buffer , length );
		return HTTP_LONGCONN_SUCCESS;
	}

	//回包为空包包体
	BUILDPACKET( 0 , header , "" , buffer, length );
	return HTTP_LONGCONN_SUCCESS;
}

int Process_Request( char *buffer, int &length, int64_t id )
{
	int HeaderLen = ntohs(*((unsigned short*)&buffer[1]));
	int PacketLen = ntohs(*((unsigned short*)&buffer[3]));

	//Get Header
	ReqHeader header;
	string s("");
	int iRet = (int) header.ParseFromArray(buffer+5, HeaderLen);
	if (!iRet)
	{
		BUILDPACKET( 1 , header , "Invalid Request" , buffer , length );
	}
	else
	{
		string packet(buffer+5+HeaderLen, PacketLen);	//packet的位置

		switch (header.cmd())
		{
		case CMD_LOGIN:
			iRet = ProcessLogin( buffer , length , id , header, packet );
			break;
		case CMD_RELOGIN:
			iRet = ProcessReLogin( buffer, length , id , header , packet );		//重连验证token
			break;
		case CMD_HEARTBEAT:
			iRet = ProcessHeartBeat( buffer , length , id , header , packet );	//续期
			break;
		case CMD_UPMSG:
			iRet = ProcessUpMsg( buffer , length , id , header , packet );	//C2C
			break;
		case CMD_PUSHMSGACK:
			iRet = ProcessPushMsgAck( buffer , length , id , header , packet );	//C2C
			break;
		case CMD_UPGRPMSG:
			iRet=ProcessUpGrpMsg( buffer , length , id , header , packet ); //群聊逻辑
			break;
		case CMD_PUSHGRPMSGACK:
			//处理群聊消息已经被接收逻辑
			iRet = ProcessPushGrpMsgAck( buffer , length , id , header , packet );	//Grp
			break;
		case CMD_OFFLINEMSG:
			iRet = ProcessOfflineMsg( buffer , length , id , header , packet );	//C2C
			break;
		case CMD_CHECKGRPMSG:
			iRet = ProcessCheckGrpMsg( buffer , length , id , header , packet );//Grp
			break;
		case CMD_GETGRPMSG:
			iRet= ProcessGetGrpMsg( buffer , length , id , header , packet );//Grp
			break;
		case CMD_NOTICE:
			iRet = ProcessNotice( buffer , length , id , header , packet );
			break;
		case CMD_NOTICEACK:
			iRet = ProcessNoticeAck( buffer , length , id , header , packet );
			break;
		default:
			BUILDPACKET( 2 , header, "Request unsupported" , buffer , length );
		}
	}

	LOG( LOG_DEBUG , "REQ UIN[%u] CMD[%d] SEQ[%d] LEN[%d] RET[%d]" , header.uin(), header.cmd(), header.seq() , length, iRet );

	return iRet;
}


