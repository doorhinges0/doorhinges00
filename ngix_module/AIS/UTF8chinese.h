char	tagchinautf8[]="tag:address:country:中国";
char    *unknown = "未知";

enum ecode{
SUCCESS ,/*"OK"*/
SYSTEM_ERROR,
UNKNOWN_METHOD ,
PARAMETRER_ILLEGAL ,
KEY_EMPTY_PARAMETRER_ILLEGAL , /*"request parameter (SdkKey) is missing"},*/
APPNAME_EMPTY_PARAMETRER_ILLEGAL ,/*"request parameter (AppName) is missing"},*/
UUID_EMPTY_PARAMETRER_ILLEGAL ,/*"request parameter (Uuid) is missing"},*/
CLIENT_EMPTY_PARAMETRER_ILLEGAL ,/*"request parameter (Client) is missing"},*/
NET_EMPTY_PARAMETRER_ILLEGAL ,/*"request parameter (Net) is missing"},*/
OPERATOR_EMPTY_PARAMETRER_ILLEGAL ,/*"request parameter (Operator) is missing"},*/
IP_EMPTY_PARAMETRER_ILLEGAL ,/*"request parameter (Ip) is missing"},*/
IP_WRONGTYPE_PARAMETRER_ILLEGAL ,/*"request parameter (SdkKey) is missing"},*/
CATEGORY_EMPTY_PARAMETRER_ILLEGAL ,/*"request parameter (Category) is missing"},*/
OSVERSION_EMPTY_PARAMETRER_ILLEGAL ,/*"request parameter (Os_version) is missing"},*/
DENSITY_EMPTY_PARAMETRER_ILLEGAL ,/*"request parameter (Density) is missing"},*/
AW_EMPTY_PARAMETRER_ILLEGAL ,/*"request parameter (Aw) is missing"},*/
AH_EMPTY_PARAMETRER_ILLEGAL ,/*"request parameter (Ah) is missing"},*/
TIMESTAMP_EMPTY_PARAMETRER_ILLEGAL , /*"request parameter (TimeStamp) is missing"},*/
TIMESTAMP_FORMAT_PARAMETRER_ILLEGAL ,/*"request parameter (TimeStamp) format not correct"},*/
SIGN_EMPTY_PARAMETRER_ILLEGAL ,/*"request parameter (Sign) is missing"},*/
SIGN_NOTMATCH_PARAMETRER_ILLEGAL ,/*"SIGN do not match"},*/
ADID_EMPTY_PARAMETRER_ILLEGAL ,/*"request parameter (AdId) is missing"},*/
ADID_WRONGTYPE_PARAMETRER_ILLEGAL ,/*"wrong type of parameter (AdId)"},*/
LASTUPDATE_EMPTY_PARAMETRER_ILLEGAL ,/*"request parameter (LastUpdate) is missing"},*/
SESSIONID_EMPTY_PARAMETRER_ILLEGAL ,/*"request parameter (Sessionid) is missing"},*/
CALLBACKTYPE_EMPTY_PARAMETRER_ILLEGAL ,/*"request parameter (CallbackType) is missing"},*/
AW_NAN_PARAMETRER_ILLEGAL, 
AH_NAN_PARAMETRER_ILLEGAL, 
IP_NAN_PARAMETRER_ILLEGAL, 
APPNAME_NAN_PARAMETRER_ILLEGAL,
APPID_NAN_PARAMETRER_ILLEGAL,
UUID_NAN_PARAMETRER_ILLEGAL,
CLIENT_NAN_PARAMETRER_ILLEGAL,
OPERATOR_NAN_PARAMETRER_ILLEGAL,
CATEGORY_NAN_PARAMETRER_ILLEGAL,
NET_NAN_PARAMETRER_ILLEGAL,
DEVICETYPE_NAN_PARAMETRER_ILLEGAL,
ADSPACETYPE_NAN_PARAMETRER_ILLEGAL,
OS_VERSION_NAN_PARAMETRER_ILLEGAL,
UA_NAN_PARAMETRER_ILLEGAL, 
FORMAT_NAN_PARAMETRER_ILLEGAL,
VER_NAN_PARAMETRER_ILLEGAL,
DENSITY_NAN_PARAMETRER_ILLEGAL,
LONG_NAN_PARAMETRER_ILLEGAL,
LAT_NAN_PARAMETRER_ILLEGAL,
PW_NAN_PARAMETRER_ILLEGAL,
PH_NAN_PARAMETRER_ILLEGAL,
DEVICENUM_NAN_PARAMETRER_ILLEGAL,
URL_NAN_PARAMETRER_ILLEGAL,
SDKVERSION_NAN_PARAMETRER_ILLEGAL,
ADNUM_NAN_PARAMETRER_ILLEGAL,
TIMESTAMP_EXPIRED ,/*"(TimeStamp) Has expired"},*/
/*SIGN_NOTMATCH ,*/
PLATFORM_EMPTY ,/*"There is no (ADSDK) information"},*/
PLATFORM_ERROR ,/*"ADSDK anomaly"},*/

AD_NOFIND ,/*"Sorry,*/
KEY_NOFIND_AD,
BODY_NOFIND ,/*"Sorry,*/
SESSIONID_NOFIND ,/*"Sorry,*/
IP_BRUSH ,/*"Sorry,Too frequent request,*/
ADSPACETYPE_EMPTY ,
HTTPMETHOD ,
CALLBACKTYPE_ERROR ,
ADBODY_ERROR ,
DEVICETYPE_EMPTY ,
SDKVERSION_EMPTY ,
APPID_EMPTY ,
NOINFO ,
CHEAT_CALLBACK ,
STARTDATE_EMPTY ,
STARTDATE_FORMAT ,
ENDDATE_EMPTY ,
ENDDATE_FORMAT ,
PARAMETRER_NOTFULL,
ADPLATFORMPRICE_NO,
SDKVERSION_DISABLED,
PARAMETER_CONTAINING_ILLEGAL_CHARACTERS,
IP_CHANGE,
CONNECT_ERROR,
CONTEXT_LONG,
SQL_ERROR,
DATE_SUB_ERR,
DATE_GT_THREE,
PID_EMPTY_PARAMETRER_ILLEGAL,
PID_INVALID_PARAMETRER_ILLEGAL,
URL302_EMPTY_PARAMETRER_ILLEGAL,
ECODE_BULT};

struct MsgError{
enum ecode e;
char * code;
char * msg;
}msgerror[]={
{SUCCESS , "100" , "成功"},/*"OK"*/
{SYSTEM_ERROR,"101","系统错误，请联系管理员"},
{UNKNOWN_METHOD ,"102","抱歉，资源不存在"},
{PARAMETRER_ILLEGAL , "104","请求参数不规范"},
{KEY_EMPTY_PARAMETRER_ILLEGAL , "104" , "缺少参数（key）"}, /*"request parameter (SdkKey) is missing"},*/
{APPNAME_EMPTY_PARAMETRER_ILLEGAL , "104" , "缺少参数（appname）"},/*"request parameter (AppName) is missing"},*/
{UUID_EMPTY_PARAMETRER_ILLEGAL , "104" , "缺少参数（uuid）"},/*"request parameter (Uuid) is missing"},*/
{CLIENT_EMPTY_PARAMETRER_ILLEGAL , "104" , "缺少参数（client）"},/*"request parameter (Client) is missing"},*/
{NET_EMPTY_PARAMETRER_ILLEGAL , "104" , "缺少参数（net）"},/*"request parameter (Net) is missing"},*/
{OPERATOR_EMPTY_PARAMETRER_ILLEGAL , "104" , "缺少参数（operator）"},/*"request parameter (Operator) is missing"},*/
{IP_EMPTY_PARAMETRER_ILLEGAL , "104" , "缺少参数或者ip格式错误（ip）"},/*"request parameter (Ip) is missing"},*/
{IP_WRONGTYPE_PARAMETRER_ILLEGAL , "104" , "缺少参数（sdkkey）"},/*"request parameter (SdkKey) is missing"},*/
{CATEGORY_EMPTY_PARAMETRER_ILLEGAL , "104" , "缺少参数（category）"},/*"request parameter (Category) is missing"},*/
{OSVERSION_EMPTY_PARAMETRER_ILLEGAL , "104" , "缺少参数（os_version）"},/*"request parameter (Os_version) is missing"},*/
{DENSITY_EMPTY_PARAMETRER_ILLEGAL , "104" , "缺少参数（density）"},/*"request parameter (Density) is missing"},*/
{AW_EMPTY_PARAMETRER_ILLEGAL , "104" , "缺少参数（aw）"},/*"request parameter (Aw) is missing"},*/
{AH_EMPTY_PARAMETRER_ILLEGAL , "104" , "缺少参数（ah）"},/*"request parameter (Ah) is missing"},*/
{TIMESTAMP_EMPTY_PARAMETRER_ILLEGAL , "104" , "缺少参数（timestamp）"}, /*"request parameter (TimeStamp) is missing"},*/
{TIMESTAMP_FORMAT_PARAMETRER_ILLEGAL , "104" , "参数（timestamp）格式不正确"},/*"request parameter (TimeStamp) format not correct"},*/
{SIGN_EMPTY_PARAMETRER_ILLEGAL , "104" , "缺少参数（sign）"},/*"request parameter (Sign) is missing"},*/
{SIGN_NOTMATCH_PARAMETRER_ILLEGAL , "106" , "sign错误"},/*"SIGN do not match"},*/
{ADID_EMPTY_PARAMETRER_ILLEGAL , "104" , "缺少参数（adid）"},/*"request parameter (AdId) is missing"},*/
{ADID_WRONGTYPE_PARAMETRER_ILLEGAL , "104" , "参数（adid）不规范"},/*"wrong type of parameter (AdId)"},*/
{LASTUPDATE_EMPTY_PARAMETRER_ILLEGAL , "104" , "缺少参数（lastupdate）"},/*"request parameter (LastUpdate) is missing"},*/
{SESSIONID_EMPTY_PARAMETRER_ILLEGAL , "104" , "缺少参数（sessionid）"},/*"request parameter (Sessionid) is missing"},*/
{CALLBACKTYPE_EMPTY_PARAMETRER_ILLEGAL , "104" , "缺少参数（callbacktype）"},/*"request parameter (CallbackType) is missing"},*/
{AW_NAN_PARAMETRER_ILLEGAL , "104" , "参数（aw）不规范"},
{AH_NAN_PARAMETRER_ILLEGAL , "104" , "参数（ah）不规范"},
{IP_NAN_PARAMETRER_ILLEGAL , "104" , "参数（ip）不规范"},
{APPNAME_NAN_PARAMETRER_ILLEGAL , "104" , "参数（appname）不规范"},
{APPID_NAN_PARAMETRER_ILLEGAL , "104" , "参数（appid）不规范"},
{UUID_NAN_PARAMETRER_ILLEGAL , "104" , "参数（uuid）不规范"},
{CLIENT_NAN_PARAMETRER_ILLEGAL , "104" , "参数（client）不规范"},
{OPERATOR_NAN_PARAMETRER_ILLEGAL , "104" , "参数（operator）不规范"},
{CATEGORY_NAN_PARAMETRER_ILLEGAL , "104" , "参数（category）不规范"},
{NET_NAN_PARAMETRER_ILLEGAL , "104" , "参数（net）不规范"},
{DEVICETYPE_NAN_PARAMETRER_ILLEGAL , "104" , "参数（devicetype）不规范"},
{ADSPACETYPE_NAN_PARAMETRER_ILLEGAL , "104" , "参数（adspacetype）不规范"},
{OS_VERSION_NAN_PARAMETRER_ILLEGAL , "104" , "参数（os_version）不规范"},
{UA_NAN_PARAMETRER_ILLEGAL , "104" , "参数（ua）不规范"},
{FORMAT_NAN_PARAMETRER_ILLEGAL , "104" , "参数（format）不规范"},
{VER_NAN_PARAMETRER_ILLEGAL,      "104", "param (ver) illegal"},
{DENSITY_NAN_PARAMETRER_ILLEGAL , "104" , "参数（density）不规范"},
{LONG_NAN_PARAMETRER_ILLEGAL , "104" , "参数（long）不规范"},
{LAT_NAN_PARAMETRER_ILLEGAL , "104" , "参数（lat）不规范"},
{PW_NAN_PARAMETRER_ILLEGAL , "104" , "参数（pw）不规范"},
{PH_NAN_PARAMETRER_ILLEGAL , "104" , "参数（ph）不规范"},
{DEVICENUM_NAN_PARAMETRER_ILLEGAL , "104" , "参数（devicenum）不规范"},
{URL_NAN_PARAMETRER_ILLEGAL,        "104",   "参数（url）不规范"},
{SDKVERSION_NAN_PARAMETRER_ILLEGAL , "104" , "参数（sdkverion）不规范"},
{ADNUM_NAN_PARAMETRER_ILLEGAL , "104" , "参数（adnum）不规范"},

{TIMESTAMP_EXPIRED , "105" , "timestamp已过期"},/*"(TimeStamp) Has expired"},*/
/*{SIGN_NOTMATCH , "106" , "sign错误"},*/
{PLATFORM_EMPTY , "107" , "平台商信息不存在"},/*"There is no (ADSDK) information"},*/
{PLATFORM_ERROR , "108" , "平台商信息异常"},/*"ADSDK anomaly"},*/

{AD_NOFIND , "200" , "找不到符合的广告信息"},/*"Sorry, Can not find data(Ad)"},*/
{KEY_NOFIND_AD, "108",   "平台商没有该广告"},
{BODY_NOFIND , "200" , "找不到符合的广告信息"},/*"Sorry, Can not find data(body)"},*/
{SESSIONID_NOFIND , "300" , "抱歉，找不到相关的sessionId"},/*"Sorry, Can not find data(SessionId)"},*/
{IP_BRUSH , "301" , "抱歉，过于频繁的请求，请稍后再试"},/*"Sorry,Too frequent request, Please try again later"},*/
{ADSPACETYPE_EMPTY , "104" , "缺少参数（adspacetype）"},
{HTTPMETHOD , "111" , "抱歉，目前请求方式只支持GET"},
{CALLBACKTYPE_ERROR , "303" , "广告投放类型错误"},
{ADBODY_ERROR , "201" , "广告体异常"},
{DEVICETYPE_EMPTY , "104" , "缺少参数（devicetype）"},
{SDKVERSION_EMPTY , "104" , "缺少参数（sdkversion）"},
{APPID_EMPTY , "104" , "缺少参数（appid）"},
{NOINFO , "400" , "抱歉，暂无数据"},
{CHEAT_CALLBACK , "302" , "作弊的回发请求"},
{STARTDATE_EMPTY , "104" , "缺少参数（startdate）"},
{STARTDATE_FORMAT , "104" , "参数（startdate）格式不正确"},
{ENDDATE_EMPTY , "104" , "缺少参数（enddate）"},
{ENDDATE_FORMAT , "104" , "参数（enddate）格式不正确"},
{PARAMETRER_NOTFULL , "104" , "缺少参数"},
{ADPLATFORMPRICE_NO,"203","没有成本价格"},
{SDKVERSION_DISABLED,"202","SDK版本被禁用"},
{PARAMETER_CONTAINING_ILLEGAL_CHARACTERS,"502","参数含非法字符"},
{IP_CHANGE,"304","抱歉，网络环境发生变化，请稍后再试"},

{CONNECT_ERROR,"601","连接sqlserver出错"},
{CONTEXT_LONG,"602","输出内容超长"},
{SQL_ERROR,"603","sql执行出错"},
{DATE_SUB_ERR,"104","开始时间大于结束时间"},
{DATE_GT_THREE,"104","只能查三个月的报表数据"},
{PID_EMPTY_PARAMETRER_ILLEGAL , "104" , "缺少参数（pid）"},/*"request parameter (Sessionid) is missing"},*/
{PID_INVALID_PARAMETRER_ILLEGAL , "104" , "参数值非法（pid）"},/*"request parameter (Sessionid) is missing"},*/
{URL302_EMPTY_PARAMETRER_ILLEGAL , "104" , "缺少参数（url302）"},/*"request parameter (Sessionid) is missing"},*/
{ECODE_BULT,"600","未知错误"}
};

