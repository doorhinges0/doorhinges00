#include <arpa/inet.h>
#include <assert.h>
#include <ctype.h>
#include <dirent.h>
#include <inttypes.h>
#include <limits.h>
#include <math.h>
#include <pwd.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>         /* 44BSD stdarg.h uses abort() */
#include <string.h>
#include <sybdb.h>
#include <sys/select.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <uuid/uuid.h>
#include "UTF8chinese.h"
#include "redisc/async.h"
#include "redisc/hiredis.h"
#include "redis_keepalive.h"
#include "json/json.h"
#include "cgi_util.h"
#include "ais.h"


extern char   *mystrdup(const char *str);
extern sql_conn_t *g_sql_conn;
extern struct ais_context  g_ctx;
static int cliAuth(redisContext *context);

#define AD_REDIS_STAT (0)

#define COST_TIME_ERROR_LOG(x)\
    do{\
        ERROR_LOG(x);\
        if (cost_time>5000)\
            ERROR_LOG(("session:%s, much time[%ld]", req_session, cost_time));\
    }while(0);

    
#ifdef __cplusplus
extern "C" {
#endif // __cplusplus
    struct AdPlatformProvider{
        /// <summary>
        /// 广告平台商ID
        /// </summary>
        char * PlatformProviderID;
        /// <summary>
        /// 展示量投放价格
        /// </summary>
        char * ShowPutInPrice ;
        /// <summary>
        /// 点击量投放价格
        /// </summary>
        char * ClickPutInPrice ;
        /// <summary>
        /// 平台商Key
        /// </summary>
        char * Key ;
        /// <summary>
        /// 加密key
        /// </summary>
        char *SecretKey;
        /// <summary>
        /// 回发来源，2为SDK，1平台商
        /// </summary>
        char * CallbackSource;
		char *Category;
    };
    struct ais_adparams {

        char * Ah;
        char * AppName;
        char * Aw;
        char * Category;
        char * Client;

        char * Density;
        char * DeviceNum;
        char * Format;
        char * Ip;
        char * Lat;


        char * Long;
        char * Net;
        char * Operator;
        char * Os_version;
        char * Ph;

        char * Pw;
        char * Key;
        char * TimeStamp;
        char * Uuid;
        char * Sign;

        char * AdSpaceType;
        char * DeviceType;
        char * SdkVersion;
        char * AppId;
        //char * Format;

        char * uri;
        char * nip;
        char *  AdNum;
        unsigned int flags;

    };

    struct ais_adparams_body {
        /*为移动广告平台分配的SdkID*/
        char * SdkKey;
        char *adid;
        char *sign; //signal
        char *format;
        char * timestamp;
        char *lastupdate;

        char *one;
        char * uri;
        char * nip;
        unsigned int flags;
    };

    struct ais_adparams_report {
#if 1
        char * lat ;
        char * longg ;
        char * net ;
        char * oprter ;
        char * os_version ;
        char * ph ;
        char * pw ;
        char * appname ;
        char *  client ;
        char * density ;
        char * device ;
        char * uuid ;

        //add

        char * CId ;
        char * Ip ;
        char * Category ;
        char * DeviceType ;
        char * SdkVersion ;
        char * AppId ;

        char * one;
	char * groupby;
        //char * Key ;

#endif
        char * Format ;
        char * Key ;
        char * Startdate ;
        char * Enddate ;

        char  * TimeStamp;
        char * Sign;

        unsigned int flags;
    };



    struct ais_adparams_callback {
#if 1
        char * lat ;
        char * longg ;
        char * net ;
        char * oprter ;
        char * os_version ;
        char * ph ;
        char * pw ;
        char * appname ;
        char *  client ;
        char * density ;
        char * device ;
        char * uuid ;

        //add

        char * CId ;
        char * Ip ;
        char * Category ;
        char * DeviceType ;
        char * SdkVersion ;
        char * AppId ;

        char * one;
        char * Key ;

#endif
        char * Format ;
        char * Schema ;
        char * sessionid ;
        char * callbacktype ;

        char * id;

        char * uri;
        char * nip;
        char * url;
        unsigned int flags;
    };

    typedef struct ais_logparams
    {
        char *session_id;
        char *aid;
        char *pp_id;
        char *sdk_ver;
        char *key;
        char *putin_type;
        char *request_mode;
        char *adp_price;
        char *ip;
        char *app_id;
        char *app_name;
        char *uuid;
        char *device_num;
        char *clinet;
        char *os_ver;
        char *density;
        char *pw;
        char *ph;
        char *lg;
        char *lat;
        char *opreator;
        char *net;
        char *is_cheat;
        char *is_billing;
        char *ad_space_type;
        char *device_type;
        char *aw;
        char *ah;
        char *format;
		char *category;
        int year;
        int month;
        int mday;
        int hour;
        int min;
        int sec;
        int level;
		int adnum;
        char *aid_list;
		char *callback_source;
		char *url;
		char *timestamp;
    }ais_logparams_t;

typedef struct ais_adparams_offline
{
    char *key;
    char *adid;
    char *adspace;
    char *url;
    char *ver;
    char *sign;
    char *Format;
	char *uuid;
    unsigned int flags;
    char *uri;
    char *nip;
	char *callbacktype;
	char *device;
	char *client;
    char *device_type;
    char *os_ver;
    char *device_num;
}ais_adparams_offline_t;

#define AIDLEN 10
#define DEBUGPID "debug13"
#define APPID_LEN 100
#define APPNAME_LEN 50
#define UUID_LEN 50
#define CLIENT_LEN 1
#define OPERATOR_LEN 1
#define NET_LEN 1
#define DEVICETYPE_LEN 1
#define ADSPACETYPE_LEN 1
#define CATEGORY_LEN 2
#define IP_LEN 20
#define OS_VERSION_LEN 20
#define AW_LEN 5
#define AH_LEN 5
#define UA_LEN 200
#define FORMAT_LEN 4
#define DENSITY_LEN 5
#define LONG_LEN 20
#define LAT_LEN 20
#define PW_LEN 5
#define PH_LEN 5
#define DEVICENUM_LEN 20
#define SDKVERSION_LEN 20
#define ADNUM_LEN 10

//default value list
#define DEF_DEVICE_TYPE   "4"
#define DEF_CLIENT        "4"
#define DEF_DENSITY       "0"
#define DEF_PH            "0"
#define DEF_PW            "0"
#define DEF_LONG          ""
#define DEF_LAT           ""
#define DEF_OPERATOR      "4"
#define DEF_NET           "4"
#define DEF_APPID         "0"
#define DEF_CATEGORY      "12"


#define AIS_ADPARAMS_AH_MASK 1<<0
#define AIS_ADPARAMS_APPNAME_MASK 1<<1
#define AIS_ADPARAMS_AW_MASK 1<<2
#define AIS_ADPARAMS_CATEGORY_MASK 1<<3
#define AIS_ADPARAMS_CLIENT_MASK 1<<4
#define AIS_ADPARAMS_DENSITY_MASK 1<<5
#define AIS_ADPARAMS_DEVICENUM_MASK 1<<6
#define AIS_ADPARAMS_FORMAT_MASK 1<<7
#define AIS_ADPARAMS_IP_MASK 1<<8
#define AIS_ADPARAMS_LAT_MASK 1<<9


#define AIS_ADPARAMS_LONG_MASK 1<<10
#define AIS_ADPARAMS_NET_MASK 1<<11
#define AIS_ADPARAMS_OPERATOR_MASK 1<<12
#define AIS_ADPARAMS_OS_VERSION_MASK 1<<13
#define AIS_ADPARAMS_PH_MASK 1<<14
#define AIS_ADPARAMS_PW_MASK 1<<15
#define AIS_ADPARAMS_KEY_MASK 1<<16
#define AIS_ADPARAMS_TIMESTAMP_MASK 1<<17
#define AIS_ADPARAMS_UUID_MASK 1<<18
#define AIS_ADPARAMS_SIGN_MASK 1<<19
#define AIS_ADPARAMS_ADSPACETYPE_MASK 1<<20
#define AIS_ADPARAMS_DEVICETYPE_MASK 1<<21
#define AIS_ADPARAMS_SDKVERSION_MASK 1<<22
#define AIS_ADPARAMS_APPID_MASK 1<<23
#define AIS_ADPARAMS_ASSURI_MASK 1<<24
#define AIS_ADPARAMS_ASSIP_MASK 1<<25

#define AIS_ADPARAMS_REQUIRE_MASK	(AIS_ADPARAMS_KEY_MASK | AIS_ADPARAMS_UUID_MASK | \
        AIS_ADPARAMS_CLIENT_MASK |  AIS_ADPARAMS_OPERATOR_MASK | \
        AIS_ADPARAMS_IP_MASK | AIS_ADPARAMS_CATEGORY_MASK | AIS_ADPARAMS_ADSPACETYPE_MASK | \
        AIS_ADPARAMS_AW_MASK | AIS_ADPARAMS_AH_MASK | \
        AIS_ADPARAMS_TIMESTAMP_MASK | AIS_ADPARAMS_SIGN_MASK |  AIS_ADPARAMS_APPID_MASK)

#define AIS_OFLPARAMS_KEY_MASK         1 << 0
#define AID_OFLPARAMS_ADID_MASK        1 << 1
#define AID_OFLPARAMS_ADSPACE          1 << 2
#define AIS_OFLPARAMS_URL                1 << 3
#define AIS_OFLPARAMS_VER                1 << 4
#define AIS_OFLPARAMS_SIGN               1 << 5
#define AIS_OFLPARAMS_ASSURI_MASK     1 << 6
#define AIS_OFLPARAMS_ASSIP_MASK      1 << 7

#define AIS_OFLPARAMS_REQUIRE_MASK    (AIS_OFLPARAMS_KEY_MASK | AID_OFLPARAMS_ADID_MASK | \
        AID_OFLPARAMS_ADSPACE | AIS_OFLPARAMS_VER | AIS_OFLPARAMS_SIGN)

#define  AIS_OK          0
#define  AIS_ERROR      -1
#define  AIS_AGAIN      -2
#define  AIS_BUSY       -3
#define  AIS_DONE       -4
#define  AIS_DECLINED   -5
#define  AIS_ABORT      -6

#define ARR_LEN(arr) sizeof((arr)) / sizeof((arr)[0])
#define CB_LEN 10
#define CB_SESSION "session"
#define CB_URL "url"
#define CB_FORMAT "format"
#define CB_PID "pid"
#define CB_TIMESTAMP "timestamp"

const static char *SC_302[] = {"=", "%", "%=%=%=%=%", CB_URL, CB_SESSION, CB_FORMAT, CB_PID, CB_TIMESTAMP};
const static char *SC_CB[] = {"=", "%", "%=%=%=%", CB_SESSION, CB_FORMAT, CB_PID, CB_TIMESTAMP};

    int     alldig(const char *string);
    char *str_trim(char *p);
    int str_split(char *c, char sep, const char **new_strings, int max_new_strings);
    char   *mystrtok(char **src, const char *sep);
    char * my_strlcpy(char *dst, const char *src, size_t len) ;
    void str_replace(const char* what, const char* with, const char* in, char* where);
    double GetDistance(double lat1, double lng1, double lat2, double lng2);
	static const char * ConvertPutintime(char * key,struct tm* date);
	const char * ConvertAdType(unsigned short supportType);
    const char * ConvertSupportType(unsigned short supportType);
    const char * ConvertClient(unsigned int  client);
    const char * ConvertNet(unsigned short net);
    const char * ConvertOperator(unsigned short _operator);
    const char * ConvertWidth(int width);
    const char * NearbyWidth(int width);

#ifdef __cplusplus
}
#endif // __cplusplus


#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

    struct ais_context;     // Handle for the  service itself
    struct ais_connection;  // Handle for the individual connection
    // Get a value of particular form variable.
    //
    // Parameters:
    //   data: pointer to form-uri-encoded buffer. This could be either POST data,
    //         or request_info.query_string.
    //   data_len: length of the encoded data.
    //   var_name: variable name to decode from the buffer
    //   buf: destination buffer for the decoded variable
    //   buf_len: length of the destination buffer
    //
    // Return:
    //   On success, length of the decoded variable.
    //   On error, -1 (variable not found, or destination buffer is too small).
    //
    // Destination buffer is guaranteed to be '\0' - terminated. In case of
    // failure, dst[0] == '\0'.
    int ais_get_var(const char *data, size_t data_len,
            const char *var_name, char *buf, size_t buf_len);

    // MD5 hash given strings.
    // Buffer 'buf' must be 33 bytes long. Varargs is a NULL terminated list of
    // asciiz strings. When function returns, buf will contain human-readable
    // MD5 hash. Example:
    //   char buf[33];
    //   ais_md5(buf, "aa", "bb", NULL);
    void ais_md5(char *buf,const  char ** , char *);
    void ais_md5_(char *buf, ...);

#ifdef __cplusplus
}
#endif // __cplusplus


/* mystrdup - save string to heap */
char   *mystrdup(const char *str)
{
    if (str == 0)
        return NULL;
    return (strcpy((char *)malloc(strlen(str) + 1), str));
}

//region Ip库
/// <summary>
/// Ip库 Set IPArea实体
/// </summary>
const char * iparea = "iparea";
//endregion

//region 地区

/// <summary>
/// 城市
/// </summary>
const char * tag_address_city = "tag:address:city:";
/// <summary>
/// 省会
/// </summary>
const char * tag_address_province = "tag:address:province:";
/// <summary>
/// 国家(全中国)
/// </summary>
char * tag_address_country = "tag:address:country:中国";
//endregion

//region 设备类型
/// <summary>
/// 设备类型--ISO
/// </summary>
const char * tag_client_ios = "tag:client:ios";
/// <summary>
/// 设备类型--安卓
/// </summary>
const char * tag_client_android = "tag:client:android";
/// <summary>
/// 设备类型--WP
/// </summary>
const char * tag_client_wp = "tag:client:wp";

/// <summary>
/// 设备类型--ALL
/// </summary>
const char * tag_client_all = "tag:client:all";
//endregion


//region 广告类型
/// <summary>
/// 广告类型--打开网页
/// </summary>
const char * tag_type_web = "tag:type:web";

/// <summary>
/// 广告类型--下载应用
/// </summary>
const char * tag_type_app = "tag:type:app";

/// <summary>
/// 广告类型--打开全屏图片
/// </summary>
const char * tag_type_full = "tag:type:full";

/// <summary>
/// 广告类型--打电话
/// </summary>
const char * tag_type_call = "tag:type:call";
const char * tag_adtype_img = "tag:adtype:img";

/// <summary>
/// 创意形式(广告类型)--图文
/// </summary>
const char * tag_adtype_text = "tag:adtype:text";

/// <summary>
/// 创意形式(广告类型)--HTML5
/// </summary>
const char * tag_adtype_html5 = "tag:adtype:html5";

/// <summary>
/// 点击后效果--GIF动画
/// </summary>
const char * tag_adtype_gif = "tag:adtype:gif";
//endregion

//region 运营商
/// <summary>
/// 运营商--移动
/// </summary>
const char * tag_operator_10086 = "tag:operator:10086";

/// <summary>
/// 运营商--联通
/// </summary>
const char * tag_operator_10010 = "tag:operator:10010";

/// <summary>
/// 运营商--电信
/// </summary>
const char * tag_operator_10000 = "tag:operator:10000";

/// <summary>
/// 运营商--其他
/// </summary>
const char * tag_operator_other = "tag:operator:other";

/// <summary>
/// 运营商--全部
/// </summary>
const char * tag_operator_all = "tag:operator:all";
//endregion

//region 网络类型
//const char * tag_net_all = "tag:net:all";

/// <summary>
/// 网络类型--2g
/// </summary>
const char * tag_net_2g = "tag:net:2g";

/// <summary>
/// 网络类型--3g
/// </summary>
const char * tag_net_3g = "tag:net:3g";

/// <summary>
/// 网络类型--wifi
/// </summary>
const char * tag_net_wifi = "tag:net:wifi";

/// <summary>
/// 网络类型--其它
/// </summary>
const char * tag_net_other = "tag:net:other";

/// <summary>
/// 网络类型--全部
/// </summary>
const char * tag_net_all = "tag:net:all";

//endregion

//region 年龄段

/// <summary>
/// 年龄段--18-24
/// </summary>
const char * tag_age_18_24 = "tag:age:18_24";

/// <summary>
/// 年龄段--25-34
/// </summary>
const char * tag_age_25_34 = "tag:age:25_34";

/// <summary>
/// 年龄段--35-44
/// </summary>
const char * tag_age_35_44 = "tag:age:35_44";

/// <summary>
/// 年龄段--45-54
/// </summary>
const char * tag_age_45_54 = "tag:age:45_54";

/// <summary>
/// 年龄段--55-64
/// </summary>
const char * tag_age_55_64 = "tag:age:55_64";

/// <summary>
/// 年龄段--65+
/// </summary>
const char * tag_age_65_ = "tag:age:65_";

/// <summary>
/// 年龄段--全部
/// </summary>
const char * tag_age_all = "tag:age:all";
//endregion


//region 广告尺寸

/// <summary>
/// 广告尺寸--160x25 较小图片
/// </summary>
const char * tag_display_160x25 = "tag:display:160x25";

/// <summary>
/// 广告尺寸--320x50  正常
/// </summary>
const char * tag_display_320x50 = "tag:display:320x50";

/// <summary>
/// 广告尺寸--640x100  高清大图
/// </summary>
const char * tag_display_640x100 = "tag:display:640x100";

/// <summary>
/// 广告尺寸--全部
/// </summary>
const char * tag_display_all = "tag:display:all";




//endregion

//region SDK信息
/// <summary>
/// SDK信息 Hash AdSDK实体
/// </summary>
const char * adsdk = "adsdk";
//endregion

//region 广告平台商信息
/// <summary>
/// 广告平台商信息 Hash AdPlatformProvider实体
/// </summary>
const char * adplatform = "adplatform";
//endregion


//region  需要同步到MsSql的广告ID集合(Set)
/// <summary>
/// 需要同步到MsSql的广告ID集合(Set)
/// </summary>
const char * tomssqladid = "tomssqladid";
//endregion

//region 达到每日最大值的广告ID集合(Set)
/// <summary>
/// 达到每日最大值的广告ID集合(Set)
/// </summary>
const char * daymaxadid = "daymaxadid";
//endregion

/// <summary>
/// 终端最近一次请求的广告(string)，有效期为1小时 ,{0}为 SessionId
/// </summary>
const char * mobile_lastrequest = "mobile:{0}:lastrequest";

/// <summary>
/// 终端当天点击的广告(string)，过期时间在下一天 ,{0}为 SessionId
/// </summary>
const char * mobile_lastclick = "mobile:{0}:lastclick";

/// <summary>
/// 终端最近回发次数，有效期为2分钟 ,{0}为 SessionId
/// </summary>
const char * mobile_lastcallback = "mobile:{0}:lastcallback";

//region 广告信息

/// <summary>
/// 广告--编号 {0}为广告Id
/// </summary>
const char * ad_id = "ad:{0}:id";

/// <summary>
/// 广告--广告主ID {0}为广告Id
/// </summary>
const char * ad_advertiserid = "ad:{0}:advertiserid";

/// <summary>
/// 广告--投放开始时间 {0}为广告Id
/// </summary>
const char * ad_putinstarttime = "ad:{0}:putinstarttime";

/// <summary>
/// 广告--投放结束时间 {0}为广告Id
/// </summary>
const char * ad_putinendtime = "ad:{0}:putinendtime";

/// <summary>
/// 广告--投放模式	 {0}为广告Id
/// </summary>
const char * ad_putinmodeltype = "ad:{0}:putinmodeltype";

/// <summary>
/// 广告--期望展示总数 {0}为广告Id	
/// </summary>
const char * ad_wantshownum = "ad:{0}:wantshownum";

/// <summary>
/// 广告--实际展示总数 {0}为广告Id	
/// </summary>
const char * ad_realshownum = "ad:{0}:realshownum";

/// <summary>
/// 广告--每日最大展示总数 {0}为广告Id	
/// </summary>
const char * ad_daymaxshownum = "ad:{0}:daymaxshownum";

/// <summary>
/// 广告--期望点击总数	 {0}为广告Id	
/// </summary>
const char * ad_wantclicknum = "ad:{0}:wantclicknum";

/// <summary>
/// 广告--实际点击总数	 {0}为广告Id		
/// </summary>
const char * ad_realclicknum = "ad:{0}:realclicknum";

/// <summary>
/// 广告--每日最大点击总数 {0}为广告Id				
/// </summary>
const char * ad_daymaxclicknum = "ad:{0}:daymaxclicknum";

/// <summary>
/// 广告--最后更新时间 {0}为广告Id				
/// </summary>
const char * ad_lastupdate = "ad:{0}:lastupdate";

/// <summary>
/// 广告--标签集合 以,相隔 如 1,2,3,4,5	
/// </summary>
const char * ad_tags = "ad:{0}:tags";

/// <summary>
/// 广告--广告请求数 {0}为广告Id				
/// </summary>
const char * ad_requestnum = "ad:{0}:requestnum";

/// <summary>
///广告--广告价格 {0}为广告Id
/// </summary>
const char * ad_price = "ad:{0}:price";

/// <summary>
/// 广告--广告覆盖面的经纬度列表
/// </summary>
const char * ad_longlatlist = "ad:{0}:longlatlist";

/// <summary>
/// 广告--计费点击(每天同一请求只记录一次) {0}为广告Id
/// </summary>
const char * ad_billingclicknum = "ad:{0}:billingclicknum";

/// <summary>
/// 广告--广告订单ID {0}为广告Id
/// </summary>
const char * ad_orderid = "ad:{0}:orderid";

//endregion



//region 广告覆盖面

/// <summary>
/// 广告覆盖面--type=Hash, key=adcoverage,field=广告id,value=AdCoverage的json实体
/// </summary>
const char * adcoverage = "adcoverage";

//endregion

//region 投放时间段(精确到小时)

/// <summary>
/// 投放时间段--00点
/// </summary>
const char * tag_putintime_00 = "tag:putintime:00";

/// <summary>
/// 投放时间段--01点
/// </summary>
const char * tag_putintime_01 = "tag:putintime:01";

/// <summary>
/// 投放时间段--02点
/// </summary>
const char * tag_putintime_02 = "tag:putintime:02";

/// <summary>
/// 投放时间段--03点
/// </summary>
const char * tag_putintime_03 = "tag:putintime:03";

/// <summary>
/// 投放时间段--04点
/// </summary>
const char * tag_putintime_04 = "tag:putintime:04";

/// <summary>
/// 投放时间段--05点
/// </summary>
const char * tag_putintime_05 = "tag:putintime:05";

/// <summary>
/// 投放时间段--06点
/// </summary>
const char * tag_putintime_06 = "tag:putintime:06";

/// <summary>
/// 投放时间段--07点
/// </summary>
const char * tag_putintime_07 = "tag:putintime:07";

/// <summary>
/// 投放时间段--08点
/// </summary>
const char * tag_putintime_08 = "tag:putintime:08";

/// <summary>
/// 投放时间段--09点
/// </summary>
const char * tag_putintime_09 = "tag:putintime:09";

/// <summary>
/// 投放时间段--10点
/// </summary>
const char * tag_putintime_10 = "tag:putintime:10";

/// <summary>
/// 投放时间段--11点
/// </summary>
const char * tag_putintime_11 = "tag:putintime:11";

/// <summary>
/// 投放时间段--12点
/// </summary>
const char * tag_putintime_12 = "tag:putintime:12";

/// <summary>
/// 投放时间段--13点
/// </summary>
const char * tag_putintime_13 = "tag:putintime:13";

/// <summary>
/// 投放时间段--14点
/// </summary>
const char * tag_putintime_14 = "tag:putintime:14";

/// <summary>
/// 投放时间段--15点
/// </summary>
const char * tag_putintime_15 = "tag:putintime:15";

/// <summary>
/// 投放时间段--16点
/// </summary>
const char * tag_putintime_16 = "tag:putintime:16";

/// <summary>
/// 投放时间段--17点
/// </summary>
const char * tag_putintime_17 = "tag:putintime:17";

/// <summary>
/// 投放时间段--18点
/// </summary>
const char * tag_putintime_18 = "tag:putintime:18";

/// <summary>
/// 投放时间段--19点
/// </summary>
const char * tag_putintime_19 = "tag:putintime:19";

/// <summary>
/// 投放时间段--20点
/// </summary>
const char * tag_putintime_20 = "tag:putintime:20";

/// <summary>
/// 投放时间段--21点
/// </summary>
const char * tag_putintime_21 = "tag:putintime:21";

/// <summary>
/// 投放时间段--22点
/// </summary>
const char * tag_putintime_22 = "tag:putintime:22";

/// <summary>
/// 投放时间段--23点
/// </summary>
const char * tag_putintime_23 = "tag:putintime:23";



//endregion

//region 广告体

/// <summary>
/// 广告体 AdBody实体信息 {0}为广告Id
/// </summary>
const char * ad_body = "ad:{0}:body";

//endregion

//region 广告回发记录(天)

/// <summary>
///String 每天回发的点击/展示量 {0}为广告Id
/// </summary>
const char * ad_count = "ad:{0}:count";

//endregion

//region 广告请求Session

/// <summary>
/// 广告请求Session,{0}为SessionId, 如 adsession:12345
/// </summary>
const char * adsession = "adsession:{0}";

//endregion

//region 广告回发队列

/// <summary>
/// 回发队列  List 
/// </summary>
const char * queue_adcallback = "queue:adcallback";

//endregion

//region 第三方回调队列
/// <summary>
/// 第三方回调队列
/// </summary>
const char * queue_thirdcallback = "queue:thirdcallback";
//endregion

//region 日志队列       
/// 广告日志
/// </summary>
const char * log_info = "log:info";       
//endregion

const char *null_format_string = "";

const double EARTH_RADIUS = 6378137;//地球半径
const double PI=3.14159265;


//#define isascii(c)	((unsigned)(c)<=0177)
#define ISASCII(c)	isascii(_UCHAR_(c))
#define _UCHAR_(c)	((unsigned char)(c))
#define ISSPACE(c)	(ISASCII(c) && isspace((unsigned char)(c)))
#define ISDIGIT(c)	(isdigit (c))

int     strncasecmp(const char *s1, const char *s2, size_t n)
{
    if (n != 0) {
        const unsigned char *us1 = (const unsigned char *) s1;
        const unsigned char *us2 = (const unsigned char *) s2;

        do {
            if (tolower(*us1) != tolower(*us2++))
                return (tolower(*us1) - tolower(*--us2));
            if (*us1++ == '\0')
                break;
        } while (--n != 0);
    }
    return (0);
}

int     alldig(const char *string)
{
    const char *cp;

    if (*string == 0)
        return (0);
    for (cp = string; *cp != 0; cp++)
        if (!ISDIGIT(*cp))
            return (0);
    return (1);
}

/*剔除字符串两边的whitespace*/
char *str_trim(char *p)
{
    char *trim = p;
    if (p == 0)
        return 0;
    while (*trim && ISSPACE(*trim))			/* Skip whitespace */
        trim++;
    while (*p)
        p++;
    while (--p >= trim)
    {
        if (*p == ' ')
            *p = '\0';
        else
            break;
    }
#if 0
    while(*p)
        p++;
    --p;                                    /*Skip '\0'*/
    while(*p && ISSPACE(*p))
        p--;
    p++;
    *p = '\0';
#endif

    return trim;
}

    int
str_split(char *c, char sep, const char **new_strings, int max_new_strings)
{
    int num_new_strings = 1;

    new_strings[0] = c;

    while (*c != '\0') {
        if (*c == sep) {
            if (num_new_strings < max_new_strings) {
                *c = '\0';
            }
            c++;
            while (*c == sep) {
                c++;
            }

            if (num_new_strings < max_new_strings) {
                new_strings[num_new_strings] = c;
            }
            num_new_strings++;
        }
        c++;
    }

    return num_new_strings;
}

    static int
strcopy(char* dest, size_t* dest_position, const char* src, int copy_len)
{
    int position = *dest_position;
    int i = 0;
    for(i = 0; src[i] != '\0'; ++i)
    {
        dest[position + i] = src[i];

        if (copy_len > -1 && (i == copy_len))
        {
            break;
        }
    }

    *dest_position = position + i;
    return i;
}

void str_replace(const char* what, const char* with, const char* in, char* where)
{
    char* substr = const_cast<char *>(strstr(in, what));

    if ((substr != NULL) && (what != NULL))
    {
        size_t pos = 0;
        int prefix_len = substr - in;
        strcopy(where, &pos, in, prefix_len);

        if (with != NULL)
        {
            strcopy(where, &pos, with, -1);
        }

        char* sufix = substr + strlen(what);
        strcopy(where, &pos, sufix, -1);
    }
    else
    {
        strcpy(where, in);
    }
}


/* mystrtok - safe tokenizer */
char   *mystrtok(char **src, const char *sep)
{
    char   *start = *src;
    char   *end;

    /*
     * Skip over leading delimiters.
     */
    start += strspn(start, sep);
    if (*start == 0) {
        *src = start;
        return (0);
    }

    /*
     * Separate off one token.
     */
    end = start + strcspn(start, sep);
    if (*end != 0)
        *end++ = 0;
    *src = end;
    return (start);
}

char * my_strlcpy3(char *dst, const char *src, size_t len) {
    strncpy(dst, src, len+1);
    dst[len ] = ',';

    return dst+ len+1;
}
char * my_strlcpy(char *dst, const char *src, size_t len) {
    strncpy(dst, src, len+1);
    dst[len ] = '\0';

    return dst+ len+1;
}

char* my_strlcpy2(char *dst, const char *src, size_t len) 
{
    strncpy(dst, src, len);

    return dst + len;
}
char* my_strlcpy4(char *dst, const char *src) 
{
   int len=strlen(src);
    strncpy(dst, src, len);

    return dst + len;
}

/// <summary>
/// 转换时间段
/// </summary>
/// <returns></returns>
static const char * ConvertPutintime(char *key,struct tm* date)
{

    //time_t t = time(NULL);
   // struct tm *date; 
	//date= localtime_r( &t,date);
    sprintf(key,"tag:putintime:%d-%02d-%02d:%02d",date->tm_year+1900,date->tm_mon+1,date->tm_mday,date->tm_hour);

    return key;
}

static const char * ConvertCategoryType(const char * category)
{
	if(category != NULL){
	    switch  (atoi(category)){
	        case 1:
	            return "tag:apptype:tool";
	        case 2:
	            return "tag:apptype:game";
	        case 3:
	            return "tag:apptype:read";
	        case 4:
	            return "tag:apptype:ent";
	        case 5:
	            return "tag:apptype:trip";
	        case 6:
	            return "tag:apptype:money";
	        case 7:
	            return "tag:apptype:auto";
	        case 8:
	            return "tag:apptype:lady";
	        case 9:
	            return "tag:apptype:news";
	        case 10:
	            return "tag:apptype:life";
	        case 11:
	            return "tag:apptype:sports";
	    }
	}

    return "tag:apptype:other";
}


// 转换广告类型
const char * ConvertSupportType(unsigned short supportType)
{
    const char *key = NULL;
    switch (supportType)
    {
        case 1:
            key = tag_type_web;
            break;
        case 2:
            key = tag_type_app;
            break;
        case 4:
            key = tag_type_full;
            break;
        case 5:
            key = tag_type_call;
            break;


    }
    return key;
}


const char * ConvertAdType(unsigned short supportType)
{
    const char *key = NULL;
    switch (supportType)
    {
        case 1:
            key = tag_adtype_img;
            break;
        case 2:
            key = tag_adtype_text;
            break;
        case 3:
            key = tag_adtype_html5;
            break;
        case 4:
            key = tag_adtype_gif;
            break;
    }
    return key;
}

double GetDistance(double lng1, double lat1, double lng2, double lat2)
{
    double RAD = PI / 180.0; 
    double radLat1 = lat1 * RAD;
    double radLat2 = lat2 * RAD;
    double a = radLat1 - radLat2;
    double b = (lng1 - lng2) * RAD;
    double s = 2 * asin(sqrt(sin(a / 2)*sin(a / 2) + cos(radLat1) * cos(radLat2) *sin(b / 2)*sin(b / 2) ));


    s = s * EARTH_RADIUS;
    s = round(s * 10000) / 10000;
    return s;
} 


//region 转换设备类型
/// <summary>
/// 转换设备类型
/// </summary>
/// <param name='client'></param>
/// <returns></returns>
const char * ConvertClient(unsigned int  client)
{
    const char * key = NULL;
    switch (client)
    {
        case 1:
            key = tag_client_android;
            break;
        case 2:
            key = tag_client_ios;
            break;
        case 3:
            key = tag_client_wp;
            break;

    }
    return key;
}

//endregion

//region  转换网?缋嘈?
/// <summary>
/// 转换网络类型
/// </summary>
/// <param name='net'></param>
/// <returns></returns>
const char * ConvertNet(unsigned short net)
{
    const char * key = NULL;
    switch (net)
    {
        case 0:
            key = tag_net_all;
            break;
        case 1:
            key = tag_net_2g;
            break;
        case 2:
            key = tag_net_3g;
            break;
        case 3:
            key = tag_net_wifi;
            break;
        case 4:
            key = tag_net_other;
            break;
    }
    return key;
}
//endregion
//#region 广告位类型
/// <summary>
/// 广告位类型 - banner
/// </summary>
const char * tag_adspace_banner = "tag:adspace:banner";
/// <summary>
///广告位类型 -  插屏
/// </summary>
const char * tag_adspace_insert = "tag:adspace:insert";
/// <summary>
/// 广告位类型 -	全屏
/// </summary>
const char *tag_adspace_full = "tag:adspace:full";
// #endregion

const char * ConvertAdSpaceType(unsigned short ast)
{
    const char * key = NULL;
    switch (ast)
    {
        case 1:
            key = tag_adspace_banner;
            break;
        case 2:
            key = tag_adspace_insert;
            break;
        case 3:
            key = tag_adspace_full;
            break;

    }
    return key;
}

//#region 转换设备类型
/// <summary>
/// 转换设备类型
/// </summary>
/// <param name="sType"></param>
/// <returns></returns>
const char * tag_devicetype_phone = "tag:devicetype:phone";
/// <summary>
/// pad
/// </summary>
const char *  tag_devicetype_pad = "tag:devicetype:pad";            
const char * ConvertDeviceType(unsigned short dt)
{
    const char *  key = NULL;
    switch (dt)
    {

        case 2:
            key = tag_devicetype_pad;
            break;
        default:
            key = tag_devicetype_phone;
            break;
    }
    return key;
}
//#endregion


//region  转换运营商
/// <summary>
/// 转换运营商
/// </summary>
/// <param name='net'></param>
/// <returns></returns>
const char * ConvertOperator(unsigned short _operator)
{
    const char * key = NULL;
    switch (_operator)
    {
        case 1:
            key = tag_operator_10086;
            break;
        case 2:
            key = tag_operator_10010;
            break;
        case 3:
            key = tag_operator_10000;
            break;
        case 4:
            key = tag_operator_other;
            break;
    }
    return key;
}

//endregion
static int comp(const void *a,const void *b)
{
    return *(int *)a-*(int *)b;
}


//    #region  就近取宽
/// <summary>
/// 就近取宽
/// </summary>
/// <param name="width"></param>
/// <returns></returns>
const char * NearbyWidth(int width)
{
    const char  *widthArray[3] = { "160_25", "320_50", "640_100" };//"160", "320", "640" };
int widthArrayNum[3] = { 160, 320, 640 };
int temp[3] = {0,};
int index[3] = {0,};
int i;
int r;
for ( i = 0; i < 3; i++)
{
    index[i] = (abs(width - widthArrayNum[i]));
    temp[i] = (abs(width - widthArrayNum[i]));
}
qsort(temp,3,sizeof(int ),comp);
const char * swValue = 0;
for( i = 0; i < 3; i++)
{
    if (index[i] == temp[0] ) break;
}
if (temp[0] != temp[1])
{

    swValue = widthArray[i];
}
else
{
    r = rand() % 2;
    swValue = widthArray[i + r];
}
return swValue;
}


//  #endregion
//region  转换广告尺寸
/// <summary>
/// 转换广告尺寸
/// </summary>
/// <param name='net'></param>
/// <returns></returns>
const char * ConvertWidth(int width)
{
    const char * key = NULL;
    switch (atoi(NearbyWidth(width)))
    {
        case 160:
            key = tag_display_160x25;
            break;
        case 320:
            key = tag_display_320x50;
            break;
        case 640:
            key = tag_display_640x100;
            break;
    }
    return key;
}
//endregion




/*
 *Copyright (c) 2012-2012 Allen Chueng DONSON.INC
 */


//#define _XOPEN_SOURCE 600     // For flockfile() on Linux
//#define _LARGEFILE_SOURCE     // Enable 64-bit file offsets
#define __STDC_FORMAT_MACROS  // <inttypes.h> wants this for C++
#define __STDC_LIMIT_MACROS   // C++ wants that for INT64_MAX


#ifndef O_BINARY
#define O_BINARY  0
#endif // O_BINARY
extern struct settings settings;

#define IsNullOrEmpty(p) ( p == NULL || strlen(p) == 0 )
#if 0 /* for release todo */
#define SET_ADSDK_FORMJSON(k,f) \
    jstr = json_object_get_string(json_object_object_get(new_obj,k)); \
app->f = ais_strdup(jstr)
#endif

#define SET_ADSDK_FORMJSON(k,f) \
    jstr = json_object_get_string(json_object_object_get(new_obj,k)); \
	templen = strlen(jstr); \
	conn->app.f =  conn->adparams_buf_last; \
	conn->adparams_buf_last = my_strlcpy(conn->app.f, jstr, templen);


#define SET_ADCALLBACK_FORMJSON(k,f) \
    jstr = json_object_get_string(json_object_object_get(adsession_obj,k)); \
if (jstr){ \
    templen = strlen(jstr); \
    conn->apc.f =  conn->adparams_buf_last; \
    conn->adparams_buf_last = my_strlcpy(conn->apc.f, jstr, templen); \
}else { \
    conn->apc.f = ""; \
}


//%lu (unsigned long) time(NULL), 
struct timeval tzyl; 
#if defined(DEBUG)
#define DEBUG_TRACE(x) do { \
    flockfile(stdout); \
    GETTIMEOFDAY(&tzyl); \
    printf("***  [%ld.%06ld].%p.%s.%d: ", \
            tzyl.tv_sec,tzyl.tv_usec,(void *) pthread_self(), \
            __func__, __LINE__); \
    printf x; \
    putchar('\n'); \
    fflush(stdout); \
    funlockfile(stdout); \
} while (0)
#else
#define DEBUG_TRACE(x)
#endif // DEBUG

#ifdef DEBUG
#define DEBUG_ERROT(x) do { \
    flockfile(stdout); \
    GETTIMEOFDAY(&tzyl); \
    printf("***  [%ld.%06ld].%p.%s.%d: ", \
            tzyl.tv_sec,tzyl.tv_usec,(void *) pthread_self(), \
            __func__, __LINE__); \
    printf x; \
    putchar('\n'); \
    fflush(stdout); \
    funlockfile(stdout); \
} while (0)
#else
#define DEBUG_ERROT(x)
#endif // DEBUG



#define MAX_REQUEST_BUF_SIZE 16384
#define MAX_PARAMS_BASE64_BUF_SIZE 128
#define PARAMS_BASE64_SWAP_LEN   3
enum {
    IMG_SIZE, 
    IMG_DOMAIN, 
    IMG_URL,
    OPEN_SIGN,
    OPEN_TIMESTAMP,
    DEBUG_LOG_FILE,
    OPEN_WRITE_LOG,

    AD_RATIO, 
    LISTENING_PORTS ,
    LOG_WORK_PATH,
    LOG_SWITCH_TIME,
    NUM_THREADS, 

    RUN_AS_USER, 
    REDIS_IP,
    REDIS_IP_PORT,
    REDIS_SLAVE_IP,
    REDIS_SLAVE_IP_PORT,
    REDIS_PASSWORD,

    OPEN_IP_BLACK,
    SESSION_ID_MAX_BRUSH,
    IP_MAX_BRUSH,
    CHECK_IP,
	DAEMON,  
	ADSTYLE,
	SQL_CONN,
	SQL_USER,
	SQL_PWD,
	DB_NAME,
	NUM_OPTIONS
};

static const char *config_options[] = {
    "c", "img_size", NULL,
    "I", "img_domain", NULL,
    "l","img_url",NULL,
    "S", "open_sign", "no",  
    "T", "open_timestamp", "no",
    "a", "debug_log_file", NULL,
    "d", "open_write_log", "no",
    "i", "ad_ratio", "1000",
    "p", "listening_ports", "8888",
    "r", "log_work_path",  ".",
    "s", "log_switch_time", "300",
    "t", "num_threads", "1",
    "u", "run_as_user", NULL,
    "w", "redis_ip", "127.0.0.1",
    "x", "redis_ip_port", "6379",
    "L", "redis_save_ip", NULL,
    "y", "redis_save_ip_port", NULL,
    "z", "redis_password", "donson.redis.0755",
    "B", "open_ip_black", "yes",
    "m", "session_id_max_brush","10",  
    "M", "ip_max_brush","200",
    "h", "check_ip","no",
    "d", "daemon", "no",
    "b", "adstyle", "160_25,320_50,640_100",
    "W", "sql_conn","SQL2005",
    "X", "sql_user","sa",
    "Y", "sql_pwd","Donson",
    "Z", "db_name","MASP",
    NULL
};
#define ENTRIES_PER_CONFIG_OPTION 3


//#define MAX_REDIS_POOL 200

//redisContext *g_redisctx[MAX_REDIS_POOL]; 
//int g_ri = 0;

#define MAX_IMG_SIZE 20

struct ais_context {
    char *config[NUM_OPTIONS];    // AIS configuration parameters
    char *img_size[MAX_IMG_SIZE];
    int img_size_len;
};

struct ais_connection {
    struct ais_context *ctx;
    struct ais_adparams ap;
    //  struct ais_adparams_body apb;
    struct ais_adparams_callback apc;
    struct ais_adparams_report  apr;
    struct    AdPlatformProvider app;
    struct ais_adparams_offline apo;
    char *adparams_buf;  
    char *adparams_buf_last;
    // char * temp_buf;
    //char * temp_buf_last;
    redisContext *redisctx; 
    redisContext *rredisctx; 
    redisContext *sredisctx; 
    redisContext *sredisctx_async;
    ht_node_pool_t *node_pool; 
    ht_map_pool_t *map_pool; 
#ifdef TXT_LOG
    eg_cycle_queue_t *cq;
#endif
#ifdef DEBUG_TIME	
    int64_t org_start; //add for time debug
    int64_t start, end;
    time_t birth_time;          // Time connection was accepted
    time_t die_time;          // Time connection was accepted
#endif
    char *buf;                  // Buffer for received data
    char *bufr;
    int bufr_len; 
    int buf_size;               // Buffer size
    conn *c;
};

#define	STR_EQ(x,y) ( x[0] == y[0] && strcmp((x), (y)) == 0)
#define jval(o,a)  json_object_to_json_string(json_object_object_get(o,(a)))
#define MAX_DEMli 1024
#define MAX_DELIM_NUM   16

int ais_ad_set(struct ais_connection * conn,const char **, int *, char *req_session=NULL  );
int ais_offline_set(struct ais_connection *conn, const char **, int *);
int ais_adbody_set(struct ais_connection * conn);
int ais_adcallback_set(struct ais_connection * conn,const char **, int *  );
int ais_adreport_set(struct ais_connection * conn,const char **, int *  );
int check_date(char *start, char *end);

static int get_option_index(const char *name) {
    int i;

    for (i = 0; config_options[i] != NULL; i += ENTRIES_PER_CONFIG_OPTION) {
        if (strcmp(config_options[i], name) == 0 ||
                strcmp(config_options[i + 1], name) == 0) {
            return i / ENTRIES_PER_CONFIG_OPTION;
        }
    }
    return -1;
}

//date format : XXXX-XX-XX
int check_date(char *start, char *end)
{
    int year1;
    int year2;
    int mon1;
    int mon2;
    int day1;
    int day2;
    int rs;

    if ('-' != start[4] || '-' != start[7])
    {
        return STARTDATE_FORMAT;
    }

    if ('-' != end[4] && '-' != end[7])
    {
        return ENDDATE_FORMAT;
    }

    start[4] = 0;
    start[7] = 0;
    year1 = atoi(start);
    mon1 = atoi(start + 5);
    day1 = atoi(start + 8);
    if (mon1 < 0 || mon1 > 12 || day1 < 0 || day1 > 31)
    {
        return STARTDATE_FORMAT;
    }

    end[4] = 0;
    end[7] = 0;
    year2 = atoi(end);
    mon2 = atoi(end + 5);
    day2 = atoi(end + 8);

    if (mon2 < 0 || mon2 > 12 || day2 < 0 || day2 > 31)
    {
        return ENDDATE_FORMAT;
    }

    rs = ((year2 - year1) * 12 + mon2 - mon1) * 30 + day2 - day1;

    if (rs < 0) return DATE_SUB_ERR;
    
    if (rs > 90) return DATE_GT_THREE;

    start[4] = '-';
    start[7] = '-';
    end[4] = '-';
    end[7] = '-';
    return SUCCESS;
}

static void ais_strlcpy(register char *dst, register const char *src, size_t n) {
    for (; *src != '\0' && n > 1; n--) {
        *dst++ = *src++;
    }
    *dst = '\0';
}

static int lowercase(const char *s) {
    return tolower(* (const unsigned char *) s);
}

static int ais_strncasecmp(const char *s1, const char *s2, size_t len) {
    int diff = 0;

    if (len > 0)
        do {
            diff = lowercase(s1++) - lowercase(s2++);
        } while (diff == 0 && s1[-1] != '\0' && --len > 0);

    return diff;
}

static int ais_strcasecmp(const char *s1, const char *s2)
{
    const unsigned char *us1 = (const unsigned char *) s1;
    const unsigned char *us2 = (const unsigned char *) s2;

    while (tolower(*us1) == tolower(*us2++))
        if (*us1++ == '\0')
            return (0);
    return (tolower(*us1) - tolower(*--us2));
}

static char * ais_strndup(const char *ptr, size_t len) {
    char *p;

    if ((p = (char *) malloc(len + 1)) != NULL) {
        ais_strlcpy(p, ptr, len + 1);
    }

    return p;
}

static char * ais_strdup(const char *str) {
    return ais_strndup(str, strlen(str));
}

//add by chenhuizhong
char *url_encode(const char *string, char *outbuf, int buflen, int do_special_char)
{
    const char *ptr  = string;
    char *out = outbuf;
    const char *mark = "-_.!~*'()";

    while (*ptr && out - outbuf < buflen - 1) { 
        if ((const signed char) *ptr < 32 || *ptr == 0x7f || *ptr == '%' ||
                (do_special_char &&
                 !(*ptr >= '0' && *ptr <= '9') &&      /*  num */
                 !(*ptr >= 'A' && *ptr <= 'Z') &&      /*  ALPHA */
                 !(*ptr >= 'a' && *ptr <= 'z') &&      /*  alpha */
                 !strchr(mark, *ptr))) {               /*  mark set */
            if (out - outbuf >= buflen - 3) { 
                break;
            }    

            out += sprintf(out, "%%%02X", (unsigned char) *ptr);
        } else {
            *out = *ptr;
            out++;
        }    
        ptr++;
    }    

    if (buflen) {
        *out = '\0';
    }    

    return outbuf;
}

// URL-decode input buffer into destination buffer.
// 0-terminate the destination buffer. Return the length of decoded data.
// form-url-encoded data differs from URI encoding in a way that it
// uses '+' as character for space, see RFC 1866 section 8.2.1
// http://ftp.ics.uci.edu/pub/ietf/html/rfc1866.txt
static int url_decode(const char *src, size_t src_len, char *dst,
        size_t dst_len, int is_form_url_encoded) {
    int i, j;
    int a, b;
#define HEXTOI(x) (isdigit(x) ? x - '0' : x - 'W')

    for (i = j = 0; i < src_len && j < dst_len - 1; i++, j++) {
        if (src[i] == '%' &&
                isxdigit(* (const unsigned char *) (src + i + 1)) &&
                isxdigit(* (const unsigned char *) (src + i + 2))) {
            a = tolower(* (const unsigned char *) (src + i + 1));
            b = tolower(* (const unsigned char *) (src + i + 2));
            dst[j] = (char) ((HEXTOI(a) << 4) | HEXTOI(b));
            i += 2;
        } else if (is_form_url_encoded && src[i] == '+') {
            dst[j] = ' ';
        } else {
            /*if (src[i] == ',')
            {
                return -1;
            } */
            dst[j] = src[i];
        }
    }

    dst[j] = '\0'; // Null-terminate the destination

    return j;
}


// URL-decode input buffer into destination buffer.
// 0-terminate the destination buffer. Return the length of decoded data.
// form-url-encoded data differs from URI encoding in a way that it
// uses '+' as character for space, see RFC 1866 section 8.2.1
// http://ftp.ics.uci.edu/pub/ietf/html/rfc1866.txt
static int url_decode1(const char *src, size_t src_len, char *dst,
        size_t dst_len, int is_form_url_encoded) {
    int i, j;
    int a, b;
#define HEXTOI(x) (isdigit(x) ? x - '0' : x - 'W')

    for (i = j = 0; i < src_len && j < dst_len - 1; i++, j++) {
        if (src[i] == '%' &&
                isxdigit(* (const unsigned char *) (src + i + 1)) &&
                isxdigit(* (const unsigned char *) (src + i + 2))) {
            a = tolower(* (const unsigned char *) (src + i + 1));
            b = tolower(* (const unsigned char *) (src + i + 2));
            dst[j] = (char) ((HEXTOI(a) << 4) | HEXTOI(b));
            i += 2;
        } else if (is_form_url_encoded && src[i] == '+') {
            dst[j] = ' ';
        } else {
            if (src[i] == ',')
            {
                dst[j]=' ';
                continue;
            } 
            dst[j] = src[i];
        }
    }

    dst[j] = '\0'; // Null-terminate the destination

    return j;
}



int ais_get_var(const char *buf, size_t buf_len, const char *name,
        char *dst, size_t dst_len) {
    const char *p, *e, *s;
    int name_len, len;

    name_len = strlen(name);
    e = buf + buf_len;
    len = -1;
    dst[0] = '\0';

    // buf is "var1=val1&var2=val2...". Find variable first
    for (p = buf; p != NULL && p + name_len < e; p++) {
        if ((p == buf || p[-1] == '&') && p[name_len] == '=' &&
                !ais_strncasecmp(name, p, name_len)) {

            // Point p to variable value
            p += name_len + 1;

            // Point s to the end of the value
            s = (const char *) memchr(p, '&', (size_t)(e - p));
            if (s == NULL) {
                s = e;
            }
            assert(s >= p);

            // Decode variable into destination buffer
            if ((size_t) (s - p) < dst_len) {
		if (!ais_strncasecmp(name,"groupby",strlen("groupby")))
                len = url_decode(p, (size_t)(s - p), dst, dst_len, 1);
		else
	          len = url_decode1(p, (size_t)(s - p), dst, dst_len, 1);
                if (len == -1)
                {
                    return -1;
                }
            }
            break;
        }
    }

    return len;
}


#ifndef HAVE_MD5
typedef struct MD5Context {
    uint32_t buf[4];
    uint32_t bits[2];
    unsigned char in[64];
} MD5_CTX;

#if defined(__BYTE_ORDER) && (__BYTE_ORDER == 1234)
#define byteReverse(buf, len) // Do nothing
#else
static void byteReverse(unsigned char *buf, unsigned longs) {
    uint32_t t;
    do {
        t = (uint32_t) ((unsigned) buf[3] << 8 | buf[2]) << 16 |
            ((unsigned) buf[1] << 8 | buf[0]);
        *(uint32_t *) buf = t;
        buf += 4;
    } while (--longs);
}
#endif

#define F1(x, y, z) (z ^ (x & (y ^ z)))
#define F2(x, y, z) F1(z, x, y)
#define F3(x, y, z) (x ^ y ^ z)
#define F4(x, y, z) (y ^ (x | ~z))

#define MD5STEP(f, w, x, y, z, data, s) \
    ( w += f(x, y, z) + data,  w = w<<s | w>>(32-s),  w += x )

// Start MD5 accumulation.  Set bit count to 0 and buffer to mysterious
// initialization constants.
static void MD5Init(MD5_CTX *ctx) {
    ctx->buf[0] = 0x67452301;
    ctx->buf[1] = 0xefcdab89;
    ctx->buf[2] = 0x98badcfe;
    ctx->buf[3] = 0x10325476;

    ctx->bits[0] = 0;
    ctx->bits[1] = 0;
}

static void MD5Transform(uint32_t buf[4], uint32_t const in[16]) {
    register uint32_t a, b, c, d;

    a = buf[0];
    b = buf[1];
    c = buf[2];
    d = buf[3];

    MD5STEP(F1, a, b, c, d, in[0] + 0xd76aa478, 7);
    MD5STEP(F1, d, a, b, c, in[1] + 0xe8c7b756, 12);
    MD5STEP(F1, c, d, a, b, in[2] + 0x242070db, 17);
    MD5STEP(F1, b, c, d, a, in[3] + 0xc1bdceee, 22);
    MD5STEP(F1, a, b, c, d, in[4] + 0xf57c0faf, 7);
    MD5STEP(F1, d, a, b, c, in[5] + 0x4787c62a, 12);
    MD5STEP(F1, c, d, a, b, in[6] + 0xa8304613, 17);
    MD5STEP(F1, b, c, d, a, in[7] + 0xfd469501, 22);
    MD5STEP(F1, a, b, c, d, in[8] + 0x698098d8, 7);
    MD5STEP(F1, d, a, b, c, in[9] + 0x8b44f7af, 12);
    MD5STEP(F1, c, d, a, b, in[10] + 0xffff5bb1, 17);
    MD5STEP(F1, b, c, d, a, in[11] + 0x895cd7be, 22);
    MD5STEP(F1, a, b, c, d, in[12] + 0x6b901122, 7);
    MD5STEP(F1, d, a, b, c, in[13] + 0xfd987193, 12);
    MD5STEP(F1, c, d, a, b, in[14] + 0xa679438e, 17);
    MD5STEP(F1, b, c, d, a, in[15] + 0x49b40821, 22);

    MD5STEP(F2, a, b, c, d, in[1] + 0xf61e2562, 5);
    MD5STEP(F2, d, a, b, c, in[6] + 0xc040b340, 9);
    MD5STEP(F2, c, d, a, b, in[11] + 0x265e5a51, 14);
    MD5STEP(F2, b, c, d, a, in[0] + 0xe9b6c7aa, 20);
    MD5STEP(F2, a, b, c, d, in[5] + 0xd62f105d, 5);
    MD5STEP(F2, d, a, b, c, in[10] + 0x02441453, 9);
    MD5STEP(F2, c, d, a, b, in[15] + 0xd8a1e681, 14);
    MD5STEP(F2, b, c, d, a, in[4] + 0xe7d3fbc8, 20);
    MD5STEP(F2, a, b, c, d, in[9] + 0x21e1cde6, 5);
    MD5STEP(F2, d, a, b, c, in[14] + 0xc33707d6, 9);
    MD5STEP(F2, c, d, a, b, in[3] + 0xf4d50d87, 14);
    MD5STEP(F2, b, c, d, a, in[8] + 0x455a14ed, 20);
    MD5STEP(F2, a, b, c, d, in[13] + 0xa9e3e905, 5);
    MD5STEP(F2, d, a, b, c, in[2] + 0xfcefa3f8, 9);
    MD5STEP(F2, c, d, a, b, in[7] + 0x676f02d9, 14);
    MD5STEP(F2, b, c, d, a, in[12] + 0x8d2a4c8a, 20);

    MD5STEP(F3, a, b, c, d, in[5] + 0xfffa3942, 4);
    MD5STEP(F3, d, a, b, c, in[8] + 0x8771f681, 11);
    MD5STEP(F3, c, d, a, b, in[11] + 0x6d9d6122, 16);
    MD5STEP(F3, b, c, d, a, in[14] + 0xfde5380c, 23);
    MD5STEP(F3, a, b, c, d, in[1] + 0xa4beea44, 4);
    MD5STEP(F3, d, a, b, c, in[4] + 0x4bdecfa9, 11);
    MD5STEP(F3, c, d, a, b, in[7] + 0xf6bb4b60, 16);
    MD5STEP(F3, b, c, d, a, in[10] + 0xbebfbc70, 23);
    MD5STEP(F3, a, b, c, d, in[13] + 0x289b7ec6, 4);
    MD5STEP(F3, d, a, b, c, in[0] + 0xeaa127fa, 11);
    MD5STEP(F3, c, d, a, b, in[3] + 0xd4ef3085, 16);
    MD5STEP(F3, b, c, d, a, in[6] + 0x04881d05, 23);
    MD5STEP(F3, a, b, c, d, in[9] + 0xd9d4d039, 4);
    MD5STEP(F3, d, a, b, c, in[12] + 0xe6db99e5, 11);
    MD5STEP(F3, c, d, a, b, in[15] + 0x1fa27cf8, 16);
    MD5STEP(F3, b, c, d, a, in[2] + 0xc4ac5665, 23);

    MD5STEP(F4, a, b, c, d, in[0] + 0xf4292244, 6);
    MD5STEP(F4, d, a, b, c, in[7] + 0x432aff97, 10);
    MD5STEP(F4, c, d, a, b, in[14] + 0xab9423a7, 15);
    MD5STEP(F4, b, c, d, a, in[5] + 0xfc93a039, 21);
    MD5STEP(F4, a, b, c, d, in[12] + 0x655b59c3, 6);
    MD5STEP(F4, d, a, b, c, in[3] + 0x8f0ccc92, 10);
    MD5STEP(F4, c, d, a, b, in[10] + 0xffeff47d, 15);
    MD5STEP(F4, b, c, d, a, in[1] + 0x85845dd1, 21);
    MD5STEP(F4, a, b, c, d, in[8] + 0x6fa87e4f, 6);
    MD5STEP(F4, d, a, b, c, in[15] + 0xfe2ce6e0, 10);
    MD5STEP(F4, c, d, a, b, in[6] + 0xa3014314, 15);
    MD5STEP(F4, b, c, d, a, in[13] + 0x4e0811a1, 21);
    MD5STEP(F4, a, b, c, d, in[4] + 0xf7537e82, 6);
    MD5STEP(F4, d, a, b, c, in[11] + 0xbd3af235, 10);
    MD5STEP(F4, c, d, a, b, in[2] + 0x2ad7d2bb, 15);
    MD5STEP(F4, b, c, d, a, in[9] + 0xeb86d391, 21);

    buf[0] += a;
    buf[1] += b;
    buf[2] += c;
    buf[3] += d;
}

static void MD5Update(MD5_CTX *ctx, unsigned char const *buf, unsigned len) {
    //DEBUG_TRACE(("MD5Update buf [%s] len[%d]",buf,len));
    uint32_t t;

    t = ctx->bits[0];
    if ((ctx->bits[0] = t + ((uint32_t) len << 3)) < t)
        ctx->bits[1]++;
    ctx->bits[1] += len >> 29;

    t = (t >> 3) & 0x3f;

    if (t) {
        unsigned char *p = (unsigned char *) ctx->in + t;

        t = 64 - t;
        if (len < t) {
            memcpy(p, buf, len);
            return;
        }
        memcpy(p, buf, t);
        byteReverse(ctx->in, 16);
        MD5Transform(ctx->buf, (uint32_t *) ctx->in);
        buf += t;
        len -= t;
    }

    while (len >= 64) {
        memcpy(ctx->in, buf, 64);
        byteReverse(ctx->in, 16);
        MD5Transform(ctx->buf, (uint32_t *) ctx->in);
        buf += 64;
        len -= 64;
    }

    memcpy(ctx->in, buf, len);
}

static void MD5Final(unsigned char digest[16], MD5_CTX *ctx) {
    unsigned count;
    unsigned char *p;

    count = (ctx->bits[0] >> 3) & 0x3F;

    p = ctx->in + count;
    *p++ = 0x80;
    count = 64 - 1 - count;
    if (count < 8) {
        memset(p, 0, count);
        byteReverse(ctx->in, 16);
        MD5Transform(ctx->buf, (uint32_t *) ctx->in);
        memset(ctx->in, 0, 56);
    } else {
        memset(p, 0, count - 8);
    }
    byteReverse(ctx->in, 14);

    ((uint32_t *) ctx->in)[14] = ctx->bits[0];
    ((uint32_t *) ctx->in)[15] = ctx->bits[1];

    MD5Transform(ctx->buf, (uint32_t *) ctx->in);
    byteReverse((unsigned char *) ctx->buf, 4);
    memcpy(digest, ctx->buf, 16);
    memset((char *) ctx, 0, sizeof(*ctx));
}
#endif // !HAVE_MD5

// Stringify binary data. Output buffer must be twice as big as input,
// because each byte takes 2 bytes in string representation
static void bin2str(char *to, const unsigned char *p, size_t len) {
    static const char *hex = "0123456789abcdef";

    for (; len--; p++) {
        *to++ = hex[p[0] >> 4];
        *to++ = hex[p[0] & 0x0f];
    }
    *to = '\0';
}

void replace_illegal_char(char *start, char *end)
{
    char *tmp;
    for (tmp = start; tmp < end; ++tmp)
    {
        if (',' == *tmp || '\"' == *tmp || '\n' == *tmp || '\r' == *tmp)
        {
            *tmp = ' ';
        }
    }
}


// Return stringified MD5 hash for list of vectors. Buffer must be 33 bytes.
void ais_md5(char *buf, const char ** md5,char * ads) {
    char tempbuf[1024] = {0};
    char *last = tempbuf;
    unsigned char hash[16];
    const char *p;
    //va_list ap;
    MD5_CTX ctx;
    int j = 0; 


    MD5Init(&ctx);

    // va_start(ap, buf);
    while ((p = md5[j]) != NULL) {
        j++;
        #if 1
        MD5Update(&ctx, (const unsigned char *) p, (unsigned) strlen(p));
        #else
        int len = strlen(p);
        memcpy(last, p, len);
        last += len;
    }
    memcpy(last, ads, strlen(ads));
    last += strlen(ads);
    *last = '\0';
    MD5Update(&ctx, (const unsigned char *) tempbuf, (unsigned) (last - tempbuf));
        #endif
    //  va_end(ap);
    }
    MD5Update(&ctx, (const unsigned char *) ads, (unsigned) strlen(ads));

    MD5Final(hash, &ctx);
    bin2str(buf, hash, sizeof(hash));
}


// Return stringified MD5 hash for list of vectors. Buffer must be 33 bytes.
void ais_md5_(char *buf, ...) {
    unsigned char hash[16];
    const char *p;
    va_list ap;
    MD5_CTX ctx;

    MD5Init(&ctx);

    va_start(ap, buf);
    while ((p = va_arg(ap, const char *)) != NULL) {
        MD5Update(&ctx, (const unsigned char *) p, (unsigned) strlen(p));
    }
    va_end(ap);

    MD5Final(hash, &ctx);
    bin2str(buf, hash, sizeof(hash));
}

unsigned int  address_to_long(  char *ipstr )
{
    unsigned int ret = 0;
    int a1, a2, a3, a4;
    sscanf( ipstr, "%d.%d.%d.%d", &a1, &a2, &a3, &a4 );
    DEBUG_TRACE(("%d.%d.%d.%d",a1,a2,a3,a4));
    ret = ((a1<<24)|(a2<<16)|(a3<<8)|a4);
    return ret;
}




static void reset_per_request_attributes(struct ais_connection *conn) {
    struct ais_adparams *ap = &(conn->ap);
    //	struct ais_adparams_body *apb = &(conn->apb);
    struct ais_adparams_callback *apc = &(conn->apc);
    memset(ap,0,sizeof(*ap));
    //	memset(apb,0,sizeof(*apb));
    memset(apc,0,sizeof(*apc));
    conn->adparams_buf_last =conn->adparams_buf= (char *) (conn + 1);


}

#if 0
#include "mxml/config.h"
#include "mxml/mxml.h"
#ifndef WIN32
#  include <unistd.h>
#endif /* !WIN32 */
#include <fcntl.h>
#ifndef O_BINARY
#  define O_BINARY 0
#endif /* !O_BINARY */
/*
 * 'whitespace_cb()' - Let the mxmlSaveFile() function know when to insert
 *                     newlines and tabs...
 */

    static const char *				/* O - Whitespace string or NULL */
whitespace_cb(mxml_node_t *node,	/* I - Element node */
        int         where)	/* I - Open or close tag? */
{
    return (NULL);
    mxml_node_t	*parent;		/* Parent node */
    int		level;			/* Indentation level */
    const char	*name;			/* Name of element */
    static const char *tabs = "\t\t\t\t\t\t\t\t";
    /* Tabs for indentation */


    /*
     * We can conditionally break to a new line before or after any element.
     * These are just common HTML elements...
     */

    name = node->value.element.name;

    if (!strcmp(name, "html") || !strcmp(name, "head") || !strcmp(name, "body") ||
            !strcmp(name, "pre") || !strcmp(name, "p") ||
            !strcmp(name, "h1") || !strcmp(name, "h2") || !strcmp(name, "h3") ||
            !strcmp(name, "h4") || !strcmp(name, "h5") || !strcmp(name, "h6"))
    {
        /*
         * Newlines before open and after close...
         */

        if (where == MXML_WS_BEFORE_OPEN || where == MXML_WS_AFTER_CLOSE)
            return ("\n");
    }
    else if (!strcmp(name, "dl") || !strcmp(name, "ol") || !strcmp(name, "ul"))
    {
        /*
         * Put a newline before and after list elements...
         */

        return ("\n");
    }
    else if (!strcmp(name, "dd") || !strcmp(name, "dt") || !strcmp(name, "li"))
    {
        /*
         * Put a tab before <li>'s, <dd>'s, and <dt>'s, and a newline after them...
         */

        if (where == MXML_WS_BEFORE_OPEN)
            return ("\t");
        else if (where == MXML_WS_AFTER_CLOSE)
            return ("\n");
    }
    else if (!strncmp(name, "?xml", 4))
    {
        if (where == MXML_WS_AFTER_OPEN)
            return ("\n");
        else
            return (NULL);
    }
    else if (where == MXML_WS_BEFORE_OPEN ||
            ((!strcmp(name, "choice") || !strcmp(name, "option")) &&
             where == MXML_WS_BEFORE_CLOSE))
    {
        for (level = -1, parent = node->parent;
                parent;
                level ++, parent = parent->parent);

        if (level > 8)
            level = 8;
        else if (level < 0)
            level = 0;

        return (tabs + 8 - level);
    }
    else if (where == MXML_WS_AFTER_CLOSE ||
            ((!strcmp(name, "group") || !strcmp(name, "option") ||
              !strcmp(name, "choice")) &&
             where == MXML_WS_AFTER_OPEN))
        return ("\n");
    else if (where == MXML_WS_AFTER_OPEN && !node->child)
        return ("\n");

    /*
     * Return NULL for no added whitespace...
     */

    return (NULL);
}
#endif



/*
   {"status":"100","msg":"成功","data":{"ad":%s}}
   {"aid":"1","ts":"1355799274","sessionid":"0c1d2b0667254e17abf60375d9ba6d5e","creative":{"cid":"1","ts":"1355799221","displayinfo":{"type":"1","img":"http:\/\/rm.lomark.cn\/Upload\/201212\/201212181053416444OS_320_50.jpg","schema":"fb98e702541d4f17acd7af0015e0779a"},"clkinfos":[{"type":"1","schema":"fb98e702541d4f17acd7af0015e0779a","url":"http:\/\/www.donson.com.cn"}],"trackers":{"clicks":[{"schema":"fb98e702541d4f17acd7af0015e0779a","urls":["http:\/\/192.168.0.42:5555\/callback?Key={$Key$}&AppId={$AppId$}&AppName={$AppName$}&Uuid={$Uuid$}&Client={$Client$}&Operator={$Operator$}&Net={$Net$}&DeviceType={$DeviceType$}&AdSpaceType={$AdSpaceType$}&Category={$Category$}&Ip={$Ip$}&Os_version={$Os_version$}&Aw={$Aw$}&Ah={$Ah$}&TimeStamp={$TimeStamp$}&Sign={$Sign$}&Format={$Format$}&Density={$Density$}&Long={$Long$}&Lat={$Lat$}&DeviceNum={$DeviceNum$}&SdkVersion={$SdkVersion$}&CId={$CId$}&CallbackType={$CallbackType$}&Schema={$Schema$}&Pw={$Pw$}&Ph={$Ph$}"]}],"display":{"urls":["http:\/\/192.168.0.42:5555\/callback?Key={$Key$}&AppId={$AppId$}&AppName={$AppName$}&Uuid={$Uuid$}&Client={$Client$}&Operator={$Operator$}&Net={$Net$}&DeviceType={$DeviceType$}&AdSpaceType={$AdSpaceType$}&Category={$Category$}&Ip={$Ip$}&Os_version={$Os_version$}&Aw={$Aw$}&Ah={$Ah$}&TimeStamp={$TimeStamp$}&Sign={$Sign$}&Format={$Format$}&Density={$Density$}&Long={$Long$}&Lat={$Lat$}&DeviceNum={$DeviceNum$}&SdkVersion={$SdkVersion$}&CId={$CId$}&CallbackType={$CallbackType$}&Schema={$Schema$}&Pw={$Pw$}&Ph={$Ph$}"]}}}}


*/
#define AIS_QUICK_FORMT(p,ad) if ( *(p->Format) == 'x' || *(p->Format) == 'X') { \
    snprintf(buffer,MAX_REQUEST_BUF_SIZE, "<?xml version=\"1.0\" encoding=\"utf-8\"?><result><status>%s</status><msg>%s</msg><data>%s</data></result>",msgerror[SUCCESS].code,msgerror[SUCCESS].msg,ad); 	\
} \
else {\
    snprintf(buffer,MAX_REQUEST_BUF_SIZE,"{\"status\":\"%s\",\"msg\":\"%s\",\"data\":{\"ad\":%s}}",msgerror[SUCCESS].code,msgerror[SUCCESS].msg,ad); \
}	\
out_string(conn->c, buffer)

#define AIS_QUICK_FORMT_WAP(p,ad,w,h) \
    snprintf(buffer,MAX_REQUEST_BUF_SIZE,"LomarkSetJson({\"status\":\"%s\",\"msg\":\"%s\",\"data\":{\"ad\":%s}},%s,%s)",msgerror[SUCCESS].code,msgerror[SUCCESS].msg,ad,w,h); \
    out_string(conn->c, buffer)

#define AIS_QUICK_FORMT1(p,ad) if ( *(p->Format) == 'x' || *(p->Format) == 'X') { \
    snprintf(buffer,MAX_REQUEST_BUF_SIZE, "<?xml version=\"1.0\" encoding=\"utf-8\"?><result><status>%s</status><msg>%s</msg><data>%s</data></result>",msgerror[SUCCESS].code,msgerror[SUCCESS].msg,ad); 	\
} \
else {\
    snprintf(buffer,MAX_REQUEST_BUF_SIZE,"{\"status\":\"%s\",\"msg\":\"%s\",\"data\":{\"ad\":[%s]}}",msgerror[SUCCESS].code,msgerror[SUCCESS].msg,ad); \
}	\
out_string(conn->c, buffer)
//printf("-----%s,length=%d\n",buffer,strlen(buffer));
//if (*( conn->ctx->config[OPEN_SIGN] ) == 'y' ){

#define AIS_ERROR_FOMAT(p,error,code) if ( *(p->Format) == 'x' || *(p->Format) == 'X') \
{ \
    snprintf(buffer,MAX_REQUEST_BUF_SIZE, "<?xml version=\"1.0\" encoding=\"utf-8\"?><result><status>%s</status><msg>%s</msg><data></data></result>",code,error);	\
} \
else \
{ \
    snprintf(buffer,MAX_REQUEST_BUF_SIZE, "{\"status\":\"%s\",\"msg\":\"%s\",\"data\":null}",code,error);	\
} \
out_string(conn->c, buffer);


#define AIS_ERROR_FORMAT(p,zyl) if (p->Format && (*(p->Format) == 'x' || *(p->Format) == 'X')) \
{ \
    snprintf(buffer,MAX_REQUEST_BUF_SIZE, "<?xml version=\"1.0\" encoding=\"utf-8\"?><result><status>%s</status><msg>%s</msg><data></data></result>",msgerror[zyl].code,msgerror[zyl].msg);	\
} \
else \
{ \
    snprintf(buffer,MAX_REQUEST_BUF_SIZE, "{\"status\":\"%s\",\"msg\":\"%s\",\"data\":null}",msgerror[zyl].code,msgerror[zyl].msg);	\
} \
out_string(conn->c, buffer);

#define AIS_ERROR_FORMAT_WAP(p,zyl)\
    snprintf(buffer,MAX_REQUEST_BUF_SIZE, "LomarkSetJson({\"status\":\"%s\",\"msg\":\"%s\",\"data\":null},0,0)",msgerror[zyl].code,msgerror[zyl].msg);	\
    out_string(conn->c, buffer)

static char *get_logparam(const char *key, ais_logparams_t *log_ps)
{
    if (!strcasecmp(key, CB_SESSION))
    {
        return log_ps->session_id;
    }else if (!strcasecmp(key, CB_URL))
    {
        return log_ps->url;
    }else if (!strcasecmp(key, CB_FORMAT))
    {
        return log_ps->format;
    }else if (!strcasecmp(key, CB_PID))
    {
        return log_ps->pp_id;
    }else if (!strcasecmp(key, CB_TIMESTAMP))
    {
        return log_ps->timestamp;
    }

    return "";
}

static int set_cb_param(const char *key, char *val, struct ais_connection *conn)
{
    struct ais_adparams_callback *apc = &(conn->apc);
    struct AdPlatformProvider *app = &(conn->app);

    if (!strcasecmp(key, CB_SESSION))
    {
        apc->sessionid = val;
    }else if (!strcasecmp(key, CB_URL))
    {
        apc->url = val;
    }else if (!strcasecmp(key, CB_FORMAT))
    {
        apc->Format = val;
    }else if (!strcasecmp(key, CB_PID))
    {
        app->PlatformProviderID = val;
    }else if (!strcasecmp(key, CB_TIMESTAMP))
    {
        return 0;
    }else
    {
        return -1;
    }

    return 0;
}

static int parse_user_agent(const char *user_agent, struct ais_connection *conn)
{
    struct ais_adparams_offline *apo = &(conn->apo);
    char *p = apo->device; 
    char * start = NULL;
    char * end = NULL;
    int flag = 0;
    char *device_num = unknown;
    char *device_type = DEF_DEVICE_TYPE;
    char *os_ver = unknown;
	char *client = DEF_CLIENT;
    /*ua example :    Mozilla/5.0 (Linux; U; Android 2.3.7; en-us; Nexus One Build/FRF91) AppleWebKit/533.1 (KHTML, like Gecko) Version/4.0 Mobile Safari/533.1*/
    if (!apo->device || strlen(apo->device) <= 0) {
        goto return_back;
    }
    /*get the word    after build, use it as device num 
    get device start!*/
    while (*p!='\0'){
        if (*p==',')
            *p=' ';
        p++;
    }
    start = strchr(apo->device, '(');
	if (!start)
    {
        goto return_back;
    }
	end = strchr(start, ';');
	if (!end)
    {
        goto return_back;
    }
	p = start + 1;
    *(end++) = '\0';
	/*parse device_type and client*/
	if (0 == strncasecmp(p, "iphone", strlen("iphone")))
    {
        device_type = "1";
		client = "2";
	}
	else if (0 == strncasecmp(p, "ipod", strlen("ipod")))
    {
        device_type = "1";
        client = "2";
    }
	else if (0 == strncasecmp(p, "ipad", strlen("ipad")))
    {
        device_type = "2";
        client = "2";
    }
    else if (0 == strncasecmp(p, "android", strlen("android"))
		|| 0 == strncasecmp(p, "linux", strlen("linux")))
    {
        device_type = "1";
        client = "1";
    }
	
    /*parse os_ver*/
	p = end;
	if (!p)
    {
        goto return_back;
    }
	if (0 == strcmp(client, "2"))   //ios
    {
	    start = strcasestr(p, "cpu");
	    if (!start)
		{
            goto return_back;
        }
	
    	end = strcasestr(p, "like");
	    if (NULL == end || end - start >= 50)
		{
            goto return_back;
        }
		while(*(++start) == ' ');
		while(*(--end) == ' ');
		*(++end) = '\0';
        os_ver = start;
	}
	else if (0 == strcmp(client, "1")) //android
	{
        //case 1: (Linux; U; Android 4.0.3; en-us; Transformer Prime TF201 Build/IML74K) 
        start = strcasestr(p, "android");
        if (!start)
        {
            //case 2: (Android 4.1; Linux; Opera Mobi/ADR-1205181138; U; en-GB)
            start = strcasestr(apo->device, "android");
			if (!start)
                goto return_back;

			os_ver = start;
        }
		else
        {
            end = strchr(p, ';');
            if (NULL == end)
            {
                end = strchr(p, ')');
                
            }
            if (NULL == end || end - start >= 50 )
            {
                 goto return_back;
            }
		    *(end++) = '\0';
		    os_ver = start;
		}
	}
    /*use semicolon as delimiter*/
    p = end;
    while((start = strchr(p,';'))){
        *start='\0';
        if ((end=strstr(p,"Build"))){
            *end=0;
            device_num=p;
            if (strlen(device_num)>50)
                *(device_num+50)='\0';
                flag=1;
                break;
        }
        p=start+1;
    }
    if (!flag){
        if ((end=strstr(p,"Build"))){
            *end=0;
            device_num=p;
            if (strlen(device_num)>50)
                *(device_num+50)='\0';
        }
    }

return_back:

    apo->client = conn->adparams_buf_last;
    conn->adparams_buf_last = my_strlcpy(apo->client, client, strlen(client));
	*(conn->adparams_buf_last++) = '\0';

    apo->device_type = conn->adparams_buf_last;
    conn->adparams_buf_last = my_strlcpy(apo->device_type, device_type, strlen(device_type));
	*(conn->adparams_buf_last++) = '\0';

	apo->device_num = conn->adparams_buf_last;
    conn->adparams_buf_last = my_strlcpy(apo->device_num, device_num, strlen(device_num));
	*(conn->adparams_buf_last++) = '\0';

    apo->os_ver = conn->adparams_buf_last;
    conn->adparams_buf_last = my_strlcpy(apo->os_ver, os_ver, strlen(os_ver));
	*(conn->adparams_buf_last++) = '\0';

    DEBUG_INFO(("clinet[%s], device_type[%s], device_num[%s], os_ver[%s]", apo->client, apo->device_type, apo->device_num, apo->os_ver));
	return 0;

}

#define DEBUG_TRACE_REDIS_CTX(X) \
    DEBUG_TRACE(("fd:%d, flags:%d, obuf:%p , reader:%p, err:%d, errstr:%s", \
    X->fd, X->flags, X->obuf, X->reader, X->err, X->errstr));

int safeRedisAppendCommandArgv(redisContext *c, int argc, const char **argv, const size_t *argvlen) {
    DEBUG_TRACE(("fd:%d, flags:%d, obuf:%p , reader:%p\n",
        c->fd, c->flags, c->obuf, c->reader));
    if (!(c->flags & REDIS_CONNECTED))
           return REDIS_ERR;
    return redisAppendCommandArgv(c, argc, argv, argvlen) ;
}

int safeRedisAppendCommand(redisContext *c, const char *format, ...) {
    if (!(c->flags & REDIS_CONNECTED))
        return -1;
    
    va_list ap;
    int ret;

    va_start(ap,format);
    ret = redisvAppendCommand(c,format,ap);
    va_end(ap);
    return ret;
}

//Write redis
static void send_redis_command(redisContext *c, const char *format, ...) {
    int done = 0;
    int err = 0;

    if (!(c->flags & REDIS_CONNECTED)) {
        if (rediskeepalive::__AsyncHandleConnect(c, err) != REDIS_OK)
        {
            rediskeepalive::__disConnect(c);
            ERROR_LOG(("==>__disConnect, fd:%d, flags:%d, obuf:%p , reader:%p\n", c->fd, c->flags, c->obuf, c->reader));
            rediskeepalive::__AsyncConnect(c, (char*)(g_ctx.config[REDIS_IP]), atoi(g_ctx.config[REDIS_IP_PORT]));
            ERROR_LOG(("==>reconnect [%s:%d] err[%d], try again, fd:%d, flags:%d, obuf:%p , reader:%p",
                    (char*)(g_ctx.config[REDIS_IP]), atoi(g_ctx.config[REDIS_IP_PORT]), err, c->fd, c->flags, c->obuf, c->reader));
            return;
        }

        /* Try again later when the context is still not connected. */
        if (!(c->flags & REDIS_CONNECTED))
            return;

        ERROR_LOG(("==>OK [%s:%d], fd:%d, flags:%d, obuf:%p , reader:%p",
                            (char*)(g_ctx.config[REDIS_IP]), atoi(g_ctx.config[REDIS_IP_PORT]), c->fd, c->flags, c->obuf, c->reader));

        c->flags |= REDIS_BLOCK;
        rediskeepalive::__SetBlocking(c,c->fd,1);
        struct timeval tval;
        tval.tv_sec=1;
        tval.tv_usec=0;
        redisSetTimeout(c, tval);

        if (cliAuth(c) != REDIS_OK)
        {
            ERROR_LOG(("==>cliAuth failed [%s:%d]", (char*)(g_ctx.config[REDIS_IP]), atoi(g_ctx.config[REDIS_IP_PORT])));
            if ( (REDIS_ERR_IO==c->err) || (REDIS_ERR_EOF==c->err) )
            {
                ERROR_LOG(("==>cliAuth() err[%d][%s], reconnect [%s:%d]...",
                    c->err, c->errstr, (char*)(g_ctx.config[REDIS_IP]), atoi(g_ctx.config[REDIS_IP_PORT])));
                rediskeepalive::__disConnect(c);
                rediskeepalive::__AsyncConnect(c, (char*)(g_ctx.config[REDIS_IP]), atoi(g_ctx.config[REDIS_IP_PORT]));
            }
            return;
        }
    }

    /* sync write data until it's done */
    va_list ap;    
    va_start(ap,format);
    redisvAppendCommand(c, format, ap);
    va_end(ap);
    do{
        if (redisBufferWrite(c,&done) == REDIS_ERR)
        {
        /*#define EAGAIN      11   Try again */
        /*#define ECONNREFUSED    111  Connection refused */ 
            ERROR_LOG(("==>redisBufferWrite() %s:%d errno[%d], reconnect...",
                (char*)(g_ctx.config[REDIS_IP]), atoi(g_ctx.config[REDIS_IP_PORT]), errno));
            rediskeepalive::__disConnect(c);
            rediskeepalive::__AsyncConnect(c, (char*)(g_ctx.config[REDIS_IP]), atoi(g_ctx.config[REDIS_IP_PORT]));
            break;
        }
    }while (!done);
}

//Write redis
static void *redis_command_pending(redisContext *c, const char *format, ...) {
    int err = 0;

    if (!(c->flags & REDIS_CONNECTED)) {
        if (rediskeepalive::__AsyncHandleConnect(c, err) != REDIS_OK)
        {
            rediskeepalive::__disConnect(c);
            ERROR_LOG(("-->__disConnect, fd:%d, flags:%d, obuf:%p , reader:%p\n", c->fd, c->flags, c->obuf, c->reader));
            rediskeepalive::__AsyncConnect(c, (char*)(g_ctx.config[REDIS_IP]), atoi(g_ctx.config[REDIS_IP_PORT]));
            ERROR_LOG(("-->reconnect [%s:%d] err[%d], try again, fd:%d, flags:%d, obuf:%p , reader:%p",
                    (char*)(g_ctx.config[REDIS_IP]), atoi(g_ctx.config[REDIS_IP_PORT]), err, c->fd, c->flags, c->obuf, c->reader));
            return NULL;
        }

        /* Try again later when the context is still not connected. */
        if (!(c->flags & REDIS_CONNECTED))
            return NULL;

        ERROR_LOG(("-->OK [%s:%d], fd:%d, flags:%d, obuf:%p , reader:%p",
            (char*)(g_ctx.config[REDIS_IP]), atoi(g_ctx.config[REDIS_IP_PORT]), c->fd, c->flags, c->obuf, c->reader));

        c->flags |= REDIS_BLOCK;
        rediskeepalive::__SetBlocking(c,c->fd,1);
    /* there is some problem of function redisvCommand()
     * for it will block for reply until there is a response
     * the thread is doing nothing while the network is broken
     * so timeout is set to solve the problem
     */
        struct timeval tval;
        tval.tv_sec=1;
        tval.tv_usec=0;
        redisSetTimeout(c, tval);

        DEBUG_TRACE_REDIS_CTX(c);
        if (cliAuth(c) != REDIS_OK){
            DEBUG_TRACE_REDIS_CTX(c);
            ERROR_LOG(("-->cliAuth failed [%s:%d]", (char*)(g_ctx.config[REDIS_IP]), atoi(g_ctx.config[REDIS_IP_PORT])));

            if ( (REDIS_ERR_IO==c->err) || (REDIS_ERR_EOF==c->err) )
            {
                ERROR_LOG(("-->cliAuth() err[%d][%s], reconnect [%s:%d]...",
                    c->err, c->errstr, (char*)(g_ctx.config[REDIS_IP]), atoi(g_ctx.config[REDIS_IP_PORT])));
                rediskeepalive::__disConnect(c);
                rediskeepalive::__AsyncConnect(c, (char*)(g_ctx.config[REDIS_IP]), atoi(g_ctx.config[REDIS_IP_PORT]));
            }
            return NULL;
        }
    }

    va_list ap;
    void *reply = NULL;
    va_start(ap,format);
    reply = redisvCommand(c,format,ap);
    va_end(ap);
    if (NULL==reply)
    {
        ERROR_LOG(("-->redisvCommand() err[%d][%s]...", c->err, c->errstr));
        if ( (REDIS_ERR_IO==c->err) || (REDIS_ERR_EOF==c->err) )
        {
            ERROR_LOG(("-->__redisBlockForReply() err[%d][%s], reconnect [%s:%d]...",
                c->err, c->errstr, (char*)(g_ctx.config[REDIS_IP]), atoi(g_ctx.config[REDIS_IP_PORT])));
            rediskeepalive::__disConnect(c);
            rediskeepalive::__AsyncConnect(c, (char*)(g_ctx.config[REDIS_IP]), atoi(g_ctx.config[REDIS_IP_PORT]));
        }
    }
    
    return reply;
}

//redisCommandArgv
static void *slave_redis_command_pending(redisContext *c, const char *format, ...) {
    int done = 0;
    int err = 0;

    if (!(c->flags & REDIS_CONNECTED)) {
        if (rediskeepalive::__AsyncHandleConnect(c, err) != REDIS_OK)
        {
            rediskeepalive::__disConnect(c);
            ERROR_LOG(("+++>__disConnect, fd:%d, flags:%d, obuf:%p , reader:%p\n", c->fd, c->flags, c->obuf, c->reader));
            rediskeepalive::__AsyncConnect(c, (char*)(g_ctx.config[REDIS_SLAVE_IP]), atoi(g_ctx.config[REDIS_SLAVE_IP_PORT]));
            ERROR_LOG(("+++>reconnect [%s:%d] err[%d], try again, fd:%d, flags:%d, obuf:%p , reader:%p",
                    (char*)(g_ctx.config[REDIS_SLAVE_IP]), atoi(g_ctx.config[REDIS_SLAVE_IP_PORT]), err, c->fd, c->flags, c->obuf, c->reader));
            return NULL;
        }

        /* Try again later when the context is still not connected. */
        if (!(c->flags & REDIS_CONNECTED))
            return NULL;

        ERROR_LOG(("+++>OK [%s:%d], fd:%d, flags:%d, obuf:%p , reader:%p",
                            (char*)(g_ctx.config[REDIS_SLAVE_IP]), atoi(g_ctx.config[REDIS_SLAVE_IP_PORT]), c->fd, c->flags, c->obuf, c->reader));

        c->flags |= REDIS_BLOCK;
        rediskeepalive::__SetBlocking(c,c->fd,1);
        struct timeval tval;
        tval.tv_sec=1;
        tval.tv_usec=0;
        redisSetTimeout(c, tval);

        if (cliAuth(c) != REDIS_OK)
        {
            ERROR_LOG(("+++>cliAuth failed [%s:%d]", (char*)(g_ctx.config[REDIS_SLAVE_IP]), atoi(g_ctx.config[REDIS_SLAVE_IP_PORT])));
            if ( (REDIS_ERR_IO==c->err) || (REDIS_ERR_EOF==c->err) )
            {
                ERROR_LOG(("+++>cliAuth() err[%d][%s], reconnect [%s:%d]...",
                    c->err, c->errstr, (char*)(g_ctx.config[REDIS_SLAVE_IP]), atoi(g_ctx.config[REDIS_SLAVE_IP_PORT])));
                rediskeepalive::__disConnect(c);
                rediskeepalive::__AsyncConnect(c, (char*)(g_ctx.config[REDIS_SLAVE_IP]), atoi(g_ctx.config[REDIS_SLAVE_IP_PORT]));
            }
            return NULL;
        }
    }

    va_list ap;
    void *reply = NULL;
    va_start(ap,format);
    reply = redisvCommand(c,format,ap);
    va_end(ap);
    if (NULL==reply)
    {
        ERROR_LOG(("+++>redisvCommand() err[%d][%s]...", c->err, c->errstr));
        if ( (REDIS_ERR_IO==c->err) || (REDIS_ERR_EOF==c->err) )
        {
            ERROR_LOG(("+++>__redisBlockForReply() err[%d][%s], reconnect [%s:%d]...",
                c->err, c->errstr, (char*)(g_ctx.config[REDIS_SLAVE_IP]), atoi(g_ctx.config[REDIS_SLAVE_IP_PORT])));
            rediskeepalive::__disConnect(c);
            rediskeepalive::__AsyncConnect(c, (char*)(g_ctx.config[REDIS_SLAVE_IP]), atoi(g_ctx.config[REDIS_SLAVE_IP_PORT]));
        }
    }
    return reply;
}


static void slave_redis_command_nopending(redisContext *c, const char *format, ...) {
    int done = 0;
    int err = 0;

    if (!(c->flags & REDIS_CONNECTED)) {
        if (rediskeepalive::__AsyncHandleConnect(c, err) != REDIS_OK)
        {
            rediskeepalive::__disConnect(c);
            ERROR_LOG(("###>__disConnect, fd:%d, flags:%d, obuf:%p , reader:%p\n", c->fd, c->flags, c->obuf, c->reader));
            rediskeepalive::__AsyncConnect(c, (char*)(g_ctx.config[REDIS_SLAVE_IP]), atoi(g_ctx.config[REDIS_SLAVE_IP_PORT]));
            ERROR_LOG(("###>reconnect [%s:%d] err[%d], try again, fd:%d, flags:%d, obuf:%p , reader:%p",
                    (char*)(g_ctx.config[REDIS_SLAVE_IP]), atoi(g_ctx.config[REDIS_SLAVE_IP_PORT]), err, c->fd, c->flags, c->obuf, c->reader));
            return;
        }

        /* Try again later when the context is still not connected. */
        if (!(c->flags & REDIS_CONNECTED))
            return;

        ERROR_LOG(("###>OK [%s:%d], fd:%d, flags:%d, obuf:%p , reader:%p",
                            (char*)(g_ctx.config[REDIS_SLAVE_IP]), atoi(g_ctx.config[REDIS_SLAVE_IP_PORT]), c->fd, c->flags, c->obuf, c->reader));

        c->flags |= REDIS_BLOCK;
        rediskeepalive::__SetBlocking(c,c->fd,1);
        struct timeval tval;
        tval.tv_sec=1;
        tval.tv_usec=0;
        redisSetTimeout(c, tval);

        if (cliAuth(c) != REDIS_OK)
        {
            ERROR_LOG(("###>cliAuth failed [%s:%d]", (char*)(g_ctx.config[REDIS_SLAVE_IP]), atoi(g_ctx.config[REDIS_SLAVE_IP_PORT])));
            if ( (REDIS_ERR_IO==c->err) || (REDIS_ERR_EOF==c->err) )
            {
                ERROR_LOG(("###>cliAuth() err[%d][%s], reconnect [%s:%d]...",
                    c->err, c->errstr, (char*)(g_ctx.config[REDIS_SLAVE_IP]), atoi(g_ctx.config[REDIS_SLAVE_IP_PORT])));
                rediskeepalive::__disConnect(c);
                rediskeepalive::__AsyncConnect(c, (char*)(g_ctx.config[REDIS_SLAVE_IP]), atoi(g_ctx.config[REDIS_SLAVE_IP_PORT]));
            }
            return;
        }
    }

    /* sync write data until it's done */
    va_list ap;
    va_start(ap,format);
    redisvAppendCommand(c, format, ap);
    va_end(ap);
    do{
        if (redisBufferWrite(c,&done) == REDIS_ERR)
        {
        /*#define EAGAIN      11   Try again */
        /*#define ECONNREFUSED    111  Connection refused */
            ERROR_LOG(("###>redisBufferWrite() %s:%d errno[%d], reconnect...",
                (char*)(g_ctx.config[REDIS_SLAVE_IP]), atoi(g_ctx.config[REDIS_SLAVE_IP_PORT]), errno));
            rediskeepalive::__disConnect(c);
            rediskeepalive::__AsyncConnect(c, (char*)(g_ctx.config[REDIS_SLAVE_IP]), atoi(g_ctx.config[REDIS_SLAVE_IP_PORT]));
            break;
        }
    }while (!done);
}

static void redis_write_log(redisContext *c, const char *format, ...)
{
    int done = 0;
    int err = 0;

    if (!(c->flags & REDIS_CONNECTED)) {
        if (rediskeepalive::__AsyncHandleConnect(c, err) != REDIS_OK)
        {
            rediskeepalive::__disConnect(c);
            ERROR_LOG(("!!!>__disConnect, fd:%d, flags:%d, obuf:%p , reader:%p\n", c->fd, c->flags, c->obuf, c->reader));
            rediskeepalive::__AsyncConnect(c, (char*)(g_ctx.config[REDIS_SLAVE_IP]), atoi(g_ctx.config[REDIS_SLAVE_IP_PORT]));
            ERROR_LOG(("!!!>reconnect [%s:%d] err[%d], try again, fd:%d, flags:%d, obuf:%p , reader:%p",
                    (char*)(g_ctx.config[REDIS_SLAVE_IP]), atoi(g_ctx.config[REDIS_SLAVE_IP_PORT]), err, c->fd, c->flags, c->obuf, c->reader));
            return;
        }

        /* Try again later when the context is still not connected. */
        if (!(c->flags & REDIS_CONNECTED))
            return;

        ERROR_LOG(("!!!>OK [%s:%d], fd:%d, flags:%d, obuf:%p , reader:%p",
                            (char*)(g_ctx.config[REDIS_SLAVE_IP]), atoi(g_ctx.config[REDIS_SLAVE_IP_PORT]), c->fd, c->flags, c->obuf, c->reader));

        c->flags |= REDIS_BLOCK;
        rediskeepalive::__SetBlocking(c,c->fd,1);
        struct timeval tval;
        tval.tv_sec=1;
        tval.tv_usec=0;
        redisSetTimeout(c, tval);

        if (cliAuth(c) != REDIS_OK)
        {
            ERROR_LOG(("!!!>cliAuth failed [%s:%d]", (char*)(g_ctx.config[REDIS_SLAVE_IP]), atoi(g_ctx.config[REDIS_SLAVE_IP_PORT])));
            if ( (REDIS_ERR_IO==c->err) || (REDIS_ERR_EOF==c->err) )
            {
                ERROR_LOG(("!!!>cliAuth() err[%d][%s], reconnect [%s:%d]...",
                    c->err, c->errstr, (char*)(g_ctx.config[REDIS_SLAVE_IP]), atoi(g_ctx.config[REDIS_SLAVE_IP_PORT])));
                rediskeepalive::__disConnect(c);
                rediskeepalive::__AsyncConnect(c, (char*)(g_ctx.config[REDIS_SLAVE_IP]), atoi(g_ctx.config[REDIS_SLAVE_IP_PORT]));
            }
            return;
        }
    }

    va_list ap;
    va_start(ap,format);
    redisvAppendCommand(c, format, ap);
    va_end(ap);

    do{
        if (redisBufferWrite(c,&done) == REDIS_ERR)
        {
        /*#define EAGAIN      11   Try again */
        /*#define ECONNREFUSED    111  Connection refused */
            ERROR_LOG(("!!!>redisBufferWrite() %s:%d errno[%d], reconnect...",
                (char*)(g_ctx.config[REDIS_SLAVE_IP]), atoi(g_ctx.config[REDIS_SLAVE_IP_PORT]), errno));
            rediskeepalive::__disConnect(c);
            rediskeepalive::__AsyncConnect(c, (char*)(g_ctx.config[REDIS_SLAVE_IP]), atoi(g_ctx.config[REDIS_SLAVE_IP_PORT]));
            break;
        }
    }while (!done);
}


#define IFELSESIGNEMD5P( f, t ) 	( f && f[0] != 0 ) ? (t) : (""), ( f && f[0] != 0 ) ? f : (""),
#define  ELAPSED_END_TIME 	

#ifdef TXT_LOG
extern int g_log_empty;
extern sem_t g_log_sem;
#endif

extern struct ais_context  g_ctx;

//add by chenhuizhong
void near_by_width1(int width, int high, char **res)
{
    int i;
    char* str;
    char* cp = NULL;
    int min = 0x7fffffff;
    int pos = 0;
    int tmp;
    int w1, w2, h1, h2, per;

    if (!width || !high)
    {
        *res = g_ctx.img_size[0];

        return;
    }

    per = width * 100 / high; 
    for (i = 0; i < g_ctx.img_size_len; i++) 
    {
        str = g_ctx.img_size[i];
        if ((cp = strchr(str, 'X')) != NULL)
        {
            tmp = abs(atoi(str)- width);
            if (min > tmp)
            {
                min = tmp;
                pos = i;
            }else if (min == tmp)
            {
                str = g_ctx.img_size[pos];
                cp = strchr(str, 'X');
                w1 = atoi(str); 
                h1 = atoi(cp + 1);

                str = g_ctx.img_size[i];
                cp = strchr(str, 'X');
                w2 = atoi(str); 
                h2 = atoi(cp + 1);

                if (abs(w1 * 100 / h1 - per) > abs(w2 * 100 / h2 - per)) pos = i;
            }
        }
    }
    
    *res = g_ctx.img_size[pos];

}

//add by chenhuizhong
void near_by_width2(int width, int high , redisReply * hsImageSize, char **res)
{
    int i;
    char* str;
    char* cp = NULL;
    int min = 0x7fffffff;
    int pos = 0;
    int tmp;
    int w1, w2, h1, h2, per;

    if (!width || !high)
    {
        *res = hsImageSize->element[0]->str;

        return;
    }

    per = width * 100 / high; 
    for (i = 0; i < hsImageSize->elements; i++) 
    {
        str = hsImageSize->element[i]->str;
        if ((cp = strchr(str, 'X')) != NULL)
        {
            tmp = abs(atoi(str)- width);
            if (min > tmp)
            {
                min = tmp;
                pos = i;
            }else if (min == tmp)
            {
                str = hsImageSize->element[pos]->str;
                cp = strchr(str, 'X');
                w1 = atoi(str); 
                h1 = atoi(cp + 1);

                str = hsImageSize->element[i]->str;
                cp = strchr(str, 'X');
                w2 = atoi(str); 
                h2 = atoi(cp + 1);

                if (abs(w1 * 100 / h1 - per) > abs(w2 * 100 / h2 - per)) pos = i;
            }
        }
    }

    *res = hsImageSize->element[pos]->str;
}




void DeductMoney(struct ais_connection *conn , char *strOrderID , char *jstrAdId , char *t , int h )
{
	char * tp = NULL;
	char *strPutInModelType = "";
	int flag=0;
	redisReply * sPutInModelType=(redisReply*)slave_redis_command_pending( conn->sredisctx , "Get ad:%s:putinmodeltype" , jstrAdId);

	if ( sPutInModelType && ( sPutInModelType->type == REDIS_REPLY_STRING ) )
	{
		tp = sPutInModelType->str;
		if ((size_t)(conn->adparams_buf+conn->buf_size - conn->adparams_buf_last) <strlen(tp) +2)
		{
			DEBUG_INFO(("ERROR ad params buf is full"));
		}
		strPutInModelType = conn->adparams_buf_last;//atoi(var);
		conn->adparams_buf_last = my_strlcpy( strPutInModelType , tp , strlen( tp ) );

	}

	freeReplyObject(sPutInModelType);

	char *strPrice = "";
	redisReply * sPrice = (redisReply*)slave_redis_command_pending( conn->sredisctx , "Get ad:%s:price" , jstrAdId);

	if ( sPrice && ( sPrice->type == REDIS_REPLY_STRING ) )
	{
		tp = sPrice->str;
		if ((size_t)(conn->adparams_buf+conn->buf_size - conn->adparams_buf_last) <strlen(tp) +2)
		{
			DEBUG_INFO(("ERROR ad params buf is full"));
		}
		strPrice = conn->adparams_buf_last;//atoi(var);
		conn->adparams_buf_last = my_strlcpy( strPrice , tp , strlen( tp ) );

	}

	freeReplyObject(sPrice);

	double dlUnallocated_Balance;
	double dlPrice = atof(strPrice) ;
	send_redis_command(conn->redisctx, "HINCRBYFLOAT usedmoney:%s:day %s %s",strOrderID, t, strPrice);
	if (*strPutInModelType == '1')
	{			
		send_redis_command(conn->redisctx, "HINCRBYFLOAT usedmoney:%s:%s:day %s %s",strOrderID ,jstrAdId, t, strPrice);
	}
	else if (*strPutInModelType == '2')
	{
		send_redis_command(conn->redisctx, "HINCRBYFLOAT usedmoney:%s:hours:%s  %02d %s" , strOrderID, t, h, strPrice);
		send_redis_command(conn->redisctx, "HINCRBYFLOAT usedmoney:%s:%s:hours:%s  %02d %s" , strOrderID ,jstrAdId, t, h, strPrice);
	}
	redisReply *hsNotDay=(redisReply*)slave_redis_command_pending(conn->sredisctx,"smembers notbudgetsday:%s",strOrderID);
	char * p=NULL;
	if (!hsNotDay || hsNotDay->type!=REDIS_REPLY_ARRAY){
		flag=0;}
	else{
		for(int k=0;k<hsNotDay->elements;k++){
			if ((p=strstr(hsNotDay->element[k]->str,t))){
				flag=1;
				break;
			}
		}
	}
    freeReplyObject(hsNotDay);

	if (flag==1){
		redisReply * sUnallocated_Balance = (redisReply *)slave_redis_command_pending( conn->sredisctx , "HGET order:%s:money unallocated_balance" , strOrderID );
		if ( ( !sUnallocated_Balance || sUnallocated_Balance->type != REDIS_REPLY_STRING ) )
		{
			freeReplyObject( sUnallocated_Balance );
			redisReply * sUnallocated_Balance = (redisReply *)slave_redis_command_pending( conn->sredisctx , "HGET order:%s:money unallocated" , strOrderID );
			if ( ( !sUnallocated_Balance || sUnallocated_Balance->type != REDIS_REPLY_STRING ) )
			{
				dlUnallocated_Balance = 0;
			}else{
				send_redis_command(conn->redisctx, "HINCRBYFLOAT order:%s:money unallocated_balance %f",strOrderID,-dlPrice);
			}
			freeReplyObject( sUnallocated_Balance );
		}else{
			send_redis_command(conn->redisctx, "HINCRBYFLOAT order:%s:money unallocated_balance %f",strOrderID,-dlPrice);
			freeReplyObject( sUnallocated_Balance );
		}
	}

	send_redis_command(conn->redisctx, "HINCRBYFLOAT order:%s:money balance %f",strOrderID,-dlPrice);
}



int  init_adsdk()
{
#if 0
    hadkey = htable_create(100);
    hsdkver = htable_create(10000);
    hadplatformprice = htable_create(1000);
    const char *jstr;
    struct AdPlatformProvider *app = NULL;
    char *unixpath = NULL ;
    redisContext *redisctx = NULL ;
    struct timeval timeout = { 1, 500000 }; // 1.5 seconds
    if ( g_ctx.config[REDIS_IP] != NULL )
    {
        if ( (unixpath = strstr(g_ctx.config[REDIS_IP] , "unix:") ) != NULL )
        {	
            redisctx=redisConnectUnixWithTimeout(unixpath+sizeof("unix:")-1, timeout) ;
            if (redisctx->err) {  

                return -1;
            }

        }
        else
        {
            redisctx= redisConnectWithTimeout((char*)(g_ctx.config[REDIS_IP]), 6379, timeout);
            if (redisctx->err) {  

                return -1;
            }

        }
    }
    else
    {
        redisctx= redisConnectWithTimeout((char*)"127.0.0.1", 6379, timeout);
        if (redisctx->err) {  

            return -1 ;
        }
    }
    if (cliAuth(redisctx) != REDIS_OK) return REDIS_ERR;


    int i = 0 ;
    redisReply *hgetreply = NULL ;
    struct json_object *new_obj = NULL ;
    hgetreply = slave_redis_command_pending( redisctx , "HGETALL adplatform" ) ;
    if ( !hgetreply || hgetreply->type != REDIS_REPLY_ARRAY ) {
        //AIS_ERROR_FOMAT( aap , "SDKID error" , ais_error_500 );
        fprintf(stderr, "slave_redis_command_pending HGETALL adplatform \n");
        freeReplyObject( hgetreply );
        return -1;
    }


    for ( i = 0; i < hgetreply->elements; i++ ) {
        DEBUG_TRACE(("%u) %s\n", i, hgetreply->element[i]->str));
    }

    for ( i = 1; i < hgetreply->elements; i += 2 ) {
        /*must do not have \u  4hexchar  UTF8 bugs*/
        new_obj = json_tokener_parse(hgetreply->element[i]->str);
        if ( is_error( new_obj ) || json_type_object != json_object_get_type( new_obj ) ) {
            freeReplyObject( hgetreply );
            return -1;
        }
        app = calloc(1,sizeof(struct AdPlatformProvider));
        SET_ADSDK_FORMJSON("PlatformProviderID",PlatformProviderID);
        SET_ADSDK_FORMJSON("ShowPutInPrice",ShowPutInPrice);
        SET_ADSDK_FORMJSON("ClickPutInPrice",ClickPutInPrice);
        SET_ADSDK_FORMJSON("Key",Key);
        SET_ADSDK_FORMJSON("SecretKey",SecretKey);
        SET_ADSDK_FORMJSON("CallbackSource",CallbackSource); 
        htable_enter( hadkey, hgetreply->element[i-1]->str, (char *)app);
        json_object_put(new_obj);/*FIX ME free it*/
    }
    freeReplyObject(hgetreply);/*free it FIX ME*/
    redisFree(redisctx);
#endif
    return 0 ;
}

//struct    ais_adsdk g_as;
const char * imgsize =  "{$ImgSize$}";
const char * imgdomain = "{$ImgDomain$}";
const char  sessionid[] = "{$SessionId$}";
extern char	tagchinautf8[];
const char base[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/=";   
char* base64_encode(const char* data, int data_len);   
char *base64_decode(const char* data, int data_len);   
static char find_pos(char ch);   
char * UrlBase64(char * sessionid,char *  format,char * url, char * Adplatformid);
void head_tail_swap(char *buf, int buf_len, int swap_len);
void replace_encode_char(char * buf, int buf_len, int step_len);
void replace_decode_char(char * buf, int buf_len, int step_len);


int split(char * str,char n,char  **dest )
{
 int num=0;
 char * p=str;
 char * ptr=p;
 char * tmp;
 while ((tmp=strchr(ptr,n))!=NULL)
 	{
 	  *tmp='\0';
   dest[num]=ptr;
	  ptr=tmp+1;
	  num++;
 	}
   dest[num]=ptr;
  
   return num+1;
}

void  replace_char(char * str,const char * erase,const char * rep)
{
    char  str1[400]={0};
    char * tmp=str1;
    char *p=str;
    char *q=str;
    while((q = strstr(p,erase))){
        strncpy(tmp,p,q-p);
        tmp+=q-p;
        p+=q-p+1;
        if (rep){
            strncpy(tmp,rep,strlen(rep));
            tmp+=strlen(rep);
        }
    }
    strcpy(tmp,p);
    if (strlen(str1)>strlen(str)){
        str=(char *)realloc(str,strlen(str1)+1);
        strcpy(str,str1);
    }
    else{
        strcpy(str,str1);
    }             
}

#define CRLF  ""

/* add by chenhuizhong */
void init_logparams(ais_logparams_t *log, struct ais_adparams *aap, struct AdPlatformProvider *app, struct tm *date)
{
    log->category = aap->Category;
    log->session_id = "";
    log->aid = "0";
    log->pp_id = IFELSE(app->PlatformProviderID, const_cast<char *>(""));
    log->sdk_ver = IFELSE(aap->SdkVersion,unknown);
    log->key = app->Key;
    log->putin_type = ""; 
    log->request_mode = "1";
    log->adp_price = "";
    log->year = 1900 + date->tm_year;
    log->month = 1 + date->tm_mon;
    log->mday = date->tm_mday;
    log->hour = date->tm_hour;
    log->min = date->tm_min;
    log->sec = date->tm_sec; 
    log->ip = aap->Ip;
    log->app_id = aap->AppId;
    log->app_name = IFELSE(aap->AppName, const_cast<char *>(""));
    log->uuid = aap->Uuid;
    log->device_num = IFELSE(aap->DeviceNum,const_cast<char *>(""));
    log->clinet = aap->Client;
    log->os_ver = aap->Os_version;
    log->density = IFELSE(aap->Density, const_cast<char *>(""));
    log->pw = IFELSE(aap->Pw, const_cast<char *>(""));
    log->ph = IFELSE(aap->Ph, const_cast<char *>(""));
    log->lg = IFELSE(aap->Long ,const_cast<char *>(""));
    log->lat = IFELSE( aap->Lat ,const_cast<char *>(""));
    log->opreator = aap->Operator; 
    log->net = aap->Net;
    log->ad_space_type = aap->AdSpaceType;
    log->device_type = aap->DeviceType;
    log->aw = aap->Aw;
    log->ah = aap->Ah;
    log->format = aap->Format;
    log->level = 1;
    log->is_cheat = "0";
    log->is_billing = "0";
    log->callback_source = app->CallbackSource;
    log->timestamp = aap->TimeStamp;

    if (NULL == aap->AdNum)
    {
        log->adnum = 1;
    }
    else
    {
        log->adnum = atoi(aap->AdNum);
        log->adnum = log->adnum > 0 ? log->adnum : 1;
    }
}

/* add by chenhuizhong */
void log_handler(struct ais_connection *conn, ais_logparams_t *log)
{
#ifdef TXT_LOG
    if (*(conn->ctx->config[OPEN_WRITE_LOG]) == 'y')
    {
        char *str = (char *)eg_cq_get_cur_buf(conn->cq);
        snprintf(str, LOG_CONTENT_LEN, "%s,{AdvertisersID},{ADOrderID},%s,%s,%s,%s," \
                    "%s,%s,{ADPrice},%s,%d-%02d-%02d %02d:%02d:%02d,%s,%s,%s,%s,%s," \
                    "%s,%s,%s,%d,%d,%s,%s,{ProName},{CityName},%s," \
                    "{ISPName},%s,{NetName},%s,%s,%s,{AdSpaceTypeName},%s,%d,%s", 
                    //		{District},{PayMode},"0","0","0","0"
                    log->session_id, log->aid, log->pp_id, log->sdk_ver, log->key, log->putin_type, 
                    log->request_mode, log->adp_price, log->year, log->month, log->mday, log->hour, 
                    log->min, log->sec, log->ip, log->app_id, log->app_name, log->uuid, log->device_num, 
                    log->clinet, log->os_ver, log->density, atoi(log->pw), atoi(log->ph), log->lg, log->lat, 
                    log->opreator, log->net, log->is_cheat, log->is_billing, log->ad_space_type, 
                    log->device_type, log->level, log->category);
        eg_cq_push(conn->cq, str);
        if (g_log_empty) 
        {
            g_log_empty = 0;
            sem_post(&g_log_sem);
        }
    }
#endif
}

/* add by chenhuizhong */
int sdkver_handler(char *sdkver, char **buf, const char **ver_array, size_t *rs)
{
    const char *parray[MAX_DEMli] ={NULL,};
    char * sClickEffect = NULL;
    const char * tp =NULL;
    const char **tempparray = ver_array;
    size_t ndemlisize;
    size_t ndemli;

    ndemli = str_split( sdkver, '|' , parray , MAX_DEMli );

    if (ndemli != 2) return AD_NOFIND;

    if (*parray[0] == '0') return SDKVERSION_DISABLED; 

    if (IsNullOrEmpty(parray[1])) return AD_NOFIND;

    ndemli = 0;
    sClickEffect = *buf;
    *buf = my_strlcpy( sClickEffect , parray[1], strlen( parray[1]));
    ndemlisize = str_split( sClickEffect , ',' , parray , MAX_DEMli );

    DEBUG_TRACE(("sClickEffect %s\n", sClickEffect));
    DEBUG_TRACE(("sClickEffect ndemlisize = %d\n", (int)ndemlisize));

    for(int i = 0; i < ndemlisize; i++)
    {
        DEBUG_TRACE((" org array[%d] = %s",(int)i, parray[i]));

        if ((tp=ConvertSupportType(atoi(parray[i])))!=NULL)
        {
            tempparray[++ndemli] = tp;
            DEBUG_TRACE((" tempparray[%d] = %s", (int)ndemli, tempparray[ndemli]));
        }
    }
    *rs = ndemli;

    if (!ndemli) return AD_NOFIND;

    return SUCCESS; 
}

/* add by chenhuizhong */
void init_map(eg_hashtable_t *ht, ht_node_pool_t *np, redisReply *r)
{
    int i;
    char * str;
    int len = r->elements;
    eg_ht_bucket_t *b;
    unsigned long key;
    
    for(i = 0; i < len; ++i)
    {
        str = r->element[i]->str;
        b = get_ht_bucket(np);
        if (NULL==b){
            ERROR_LOG(("pool->cur:%d, pool->size:%d", np->cur, np->size));
            return;
        }
        key = atoi(str);
        b->key = (void *)key;
        b->val = (void *)str;
        ht_insert_bucket(ht, b);
    }
}

/* add by chenhuizhong */
void  get_inter(eg_hashtable_t **maps, int len)
{
    int i;
    eg_ht_iterator_t *it;

    for (it = eg_ht_begin(maps[0]); it != NULL; )
    {
        for (i = 1; i <= len; ++i)
        {
            if (!eg_ht_find(maps[i], it->key)) 
            {
                ht_erase_bucket(maps[0], &it);
                break;
            }
        }

        if (i > len) eg_ht_next(&it);/* if delete iter, iter auto point next */
    }
}
int array_reply_empty(redisReply *reply)
{
    return !reply || reply->type != REDIS_REPLY_ARRAY || 0 == reply->elements;
}

/* add by chenhuizhong */
void get_ad(const char* req_session, struct ais_connection *conn, ais_logparams_t *log, const char **parray, size_t ndemli,struct tm* date)
{
    struct timeval start_time;
    struct timeval end_time;
    long cost_time;

    const char *tp;
    char temp_private[256];
    char buf[256] = {0};
    char bufmd5[33] = {0};
    int i, j;
    size_t temp;
    const char *tags[MAX_DEMli] ={NULL};
    const char **tempparray = parray;
    redisContext *c = conn->sredisctx;
    redisReply *sunionreply = NULL;
    redisReply *sinterreply = NULL;
    redisReply *existsreply = NULL;
    redisReply *ipreply = NULL ;
    redisReply  *lastHistoryreply = NULL;
    redisReply *ipbytelist = NULL;
    redisReply *tempCoverageReply = NULL;
    redisReply *appid_reply = NULL;
    redisReply *apptype_reply = NULL;
    redisReply *uuid_reply = NULL;
    redisReply *localapp_reply = NULL;
    redisReply *country_reply = NULL;
    ht_node_pool_t *np = conn->node_pool; 
    ht_map_pool_t *mp = conn->map_pool;
    eg_hashtable_t *maps[MAP_POOL_SIZE]; 
    eg_hashtable_t *tmp;
    eg_ht_iterator_t *it;
    eg_ht_bucket_t *b;
    unsigned long key;
    int map_len = -1;
    int ii;
    char *p_4_strsep;
    char *s2;
    int empty;
#ifdef DEBUGMODE
    int debug = 0;
    redisReply *debugip_reply = NULL;
#endif

    /*if required parameter  exits?*/
    if((NULL==log->pp_id)
        ||(NULL==log->app_id)
        || (NULL==log->category)
        || (NULL==log->uuid)
        || (NULL==log->clinet)
        || (NULL==log->opreator)
        || (NULL==log->ad_space_type)
        || (NULL==log->device_type))
    {
        ERROR_LOG(("param invalid, pp_id:%s, app_id:%s, category:%s, uuid:%s, clinet:%s, opreator:%s, ad_space_type:%s, device_type:%s",
            log->pp_id, log->app_id, log->category, log->uuid, log->clinet, log->opreator,log->ad_space_type,log->device_type));
            out_string(conn->c, "required parameter missing");
        return;
    }
    
    do
    {
        gettimeofday(&start_time, NULL);

        tags[temp = 0] = "SINTER"; 
        if (1 == ndemli)
        {
            tags[++temp] = tempparray[1];
        }
        else if (ndemli > 1)
        {
            tempparray[0] = "SUNION";
            redisAppendCommandArgv(c, ndemli + 1, tempparray, NULL);/* sunionreply */
        }

        if ((tp=ConvertClient(atoi(log->clinet)))!=NULL){tags[++temp] = tp;}
        if ((tp=ConvertOperator(atoi(log->opreator)))!=NULL){tags[++temp] = tp;}
        if ((tp=ConvertNet(atoi(log->net)))!=NULL){tags[++temp] = tp;}
        if ((tp=ConvertPutintime(buf,date))!=NULL) {tags[++temp] = tp;}
        if ((tp=ConvertAdSpaceType(atoi(log->ad_space_type)))!=NULL){tags[++temp] = tp;}
        if ((tp=ConvertDeviceType(atoi(log->device_type)))!=NULL){tags[++temp] = tp;}
        redisAppendCommandArgv(c, temp + 1, tags, NULL);/* sinterreply */


        snprintf( temp_private , sizeof temp_private , "temp:private:%s" , log->pp_id);
        redisAppendCommand(c, "sunion %s temp:private:all", temp_private);/* existsreply */


        snprintf( buf, sizeof(buf), "%llu", (unsigned long long)((unsigned long long)(address_to_long(log->ip))*10000 + 1 ) );
        memset(tempparray , 0 , MAX_DEMli * sizeof(const char*));	
        tempparray[i = 0] = "ZRANGEBYSCORE";
        tempparray[++i] = iparea;
        tempparray[++i] = buf;
        tempparray[++i] = "9223372036854775807";
        tempparray[++i] = "LIMIT";
        tempparray[++i] = "0";
        tempparray[++i] = "1";
        redisAppendCommandArgv(c, i + 1, tempparray, NULL);/* ipreply */

        ais_md5_( bufmd5 , "Uuid=" , log->uuid, "PlatformId=" , log->pp_id, NULL );
        redisAppendCommand(c, "GET mobile:%s:lastrequest", bufmd5 );/* lastHistoryreply */

        redisAppendCommand(c, "SMembers %s" , tagchinautf8);/* ipbytelist */ 

        snprintf( buf, sizeof(buf), "app:lib:%s", log->pp_id);
        redisAppendCommand(c, "Hget %s %s" , buf, log->app_id);/* app_id */ 

        redisAppendCommand(c, "SMembers %s" , ConvertCategoryType(log->category));/* app_type */ 
        redisAppendCommand(c, "SMembers tag:uuid:%s" , log->uuid);/* uuid */ 
#ifdef DEBUGMODE
        redisAppendCommand(c, "SMembers debugip");/* debugip */ 
#endif

        empty = 0;
        if (ndemli > 1 && REDIS_OK != redisGetReply(c, (void**)&sunionreply)) 
        {  
            AIS_FREE_REPLYOBJECT(sunionreply)
        }else if (ndemli > 1)
        {
            empty = array_reply_empty(sunionreply);
            if (!empty)
            {
                maps[++map_len] = get_ht_map(mp);
                init_map(maps[map_len], np, sunionreply);
            }
        }

        if (REDIS_OK != redisGetReply(c, (void**)&sinterreply)) 
        {  
            AIS_FREE_REPLYOBJECT(sinterreply)
        }else if (!empty)
        {
            empty = array_reply_empty(sinterreply);
            if (!empty)
            {
                maps[++map_len] = get_ht_map(mp);
                init_map(maps[map_len], np, sinterreply);
            }
        }

        if (REDIS_OK != redisGetReply(c, (void**)&existsreply)) 
        {  
            AIS_FREE_REPLYOBJECT(existsreply)
        }else if (!empty)
        {
            empty = array_reply_empty(existsreply);
            if (!empty)
            {
                maps[++map_len] = get_ht_map(mp);
                init_map(maps[map_len], np, existsreply);
            }
        }

        if (REDIS_OK != redisGetReply(c, (void**)&ipreply)) 
        {  
            AIS_FREE_REPLYOBJECT(ipreply)
            ipreply = NULL;
        }

        if (REDIS_OK != redisGetReply(c, (void**)&lastHistoryreply)) 
        {  
            AIS_FREE_REPLYOBJECT(lastHistoryreply)
        }

        if (REDIS_OK != redisGetReply(c, (void**)&ipbytelist)) 
        {  
            AIS_FREE_REPLYOBJECT(ipbytelist)
        }

        if (REDIS_OK != redisGetReply(c, (void**)&appid_reply)) 
        {  
            AIS_FREE_REPLYOBJECT(appid_reply)
        }

        if (REDIS_OK != redisGetReply(c, (void**)&apptype_reply)) 
        {  
            AIS_FREE_REPLYOBJECT(apptype_reply)
        }
        if (REDIS_OK != redisGetReply(c, (void**)&uuid_reply)) 
        {  
            AIS_FREE_REPLYOBJECT(uuid_reply)
        }
#ifdef DEBUGMODE
        if (REDIS_OK != redisGetReply(c, (void**)&debugip_reply)) 
        {  
            AIS_FREE_REPLYOBJECT(debugip_reply)
        }else
        {
            j = debugip_reply->elements;

            for(i = 0; i < j; ++i)
            {
                tp = debugip_reply->element[i]->str;
                if (!strcasecmp(log->ip, tp))
                {
                    debug = 1;
                    break;
                }
            }
            AIS_FREE_REPLYOBJECT(debugip_reply)
        }
#endif

        gettimeofday(&end_time, NULL);
        cost_time = (end_time.tv_sec - start_time.tv_sec)* 1000000 + end_time.tv_usec - start_time.tv_usec;
        COST_TIME_ERROR_LOG( ("=====>[%s] get_ad redisAppendCommand cost_time = [%ld], %lu:%lu", req_session, cost_time, end_time.tv_sec, end_time.tv_usec));

        if(empty) break;

        char *arr[MAX_DELIM_NUM];
        int len = -1;

        if ((NULL==appid_reply)
            || ((appid_reply!=NULL)&&( appid_reply->type != REDIS_REPLY_STRING)) )
        {
            empty = 1;
        }else
        {
    /*
    hget "app:lib:30" 1000
    the result of appid_reply,eg:
        "1|\xe6\x87\x92\xe4\xba\xba\xe5\x90\xac\xe4\xb9\xa6|1410"
        "1,10,4|\xe5\xa5\x87\xe7\x8b\x97\xe5\xbd\xb1\xe8\xa7\x86|1797"
        127.0.0.1:7380> hget app:lib:13  bf_6055ACEA-6534-4567-8099-C3D57A52DD95
        "1,2|A&B|1846"
    */
            p_4_strsep = appid_reply->str;
            while ( (len<MAX_DELIM_NUM-1) && (arr[++len] = strsep(&p_4_strsep, "|")));
            if (len != 3)
            {
                empty = 1;
            }else
            {
                empty = 0;
            }
        }

        if (empty)
        {
            empty =  array_reply_empty(apptype_reply);
            if (!empty)
            {
                maps[++map_len] = get_ht_map(mp);
                init_map(maps[map_len], np, apptype_reply);
            }
        }else
        {
            gettimeofday(&start_time, NULL);

            localapp_reply = (redisReply *)slave_redis_command_pending(c, "SMembers tag:localapp:%s", arr[2]);

            gettimeofday(&end_time, NULL);
            cost_time = (end_time.tv_sec - start_time.tv_sec)* 1000000 + end_time.tv_usec - start_time.tv_usec;
            COST_TIME_ERROR_LOG( ("=====>[%s] get_ad SMembers tag:localapp cost_time = [%ld], %lu:%lu", req_session, cost_time, end_time.tv_sec, end_time.tv_usec));

            empty =  array_reply_empty(localapp_reply);
            if (empty)
            {
                if (apptype_reply) {
                    AIS_FREE_REPLYOBJECT(apptype_reply)
                }

                const char *type_arr[MAX_DELIM_NUM];

                len = -1;
                p_4_strsep = arr[0];
                while ((len<MAX_DELIM_NUM-1) &&(type_arr[++len] = strsep(&p_4_strsep, ",")));
                if (len>0)
                {
                    for (i = len; i > 0; --i)
                    {
                        type_arr[i] = ConvertCategoryType(type_arr[i - 1]);
                    }
                    type_arr[0] = "SUNION";

                    gettimeofday(&start_time, NULL);

                    apptype_reply = (redisReply *)redisCommandArgv(c, len + 1, type_arr, NULL);

                    gettimeofday(&end_time, NULL);
                    cost_time = (end_time.tv_sec - start_time.tv_sec)* 1000000 + end_time.tv_usec - start_time.tv_usec;
                   COST_TIME_ERROR_LOG( ("=====>[%s] get_ad SUNION CategoryType cost_time = [%ld], %lu:%lu", req_session, cost_time, end_time.tv_sec, end_time.tv_usec));

                    empty =  array_reply_empty(apptype_reply);
                    if (!empty)
                    {
                        maps[++map_len] = get_ht_map(mp);
                        init_map(maps[map_len], np, apptype_reply);
                    }
                }
            }else
            {
                maps[++map_len] = get_ht_map(mp);
                init_map(maps[map_len], np, localapp_reply);
            }
        }

        if (empty) break;
        gettimeofday(&start_time, NULL);

        for(i = 1; i <= map_len; ++i)
        {
            if (eg_ht_elements(maps[0]) > eg_ht_elements(maps[i]))
            {
                tmp = maps[i];
                maps[i] = maps[0];
                maps[0] = tmp;
            }
        }
        get_inter(maps, map_len);
        if (eg_ht_empty(maps[0])) break;
        if (!array_reply_empty(uuid_reply))
        {
            for(i = 0; i < uuid_reply->elements; ++i)
            {
                key = atoi(uuid_reply->element[i]->str);
                ht_remove_bucket(maps[0], (void *)key);
            }
        }
        if (eg_ht_empty(maps[0])) break;

        maps[++map_len] = maps[1];
        maps[1] = get_ht_map(mp);
        gettimeofday(&end_time, NULL);
        cost_time = (end_time.tv_sec - start_time.tv_sec)* 1000000 + end_time.tv_usec - start_time.tv_usec;
        COST_TIME_ERROR_LOG( ("=====>[%s] get_ad get_inter cost_time = [%ld], %lu:%lu", req_session, cost_time, end_time.tv_sec, end_time.tv_usec));
#ifdef DEBUGMODE
        if (!debug)
        {
#endif
        if ((log->lg && log->lg[0] != 0) && (log->lat&&log->lat[0] != 0))
        {
            double longg = 0 ,lat = 0;
            gettimeofday(&start_time, NULL);

            longg = atof(log->lg);
            lat = atof(log->lat);
            tempparray[j = 0] = "HMGET";
            tempparray[++j] = adcoverage;

            for (it = eg_ht_begin(maps[0]); it != NULL; eg_ht_next(&it))
            {
                if(j >= MAX_DEMli-1)
                {
                    ERROR_LOG(("Dangerous!!!,ais_get maps[0].elements=%lu, tempparray[].size=%d", maps[0]->elements, MAX_DEMli));
                    break;
                }
                tempparray[++j] = (char *)it->val;
            }
            gettimeofday(&end_time, NULL);
            cost_time = (end_time.tv_sec - start_time.tv_sec)* 1000000 + end_time.tv_usec - start_time.tv_usec;
            COST_TIME_ERROR_LOG( ("=====>[%s] get_ad make cmd HMGET adcoverage cost_time = [%ld], %lu:%lu", req_session, cost_time, end_time.tv_sec, end_time.tv_usec));

            gettimeofday(&start_time, NULL);

            tempCoverageReply =  (redisReply *)redisCommandArgv(c, j + 1 , tempparray , NULL );

            gettimeofday(&end_time, NULL);
            cost_time = (end_time.tv_sec - start_time.tv_sec)* 1000000 + end_time.tv_usec - start_time.tv_usec;
            COST_TIME_ERROR_LOG( ("=====>[%s] get_ad HMGET adcoverage cost_time = [%ld], %lu:%lu", req_session, cost_time, end_time.tv_sec, end_time.tv_usec));
            gettimeofday(&start_time, NULL);

            if (tempCoverageReply && tempCoverageReply->type == REDIS_REPLY_ARRAY)
            {
                double coverlong ,coverlat,distanse;

                j = tempCoverageReply->elements;
                for ( i = 0; i < j; i++ ) 
                {
                    ii = -1;
                    p_4_strsep =  tempCoverageReply->element[i]->str;

                    if (!IsNullOrEmpty(p_4_strsep))
                    {
                        while((ii<MAX_DELIM_NUM-1) &&(s2 = strsep(&p_4_strsep, ",")))
                        {
                            arr[++ii] = s2;
                        }

                        if (3 == ii)
                        {
                            coverlong = atof(arr[1]);
                            coverlat = atof(arr[2]);
                            distanse = atof(arr[3]);
                            if (abs(GetDistance(longg, lat, coverlong, coverlat)) <= distanse)
                            {
                                b = get_ht_bucket(np);
                                if (NULL==b){
                                    ERROR_LOG(("pool->cur:%d, pool->size:%d", np->cur, np->size));
                                } else {
                                    key = atoi(arr[0]);
                                    b->key = (void *)key;
                                    b->val = (void *)arr[0];
                                    ht_insert_bucket(maps[1], b);
                                }
                            }
                        }
                    }
                }
            }
            gettimeofday(&end_time, NULL);
            cost_time = (end_time.tv_sec - start_time.tv_sec)* 1000000 + end_time.tv_usec - start_time.tv_usec;
            COST_TIME_ERROR_LOG( ("=====>[%s] get_ad lat_lon adcoverage cost_time = [%ld], %lu:%lu", req_session, cost_time, end_time.tv_sec, end_time.tv_usec));
        }

        if (eg_ht_empty(maps[1])) 
        {
            if ( ipreply && ipreply->type == REDIS_REPLY_ARRAY &&  ipreply->elements > 0 )
            {
                char area1[48];
                char area2[72];
                char area3[96];
                ii = -1;

                /*prase str*/
                p_4_strsep = ipreply->element[0]->str;
                while(ii < 3 && (s2 = strsep(&p_4_strsep, ",")))
                {
                    arr[++ii] = s2;
                }

                if ( ii > -1 ) 
                {
                    snprintf(area1, sizeof(area1), "tag:address:city:%s", arr[0]);
                    snprintf(area2, sizeof(area2), "tag:address:city:%s%s", arr[0], arr[1]);
                    snprintf(area3, sizeof(area3), "tag:address:city:%s%s%s", arr[0], arr[1], arr[2]);
                    memset( tempparray , 0 , MAX_DEMli * sizeof( const char* ) );	
                    tempparray[j = 0] = "SUNION";
                    tempparray[++j] = area1;
                    tempparray[++j] = area2;
                    tempparray[++j] = area3;
                    tempparray[++j] = tagchinautf8;
                    AIS_FREE_REPLYOBJECT( ipbytelist )

                    gettimeofday(&start_time, NULL);

                    ipbytelist = (redisReply *)redisCommandArgv(c, j + 1 , tempparray , NULL );

                    gettimeofday(&end_time, NULL);
                    cost_time = (end_time.tv_sec - start_time.tv_sec)* 1000000 + end_time.tv_usec - start_time.tv_usec;
                    COST_TIME_ERROR_LOG( ("=====>[%s] get_ad SUNION area1 tagchinautf8 cost_time = [%ld], %lu:%lu", req_session, cost_time, end_time.tv_sec, end_time.tv_usec));
                }

                if ( ipbytelist && ipbytelist->type == REDIS_REPLY_ARRAY ) 
                {
                    gettimeofday(&start_time, NULL);
                    init_map(maps[1], np, ipbytelist);
                    if (eg_ht_elements(maps[0]) > eg_ht_elements(maps[1]))
                    {
                        tmp = maps[0];
                        maps[0] = maps[1];
                        maps[1] = tmp;
                    }
                    get_inter(maps, 1); 
                    gettimeofday(&end_time, NULL);
                    cost_time = (end_time.tv_sec - start_time.tv_sec)* 1000000 + end_time.tv_usec - start_time.tv_usec;
                    COST_TIME_ERROR_LOG( ("=====>[%s] get_ad get_inter area1 tagchinautf8 cost_time = [%ld], %lu:%lu", req_session, cost_time, end_time.tv_sec, end_time.tv_usec));
                }
            }
            else{
            /*when the ip database is updating*/
                    gettimeofday(&start_time, NULL);

                    country_reply = (redisReply *)redisAppendCommand(c, "SMembers %s" , tagchinautf8);

                    if (REDIS_OK != redisGetReply(c, (void**)&country_reply))
                    {
                            AIS_FREE_REPLYOBJECT(country_reply)
                    }else if (!empty)
                    {
                            empty = array_reply_empty(country_reply);
                            if (!empty)
                            {
                                    //maps[1] = get_ht_map(mp);
                                    init_map(maps[1], np, country_reply);

                                    if (eg_ht_elements(maps[0]) > eg_ht_elements(maps[1]))
                                    {
                                            tmp = maps[0];
                                            maps[0] = maps[1];
                                            maps[1] = tmp;
                                    }
                                    get_inter(maps, 1);
                            }
                      }
                      gettimeofday(&end_time, NULL);
                    cost_time = (end_time.tv_sec - start_time.tv_sec)* 1000000 + end_time.tv_usec - start_time.tv_usec;
                    COST_TIME_ERROR_LOG( ("=====>[%s] get_ad SMembers tagchinautf8 cost_time = [%ld], %lu:%lu",req_session, cost_time, end_time.tv_sec, end_time.tv_usec)); 
            }

        }else
        {
            tmp = maps[0];
            maps[0] = maps[1];
            maps[1] = tmp;
        }
#ifdef DEBUGMODE
        }else
        {
            log->pp_id = DEBUGPID;
        }
#endif
        gettimeofday(&start_time, NULL);

        i = eg_ht_elements(maps[0]);
        if (i)
        { 
            if (log->adnum >= i)
            {
                log->adnum = i;
                j = 0;
            }else if (log->adnum < i)
            {    
                if ( lastHistoryreply && lastHistoryreply->type == REDIS_REPLY_STRING ) 
                {
                    key = atoi(lastHistoryreply->str);
                    ht_remove_bucket(maps[0], (void *)key);
                    --i;
                }
                j = i - log->adnum;
            }	
            it = eg_ht_begin(maps[0]);
            if (j)
            {
                j = rand() % (j + 1);
                for (i = 0; i < j; ++i)
                {
                    eg_ht_next(&it);
                }
            }

            log->aid_list = conn->adparams_buf_last;
            for (i = 0; i < log->adnum; ++i)
            {
                strcpy(conn->adparams_buf_last, (char *)it->val);
                conn->adparams_buf_last += AIDLEN;
                eg_ht_next(&it);
            }
        }
    }while (0);


    if (++map_len)   
    {      
        for(i = 0; i < map_len; ++i)
        {
            ht_clear_bucket(maps[i]);
        }
        reset_ht_node_pool(np);
        reset_ht_map_pool(mp);

    }
    gettimeofday(&end_time, NULL);
    cost_time = (end_time.tv_sec - start_time.tv_sec)* 1000000 + end_time.tv_usec - start_time.tv_usec;
    COST_TIME_ERROR_LOG( ("=====>[%s] get_ad end cost_time = [%ld], %lu:%lu", req_session, cost_time, end_time.tv_sec, end_time.tv_usec));

    if (sunionreply) freeReplyObject(sunionreply);
    if (sinterreply) freeReplyObject(sinterreply);
    if (existsreply) freeReplyObject(existsreply);
    if (ipreply) freeReplyObject(ipreply);
    if (lastHistoryreply) freeReplyObject(lastHistoryreply);
    if (ipbytelist) freeReplyObject(ipbytelist);
    if (tempCoverageReply) freeReplyObject(tempCoverageReply);
    if (appid_reply) freeReplyObject(appid_reply);
    if (apptype_reply) freeReplyObject(apptype_reply);
    if (uuid_reply) freeReplyObject(uuid_reply);
    if (localapp_reply) freeReplyObject(localapp_reply);
#ifdef DEBUGMODE
    if (debugip_reply) freeReplyObject(debugip_reply);
#endif
}

/* add by chenhuizhong */
char* get_adbody(const char* req_session, struct ais_connection *conn, ais_logparams_t *log, char* bufdaytotal)
{
    struct timeval start_time;
    struct timeval end_time;
    long cost_time;

    int i;
    int j;
    int tlen;
    char *sBody = NULL;
    char *nearWidth = NULL;
    uuid_t uuid;
    char *tp = NULL;
    char *tp1 = NULL;
    char *tp2 = NULL;
    char buf[2048]={0};/*for urlencode*/
    char buftmp[1024] = {0};
    char *buffer = conn->buf;
    const char *tempparray[8] = {0};
    redisContext *c = conn->sredisctx;
    redisContext *c1 = conn->redisctx;
    redisReply * sPutinModelTypereply = NULL;
    redisReply * bodybyterepy = NULL;
    redisReply *  hsImageSize = NULL; 
#ifdef DEBUGMODE
    int debug = 0;
    char *pid = log->pp_id;

    if (!strcasecmp(log->pp_id, DEBUGPID))
    {
        debug = 1;
        log->pp_id = IFELSE(conn->app.PlatformProviderID,(char*)"");
    }
#endif

/*
tp: mget ad:123:josn
tp1: mget ad:123:paymodeltype
*/
    tlen = strlen(log->format);
    tp = buftmp;
    tp1 = conn->adparams_buf_last;
    strcpy(tp, "MGET");
    tp += 4;
    strcpy(tp1, "MGET");
    tp1 += 4;
    for(i = 0; i < log->adnum; i ++)
    {
        strcpy(tp, " ad:");
        tp += 4;
        tp2 = log->aid_list + i * AIDLEN;
        strcpy(tp, tp2);
        tp += strlen(tp2);
        tp[0] = ':';
        ++tp;
        strcpy(tp, log->format);
        tp += tlen;

        strcpy(tp1, " ad:");
        tp1 += 4;
        strcpy(tp1, tp2);
        tp1 += strlen(tp2);
        strcpy(tp1, ":paymodeltype");
        tp1 += 13;
    }
    tp[0] = 0;
    tp1[0] = 0;

    gettimeofday(&start_time, NULL);

    redisAppendCommand(c, buftmp);/* bodybyterepy */
    redisAppendCommand(c, "SMembers adimagesize:%s", log->ad_space_type);/* hsImageSize */
    /*conn->adparams_buf_last:MGET ad:457:paymodeltype*/
    redisAppendCommand(c, conn->adparams_buf_last);/* sPutinModelTypereply */

    if (REDIS_OK != redisGetReply(c, (void**)&bodybyterepy)) 
    {  
        AIS_FREE_REPLYOBJECT(bodybyterepy);
    }

    if ( (REDIS_OK != redisGetReply(c, (void**)&hsImageSize))
        || ((hsImageSize!=NULL) && (hsImageSize->type != REDIS_REPLY_ARRAY))) 
    {  
        AIS_FREE_REPLYOBJECT(hsImageSize);
    }

    if (REDIS_OK != redisGetReply(c, (void**)&sPutinModelTypereply)) 
    {  
        AIS_FREE_REPLYOBJECT(sPutinModelTypereply);
    }

    gettimeofday(&end_time, NULL);
    cost_time = (end_time.tv_sec - start_time.tv_sec)* 1000000 + end_time.tv_usec - start_time.tv_usec;
    COST_TIME_ERROR_LOG(( "=====>[%s] get_adbody MGET ad:json ad:paymodeltype SMembers adimagesize cost_time = [%ld], %lu:%lu", req_session, cost_time, end_time.tv_sec, end_time.tv_usec));

	if(log->aw == NULL || log->ah == NULL){
            AIS_FREE_REPLYOBJECT(sPutinModelTypereply);
            AIS_FREE_REPLYOBJECT(bodybyterepy);
            AIS_FREE_REPLYOBJECT(hsImageSize);
            return NULL;
	}

    gettimeofday(&start_time, NULL);
    if (hsImageSize && hsImageSize->elements)
    {
        near_by_width2(atoi(log->aw), atoi(log->ah), hsImageSize, &nearWidth);		
    }else if (g_ctx.img_size_len && strcasecmp("banner", log->ad_space_type))
    {
        near_by_width1(atoi(log->aw), atoi(log->ah), &nearWidth);
    }else
    {
        AIS_FREE_REPLYOBJECT(bodybyterepy);
    }

    
    gettimeofday(&end_time, NULL);
    cost_time = (end_time.tv_sec - start_time.tv_sec)* 1000000 + end_time.tv_usec - start_time.tv_usec;
    COST_TIME_ERROR_LOG( ("=====>[%s] get_adbody near_by_width cost_time = [%ld], %lu:%lu", req_session, cost_time, end_time.tv_sec, end_time.tv_usec));

    if (NULL==bodybyterepy)
    {
        AIS_FREE_REPLYOBJECT(sPutinModelTypereply);
        AIS_FREE_REPLYOBJECT(hsImageSize);
        return NULL;
    }

    log->adnum = 0;
    ++log->level;
    log->is_cheat = "1";
    log->is_billing = "1";
    gettimeofday(&start_time, NULL);
    uuid_generate(uuid);
    log->session_id = conn->adparams_buf_last;
    bin2str(log->session_id, uuid, sizeof(uuid_t));
    conn->adparams_buf_last += strlen(log->session_id) + 1;
    gettimeofday(&end_time, NULL);
    cost_time = (end_time.tv_sec - start_time.tv_sec)* 1000000 + end_time.tv_usec - start_time.tv_usec;
    COST_TIME_ERROR_LOG(("=====>[%s] get_adbody session_id cost_time = [%ld], %lu:%lu", req_session, cost_time, end_time.tv_sec, end_time.tv_usec));

    sBody = conn->adparams_buf_last;
    for(j = 0; j < bodybyterepy->elements; ++j)
    {
        if (!bodybyterepy->element[j]->str) continue;

        tp2 = NULL;
        tp = bodybyterepy->element[j]->str;
        if ((long)conn->adparams_buf + conn->buf_size < (long)sBody + strlen(tp) + 500) break;

        if (log->adnum) ++sBody;
        log->aid = log->aid_list + j * AIDLEN;
        if (sPutinModelTypereply->element[j]->str)
        {
            log->putin_type = sPutinModelTypereply->element[j]->str;
        }

        if (log->adnum)
        {
            gettimeofday(&start_time, NULL);
            uuid_generate(uuid);
            bin2str(log->session_id, uuid, sizeof(uuid_t));
            gettimeofday(&end_time, NULL);
            cost_time = (end_time.tv_sec - start_time.tv_sec)* 1000000 + end_time.tv_usec - start_time.tv_usec;
            COST_TIME_ERROR_LOG( ("=====>[%s] get_adbody for session_id cost_time = [%ld], %lu:%lu", req_session, cost_time, end_time.tv_sec, end_time.tv_usec));
        }
        snprintf( buftmp, sizeof(buftmp), "adsession:%s", log->session_id);
        snprintf(buffer, MAX_REQUEST_BUF_SIZE,
                "{\"AdId\":\"%s\",\"PlatformProviderID\":\"%s\",\"AppName\":\"%s\",\"SessionId\":\"%s\",\"Ip\":\"%s\",\"CallbackType\":\"%s\",\"AdSpaceType\":\"%s\",\"AdPlatformPrice\":\"%s\","
                "\"AppId\":\"%s\",\"Client\":\"%s\",\"DeviceType\":\"%s\",\"Uuid\":\"%s\",\"SdkVersion\":\"%s\",\"Os_version\":\"%s\","
                "\"DeviceNum\":\"%s\",\"Density\":\"%s\",\"Pw\":\"%s\",\"Ph\":\"%s\",\"Long\":\"%s\",\"Lat\":\"%s\","
                "\"Operator\":\"%s\",\"Net\":\"%s\",\"PlatformKey\":\"%s\",\"Category\":\"%s\"}", 
                log->aid, log->pp_id, log->app_name, log->session_id, log->ip, log->putin_type, 
                log->ad_space_type, log->adp_price, log->app_id, log->clinet, log->device_type, 
                log->uuid, log->sdk_ver, log->os_ver, log->device_num, log->density, log->pw, log->ph, 
                log->lg, log->lat, log->opreator, log->net, log->key, log->category);
        tempparray[i = 0] = "SETEX";
        tempparray[++i] = buftmp;
        tempparray[++i] = "900";
        tempparray[++i] = buffer; 
        gettimeofday(&start_time, NULL);
        safeRedisAppendCommandArgv(c1, i + 1, tempparray , NULL);/* tempreply */
        
        gettimeofday(&end_time, NULL);
        cost_time = (end_time.tv_sec - start_time.tv_sec)* 1000000 + end_time.tv_usec - start_time.tv_usec;
        COST_TIME_ERROR_LOG( ("=====>[%s] get_adbody for safeRedisAppendCommandArgv cost_time = [%ld], %lu:%lu", req_session, cost_time, end_time.tv_sec, end_time.tv_usec));
#ifdef DEBUGMODE
        if (debug)
        {
            log->pp_id = pid;
        }
#endif
        if (!log->adnum)
        {
            ais_md5_(buf, "Uuid=" , log->uuid , "PlatformId=" , log->pp_id, NULL );
            snprintf( buftmp , sizeof(buftmp) , "mobile:%s:lastrequest" , buf);
            tempparray[i = 0] = "SETEX";
            tempparray[++i] = buftmp;
            tempparray[++i] = "1800";
            tempparray[++i] = log->aid;
            safeRedisAppendCommandArgv(c1, i + 1, tempparray ,NULL);/* lastHistoryreply */
        }

#if AD_REDIS_STAT
        safeRedisAppendCommand(c1, "INCR ad:%s:requestnum" , log->aid);/* areply */
#endif //AD_REDIS_STAT


/*
tp has the following format:
{\"aid\":\"457\",\"ts\":\"1398656416\",\"sessionid\":\"{$SessionId$}\",\"creative\":{\"cid\":\"457\",\"ts\":\"1398656416\",\"displayinfo\":{\"type\":\"1\",\"img\":\"[$img$]{$ImgDomain$}\\/Upload\\/AdMaterials\\/0ed00f83854d45e78d71dced10a7eeac_{$ImgSize$}.jpg[$endimg$]\",\"schema\":\"0BA3A354-9008-4AD2-92AA-61F8B8E62395\"},\"clkinfos\":[{\"type\":\"1\",\"schema\":\"0BA3A354-9008-4AD2-92AA-61F8B8E62395\",\"url\":\"[$url$]http:\\/\\/lomark.cn[$endurl$]\"}],\"trackers\":{\"clicks\":[{\"schema\":\"0BA3A354-9008-4AD2-92AA-61F8B8E62395\",\"urls\":[\"{$cbclk$}\"]}],\"display\":{\"urls\":[\"{$cbimp$}\"]}}}}

*/
        gettimeofday(&start_time, NULL);
        while ((tp1 = strstr(tp, "$")))
        {
            --tp1;/* add '{' or '[' */
            if (!strncasecmp(tp1, imgdomain, 6))
            {
                *tp1 = 0;
                if (conn->ctx->config[IMG_DOMAIN])
                {
                    if (tp2)
                    {
                        tp2 = my_strlcpy2(tp2, tp, strlen(tp));
                        tp2 = my_strlcpy2(tp2, conn->ctx->config[IMG_URL], strlen(conn->ctx->config[IMG_URL]));
                    }else
                    {
                        sBody = my_strlcpy2(sBody, tp, strlen(tp));
                        sBody = my_strlcpy2(sBody, conn->ctx->config[IMG_URL], strlen(conn->ctx->config[IMG_URL]));
                    }
                }
                tp = tp1 + strlen(imgdomain);
            }else if (!strncasecmp(tp1, "{$format$}", 6))
            {
                char str[128];

                *tp1 = 0;
                if (*(log->format) == 'X' || *(log->format) == 'x')
                {
                    snprintf(str, sizeof(str), "%s&amp;p=%s" , log->format, log->pp_id);
                }else
                {
                    snprintf(str, sizeof(str), "%s&p=%s" , log->format, log->pp_id);
                }
                if (tp2)
                {
                    tp2 = my_strlcpy2(tp2, tp, strlen(tp));
                    tp2 = my_strlcpy2(tp2, str, strlen(str));
                }else
                {
                    sBody = my_strlcpy2(sBody, tp, strlen(tp));
                    sBody = my_strlcpy2(sBody, str, strlen(str));
                }
                tp = tp1 + strlen("{$format$}");
            }else if (!strncasecmp(tp1, imgsize, 6))
            {
                *tp1 = 0;
                if (tp2)
                {
                    tp2 = my_strlcpy2(tp2, tp, strlen(tp));
                    tp2 = my_strlcpy2(tp2, nearWidth, strlen(nearWidth));
                }else
                {
                    sBody = my_strlcpy2(sBody, tp, strlen(tp));
                    sBody = my_strlcpy2(sBody, nearWidth, strlen(nearWidth));
                }
                tp = tp1 + strlen(imgsize);
            }else if (!strncasecmp(tp1, sessionid, 6))
            {
                *tp1 = 0;
                if (tp2)
                {
                    tp2 = my_strlcpy2(tp2, tp, strlen(tp));
                    tp2 = my_strlcpy2(tp2, log->session_id, strlen(log->session_id));
                }else
                {
                    sBody = my_strlcpy2(sBody, tp, strlen(tp));
                    sBody = my_strlcpy2(sBody, log->session_id, strlen(log->session_id));
                }
                tp = tp1 + strlen(sessionid);
            }else if (!strncasecmp(tp1, "[$img$]", 6))
            {
                *tp1 = 0;
                sBody = my_strlcpy2(sBody, tp, strlen(tp));
                tp2 = buftmp;
                tp = tp1 + strlen("[$img$]");
            }else if (!strncasecmp(tp1, "[$endimg$]", 6))
            {
                *tp1 = 0;
                tp2 = my_strlcpy2(tp2, tp, strlen(tp));
                *tp2 = 0;
                replace_char(buftmp,"\\",NULL);

                #if 0//imp just return a gif url
                if ('1' == log->callback_source[0])
                {
                    url_encode(buftmp, buf, sizeof(buf), 1);
                    log->url = buf;
                    sBody = my_strlcpy2(sBody, conn->ctx->config[IMG_DOMAIN], strlen(conn->ctx->config[IMG_DOMAIN]));

/*Here, return the standar url for calback*/
                    int byte_print = snprintf(sBody, 2048/**/, "/v2/imp?url302=%s&sessionid=%s&format=%s&pid=%s&timestamp=%s",
                        log->url, log->session_id, log->format, log->pp_id, log->timestamp);
                    if (0>byte_print){
                        AIS_FREE_REPLYOBJECT(sPutinModelTypereply);
                        AIS_FREE_REPLYOBJECT(bodybyterepy);
                        AIS_FREE_REPLYOBJECT(hsImageSize);
                        return NULL;
                    } else {sBody+=byte_print;}
                } else {//while callbacksource is sdk
                    sBody = my_strlcpy2(sBody, buftmp, strlen(buftmp));
                }
                tp = tp1 + strlen("[$endimg$]");
                tp2 = NULL;
                #endif//imp just return a gif url

                sBody = my_strlcpy2(sBody, buftmp, strlen(buftmp));
                tp = tp1 + strlen("[$endimg$]");
                tp2 = NULL;
            }else if (!strncasecmp(tp1, "[$url$]", 6))
            {
                *tp1 = 0;
                sBody = my_strlcpy2(sBody, tp, strlen(tp));
                tp2 = buftmp;
                tp = tp1 + strlen("[$url$]");
            }else if (!strncasecmp(tp1, "[$endurl$]", 6))
            {
                *tp1 = 0;
                tp2 = my_strlcpy2(tp2, tp, strlen(tp));
                *tp2 = 0;
                replace_char(buftmp,"\\",NULL);

                #if 0//clk just return a clk 302 url
                url_encode(buftmp, buf, sizeof(buf), 1);
                log->url = buf;

                sBody = my_strlcpy2(sBody, conn->ctx->config[IMG_DOMAIN], strlen(conn->ctx->config[IMG_DOMAIN]));
                int byte_print = snprintf(sBody, 2048/**/, "/v2/clk?url302=%s&sessionid=%s&format=%s&pid=%s&timestamp=%s",
                    log->url, log->session_id, log->format, log->pp_id, log->timestamp);
                if (0>byte_print){
                    AIS_FREE_REPLYOBJECT(sPutinModelTypereply);
                    AIS_FREE_REPLYOBJECT(bodybyterepy);
                    AIS_FREE_REPLYOBJECT(hsImageSize);
                    return NULL;
                } else {sBody+=byte_print;}
                #endif//clk just return a clk 302 url
                sBody = my_strlcpy2(sBody, buftmp, strlen(buftmp));
                tp = tp1 + strlen("[$endurl$]");
                tp2 = NULL;
            }else if (!strncasecmp(tp1, "{$cbimp$}", 6))
            {
                *tp1 = 0;

/*Here, return the standar url for calback*/
                sBody = my_strlcpy2(sBody, tp, strlen(tp));
                sBody = my_strlcpy2(sBody, conn->ctx->config[IMG_DOMAIN], strlen(conn->ctx->config[IMG_DOMAIN]));

                int byte_print = snprintf(sBody, 1024/**/, "/v2/cbimp?sessionid=%s&format=%s&pid=%s&timestamp=%s",
                        log->session_id, log->format, log->pp_id, log->timestamp);
                if (0>byte_print){
                    AIS_FREE_REPLYOBJECT(sPutinModelTypereply);
                    AIS_FREE_REPLYOBJECT(bodybyterepy);
                    AIS_FREE_REPLYOBJECT(hsImageSize);
                    return NULL;
                } else {sBody+=byte_print;}

                tp = tp1 + strlen("{$cbimp$}");
            }
            else if (!strncasecmp(tp1, "{$cbclk$}", 6))
            {
                *tp1 = 0;

                sBody = my_strlcpy2(sBody, tp, strlen(tp));
                sBody = my_strlcpy2(sBody, conn->ctx->config[IMG_DOMAIN], strlen(conn->ctx->config[IMG_DOMAIN]));
                int byte_print = snprintf(sBody, 1024/**/, "/v2/cbclk?sessionid=%s&format=%s&pid=%s&timestamp=%s",
                        log->session_id, log->format, log->pp_id, log->timestamp);
                if (0>byte_print){
                    AIS_FREE_REPLYOBJECT(sPutinModelTypereply);
                    AIS_FREE_REPLYOBJECT(bodybyterepy);
                    AIS_FREE_REPLYOBJECT(hsImageSize);
                    return NULL;
                } else {sBody+=byte_print;}

                tp = tp1 + strlen("{$cbclk$}");
            }
            else
            {
                if (tp2)
                {
                    tp2 = my_strlcpy2(tp2, tp, tp1 - tp + 2);
                }else
                {
                    sBody = my_strlcpy2(sBody, tp, tp1 - tp + 2);
                }
                tp = tp1 + 2;
            }
        }
        gettimeofday(&end_time, NULL);
        cost_time = (end_time.tv_sec - start_time.tv_sec)* 1000000 + end_time.tv_usec - start_time.tv_usec;
        COST_TIME_ERROR_LOG( ("=====>[%s] get_adbody for while cost_time = [%ld], %lu:%lu", req_session, cost_time, end_time.tv_sec, end_time.tv_usec));
        gettimeofday(&start_time, NULL);
        send_redis_command(conn->redisctx, "");/*send all the redis command by safeRedisAppendCommand and safeRedisAppendCommandArgv*/

        gettimeofday(&end_time, NULL);
        cost_time = (end_time.tv_sec - start_time.tv_sec)* 1000000 + end_time.tv_usec - start_time.tv_usec;
        COST_TIME_ERROR_LOG( ("=====>[%s] get_adbody for send all cost_time = [%ld], %lu:%lu", req_session, cost_time, end_time.tv_sec, end_time.tv_usec));
        
        strcpy(sBody, tp);
        sBody += strlen(sBody);
#ifdef DEBUGMODE
        if (!debug)
        {
#endif
        gettimeofday(&start_time, NULL);

        log_handler(conn, log);
        gettimeofday(&end_time, NULL);
        cost_time = (end_time.tv_sec - start_time.tv_sec)* 1000000 + end_time.tv_usec - start_time.tv_usec;
        COST_TIME_ERROR_LOG( ("=====>[%s] get_adbody for log_handler cost_time = [%ld], %lu:%lu", req_session, cost_time, end_time.tv_sec, end_time.tv_usec));
        
#ifdef DEBUGMODE
        }
#endif
        if (*(log->format) == 'J' || *(log->format) == 'j')
        {
            *sBody = ',';
        }
        ++log->adnum;
    }

    AIS_FREE_REPLYOBJECT(sPutinModelTypereply);
    AIS_FREE_REPLYOBJECT(bodybyterepy);
    AIS_FREE_REPLYOBJECT(hsImageSize);

    DEBUG_TRACE(("last result =[%s]", conn->adparams_buf_last));
    if (log->adnum <= 0) return NULL;

    *sBody = 0;
    gettimeofday(&start_time, NULL);
    slave_redis_command_nopending(conn->sredisctx_async, "HIncrby %s request %d", bufdaytotal, log->adnum);/* dayreply */

    gettimeofday(&end_time, NULL);
    cost_time = (end_time.tv_sec - start_time.tv_sec)* 1000000 + end_time.tv_usec - start_time.tv_usec;
    COST_TIME_ERROR_LOG( ("=====>[%s] get_adbody HIncrby daytotal request cost_time = [%ld], %lu:%lu", req_session, cost_time, end_time.tv_sec, end_time.tv_usec));


    return conn->adparams_buf_last;
}

/* add by chenhuizhong */
//int get_app(redisContext *c, struct AdPlatformProvider** ap, char *key)
int get_app(const char* req_session, struct ais_connection *conn, struct AdPlatformProvider** ap, char *key)
{
    //struct AdPlatformProvider *app = (struct AdPlatformProvider *) htable_find(hadkey, key);/* disable todo */
    struct AdPlatformProvider *app = NULL;
    
    struct timeval start_time;
    struct timeval end_time;
    long cost_time;

    if (app == NULL)
    {	
        const char *jstr;
        struct json_object* new_obj = NULL;
        redisContext *c = conn->sredisctx;
        redisReply *hgetreply = NULL;

        //fprintf(stderr, "%p.%s.%d: hadkey htable not find key[%s]\n", (void *) pthread_self(),__func__, __LINE__, key);
        gettimeofday(&start_time, NULL);
        hgetreply = (redisReply *)slave_redis_command_pending(c, "HGET adplatform %s " , key );
        gettimeofday(&end_time, NULL);
        cost_time = (end_time.tv_sec - start_time.tv_sec)* 1000000 + end_time.tv_usec - start_time.tv_usec;
        COST_TIME_ERROR_LOG( ("=====>[%s] get_app HGET adplatform cost_time = [%ld], %lu:%lu", req_session, cost_time, end_time.tv_sec, end_time.tv_usec));
        
        if ( !hgetreply || hgetreply->type != REDIS_REPLY_STRING ) {
            AIS_FREE_REPLYOBJECT(hgetreply);
            return PLATFORM_EMPTY;
        }
        DEBUG_TRACE(("reply type: %d %s", hgetreply->type,hgetreply->str ));

        /*must do not have \u  4hexchar  UTF8 bugs*/
        new_obj = json_tokener_parse( hgetreply->str );
        if ( is_error( new_obj ) || json_type_object != json_object_get_type( new_obj ) ) {
            AIS_FREE_REPLYOBJECT(hgetreply);

            ERROR_LOG(("json parse error!!, new_obj:%p, type:%d", new_obj, json_object_get_type( new_obj )));
            json_object_put(new_obj);
            return PLATFORM_ERROR;
        }

        app = *ap;
        //app  = calloc(1,sizeof(struct AdPlatformProvider));/*  disable todo */
        int templen;
        SET_ADSDK_FORMJSON("PlatformProviderID",PlatformProviderID);
        SET_ADSDK_FORMJSON("ShowPutInPrice",ShowPutInPrice);
        SET_ADSDK_FORMJSON("ClickPutInPrice",ClickPutInPrice);
        SET_ADSDK_FORMJSON("Key",Key);
        SET_ADSDK_FORMJSON("SecretKey",SecretKey);
        SET_ADSDK_FORMJSON("CallbackSource",CallbackSource); 
        //htable_enter(hadkey, key , (char *)app);/* disable todo */

        AIS_FREE_REPLYOBJECT(hgetreply);
        json_object_put(new_obj);
    }

    return SUCCESS;
}

/* add by chenhuizhong */
char* get_sdkver(redisContext *c, const char* sdkkey)
{
    //char *sdkver = htable_find(hsdkver, sdkkey);  /* disable todo */
    char *sdkver = NULL;  

    if (!sdkver)
    { 
        redisReply *reply = NULL;
        reply = (redisReply *)slave_redis_command_pending(c, "HGET sdkversion %s", sdkkey);

        if (reply && reply->type == REDIS_REPLY_STRING)
        {
            sdkver = ais_strdup(reply->str);
            //htable_enter(hsdkver, sdkkey , (char *)sdkver);/*  disable todo */
        }

        freeReplyObject( reply );
    }

    return sdkver;
}

/* add by chenhuizhong */
char* get_adplatformprice(redisContext *c, const char *pid, const char* ppkey)
{
    char *price = NULL;
    char key[128];

    snprintf(key, sizeof(key), "%s%s", pid, ppkey);
    //price = htable_find(hadplatformprice, key);
    if (!price)
    {
        redisReply *reply = NULL;

        reply = (redisReply *)slave_redis_command_pending(c, "HGET adplatformprice:%s %s", pid, ppkey);
        if ((reply && reply->type == REDIS_REPLY_STRING) && atof(reply->str) > 0.0 )
        {
            price = ais_strdup(reply->str);
            //htable_enter(hadplatformprice, key, (char *)price);/*  disable todo */
        }
        freeReplyObject(reply);
    }

    return price;
}

static void  ais_get(char *req_session, struct ais_connection *conn)
{
    struct timeval start_time;
    struct timeval end_time;
    long cost_time;
    pthread_t pid = pthread_self();

    gettimeofday(&start_time, NULL);

    struct ais_adparams *aap = &(conn->ap);
    struct AdPlatformProvider * app = NULL;
    size_t ndemli = 0;
    const char *tempparray[MAX_DEMli] ={NULL};
    char *adbody = NULL;
//    char *sdkver =NULL;        //delete by zhoudp gcc warning: unused variable ‘sdkver’
    char buffert [80];
    time_t t = time(NULL);
    struct tm date = {0};
	localtime_r( &t,&date);
    char bufdaytotal[255] = {0};
    strftime (buffert,80,"%Y-%m-%d",&date);
    int where = ECODE_BULT;
    char buffer[MAX_REQUEST_BUF_SIZE];	
    ais_logparams_t *log_ps;
    char *start;
    int ret;
    char *uuid;

    redisReply *reply = NULL;/* for release todo */

#ifdef DEBUG_TIME	
    conn->start = getmocrtime();
#endif

    if (url_decode(conn->bufr,conn->bufr_len,buffer,MAX_REQUEST_BUF_SIZE,1) == -1)
    {

        DEBUG_INFO(("url[%s] "CRLF" code[%s],error_msg[%s]",conn->bufr , msgerror[PARAMETER_CONTAINING_ILLEGAL_CHARACTERS].code,msgerror[PARAMETER_CONTAINING_ILLEGAL_CHARACTERS].msg));
        AIS_ERROR_FORMAT(aap , PARAMETER_CONTAINING_ILLEGAL_CHARACTERS);	

        return ;
    }

    conn->buf = buffer;
    uuid = strstr(buffer, "LOMARKID");
    if (uuid)
    {
        *uuid = 0;
        uuid += 9;
    }

    gettimeofday(&end_time, NULL);
    cost_time = (end_time.tv_sec - start_time.tv_sec)* 1000000 + end_time.tv_usec - start_time.tv_usec;
    ERROR_LOG( ("=====>[%s] ais_get cost_time = [%ld], %lu:%lu", req_session, cost_time, end_time.tv_sec, end_time.tv_usec));

    gettimeofday(&start_time, NULL);

    start = conn->adparams_buf_last;
    if ( ais_ad_set( conn ,tempparray, &where, req_session) == AIS_ERROR  )
    {
        DEBUG_INFO(("url[%s] "CRLF" code[%s],error_msg[%s]",conn->bufr , msgerror[where].code,msgerror[where].msg));
        AIS_ERROR_FORMAT( aap , where );		

        return ;
    }
               
    replace_illegal_char(start, conn->adparams_buf_last);

    if ( ( conn->ap.flags & AIS_ADPARAMS_REQUIRE_MASK ) != AIS_ADPARAMS_REQUIRE_MASK )
    {

        DEBUG_INFO(("url[%s] "CRLF" code[%s],error_msg[%s]",conn->bufr , msgerror[PARAMETRER_NOTFULL].code,msgerror[PARAMETRER_NOTFULL].msg));
        AIS_ERROR_FORMAT( aap , PARAMETRER_NOTFULL );	

        return ;
    }

	//aap->key is necessary
	if(aap->Key == NULL){
		ret = PARAMETRER_ILLEGAL;
		AIS_ERROR_FORMAT_WAP( aap , ret);
		return ;
	}

    gettimeofday(&end_time, NULL);
    cost_time = (end_time.tv_sec - start_time.tv_sec)* 1000000 + end_time.tv_usec - start_time.tv_usec;
    ERROR_LOG( ("=====>[%s] ais_ad_set cost_time = [%ld], %lu:%lu", req_session, cost_time, end_time.tv_sec, end_time.tv_usec));
    gettimeofday(&start_time, NULL);

    app = &(conn->app);
    ret = get_app(req_session, conn, &app, aap->Key);
    if (SUCCESS != ret)
    {
        app = &(conn->app);
        AIS_ERROR_FORMAT( aap , ret);

        return;
    }

    gettimeofday(&end_time, NULL);
    cost_time = (end_time.tv_sec - start_time.tv_sec)* 1000000 + end_time.tv_usec - start_time.tv_usec;
    ERROR_LOG( ("=====>[%s] get_app cost_time = [%ld], %lu:%lu", req_session, cost_time, end_time.tv_sec, end_time.tv_usec));
    gettimeofday(&start_time, NULL);

    if (*( conn->ctx->config[OPEN_TIMESTAMP] ) == 'y' && app->CallbackSource[0] != '2')
    {
	    char * ptr;
	  	if ((ptr = strchr(aap->TimeStamp,'.'))) *ptr='\0';
        time_t   yourtime = atoi(aap->TimeStamp), mytime = time(NULL);
        double   elapsed_time= difftime(mytime,yourtime);

        if ( elapsed_time < -900 || elapsed_time > 900 )
        {

            DEBUG_INFO(("url[%s] "CRLF" code[%s],error_msg[%s],elapsed_time = %lf ,mytime = %d yourtime = %d",conn->bufr , msgerror[TIMESTAMP_EXPIRED].code,msgerror[TIMESTAMP_EXPIRED].msg,elapsed_time,(int)mytime,(int)yourtime));
            AIS_ERROR_FORMAT( aap , TIMESTAMP_EXPIRED );
            return;
        }
    }

    if (*( conn->ctx->config[OPEN_SIGN] ) == 'y' )
    {
        char ha1[33] = {0,};
        ais_md5(ha1, tempparray , app->SecretKey);
        DEBUG_TRACE(("ha1 buffer[%s] result = %s", app->SecretKey,ha1));
        if (strcmp(ha1, aap->Sign) != 0)
        {
            DEBUG_INFO(("url[%s] "CRLF" code[%s],error_msg[%s]",conn->bufr , msgerror[SIGN_NOTMATCH_PARAMETRER_ILLEGAL].code,msgerror[SIGN_NOTMATCH_PARAMETRER_ILLEGAL].msg));
            AIS_ERROR_FORMAT(aap, SIGN_NOTMATCH_PARAMETRER_ILLEGAL);

            return;
        }
    }

    string param_pid = app->PlatformProviderID;
    int param_int_pid = atoi(param_pid.c_str());
    if ( !isalldigit(param_pid)
        || (param_int_pid<=0)
        || (param_int_pid>MAX_PID_NUM))
    {
        ERROR_LOG(("pid:%s invalid!, conn->bufr:%s", param_pid.c_str(), conn->bufr));
        AIS_ERROR_FORMAT( aap , PID_INVALID_PARAMETRER_ILLEGAL );                                                          
        return;
    }
    
    log_ps = (ais_logparams_t*)malloc(sizeof(ais_logparams_t));
    if (app->CallbackSource[0] == '2' && uuid) aap->Uuid = uuid;
    init_logparams(log_ps, aap, app, &date);

    /*
     * by huangli,use terminal ip, while the CallbackSource is SDK
    if (app->CallbackSource[0] == '2')
    {
        log_ps->ip = aap->nip;
    }
    */
    gettimeofday(&end_time, NULL);
    cost_time = (end_time.tv_sec - start_time.tv_sec)* 1000000 + end_time.tv_usec - start_time.tv_usec;
    ERROR_LOG( ("=====>[%s] get_check cost_time = [%ld], %lu:%lu", req_session, cost_time, end_time.tv_sec, end_time.tv_usec));
    gettimeofday(&start_time, NULL);

    snprintf(bufdaytotal,sizeof bufdaytotal, "daytotal:%s:%s", app->PlatformProviderID , buffert);
    //ERROR_LOG(("bufdaytotal:%s, PlatformProviderID:%s, buffert:%s", bufdaytotal, app->PlatformProviderID , buffert));

    /*  for release todo */ 
    //snprintf(buffer, sizeof(buffer), "%s|%s|%s|%s", buffert, aap->Client, aap->DeviceType, aap->AdSpaceType);
    log_ps->adp_price = conn->adparams_buf_last;
    reply = (redisReply *)slave_redis_command_pending(conn->sredisctx, "HGET adplatformprice:%s %s|%s|%s|%s", app->PlatformProviderID, buffert, aap->Client, aap->DeviceType, aap->AdSpaceType);
    if ((reply && reply->type == REDIS_REPLY_STRING) && atof(reply->str) > 0.0 )
    {
        conn->adparams_buf_last = my_strlcpy(log_ps->adp_price, reply->str, reply->len);
    }else
    {
        log_ps->adp_price = NULL;
    }
    freeReplyObject(reply);
    reply = NULL;
    //platformprice = get_adplatformprice(conn->sredisctx, app->PlatformProviderID, buffer);
    /*  for release todo */

    gettimeofday(&end_time, NULL);
    cost_time = (end_time.tv_sec - start_time.tv_sec)* 1000000 + end_time.tv_usec - start_time.tv_usec;
    COST_TIME_ERROR_LOG( ("=====>[%s] get_adplatformprice cost_time = [%ld], %lu:%lu", req_session, cost_time, end_time.tv_sec, end_time.tv_usec));
    gettimeofday(&start_time, NULL);

    if (!log_ps->adp_price)
    {
        ret = ADPLATFORMPRICE_NO;
        AIS_ERROR_FORMAT(aap, ret);
        slave_redis_command_nopending( conn->sredisctx_async , "HIncrby %s request_kpi 1", bufdaytotal);
        log_ps->adp_price = "0";
        log_handler(conn, log_ps);
        free(log_ps);
      
        return;
    }

#ifdef SDK
    if ( !IsNullOrEmpty(aap->SdkVersion) && strcmp(aap->SdkVersion ,unknown) )
    {
        // for release todo 
        //snprintf(buffer, sizeof(buffer), "%s|%s|%s", app->PlatformProviderID , aap->Client , aap->SdkVersion);
        reply = slave_redis_command_pending(conn->sredisctx, "HGET sdkversion %s|%s|%s", app->PlatformProviderID , aap->Client , aap->SdkVersion);
        if (reply && reply->type == REDIS_REPLY_STRING)
        {
            sdkver = reply->str;
        }else
        {
            sdkver = NULL;
        }
        freeReplyObject( reply );
        reply = NULL;
        //sdkver = get_sdkver(conn->sredisctx, buffer);
        // for release todo

        if (!sdkver)
        {
            send_redis_command(conn->redisctx, "SADD  nosdkversion %s|%s|%s" , app->PlatformProviderID , aap->Client , aap->SdkVersion);
        }
        else
        {
            ret = sdkver_handler(sdkver, &conn->adparams_buf_last, tempparray, &ndemli);
            if (ret != SUCCESS)
            {
                slave_redis_command_nopending( conn->sredisctx_async , "HIncrby %s request_kpi 1", bufdaytotal);
                AIS_ERROR_FORMAT(aap, ret);
                log_handler(conn, log_ps);
                free(log_ps);

                return;
            }
        }
    }
	
#endif


    log_ps->aid_list = NULL;
    get_ad(req_session, conn, log_ps, tempparray, ndemli,&date);
#ifdef DEBUGMODE
    if (!strcasecmp(log_ps->pp_id, DEBUGPID))
    {
        snprintf(bufdaytotal,sizeof bufdaytotal, "debug:%s:%s", app->PlatformProviderID , buffert);
    }
#endif

    gettimeofday(&end_time, NULL);
    cost_time = (end_time.tv_sec - start_time.tv_sec)* 1000000 + end_time.tv_usec - start_time.tv_usec;
   ERROR_LOG( ( "=====>[%s] get_ad cost_time = [%ld], %lu:%lu", req_session, cost_time, end_time.tv_sec, end_time.tv_usec));
   gettimeofday(&start_time, NULL);

    ++log_ps->level;
    if (!log_ps->aid_list)
    {
        log_ps->aid = "0";
        ret = AD_NOFIND;
        slave_redis_command_nopending( conn->sredisctx_async , "HIncrby %s request_valid 1", bufdaytotal);
        AIS_ERROR_FORMAT(aap, ret);
        log_handler(conn, log_ps);
        free(log_ps);

        return;
    }

   

    adbody = get_adbody(req_session, conn, log_ps, bufdaytotal);

    gettimeofday(&end_time, NULL);
    cost_time = (end_time.tv_sec - start_time.tv_sec)* 1000000 + end_time.tv_usec - start_time.tv_usec;
    ERROR_LOG( ("=====>[%s] get_adbody cost_time = [%ld], %lu:%lu", req_session, cost_time, end_time.tv_sec, end_time.tv_usec));
    gettimeofday(&start_time, NULL);

    //ERROR_LOG(("Ip:%s, nip:%s, log_ps->ip:%s", aap->Ip, aap->nip, log_ps->ip));
    if (adbody)
    {
        if (!aap->AdNum)
        {
            AIS_QUICK_FORMT(aap, adbody);
        }else
        {
            AIS_QUICK_FORMT1(aap, adbody);
        }
    }else
    {
        ret = BODY_NOFIND;
        AIS_ERROR_FORMAT(aap, ret);
        log_handler(conn, log_ps);
    }
    free(log_ps);
    DEBUG_TRACE(("ais_get end"));
    gettimeofday(&end_time, NULL);
    cost_time = (end_time.tv_sec - start_time.tv_sec)* 1000000 + end_time.tv_usec - start_time.tv_usec;
    COST_TIME_ERROR_LOG( ("=====>[%s] ais_get end cost_time = [%ld], %lu:%lu", req_session, cost_time, end_time.tv_sec, end_time.tv_usec));
}

static void  ais_wap(struct ais_connection *conn)
{
#if 0
    struct ais_adparams *aap = &(conn->ap);
    struct AdPlatformProvider * app = NULL;
    size_t ndemli = 0;
    const char *tempparray[MAX_DEMli] ={NULL};
    char *adbody = NULL;
    char buffert [80];
    time_t t = time(NULL);
	struct tm date = {0};
	localtime_r( &t,&date);
    char bufdaytotal[255] = {0};
    strftime (buffert,80,"%Y-%m-%d",&date);
    int where = ECODE_BULT;
    char buffer[MAX_REQUEST_BUF_SIZE];	
    ais_logparams_t *log_ps;
    char *start;
    int ret;
    char *uuid;

    redisReply *reply = NULL;/* for release todo */

#ifdef DEBUG_TIME	
    conn->start = getmocrtime();
#endif

    if (url_decode(conn->bufr,conn->bufr_len,buffer,MAX_REQUEST_BUF_SIZE,1) == -1)
    {

        DEBUG_INFO(("url[%s] "CRLF" code[%s],error_msg[%s]",conn->bufr , msgerror[PARAMETER_CONTAINING_ILLEGAL_CHARACTERS].code,msgerror[PARAMETER_CONTAINING_ILLEGAL_CHARACTERS].msg));
        AIS_ERROR_FORMAT_WAP(aap , PARAMETER_CONTAINING_ILLEGAL_CHARACTERS);	

        return ;
    }

    conn->buf = buffer;
    uuid = strstr(buffer, "LOMARKID");
    if (uuid)
    {
        *uuid = 0;
        uuid += 9;
    }
    start = conn->adparams_buf_last;
    if ( ais_ad_set( conn ,tempparray, &where) == AIS_ERROR  )
    {
        DEBUG_INFO(("url[%s] "CRLF" code[%s],error_msg[%s]",conn->bufr , msgerror[where].code,msgerror[where].msg));
        AIS_ERROR_FORMAT_WAP( aap , where );		

        return ;
    }

	if ( NULL==aap->Ip ){
		aap->Ip = aap->nip;
	} else {
		char *lan_ip_pre1 = "10.";
		char *lan_ip_pre2 = "192.168";
		char *lan_ip_pre3 = "127";

		if ( ( 0==strncmp(lan_ip_pre1, aap->Ip, strlen(lan_ip_pre1)) )
			|| ( 0==strncmp(lan_ip_pre2, aap->Ip, strlen(lan_ip_pre2)) )
			|| ( 0==strncmp(lan_ip_pre3, aap->Ip, strlen(lan_ip_pre3)) ) )
		{
			aap->Ip = aap->nip;
		}
	}

    replace_illegal_char(start, conn->adparams_buf_last);
		
    if (*( conn->ctx->config[OPEN_TIMESTAMP] ) == 'y' )
    {
	    char * ptr;

		if(aap->TimeStamp == NULL){
			ret = PARAMETRER_ILLEGAL;
			AIS_ERROR_FORMAT_WAP( aap , ret);
        	return;
		}else{
			if((ptr = strchr(aap->TimeStamp,'.'))) *ptr='\0';
		}
		
        time_t   yourtime = atoi(aap->TimeStamp), mytime = time(NULL);
        double   elapsed_time= difftime(mytime,yourtime);

        if ( elapsed_time < -900 || elapsed_time > 900 )
        {

            DEBUG_INFO(("url[%s] "CRLF" code[%s],error_msg[%s],elapsed_time = %lf ,mytime = %d yourtime = %d",conn->bufr , msgerror[TIMESTAMP_EXPIRED].code,msgerror[TIMESTAMP_EXPIRED].msg,elapsed_time,(int)mytime,(int)yourtime));
            AIS_ERROR_FORMAT_WAP( aap , TIMESTAMP_EXPIRED );
            return;
        }
    }

	if(aap->Key == NULL){
		ret = PARAMETRER_ILLEGAL;
//        AIS_ERROR_FORMAT_WAP( aap , ret);
		return;
	}
    app = &(conn->app);
    ret = get_app(conn, &app, aap->Key);
    if (SUCCESS != ret)
    {
        app = &(conn->app);
        AIS_ERROR_FORMAT_WAP( aap , ret);

        return;
    }
	if(app->PlatformProviderID == NULL || aap->Client == NULL || aap->DeviceType == NULL || aap->AdSpaceType == NULL){
		ret = PARAMETRER_ILLEGAL;
		AIS_ERROR_FORMAT_WAP( aap , ret);
        return;
	}

    string param_pid = app->PlatformProviderID;
    int param_int_pid = atoi(param_pid.c_str());
    if ( !isalldigit(param_pid)
        || (param_int_pid<=0)
        || (param_int_pid>MAX_PID_NUM))
    {
        ERROR_LOG(("pid:%s invalid!, conn->bufr:%s", param_pid.c_str(), conn->bufr));
        AIS_ERROR_FORMAT( aap , PID_INVALID_PARAMETRER_ILLEGAL );                                                          
        return;
    }
    
    log_ps = (ais_logparams_t*)malloc(sizeof(ais_logparams_t));
    if (app->CallbackSource[0] == '2' && uuid) aap->Uuid = uuid;
    init_logparams(log_ps, aap, app, &date);
            
    snprintf(bufdaytotal,sizeof bufdaytotal, "daytotal:%s:%s", app->PlatformProviderID , buffert);

    /*  for release todo */ 
    //snprintf(buffer, sizeof(buffer), "%s|%s|%s|%s", buffert, aap->Client, aap->DeviceType, aap->AdSpaceType);
    log_ps->adp_price = conn->adparams_buf_last;

	
    reply = (redisReply *)slave_redis_command_pending(conn->sredisctx, "HGET adplatformprice:%s %s|%s|%s|%s", app->PlatformProviderID, buffert, aap->Client, aap->DeviceType, aap->AdSpaceType);
    if ((reply && reply->type == REDIS_REPLY_STRING) && atof(reply->str) > 0.0 )
    {
        conn->adparams_buf_last = my_strlcpy(log_ps->adp_price, reply->str, reply->len);
    }else
    {
        log_ps->adp_price = NULL;
    }
    freeReplyObject(reply);
    //platformprice = get_adplatformprice(conn->sredisctx, app->PlatformProviderID, buffer);
    /*  for release todo */

    if (!log_ps->adp_price)
    {
        ret = ADPLATFORMPRICE_NO;
        AIS_ERROR_FORMAT_WAP(aap, ret);
        slave_redis_command_nopending( conn->sredisctx_async , "HIncrby %s request_kpi 1", bufdaytotal);
        log_ps->adp_price = "0";
        log_handler(conn, log_ps);
        free(log_ps);
      
        return;
    }

    log_ps->aid_list = NULL;
    get_ad(conn, log_ps, tempparray, ndemli,&date);
#ifdef DEBUGMODE
    if (!strcasecmp(log_ps->pp_id, DEBUGPID))
    {
        snprintf(bufdaytotal,sizeof bufdaytotal, "debug:%s:%s", app->PlatformProviderID , buffert);
    }
#endif

    ++log_ps->level;
    if (!log_ps->aid_list)
    {
        log_ps->aid = "0";
        ret = AD_NOFIND;
        slave_redis_command_nopending( conn->sredisctx_async , "HIncrby %s request_valid 1", bufdaytotal);
        AIS_ERROR_FORMAT_WAP(aap, ret);
        log_handler(conn, log_ps);
        free(log_ps);

        return;
    }

    adbody = get_adbody(conn, log_ps, bufdaytotal);
    if (adbody)
    {
        AIS_QUICK_FORMT_WAP(aap, adbody, log_ps->aw, log_ps->ah);
    }else
    {
        ret = BODY_NOFIND;
        AIS_ERROR_FORMAT_WAP(aap, ret);
        log_handler(conn, log_ps);
    }
    free(log_ps);
    DEBUG_TRACE(("ais_get end"));
#endif
}

static void  ais_report(struct ais_connection *conn)
{
    struct    ais_adparams_report *aap = &(conn->apr);
    struct    AdPlatformProvider * adplatform = &(conn->app);
    const char *jstr;
    //char buf[1024]={0,},buftmp[1024]={0,}, bufmd5[33]={0,};
    //	char * adss = NULL, * adse = NULL;
    //size_t  i,j ,itags,have = 0 , ndemlisize , ndemli = 0,
    size_t templen;
    const char *tempparray[MAX_DEMli] ={NULL,};
    //const char *tags[MAX_DEMli] ={NULL,};
    //const char *parray[MAX_DEMli] ={NULL,};
    //int str_len = 0;
    //const char * tp =NULL;
    //char  **cpp ,**cppi;
    //char * aid = "",*tempaid =NULL,*sAdPlatformPrice=NULL;
    redisReply *hgetreply = NULL;
    //redisReply *hgetadplatformpricereply = NULL;
    ///redisReply *hgetsdkversionreply = NULL;
    //redisReply *sunionreply = NULL;
    //redisReply *sinterreply = NULL;
    //char *sb = NULL;
    int where = ECODE_BULT;
    char buffer[MAX_REQUEST_BUF_SIZE];	

    struct json_object* new_obj = NULL;
#ifdef GROUP
    char  *gb_str[4]={"DeviceType","AdSpaceType","Client","AppID"};
    int flag[4]={0};
#endif
    int ret;
#ifdef TXT_LOG
    //struct loginfo_packet_t * lp =NULL;
#endif
#ifdef DEBUG_TIME	
    conn->start = getmocrtime();
#endif
    aap->Format = "json";


    if (url_decode(conn->bufr,conn->bufr_len,buffer,MAX_REQUEST_BUF_SIZE,1) == -1)
    {

        DEBUG_INFO(("url[%s] "CRLF" code[%s],error_msg[%s]",conn->bufr , msgerror[PARAMETER_CONTAINING_ILLEGAL_CHARACTERS].code,msgerror[PARAMETER_CONTAINING_ILLEGAL_CHARACTERS].msg));
        AIS_ERROR_FORMAT( aap , PARAMETER_CONTAINING_ILLEGAL_CHARACTERS );	
        return ;
    }
    conn->buf = buffer;
    // Request is valid, handle it
    if ( ais_adreport_set( conn ,tempparray, &where) == AIS_ERROR  )
    {
        DEBUG_INFO(("url[%s] "CRLF" code[%s],error_msg[%s]",conn->bufr , msgerror[where].code,msgerror[where].msg));
        AIS_ERROR_FORMAT( aap , where );		
        return ;
    }
    
    aap->Format = const_cast<char *>((aap->Format && ais_strcasecmp( aap->Format , "xml" ) == 0) ? "xml" : "json");
#ifdef GROUP
    char *p1=aap->groupby;
    char *tokens[10]={0};
    int iNum=0;
    if (p1==NULL || *p1=='\0'){
        for(int k=0;k<4;k++){
            flag[k]=1;
            tokens[k]=gb_str[k];}
            iNum=4;
    }
    else{
        iNum=split(aap->groupby,',',tokens);
    }
    for(int m=0;m<iNum;m++){
        int count=0;
        for (int k=0;k<4;k++){
            if (strcasecmp(tokens[m],gb_str[k])==0){
                flag[k]=1;
                break;
            }
            count++;
        }
        if (count==4){
            AIS_ERROR_FORMAT( aap , ECODE_BULT );	
            return;		 
        }      	
    }
#endif
    ret = check_date(aap->Startdate, aap->Enddate);
    if (SUCCESS != ret)
    {
        AIS_ERROR_FORMAT( aap , ret );
        return;
    }
#ifdef CACHE_ADPLATFPRM	

	if ( ( adplatform = (struct AdPlatformProvider *) htable_find(hadkey, aap->Key) ) != 0 )
	{
		DEBUG_TRACE(("ais_get  htable_find %s ",aap->Key));
	}
	else		
	{	

		fprintf(stderr, "%p.%s.%d: hadkey htable not find key[%s]\n", (void *) pthread_self(),__func__, __LINE__,
			aap->Key);
		DEBUG_TRACE(("HGET adplatform %s ",aap->Key));
		hgetreply = slave_redis_command_pending( conn->sredisctx , "HGET adplatform %s " , aap->Key );
		if ( !hgetreply || hgetreply->type != REDIS_REPLY_STRING ) {
			AIS_ERROR_FORMAT( aap , PLATFORM_EMPTY );
			freeReplyObject( hgetreply );
			return ;
		}
               DEBUG_TRACE(("reply type: %d %s", hgetreply->type,hgetreply->str ));
		
		
		/*must do not have \u  4hexchar  UTF8 bugs*/
		 new_obj = json_tokener_parse( hgetreply->str );
		 if ( is_error( new_obj ) || json_type_object != json_object_get_type( new_obj ) ) {
			AIS_ERROR_FORMAT( aap , PLATFORM_ERROR);
			freeReplyObject( hgetreply );
			return ;
		}
		adplatform  = calloc(1,sizeof(*adplatform ));
		/*{"SdkId":"1000","SdkKey":"1000","SecretKey":"QWE!R1000","SupportType":"1,2,3,4","PlatformProviderID":"1000","PlatformName":"平台商名称1000","Version":"4.5",
		"SdkName":"SDKName1000","AdType":"1,2"}*/
		SET_ADSDK_FORMJSON("PlatformProviderID",PlatformProviderID);
		SET_ADSDK_FORMJSON("ShowPutInPrice",ShowPutInPrice);
		SET_ADSDK_FORMJSON("ClickPutInPrice",ClickPutInPrice);
		SET_ADSDK_FORMJSON("Key",Key);
		SET_ADSDK_FORMJSON("SecretKey",SecretKey);
		SET_ADSDK_FORMJSON("CallbackSource",CallbackSource); 
		htable_enter(hadkey, aap->Key , (char *) adplatform );
		freeReplyObject(hgetreply);/*free it FIX ME*/
		json_object_put(new_obj);/*FIX ME free it*/
	}
#else
	DEBUG_TRACE(("HGET adplatform %s ",aap->Key));
	hgetreply = (redisReply *)slave_redis_command_pending( conn->sredisctx , "HGET adplatform %s " , aap->Key );
	if ( !hgetreply || hgetreply->type != REDIS_REPLY_STRING ) {
		DEBUG_INFO(("url[%s] "CRLF" code[%s],error_msg[%s]",conn->bufr , msgerror[PLATFORM_EMPTY].code,msgerror[PLATFORM_EMPTY].msg));
		AIS_ERROR_FORMAT( aap , PLATFORM_EMPTY );
		freeReplyObject( hgetreply );
		return ;
	}


/*must do not have \u  4hexchar  UTF8 bugs*/
	new_obj = json_tokener_parse( hgetreply->str );
	if ( is_error( new_obj ) || json_type_object != json_object_get_type( new_obj ) ) {
		DEBUG_INFO(("url[%s] "CRLF" code[%s],error_msg[%s]",conn->bufr , msgerror[PLATFORM_ERROR].code,msgerror[PLATFORM_ERROR].msg));
		AIS_ERROR_FORMAT( aap , PLATFORM_ERROR);
		freeReplyObject( hgetreply );
		return ;
	}
	 

   /*{"SdkId":"1000","SdkKey":"1000","SecretKey":"QWE!R1000","SupportType":"1,2,3,4","PlatformProviderID":"1000","PlatformName":"平台商名称1000","Version":"4.5",
   "SdkName":"SDKName1000","AdType":"1,2"}*/
   SET_ADSDK_FORMJSON("PlatformProviderID",PlatformProviderID);
   SET_ADSDK_FORMJSON("ShowPutInPrice",ShowPutInPrice);
   SET_ADSDK_FORMJSON("ClickPutInPrice",ClickPutInPrice);
   SET_ADSDK_FORMJSON("Key",Key);
   SET_ADSDK_FORMJSON("SecretKey",SecretKey);
   SET_ADSDK_FORMJSON("CallbackSource",CallbackSource); 
   freeReplyObject(hgetreply);/*free it FIX ME*/
   json_object_put(new_obj);/*FIX ME free it*/




#endif

    char buffert [80];
    time_t t = time(NULL);
    struct tm date;
    localtime_r( &t,&date);
    struct timeval tv;
    DBPROCESS *db_proc;
    char bufdaytotal[255]={0,};
    int tid = conn->c->tid;

    string param_pid = adplatform->PlatformProviderID;
    int param_int_pid = atoi(param_pid.c_str());
    if ( !isalldigit(param_pid)
        || (param_int_pid<=0)
        || (param_int_pid>MAX_PID_NUM))
    {
        ERROR_LOG(("pid:%s invalid!, conn->bufr:%s", param_pid.c_str(), conn->bufr));
        AIS_ERROR_FORMAT( aap , PID_INVALID_PARAMETRER_ILLEGAL );                                                          
        return;
    }
    
    gettimeofday(&tv, NULL);
    strftime (buffert,80,"%Y-%m-%d", &date);
    snprintf(bufdaytotal,sizeof bufdaytotal, "daytotal:%s:%s",adplatform->PlatformProviderID , buffert);

    if (g_sql_conn[tid].db_proc && tv.tv_sec - g_sql_conn[tid].create_time > 300)
    {
        dbclose(g_sql_conn[tid].db_proc);
        g_sql_conn[tid].db_proc = NULL;
    }

    if (!g_sql_conn[tid].db_proc)
    {
        if (db_connect(conn->ctx->config[SQL_USER],conn->ctx->config[SQL_PWD],conn->ctx->config[SQL_CONN],conn->ctx->config[DB_NAME],&g_sql_conn[tid].db_proc) == FAIL)
        {
            g_sql_conn[tid].db_proc = NULL;
            AIS_ERROR_FORMAT( aap , CONNECT_ERROR );	
            return;
        }else
        {
            g_sql_conn[tid].create_time = tv.tv_sec;
        }
    }
    db_proc = g_sql_conn[tid].db_proc;

    char  sql_str[1000];
    char * tmp=sql_str;
    int len=0;
    len=sprintf(tmp,"SELECT "\
            "Convert(varchar(10),StatisticsDate,120) as StatisticsDate,");
    tmp+=len;

#ifdef GROUP
    for(int m=0;m<iNum;m++){
        //printf("token[%d]=%s\n",m,tokens[m]);
        len=sprintf(tmp,"%s,",tokens[m]);
        tmp+=len;
    }
#endif
    len=sprintf(tmp,"SUM(Request) as request,"\
            "SUM(Impression) as Impression,"\
            "SUM(Click) as Click"\
            " FROM" \
            " MasChargeStatistics with(readpast)"\
            " where" \
            " StatisticsDate between '%s' and '%s'"\
            " and ADPlatformID=%s"\
            " group by StatisticsDate,",aap->Startdate,\
            aap->Enddate,adplatform->PlatformProviderID);
    tmp+=len;
#ifdef GROUP
    for(int m=0;m<iNum;m++){
        len=sprintf(tmp,"%s,",tokens[m]);
        tmp+=len;
    }
#endif
    *(tmp-1)='\0';

    //  fprintf(stderr,"sql=%s\n",sql_str);
    dbcmd(db_proc,sql_str);
    if (dbsqlexec(db_proc) == FAIL)
    {
        AIS_ERROR_FORMAT( aap , SQL_ERROR);
        return ;
    }
    char  dat[11];
#ifdef GROUP
    int  device_type;
    int  adspace_type;
    int   client;
    char  appId[100]={0};
#endif
    int reQuest;
    int imPression;
    int click;
    char tmp_buf[300];
    memset(tmp_buf,0,300);
    memset(buffer,0,sizeof(buffer));
    char * ptr_buf=buffer;
    if ( *(aap->Format) == 'x' || *(aap->Format) == 'X') {
        len=snprintf(ptr_buf,200, "<?xml version=\"1.0\" encoding=\"utf-8\"?><result><status>%s</status><msg>%s</msg><data>",msgerror[SUCCESS].code,msgerror[SUCCESS].msg);
        ptr_buf+=len;
    } 
    else {	
        len=snprintf(ptr_buf,200,"{\"status\":\"%s\",\"msg\":\"%s\",\"data\":[",msgerror[SUCCESS].code,msgerror[SUCCESS].msg);
        ptr_buf+=len;
    }

    if (dbresults(db_proc) == SUCCEED) 
    {
        int i=1;
        dbbind(db_proc,i,STRINGBIND,(DBCHAR)0,(BYTE *) dat);
#ifdef GROUP
        if (flag[0])
            dbbind(db_proc,++i,INTBIND,(DBCHAR)0,(BYTE *) (&device_type));
        if (flag[1])
            dbbind(db_proc,++i,INTBIND,(DBCHAR)0,(BYTE *) (&adspace_type));
        if (flag[2])
            dbbind(db_proc,++i,INTBIND,(DBCHAR)0,(BYTE *) (&client));
        if (flag[3])
            dbbind(db_proc,++i,STRINGBIND,(DBCHAR)0,(BYTE *) appId);
#endif
        dbbind(db_proc,++i,INTBIND,(DBCHAR)0,(BYTE *) (&reQuest));
        dbbind(db_proc,++i,INTBIND,(DBCHAR)0,(BYTE *) (&imPression));
        dbbind(db_proc,++i,INTBIND,(DBCHAR)0,(BYTE *) (&click));

        while (dbnextrow(db_proc) == REG_ROW)
        {
            if ( *(aap->Format) == 'x' || *(aap->Format) == 'X') {
                len= sprintf(ptr_buf,"<info><date>%s</date>",dat);
                ptr_buf+=len;
#ifdef GROUP
                if (flag[0]){
                    len=sprintf(ptr_buf,"<devicetype>%d</devicetype>",device_type);
                    ptr_buf+=len;
                }
                if (flag[1]){
                    len=sprintf(ptr_buf,"<adspacetype>%d</adspacetype>",adspace_type);
                    ptr_buf+=len;
                }
                if (flag[2]){
                    len=sprintf(ptr_buf,"<client>%d</client>",client);
                    ptr_buf+=len;
                }
                if (flag[3]){
                    len=sprintf(ptr_buf,"<appid>%s</appid>",appId);
                    ptr_buf+=len;
                }
#endif
                len=sprintf(ptr_buf,"<request>%d</request><impression>%d</impression><click>%d</click>"\
                        "</info>",reQuest,imPression,click);
                ptr_buf+=len;
            }
            else{
                len= sprintf(ptr_buf,"{\"date\":\"%s\",",dat);
                ptr_buf+=len;
#ifdef GROUP
                if (flag[0]){
                    len=sprintf(ptr_buf,"\"devicetype\":%d,",device_type);
                    ptr_buf+=len;
                }
                if (flag[1]){
                    len=sprintf(ptr_buf,"\"adspacetype\":%d,",adspace_type);
                    ptr_buf+=len;
                }
                if (flag[2]){
                    len=sprintf(ptr_buf,"\"client\":%d,",client);
                    ptr_buf+=len;
                }
                if (flag[3]){
                    len=sprintf(ptr_buf,"\"appid\":\"%s\",",appId);
                    ptr_buf+=len;
                }
#endif
                len=sprintf(ptr_buf,"\"request\":%d,\"impression\":%d,\"click\":%d}"
                        ,reQuest,imPression,click);
                ptr_buf+=len; 

            }
        }
    }

    if ( *(aap->Format) == 'x' || *(aap->Format) == 'X') {

        ptr_buf=my_strlcpy4(ptr_buf,"</data></result>");
    }
    else
    {

        ptr_buf=my_strlcpy4(ptr_buf,"]}");

    }
    if ((strlen(buffer)+strlen(tmp_buf))>MAX_REQUEST_BUF_SIZE)
    {
        AIS_ERROR_FORMAT( aap , CONTEXT_LONG);
        return ;
    }

    out_string(conn->c, buffer);
    DEBUG_TRACE(("ais_report end"));
}

static void  ais_callback(const char* req_session, struct ais_connection *conn, int is302,struct tm* date)
{
    struct timeval start_time;
    struct timeval end_time;
    long cost_time;
    gettimeofday(&start_time, NULL);

    struct    ais_adparams_callback*aapc = &(conn->apc);
    struct    AdPlatformProvider * app = &(conn->app);
    char sessionKey[33] = {0};
    char uuid_key[33] = {0};
    char * RequestMode = "";
    char *str = NULL ;
    size_t templen;
    char buf[1024]={0,};
    char bufdaytotal[255]={0,};
    char sAdDayKey[255]={0,};
    char buffer[MAX_REQUEST_BUF_SIZE];	
    //struct json_object* error_object;
    const char  *jstr = NULL;
    struct json_object * new_obj = NULL;
    struct json_object * adsession_obj = NULL;
    //是否作弊 0为正常,1为Session作弊，2为IP请求频繁
    int nIsCheat = 0;
    char  *device_num="";
    char *nosession;
	int client_num = 0,device_type_num = 0;
    map<string, string> Map;

    redisReply * getreply = NULL;
    redisReply * hgetreply = NULL;
    redisReply * sOrderID = NULL;
    redisReply *stats_reply = NULL;
    redisReply *tmp_reply = NULL;
    char *pid = NULL;

#ifdef DEBUGMODE
    int debug = 0;
#endif

    aapc->Format = "json";
    if (is302 != 2)
    {
/*parse buf to get cgi params*/
        /*new version         
        cgi=cbclk&sessionid=cb2a116f92f1495a87d9a5cdc442c6ee&format=json&pid=13&timestamp=1398755143                
         */
            strncpy(buffer, conn->bufr, (conn->bufr_len<MAX_REQUEST_BUF_SIZE) ? conn->bufr_len : MAX_REQUEST_BUF_SIZE);
            int ret = Map_Cgi_Params(Map, buffer);
            if ( 0!=ret )     
            {
                ERROR_LOG(("Map_Cgi_Params err[%d], buffer:%s", ret, buffer));
                AIS_ERROR_FORMAT( aapc , PARAMETER_CONTAINING_ILLEGAL_CHARACTERS );                                                          
                return;       
            }

//param check
            if (Map["sessionid"].empty()){
                ERROR_LOG(("sessionid empty, buffer:%s", buffer));
                AIS_ERROR_FORMAT( aapc , SESSIONID_EMPTY_PARAMETRER_ILLEGAL );                                                          
                return;
            }

            string param_pid = Map["pid"];
            if (param_pid.empty()){
                ERROR_LOG(("pid empty, buffer:%s", buffer));
                AIS_ERROR_FORMAT( aapc , PID_EMPTY_PARAMETRER_ILLEGAL );                                                          
                return;
            }

            if ( DEBUGPID != param_pid )
            {
            int param_int_pid = atoi(param_pid.c_str());
            if ( !isalldigit(param_pid)
                || (param_int_pid<=0)
                || (param_int_pid>MAX_PID_NUM))
            {
                ERROR_LOG(("pid:%s invalid!, buffer:%s", param_pid.c_str(), buffer));
                AIS_ERROR_FORMAT( aapc , PID_INVALID_PARAMETRER_ILLEGAL );                                                          
                return;
            }
            }
            aapc->sessionid = const_cast<char *>(Map["sessionid"].c_str());
            aapc->url = const_cast<char *>(Map["url"].c_str());
            aapc->Format = const_cast<char *>(Map["format"].c_str());
            app->PlatformProviderID = const_cast<char *>(Map["pid"].c_str());
            aapc->device = const_cast<char *>(Map["device"].c_str());
            //string cgi = param["cgi"];

            if ( "cbimp"==Map["cgi"] ){
                aapc->callbacktype= "1";
            } else if ( "cbclk"==Map["cgi"] ){    
                aapc->callbacktype= "2"; 
            } else {
                return;
            }
    }
            
    char *pp=aapc->device; 
    char * qq=NULL;
    char * qq1=NULL;
    int flag=0;
    int bIsBilling = 0;

	time_t timep; 
    //struct tm *tmp; 

	char buffert [80];
    char buffert2 [80]; 

/*
aapc->device is like this:
f&device=Mozilla/5.0 (Linux; U; Android 2.3.5; zh-cn; LT S600  Build/GRJ90) AppleWebKit/533.1 (KHTML, like Gecko) FlyFlow/2.5 Version/4.0 Mobile Safari/533.1 baidubrowser/061_3.9.5.2_diordna_084_023/nwonknu_01_5.3.2_B2
and device_num is  LT S600
*/
	/*get the word  after build, use it as device num 
	    get device start!*/
    while (*pp!='\0'){
        if (*pp==',')
            *pp=' ';
        pp++;
    }
    pp=aapc->device;
    while((qq=strchr(pp,';'))){
        *qq='\0';
        if ((qq1=strstr(pp,"Build"))){
            *qq1=0;
            device_num=pp;
            if (strlen(device_num)>50)
                *(device_num+50)='\0';
            flag=1;
            break;
        }
        pp=qq+1;
    }
    if (!flag){
        if ((qq1=strstr(pp,"Build"))){
            *qq1=0;
            device_num=pp;
            if (strlen(device_num)>50)
                *(device_num+50)='\0';
        }
    }

	//if date is null
	if(date->tm_year == 0 && date->tm_mon == 0){
    	timep = time(NULL); 
    	localtime_r(&timep,date); 	
	}

	strftime (buffert2,80,"%Y-%m-%d", date);
    snprintf(bufdaytotal,sizeof bufdaytotal, "daytotal:%s:%s", app->PlatformProviderID , buffert2);
    snprintf(sAdDayKey,sizeof sAdDayKey, "mobile:%s:clicknum", buffert2);
    strftime (buffert, 80, "%Y%m%d", date);

    pid = app->PlatformProviderID;

    /*do it easy*/
    aapc->Format = const_cast<char *>((aapc->Format && ais_strcasecmp( aapc->Format , "xml" ) == 0) ? "xml" : "json");
    if ('1' == aapc->callbacktype[0])
    {
        nosession = "nosession_show";
    }else
    {
        nosession = "nosession_click";
    }

    gettimeofday(&end_time, NULL);
    cost_time = (end_time.tv_sec - start_time.tv_sec)* 1000000 + end_time.tv_usec - start_time.tv_usec;
    COST_TIME_ERROR_LOG( ("=====>[%s] ais_callback start cost_time = [%ld], %lu:%lu", req_session, cost_time, end_time.tv_sec, end_time.tv_usec));
    gettimeofday(&start_time, NULL);
    
    getreply = (redisReply*)slave_redis_command_pending( conn->sredisctx , "GET adsession:%s" ,  aapc->sessionid);

    gettimeofday(&end_time, NULL);
    cost_time = (end_time.tv_sec - start_time.tv_sec)* 1000000 + end_time.tv_usec - start_time.tv_usec;
    COST_TIME_ERROR_LOG( ("=====>[%s] ais_callback GET adsession cost_time = [%ld], %lu:%lu", req_session, cost_time, end_time.tv_sec, end_time.tv_usec));
    gettimeofday(&start_time, NULL);


    if ( !getreply || getreply->type != REDIS_REPLY_STRING ) {
        DEBUG_INFO(("url[%s] "CRLF" code[%s],error_msg[%s]",conn->bufr , msgerror[SESSIONID_NOFIND].code,msgerror[SESSIONID_NOFIND].msg));
        AIS_ERROR_FORMAT( aapc , SESSIONID_NOFIND );
        slave_redis_command_nopending( conn->sredisctx_async , "HIncrby %s %s 1", bufdaytotal, nosession);

        AIS_FREE_REPLYOBJECT( getreply );
        return ;
    }
    DEBUG_TRACE(("reply type: %d %s""GET adsession:%s", getreply->type,getreply->str ,aapc->sessionid));

    adsession_obj = json_tokener_parse(getreply->str);
    if (is_error(adsession_obj)||json_type_object != json_object_get_type(adsession_obj)){
        AIS_ERROR_FORMAT(aapc,SESSIONID_NOFIND);

        DEBUG_INFO(("url[%s] "CRLF" code[%s],error_msg[%s]",conn->bufr , msgerror[SESSIONID_NOFIND].code,msgerror[SESSIONID_NOFIND].msg));
        slave_redis_command_nopending( conn->sredisctx_async , "HIncrby %s %s 1", bufdaytotal, nosession);

        AIS_FREE_REPLYOBJECT(getreply);
        return ;
    }

    char *jstrCallbackType = json_object_get_string(json_object_object_get(adsession_obj,"CallbackType")); 
    char *jstrAdId = json_object_get_string(json_object_object_get(adsession_obj,"AdId")); 
    char *jstrAdSpaceType = json_object_get_string(json_object_object_get(adsession_obj,"AdSpaceType")); 
    char *jstrSessionId = json_object_get_string(json_object_object_get(adsession_obj,"SessionId")); 
    char *jstrAdPlatformPrice = json_object_get_string(json_object_object_get(adsession_obj,"AdPlatformPrice")); 
    DEBUG_TRACE(("jstrCallbackType %s , jstrAdId %s , jstrAdSpaceType %s , jstrSessionId %s , jstrAdPlatformPrice %s ",
                jstrCallbackType , jstrAdId , jstrAdSpaceType , jstrSessionId , jstrAdPlatformPrice));

    /*{"SdkId":"1000","SdkKey":"1000","SecretKey":"QWE!R1000","SupportType":"1,2,3,4","PlatformProviderID":"1000","PlatformName":"平台商名称1000","Version":"4.5",
      "SdkName":"SDKName1000","AdType":"1,2"}*/
    // conn->apc.Key
    //
    //  "{\"AdId\":\"%s\",\"PlatformProviderID\":\"%s\",\"AppName\":\"%s\",\"SessionId\":\"%s\",\"Ip\":\"%s\",\"CallbackType\":\"%s\",\"AdSpaceType\":\"%s\",\"AdPlatformPrice\":\"%s\","
    SET_ADCALLBACK_FORMJSON("AppName",appname);
    SET_ADCALLBACK_FORMJSON("Ip",Ip);


    SET_ADCALLBACK_FORMJSON("AppId",AppId);
    SET_ADCALLBACK_FORMJSON("Client",client);
    SET_ADCALLBACK_FORMJSON("DeviceType",DeviceType);

	/*if device is an ipad or iphone, we have to set device_num ourself!*/
	if((aapc->client != NULL) && (aapc->DeviceType != NULL)){
		client_num = atoi(aapc->client);
		device_type_num = atoi(aapc->DeviceType);
		if(client_num == 2){
			if(device_type_num == 2){
				device_num = "iPad";	
			}else{
				device_num = "iPhone";
			}
		}
	}

    SET_ADCALLBACK_FORMJSON("Uuid",uuid);
    SET_ADCALLBACK_FORMJSON("SdkVersion",SdkVersion);
    SET_ADCALLBACK_FORMJSON("Os_version",os_version); 

    SET_ADCALLBACK_FORMJSON("DeviceNum",device);
    SET_ADCALLBACK_FORMJSON("Density",density);
    SET_ADCALLBACK_FORMJSON("Pw",pw);

    SET_ADCALLBACK_FORMJSON("Ph",ph);
    SET_ADCALLBACK_FORMJSON("Long",longg);
    SET_ADCALLBACK_FORMJSON("Lat",lat);

   SET_ADCALLBACK_FORMJSON("Operator",oprter);
   SET_ADCALLBACK_FORMJSON("Net",net);
   SET_ADCALLBACK_FORMJSON("PlatformKey",Key);
   SET_ADCALLBACK_FORMJSON("Category",Category);
 //  freeReplyObject(getreply);/*free it FIX ME*/
#if 1
    DEBUG_TRACE(("HGET adplatform %s ",aapc->Key));

    gettimeofday(&end_time, NULL);
    cost_time = (end_time.tv_sec - start_time.tv_sec)* 1000000 + end_time.tv_usec - start_time.tv_usec;
    COST_TIME_ERROR_LOG( ("=====>[%s] ais_callback parse adsession cost_time = [%ld], %lu:%lu", req_session, cost_time, end_time.tv_sec, end_time.tv_usec));
    gettimeofday(&start_time, NULL);

    hgetreply = (redisReply*)slave_redis_command_pending( conn->sredisctx , "HGET adplatform %s " ,  aapc->Key );

    gettimeofday(&end_time, NULL);
    cost_time = (end_time.tv_sec - start_time.tv_sec)* 1000000 + end_time.tv_usec - start_time.tv_usec;
    COST_TIME_ERROR_LOG( ("=====>[%s] ais_callback HGET adplatform cost_time = [%ld], %lu:%lu", req_session, cost_time, end_time.tv_sec, end_time.tv_usec));
    gettimeofday(&start_time, NULL);


    if ( !hgetreply || hgetreply->type != REDIS_REPLY_STRING ) {
        DEBUG_INFO(("url[%s] "CRLF" code[%s],error_msg[%s]",conn->bufr , msgerror[PLATFORM_EMPTY].code,msgerror[PLATFORM_EMPTY].msg));

        AIS_ERROR_FORMAT( aapc , PLATFORM_EMPTY );
        
        AIS_FREE_REPLYOBJECT(getreply);
        AIS_FREE_REPLYOBJECT( hgetreply );
        return ;
    }
    DEBUG_TRACE(("reply type: %d %s", hgetreply->type,hgetreply->str ));


    /*must do not have \u  4hexchar  UTF8 bugs*/
    new_obj = json_tokener_parse( hgetreply->str );
    if ( is_error( new_obj ) || json_type_object != json_object_get_type( new_obj ) ) {
        AIS_ERROR_FORMAT( aapc , PLATFORM_ERROR);

        DEBUG_INFO(("url[%s] "CRLF" code[%s],error_msg[%s]",conn->bufr , msgerror[PLATFORM_ERROR].code,msgerror[PLATFORM_ERROR].msg));

        AIS_FREE_REPLYOBJECT(getreply);
        AIS_FREE_REPLYOBJECT( hgetreply );
        return ;
    }

    SET_ADSDK_FORMJSON("PlatformProviderID",PlatformProviderID);
    SET_ADSDK_FORMJSON("ShowPutInPrice",ShowPutInPrice);
    SET_ADSDK_FORMJSON("ClickPutInPrice",ClickPutInPrice);
    SET_ADSDK_FORMJSON("Key",Key);
    SET_ADSDK_FORMJSON("SecretKey",SecretKey);
    SET_ADSDK_FORMJSON("CallbackSource",CallbackSource); 
    json_object_put(new_obj);/*FIX ME free it*/

#endif

#ifdef DEBUGMODE
//    if (strcasecmp(pid, app->PlatformProviderID))
    //if debugip
    if (0 == strcasecmp(pid, DEBUGPID))
    {
        debug = 1;
        snprintf(bufdaytotal,sizeof bufdaytotal, "debug:%s:%s", app->PlatformProviderID , buffert2);
    }
    if (!debug)
    {
#endif
    //客户端参数pid被认为修改,替换为正确的pid
    if (0 != strcasecmp(pid, app->PlatformProviderID))
    {
        snprintf(bufdaytotal,sizeof bufdaytotal, "daytotal:%s:%s", app->PlatformProviderID , buffert2);
        ERROR_LOG(("pid[%s] != app->PlatformProviderID[%s]", pid, app->PlatformProviderID));
    }

    char *strOrderID = "";

    gettimeofday(&end_time, NULL);
    cost_time = (end_time.tv_sec - start_time.tv_sec)* 1000000 + end_time.tv_usec - start_time.tv_usec;
    COST_TIME_ERROR_LOG( ("=====>[%s] ais_callback parse adplatform cost_time = [%ld], %lu:%lu", req_session, cost_time, end_time.tv_sec, end_time.tv_usec));
    gettimeofday(&start_time, NULL);

    sOrderID = (redisReply *)slave_redis_command_pending( conn->sredisctx , "Get ad:%s:orderid" , jstrAdId);

    gettimeofday(&end_time, NULL);
    cost_time = (end_time.tv_sec - start_time.tv_sec)* 1000000 + end_time.tv_usec - start_time.tv_usec;
    COST_TIME_ERROR_LOG( ("=====>[%s] ais_callback Get ad::orderid cost_time = [%ld], %lu:%lu", req_session, cost_time, end_time.tv_sec, end_time.tv_usec));


    if ( sOrderID && ( sOrderID->type == REDIS_REPLY_STRING ) )
    {
        str = sOrderID->str;
        if ((size_t)(conn->adparams_buf+conn->buf_size - conn->adparams_buf_last) <strlen(str) +2)
        {
            DEBUG_INFO(("ad params buf is full"));
            //	 return;
        }
        strOrderID = conn->adparams_buf_last;//atoi(var);
        conn->adparams_buf_last = my_strlcpy( strOrderID , str , strlen( str ) );

    }

    AIS_FREE_REPLYOBJECT(sOrderID);


    //redisReply * sPutInModelType = slave_redis_command_pending( conn->sredisctx , "Get ad:%s:putinmodeltype" , jstrAdId);
    //redisReply * sPrice = slave_redis_command_pending( conn->sredisctx , "Get ad:%s:price" , jstrAdId);

    int sProcessNode = 0;
    int log_flag = 1;
    int fre;
    int is_click_fre;
    int limit = 0;
    int req_times = 1;
    int i;
    
    snprintf(buf, sizeof(buf), "AppId=%sUuid=%sAId=%sPlatformId=%sIp=%s", aapc->AppId,
                        aapc->uuid, jstrAdId, app->PlatformProviderID, conn->apc.Ip);
    ais_md5_(sessionKey, buf, NULL);
    snprintf(buf, sizeof(buf), "AppId=%sUuid=%sPlatformId=%s", aapc->AppId, 
            aapc->uuid, app->PlatformProviderID);
    ais_md5_(uuid_key, buf, NULL);

    gettimeofday(&start_time, NULL);

    switch( atoi(aapc->callbacktype) )
    {	
        case 1:
            RequestMode = "2";
            tmp_reply = (redisReply *)redis_command_pending(conn->rredisctx,"incr mobile:%s:s_lock", jstrSessionId);

            if (!tmp_reply) break;

            req_times = tmp_reply->integer;
            AIS_FREE_REPLYOBJECT(tmp_reply);
            if (1 == req_times)
            {
                send_redis_command(conn->redisctx,"expire mobile:%s:s_lock 900", jstrSessionId);
            }else
            {
                nIsCheat = 10;
            }

            if (nIsCheat)
            {
                sProcessNode = -1;
                #if AD_REDIS_STAT
                send_redis_command(conn->redisctx,"INCR  ad:%s:invalidshownum",jstrAdId);
                #endif//AD_REDIS_STAT
                
                slave_redis_command_nopending( conn->sredisctx_async , "HIncrby %s noshow 1", bufdaytotal);
            }
            else
            {
                slave_redis_command_nopending(conn->sredisctx_async , "HIncrby %s show 1", bufdaytotal);

                #if AD_REDIS_STAT
                send_redis_command(conn->redisctx,"INCR ad:%s:realshownum ",jstrAdId);
                #endif//AD_REDIS_STAT
                
                stats_reply = (redisReply *)redis_command_pending(conn->rredisctx,"INCR mobile:%s:shownum", uuid_key);
                if (!stats_reply) break;

                if (1 == stats_reply->integer)
                {
                    send_redis_command(conn->redisctx, "EXPIRE mobile:%s:shownum 60", uuid_key);
                }

                if (stats_reply->integer > 12)
                {
                    nIsCheat = 10;
                }else
                {
				//not need nay more
                    //send_redis_command(conn->redisctx,"INCR  ""ad:%s:showbillingnum",jstrAdId);
                    slave_redis_command_nopending( conn->sredisctx_async, "HIncrby %s assshow 1", bufdaytotal);
                    //修改展示量
                    send_redis_command(conn->redisctx,"SADD  tomssqladid %s ",jstrAdId);
                    if (jstrCallbackType[0] == '1')
                    {
                        gettimeofday(&start_time, NULL);
                        DeductMoney(conn,strOrderID,jstrAdId,buffert2,date->tm_hour);
                        gettimeofday(&end_time, NULL);
                        cost_time = (end_time.tv_sec - start_time.tv_sec)* 1000000 + end_time.tv_usec - start_time.tv_usec;
                        COST_TIME_ERROR_LOG( ("=====>[%s] ais_callback switch DeductMoney cost_time = [%ld], %lu:%lu", req_session, cost_time, end_time.tv_sec, end_time.tv_usec));
                    }

                    tmp_reply = (redisReply *)redis_command_pending(conn->rredisctx,"get order:%s:showfre", strOrderID);
                    if ( (NULL==tmp_reply)
                        ||((NULL!=tmp_reply) &&(REDIS_REPLY_NIL==tmp_reply->type)))
                    {
                        fre = 0;
                    }else
                    {
                        fre = atoi(tmp_reply->str);
                    }
                    AIS_FREE_REPLYOBJECT(tmp_reply);

                    if (fre)
                    {
                        tmp_reply = (redisReply *)redis_command_pending(conn->rredisctx,"HINCRBY fre:orderid:%s:show:%s %s 1", 
                                strOrderID, buffert2, aapc->uuid);
                        if (NULL!=tmp_reply)
                        {
                            if (tmp_reply->integer >= fre)
                            {
                                AIS_FREE_REPLYOBJECT(tmp_reply);
                                tmp_reply = (redisReply *)slave_redis_command_pending(conn->sredisctx,"smembers order:%s:adlist", strOrderID);
                                if ( (REDIS_REPLY_NIL!=tmp_reply->type)
                                    && (tmp_reply->elements>0))
                                {
                                    for (i = tmp_reply->elements - 1; i >= 0; --i)
                                    {
                                        send_redis_command(conn->redisctx,"SADD tag:uuid:%s %s", 
                                                aapc->uuid, tmp_reply->element[i]->str);
                                    }
                                }
    							//TODO
                                //timep += 86400;
                                //tmp = localtime(&timep); 
                                strftime (buffert2, 80, "%Y-%m-%d", date);
                                send_redis_command(conn->redisctx, "HSET fre:over:show:%s %s %s", 
                                        strOrderID, aapc->uuid, buffert2);
                            }
                        }
                        AIS_FREE_REPLYOBJECT(tmp_reply);
                    }

                    if (stats_reply->integer <= 4)
                    {
                        bIsBilling = 1;		
                        slave_redis_command_nopending( conn->sredisctx_async , "HIncrby %s masshow 1", bufdaytotal);
                        send_redis_command(conn->redisctx,"SETEX mobile:%s:show 900 %d", jstrSessionId, IMP_SHOW_VALID);
                    }else
                    {
                        send_redis_command(conn->redisctx,"SETEX mobile:%s:show 900 %d", jstrSessionId, IMP_SHOW_INVALID);
                    }
                }
                AIS_FREE_REPLYOBJECT(stats_reply);
            }
            break;
        case 2:
            RequestMode = "3";	
            tmp_reply = (redisReply *)slave_redis_command_pending(conn->sredisctx,"get mobile:%s:show", jstrSessionId);
            if (!tmp_reply) break;

            if (tmp_reply->type == REDIS_REPLY_NIL)
            {
            //no show
                nIsCheat = 10;
            }else
            {
                limit = atoi(tmp_reply->str);
            }
            AIS_FREE_REPLYOBJECT(tmp_reply);

            if (!nIsCheat)
            {
                tmp_reply = (redisReply *)redis_command_pending(conn->rredisctx, "incr mobile:%s:c_lock", jstrSessionId);
                if (!tmp_reply) break;

                req_times = tmp_reply->integer;
                if (1 == req_times)
                {
                    send_redis_command(conn->redisctx,"expire mobile:%s:c_lock 900", jstrSessionId);
                }else
                {
               //one show, more than once click
                    nIsCheat = 10;
                }
                AIS_FREE_REPLYOBJECT(tmp_reply);
            }
           
            if (nIsCheat)
            {
                sProcessNode = -2;

                #if AD_REDIS_STAT
                send_redis_command(conn->redisctx,"INCR  ad:%s:invalidshownum",jstrAdId);
                #endif//AD_REDIS_STAT
                
                slave_redis_command_nopending( conn->sredisctx_async , "HIncrby %s noclick 1", bufdaytotal);
            }
            else
            {
                send_redis_command(conn->redisctx,"expire mobile:%s:c_lock 900", jstrSessionId);
                slave_redis_command_nopending( conn->sredisctx_async , "HIncrby %s click 1", bufdaytotal);

                #if AD_REDIS_STAT
                send_redis_command(conn->redisctx,"INCR  ad:%s:realclicknum ",jstrAdId);
                #endif//AD_REDIS_STAT
                
                stats_reply = (redisReply *)redis_command_pending(conn->rredisctx,"HINCRBY %s %s 1", sAdDayKey, sessionKey);
                if (!stats_reply) break;

                if (stats_reply->integer > 6)
                {
                    nIsCheat = 10;
                }else
                {
                    send_redis_command(conn->redisctx,"SADD  tomssqladid %s ",jstrAdId);
                    //not need any more
					//send_redis_command(conn->redisctx,"INCR  ad:%s:clickbillingnum",jstrAdId);
                    //每天收费点击量
                    slave_redis_command_nopending( conn->sredisctx_async , "HIncrby %s assclick 1", bufdaytotal);
                    if (jstrCallbackType[0] == '2')
                    {
                        gettimeofday(&start_time, NULL);
                        DeductMoney(conn,strOrderID,jstrAdId,buffert2,date->tm_hour);
                        gettimeofday(&end_time, NULL);
                        cost_time = (end_time.tv_sec - start_time.tv_sec)* 1000000 + end_time.tv_usec - start_time.tv_usec;
                        COST_TIME_ERROR_LOG( ("=====>[%s] ais_callback switch DeductMoney cost_time = [%ld], %lu:%lu", req_session, cost_time, end_time.tv_sec, end_time.tv_usec));
                    }

                    tmp_reply = (redisReply *)slave_redis_command_pending(conn->sredisctx,"get order:%s:clkfrestate", strOrderID);

                    if (!tmp_reply) break;
                    if (REDIS_REPLY_NIL==tmp_reply->type)
                    {
                        is_click_fre = 0;
                    }else
                    {
                        is_click_fre = atoi(tmp_reply->str);
                    }
                    AIS_FREE_REPLYOBJECT(tmp_reply);

                    if (is_click_fre)
                    {
                        tmp_reply = (redisReply *)redis_command_pending(conn->rredisctx,"get order:%s:clickfre", strOrderID);

                        if (!tmp_reply) break;
                        if (REDIS_REPLY_NIL==tmp_reply->type)
                        {
                            fre = 0;
                        }else
                        {
                            fre = atoi(tmp_reply->str);
                        }
                        AIS_FREE_REPLYOBJECT(tmp_reply);

                        if (fre)
                        {
                            tmp_reply = (redisReply *)redis_command_pending(conn->rredisctx,"HINCRBY fre:orderid:%s:click:%s %s 1", 
                                    strOrderID, buffert2, aapc->uuid);
                            if (tmp_reply->integer >= fre)
                            {
                                AIS_FREE_REPLYOBJECT(tmp_reply);
                                tmp_reply = (redisReply *)slave_redis_command_pending(conn->sredisctx,"smembers order:%s:adlist", strOrderID);

                                if (!tmp_reply) break;
                                if (REDIS_REPLY_NIL != tmp_reply->type && tmp_reply->elements > 0)
                                {
                                    for (i = tmp_reply->elements - 1; i >= 0; --i)
                                    {
                                        send_redis_command(conn->redisctx, "SADD tag:uuid:%s %s", 
                                                aapc->uuid, tmp_reply->element[i]->str);
                                    }
                                }
                                AIS_FREE_REPLYOBJECT(tmp_reply);
                                tmp_reply = (redisReply *)slave_redis_command_pending(conn->sredisctx, "get order:%s:fredays", strOrderID);

                                if (!tmp_reply) break;
                                if (REDIS_REPLY_NIL != tmp_reply->type)
                                {
                                	//TODO
                                    //timep += 86400 * atoi(tmp_reply->str);
                                    //tmp = localtime(&timep); 	
                                    strftime (buffert2,80,"%Y-%m-%d", date);
                                    send_redis_command(conn->redisctx, "HSET fre:over:click:%s %s %s", 
                                            strOrderID, aapc->uuid, buffert2);
                                }
                            }
                            AIS_FREE_REPLYOBJECT(tmp_reply);
                        }   
                    }

                    if (stats_reply->integer <= 2 && limit == 1)
                    {
                        bIsBilling = 1;
                        slave_redis_command_nopending( conn->sredisctx_async , "HIncrby %s masclick 1", bufdaytotal);
                    }
                }
                AIS_FREE_REPLYOBJECT(stats_reply);
            }
            break;
        case 3:
            break;
        default:
            break;
    }

    gettimeofday(&end_time, NULL);
    cost_time = (end_time.tv_sec - start_time.tv_sec)* 1000000 + end_time.tv_usec - start_time.tv_usec;
    COST_TIME_ERROR_LOG( ("=====>[%s] ais_callback switch cost_time = [%ld], %lu:%lu", req_session, cost_time, end_time.tv_sec, end_time.tv_usec));


    /*
       log_callbackinfo,2012/10/9 13:54:20,,/api/1.0/callback,1000,AppName,b85c774d159d47e481404efbf5dd80b4,1,Uuidshjhsd,1,1,1,4.5,1,,,,,,
       61.144.238.128,{AdvertisersID},{ADOrderID},24,1000,1000,SDKName1000,4.5,1,2,{ADPrice},2,1,{ProName},{CityName},{ADPPPrice},0
       */

    //记录错误日志
    if (log_flag && *conn->ctx->config[OPEN_WRITE_LOG] == 'y' )//strcmp(conn->ctx->config[OPEN_WRITE_LOG], "yes")==0)
    {	
        ais_logparams_t *log = (ais_logparams_t*)malloc(sizeof(ais_logparams_t));

        log->category = aapc->Category;
        log->session_id = jstrSessionId;
        log->aid = jstrAdId;
        log->pp_id = app->PlatformProviderID;
        log->sdk_ver = IFELSE(aapc->SdkVersion,unknown);
        log->key = app->Key;
        log->putin_type = jstrCallbackType; 
        log->request_mode = RequestMode;
        log->adp_price = jstrAdPlatformPrice;
        log->year = 1900 + date->tm_year;
        log->month = 1 + date->tm_mon;
        log->mday = date->tm_mday;
        log->hour = date->tm_hour;
        log->min = date->tm_min;
        log->sec = date->tm_sec; 
        log->ip = aapc->Ip;
        log->app_id = aapc->AppId;
        log->app_name = aapc->appname;
        log->uuid = aapc->uuid;
        log->device_num = aapc->device;
        if ((!(log->device_num)) || (*(log->device_num)=='\0')){
			log->device_num=device_num;}
        char *q1=log->device_num;
        if (strlen(q1)>50){
          *(q1+50)='\0';}
        log->clinet = aapc->client;
        log->os_ver = aapc->os_version;
        log->density = aapc->density;
        log->pw = aapc->pw;
        log->ph = aapc->ph;
        log->lg = aapc->longg;
        log->lat = aapc->lat;
        log->opreator = aapc->oprter; 
        log->net = aapc->net;
        log->ad_space_type = jstrAdSpaceType;
        log->device_type = aapc->DeviceType;
        log->level = sProcessNode;
        log->is_cheat = const_cast<char *>(nIsCheat > 0 ? "0" : "1");
        log->is_billing = const_cast<char *>(bIsBilling ? "1" : "0");
        log_handler(conn, log);
        free(log);
    }
#ifdef DEBUGMODE
    }else
    {
        if (atoi(aapc->callbacktype) == 1)
        {
            slave_redis_command_nopending( conn->sredisctx_async , "HIncrby %s show 1", bufdaytotal);
        }else
        {
            slave_redis_command_nopending( conn->sredisctx_async , "HIncrby %s click 1", bufdaytotal);
        }
    }
#endif
    if ( conn->bufr[0] =='c' || conn->bufr[0] =='f' || conn->bufr[0] =='g'
        || "cbclk"==Map["cgi"] || "cbimp"==Map["cgi"] || "callback"==Map["cgi"])
    {
        switch (nIsCheat)
        {
            case 1:
                //1为Session作弊
                // sCode = CustomStatus.CHEAT_CALLBACK;

                DEBUG_INFO(("url[%s] "CRLF" code[%s],error_msg[%s]",conn->bufr , msgerror[CHEAT_CALLBACK].code,msgerror[CHEAT_CALLBACK].msg));
                AIS_ERROR_FORMAT(aapc,CHEAT_CALLBACK);
                break;
            case 2:
                //2为IP请求频繁

                DEBUG_INFO(("url[%s] "CRLF" code[%s],error_msg[%s]",conn->bufr , msgerror[IP_BRUSH].code,msgerror[IP_BRUSH].msg));
                AIS_ERROR_FORMAT(aapc,IP_BRUSH);
                break;

            case 3:
                //3为网络环境发生变化
                DEBUG_INFO(("url[%s] "CRLF" code[%s],error_msg[%s]",conn->bufr , msgerror[IP_CHANGE].code,msgerror[IP_CHANGE].msg));
                AIS_ERROR_FORMAT(aapc,IP_CHANGE);
                break;
            case 4:
                //4广告投放类型出错                
                DEBUG_INFO(("url[%s] "CRLF" code[%s],error_msg[%s]",conn->bufr , msgerror[CALLBACKTYPE_ERROR].code,msgerror[CALLBACKTYPE_ERROR].msg));
                AIS_ERROR_FORMAT(aapc,CALLBACKTYPE_ERROR);
                break;
            default:
                //成功
                AIS_ERROR_FORMAT(aapc,SUCCESS);
                break;
        }

    }
    else
    {
        switch (nIsCheat)
        {
            case 1:
                //1为Session作弊
                // sCode = CustomStatus.CHEAT_CALLBACK;

                DEBUG_INFO(("url[%s] "CRLF" code[%s],error_msg[%s]",conn->bufr , msgerror[CHEAT_CALLBACK].code,msgerror[CHEAT_CALLBACK].msg));
                //AIS_ERROR_FORMAT(aapc,CHEAT_CALLBACK);
                break;
            case 2:
                //2为IP请求频繁

                DEBUG_INFO(("url[%s] "CRLF" code[%s],error_msg[%s]",conn->bufr , msgerror[IP_BRUSH].code,msgerror[IP_BRUSH].msg));
                //AIS_ERROR_FORMAT(aapc,IP_BRUSH);
                break;

            case 3:
                //3为网络环境发生变化
                DEBUG_INFO(("url[%s] "CRLF" code[%s],error_msg[%s]",conn->bufr , msgerror[IP_CHANGE].code,msgerror[IP_CHANGE].msg));
                //AIS_ERROR_FORMAT(aapc,IP_CHANGE);
                break;
            case 4:
                //4广告投放类型出错                
                DEBUG_INFO(("url[%s] "CRLF" code[%s],error_msg[%s]",conn->bufr , msgerror[CALLBACKTYPE_ERROR].code,msgerror[CALLBACKTYPE_ERROR].msg));
                //AIS_ERROR_FORMAT(aapc,CALLBACKTYPE_ERROR);
                break;
            default:
                //成功

                //AIS_ERROR_FORMAT(aapc,SUCCESS);
                break;
        }

        snprintf(buffer,MAX_REQUEST_BUF_SIZE,"1%s",aapc->id);
        out_string(conn->c, buffer);
    }


    json_object_put(adsession_obj);

    DEBUG_TRACE(("ais_getcallback end"));
    AIS_FREE_REPLYOBJECT(getreply);
    AIS_FREE_REPLYOBJECT(hgetreply);
    AIS_FREE_REPLYOBJECT(sOrderID);
    AIS_FREE_REPLYOBJECT(stats_reply);
    AIS_FREE_REPLYOBJECT(tmp_reply);
}

static void ais_imp_clk_new(struct ais_connection* conn)
{
#if 0
    struct ais_adparams_offline *apo = &(conn->apo);
    struct AdPlatformProvider *app = NULL;

    const char *tempparray[MAX_DEMli] ={NULL};
    char buffert [80];
    char buffert2 [80];
    time_t t = time(NULL);
    struct tm date = {0};
    localtime_r( &t,&date);
    char bufdaytotal[255] = {0};
    int where = ECODE_BULT;
    char buffer[MAX_REQUEST_BUF_SIZE];

    //char base64params[MAX_PARAMS_BASE64_BUF_SIZE];  //TODO
    char *start;
    int ret;
    char *uuid;
    char buf[1024]={0,};

    char *sessionKey = "offline";
    char *SecretKey = "donson.lomark+-";
//    char uuid_key[33] = {0};
    char * RequestMode = "";
    char *str = NULL ;

    redisReply * aAdidInter = NULL;
//    redisReply * sOrderID = NULL;
    redisReply * adPaymodelType = NULL;
    
//    char sAdDayKey[255]={0,};
    //是否作弊 0为正常,1为Session作弊，2为IP请求频繁
//    int nIsCheat = 0;
//    char *nosession = NULL;

    DEBUG_TRACE(("begin  offline show or click\n"));

#ifdef DEBUG_TIME    
    conn->start = getmocrtime();
#endif

    strncpy(buffer, conn->bufr, (conn->bufr_len < sizeof(buffer)-1) ? conn->bufr_len :sizeof(buffer)-1);

    if (buffer[0]=='h')
    {
        apo->callbacktype= "1";
    }
    else if (buffer[0]=='i')
    {
        apo->callbacktype= "2";
    }
    else
    {
        DEBUG_INFO(("url[%s] "CRLF" code[%s],error_msg[%s]",conn->bufr , msgerror[PARAMETER_CONTAINING_ILLEGAL_CHARACTERS].code,msgerror[PARAMETER_CONTAINING_ILLEGAL_CHARACTERS].msg));
        out_string(conn->c, "{\"status\":\"502\"}");
        return ;
    }

    /*
     * the request string has the following format:
     * d&device=Mozilla/5.0 (Windows NT 6.1; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/33.0.1750.154 Safari/537.36&http::/192.168.1.50:8073/Upload/AdMaterials/9e78c3009dad4dd798398938fee578e6.png=deb8c668711a4505b65af19bf9fc2339=json=13=1397470158&AssUri=imp&AssIp=172.16.2.224& 
     * */
    char *p_begin = NULL;
    char *p_end = NULL; 
    char *user_agent = NULL;
    
    p_begin = strstr(buffer+2, "device=");
    if ( NULL==p_begin )
    {
        ERROR_LOG(("device= not found, buffer:%s", buffer));
        out_string(conn->c, "{\"status\":\"502\"}");
        return; 
    }
    
    p_end = strchr(p_begin, '&');
    if ( NULL==p_end )
    {
        ERROR_LOG(("& not found after device=, buffer:%s", buffer));
        out_string(conn->c, "{\"status\":\"502\"}");
        return; 
    } else {
        *p_end = '\0';
        user_agent = p_begin + strlen("device="); 
    }
    apo->device = user_agent;

    //conn->buf = buffer;
    uuid = strstr(++p_end, "LOMARKID=");
    if (NULL == uuid)
    {
        ERROR_LOG(("LOMARKID= not found, buffer:%s", buffer));
        out_string(conn->c, "{\"status\":\"502\"}");
        return;
    }
    *uuid = 0;
    uuid += 9;
    str = strchr(uuid, '&');
    if (NULL == str)
    {
        ERROR_LOG(("& not found after LOMARKID=, buffer:%s", buffer));
        out_string(conn->c, "{\"status\":\"502\"}");
        return;
    }
    else
    {
        *str = '\0';
    }

    apo->uuid = uuid;
    DEBUG_INFO(("LOMARKID=[%s]", apo->uuid));
    conn->buf = ++str;
    str = NULL;

    start = conn->adparams_buf_last;
    if ( ais_offline_set( conn ,tempparray, &where) == AIS_ERROR  )
    {
        DEBUG_INFO(("url[%s] "CRLF" code[%s],error_msg[%s]",conn->bufr , msgerror[where].code,msgerror[where].msg));
        AIS_ERROR_FORMAT(apo,where);
        return ;
    }

    apo->Format = "json";
    replace_illegal_char(start, conn->adparams_buf_last);

    if ( ( conn->apo.flags & AIS_OFLPARAMS_REQUIRE_MASK) != AIS_OFLPARAMS_REQUIRE_MASK )
    {
        DEBUG_INFO(("url[%s] "CRLF" code[%s],error_msg[%s]",conn->bufr , msgerror[PARAMETRER_NOTFULL].code,msgerror[PARAMETRER_NOTFULL].msg));
        AIS_ERROR_FORMAT(apo,PARAMETRER_NOTFULL);
        return ;
    }

    app = &(conn->app);
    ret = get_app(conn, &app, apo->key);  //aap = conn->ap   根据参数key查询对于錋dPlatformProvider信息
    if (SUCCESS != ret)
    {
        app = &(conn->app);
        DEBUG_INFO(("url[%s] "CRLF" code[%s],error_msg[%s]", conn->bufr, msgerror[ret].code,msgerror[ret].msg));
        AIS_ERROR_FORMAT(apo,ret);
        return;
    }

    if (*( conn->ctx->config[OPEN_SIGN] ) == 'y' )
    {
        char ha1[33] = {0,};
        ais_md5(ha1, tempparray , SecretKey);
        DEBUG_TRACE(("ha1 buffer[%s] result = %s", SecretKey,ha1));
        if (strcmp(ha1, apo->sign) != 0)
        {
            DEBUG_INFO(("url[%s] "CRLF" code[%s],error_msg[%s]",conn->bufr , msgerror[SIGN_NOTMATCH_PARAMETRER_ILLEGAL].code,msgerror[SIGN_NOTMATCH_PARAMETRER_ILLEGAL].msg));
            AIS_ERROR_FORMAT(apo, SIGN_NOTMATCH_PARAMETRER_ILLEGAL);
            return;
        }
    }


    string param_pid = app->PlatformProviderID;
    int param_int_pid = atoi(param_pid.c_str());
    if ( !isalldigit(param_pid)
        || (param_int_pid<=0)
        || (param_int_pid>MAX_PID_NUM))
    {
        ERROR_LOG(("pid:%s invalid!, conn->bufr:%s", param_pid.c_str(), conn->bufr));
        AIS_ERROR_FORMAT( apo , PID_INVALID_PARAMETRER_ILLEGAL );                                                          
        return;
    }

    //judge adid is the elemet of ppid
    aAdidInter = (redisReply *)slave_redis_command_pending( conn->sredisctx , "SISMEMBER temp:private:%s %s",  app->PlatformProviderID, apo->adid);
    
    if ( !aAdidInter || aAdidInter->type != REDIS_REPLY_INTEGER ||  aAdidInter->integer != 1)
    {
        DEBUG_INFO(("url[%s] "CRLF" adid[%s] is not contained the ppid[%s]", conn->bufr, apo->adid, app->PlatformProviderID));
        //AIS_ERROR_FORMAT(apo,KEY_NOFIND_AD);
        if (apo->url)
        {
            out_string(conn->c, apo->url);
        }
        AIS_FREE_REPLYOBJECT(aAdidInter);
        return;
    }
    DEBUG_INFO(("url[%s] "CRLF" adid[%s] is contained the ppid[%s]", conn->bufr, apo->adid, app->PlatformProviderID));
    
    parse_user_agent(apo->device, conn);
    
    time_t timep;

    if(date.tm_year == 0 && date.tm_mon == 0){
        timep = time(NULL); 
        localtime_r(&timep,&date);    
    }

    //bufdaytotal   =   daytotal:platproviderid(13):localtime(%Y-%m-%d)
    strftime (buffert2,80,"%Y-%m-%d", &date);
    snprintf(bufdaytotal,sizeof bufdaytotal, "daytotal:%s:%s", app->PlatformProviderID , buffert2);
//    snprintf(sAdDayKey,sizeof sAdDayKey, "mobile:%s:clicknum", buffert2);
    strftime (buffert, 80, "%Y%m%d", &date);
        
    /*do it easy*/
    //aapc->Format = (aapc->Format && ais_strcasecmp( aapc->Format , "xml" ) == 0) ? "xml" : "json"; //format
    char *jstrCallbackType = "";
    adPaymodelType = (redisReply *)slave_redis_command_pending( conn->sredisctx , "get ad:%s:paymodeltype", apo->adid);

    if ( adPaymodelType && ( adPaymodelType->type == REDIS_REPLY_STRING))
    {
        str = adPaymodelType->str;
        if ((size_t)(conn->adparams_buf+conn->buf_size - conn->adparams_buf_last) <strlen(str) +2)
        {
            DEBUG_INFO(("ad params buf is full"));                  //TODO  ad params buf is full
        }
        else
        {
            jstrCallbackType = conn->adparams_buf_last;
            conn->adparams_buf_last = my_strlcpy( jstrCallbackType , str , strlen( str ) );
        }
    }

    int sProcessNode = 0;
    int log_flag = 1;
    switch( atoi(apo->callbacktype) )
    {    
        case 1: //oflshow
            RequestMode = "2";
                slave_redis_command_nopending(conn->sredisctx_async , "HIncrby %s show 1", bufdaytotal);
            break;
        case 2:   //oflclick
            RequestMode = "3";
                slave_redis_command_nopending( conn->sredisctx_async , "HIncrby %s click 1", bufdaytotal);
            break;
        case 3:
            break;
        default:
            break;
    }
	if (log_flag && *conn->ctx->config[OPEN_WRITE_LOG] == 'y' )//strcmp(conn->ctx->config[OPEN_WRITE_LOG], "yes")==0)
    {    
        ais_logparams_t *log = (ais_logparams_t*)malloc(sizeof(ais_logparams_t));

        log->category = DEF_CATEGORY;
        log->session_id = sessionKey;
        log->aid = apo->adid;
        log->pp_id = app->PlatformProviderID;
        log->sdk_ver = unknown;
        log->key = app->Key;
        log->putin_type = jstrCallbackType; 
        log->request_mode = RequestMode;
        log->year = 1900 + date.tm_year;
        log->month = 1 + date.tm_mon;
        log->mday = date.tm_mday;
        log->hour = date.tm_hour;
        log->min = date.tm_min;
        log->sec = date.tm_sec; 
        log->ip = apo->nip;
        log->app_id = DEF_APPID;
        log->app_name = "";
        log->uuid = apo->uuid;
        log->device_num = apo->device_num;
        log->clinet = apo->client;
        log->os_ver = apo->os_ver;
        log->density = DEF_DENSITY;
        log->pw = DEF_PW;
        log->ph = DEF_PH;
        log->lg = DEF_LONG;
        log->lat = DEF_LAT;
        log->opreator = DEF_OPERATOR; 
        log->net = DEF_NET;
        log->ad_space_type = apo->adspace;
        log->device_type = apo->device_type;
        log->level = sProcessNode;
        log->is_cheat = "1";
        log->is_billing = "1";
        log->adp_price = "0";
		log_handler(conn, log);
        free(log);
	}

    if ( conn->bufr[0] =='h' || conn->bufr[0] =='i')
    {
        if (apo->url)//http::/
        {
            out_string(conn->c, apo->url); //302
        }
        else
        {
            AIS_ERROR_FORMAT(apo, SUCCESS);
        }
    }
    else
    {
        AIS_ERROR_FORMAT(apo, UNKNOWN_METHOD);
    }

    DEBUG_TRACE(("ais_oflshow or ais_oflclick end"));

    AIS_FREE_REPLYOBJECT(aAdidInter);
    AIS_FREE_REPLYOBJECT(adPaymodelType);
    #endif
}


static void ais_imp_clk(const char* req_session, struct ais_connection *conn)
{
    struct timeval start_time;
    struct timeval end_time;
    long cost_time;
    gettimeofday(&start_time, NULL);


    char buffer[MAX_REQUEST_BUF_SIZE];	
    time_t t = time(NULL);
    struct tm date = {0};
    localtime_r( &t,&date);
    struct ais_adparams_callback*apc = &(conn->apc);
    struct AdPlatformProvider *app = &(conn->app);
    map<string, string> Map;
    DEBUG_TRACE(("begin  imp|clk\n"));

    /*new version         
cgi=clk&device=...&url302=http%3A%3A%2Flomark.cn&sessionid=f13d06f3906b4255a0063d49f0eb7814&format=json&pid=13&timestamp==1400150293                
     */
        strncpy(buffer, conn->bufr, (conn->bufr_len<MAX_REQUEST_BUF_SIZE) ? conn->bufr_len : MAX_REQUEST_BUF_SIZE);
        int ret = Map_Cgi_Params(Map, buffer);
        if ( 0!=ret )     
        {
            ERROR_LOG(("Map_Cgi_Params err[%d], buffer:%s", ret, buffer));
            AIS_ERROR_FORMAT( apc , PARAMETER_CONTAINING_ILLEGAL_CHARACTERS );                                                          
            return;       
        }

//param check begin
        if (Map["sessionid"].empty())
        {
            ERROR_LOG(("sessionid empty, buffer:%s", buffer));
            AIS_ERROR_FORMAT( apc , SESSIONID_EMPTY_PARAMETRER_ILLEGAL );                                                          
            return;
        }
        string param_pid = Map["pid"];
        if (param_pid.empty()){
            ERROR_LOG(("pid empty, buffer:%s", buffer));
            AIS_ERROR_FORMAT( apc , PID_EMPTY_PARAMETRER_ILLEGAL );                                                          
            return;
        }

        if ( DEBUGPID != param_pid )
        {
        int param_int_pid = atoi(param_pid.c_str());
        if ( !isalldigit(param_pid)
            || (param_int_pid<=0)
            || (param_int_pid>MAX_PID_NUM))
        {
            ERROR_LOG(("pid:%s invalid!, buffer:%s, conn->bufr:%s", param_pid.c_str(), buffer, conn->bufr));
            AIS_ERROR_FORMAT( apc , PID_INVALID_PARAMETRER_ILLEGAL );                                                          
            return;
        }
        }

        if (Map["url302"].empty())
        {
            ERROR_LOG(("url302 empty, buffer:%s", buffer));
            AIS_ERROR_FORMAT( apc , URL302_EMPTY_PARAMETRER_ILLEGAL );                                                          
            return;
        }
//param check end

        apc->sessionid = const_cast<char *>(Map["sessionid"].c_str());
        apc->url = const_cast<char *>(Map["url302"].c_str());
        apc->Format = const_cast<char *>(Map["format"].c_str());
        app->PlatformProviderID = const_cast<char *>(Map["pid"].c_str());
        apc->device = const_cast<char *>(Map["device"].c_str());
        //string cgi = param["cgi"];

        if ( "imp"==Map["cgi"] ){
            apc->callbacktype= "1";
        } else if ( "clk"==Map["cgi"] ){    
            apc->callbacktype= "2"; 
        } else {
            return;
        }
    
    ais_callback(req_session, conn, 2, &date);
    gettimeofday(&end_time, NULL);
    cost_time = (end_time.tv_sec - start_time.tv_sec)* 1000000 + end_time.tv_usec - start_time.tv_usec;
    COST_TIME_ERROR_LOG( ("=====>[%s] ais_imp_clk cost_time = [%ld], %lu:%lu", req_session, cost_time, end_time.tv_sec, end_time.tv_usec));

    //conn->apc.url[5] = '/';
    out_string(conn->c, conn->apc.url);

    //if (*session_id!='\0'){
    // printf("SSSSSSSSSSessionid=%s\n",session_id);}
    DEBUG_TRACE(("end ais_imp_clk\n"));
}
/* Send AUTH command to the server */
static int cliAuth(redisContext *context) {
    redisReply *reply;
    if (g_ctx.config[REDIS_PASSWORD] == NULL) return REDIS_OK;

    reply = (redisReply *)slave_redis_command_pending(context,"AUTH %s",g_ctx.config[REDIS_PASSWORD]);
    if (reply != NULL) {
        freeReplyObject(reply);
        return REDIS_OK;
    }
    return REDIS_ERR;
}

char * UrlBase64(char * sessionid,char *  format,char * url, char * Adplatformid)
{
    int size=512;
    char p[512]={0};
    char *rst=(char *)calloc(1,size);
    char *q;
    snprintf(p, sizeof(p), "%s|%s|%s",sessionid,format,url);
    q=base64_encode(p,strlen(p));
    snprintf(rst, sizeof(rst), "%s=!%s",q,Adplatformid);
    free(q);

    return rst;
}

char *base64_encode(const char* data, int data_len)   
{   
    //int data_len = strlen(data);   
    int prepare = 0;   
    int ret_len;   
    int temp = 0;   
    char *ret = NULL;   
    char *f = NULL;   
    int tmp = 0;   
    char changed[4];   
    int i = 0;   
    int j;
    ret_len = data_len / 3;   
    temp = data_len % 3;   
    if (temp > 0)   
    {   
        ret_len += 1;   
    }   
    ret_len = ret_len*4 + 1;   
    ret = (char *)malloc(ret_len);   

    if ( ret == NULL)   
    {   
        printf("No enough memory.\n");   
        exit(0);   
    }   
    memset(ret, 0, ret_len);   
    f = ret;   
    while (tmp < data_len)   
    {   
        temp = 0;   
        prepare = 0;   
        memset(changed, '\0', 4);   
        while (temp < 3)   
        {   
            //printf("tmp = %d\n", tmp);   
            if (tmp >= data_len)   
            {   
                break;   
            }   
            prepare = ((prepare << 8) | (data[tmp] & 0xFF));   
            tmp++;   
            temp++;   
        }   
        prepare = (prepare<<((3-temp)*8));   
        //printf("before for : temp = %d, prepare = %d\n", temp, prepare);   
        for (i = 0; i < 4 ;i++ )   
        {   
            if (temp < i)   
            {   
                changed[i] = 0x40;   
            }   
            else 
            {   
                changed[i] = (prepare>>((3-i)*6)) & 0x3F;   
            }   
            j = changed[i];
            *f = base[j];   
            f++;   
        }   
    }   
    *f = '\0';   

    return ret;   

}   
/* */ 
static char find_pos(char ch)    
{   
    char *ptr = (char*)strrchr(base, ch);//the last position (the only) in base[]   
    return (ptr - base);   
}   
/* */ 
char *base64_decode(const char *data, int data_len)   
{   
    int ret_len = (data_len / 4) * 3;   
    int equal_count = 0;   
    char *ret = NULL;   
    char *f = NULL;   
    int tmp = 0;   
    int temp = 0;   
    char need[3];   
    int prepare = 0;   
    int i = 0;   
    if (*(data + data_len - 1) == '=')   
    {   
        equal_count += 1;   
    }   
    if (*(data + data_len - 2) == '=')   
    {   
        equal_count += 1;   
    }   
    if (*(data + data_len - 3) == '=')   
    {//seems impossible   
        equal_count += 1;   
    }   
    switch (equal_count)   
    {   
        case 0:   
            ret_len += 4;//3 + 1 [1 for NULL]   
            break;   
        case 1:   
            ret_len += 4;//Ceil((6*3)/8)+1   
            break;   
        case 2:   
            ret_len += 3;//Ceil((6*2)/8)+1   
            break;   
        case 3:   
            ret_len += 2;//Ceil((6*1)/8)+1   
            break;   
    }   
    ret = (char *)malloc(ret_len);   
    if (ret == NULL)   
    {   
        printf("No enough memory.\n");   
        exit(0);   
    }   
    memset(ret, 0, ret_len);   
    f = ret;   
    while (tmp < (data_len - equal_count))   
    {   
        temp = 0;   
        prepare = 0;   
        memset(need, 0, 4);   
        while (temp < 4)   
        {   
            if (tmp >= (data_len - equal_count))   
            {   
                break;   
            }   
            prepare = (prepare << 6) | (find_pos(data[tmp]));   
            temp++;   
            tmp++;   
        }   
        prepare = prepare << ((4-temp) * 6);   
        for (i=0; i<3 ;i++ )   
        {   
            if (i == temp)   
            {   
                break;   
            }   
            *f = (char)((prepare>>((2-i)*8)) & 0xFF);   
            f++;   
        }   
    }   
    *f = '\0';   
    return ret;   
} 

/**/
void replace_decode_char(char * buf, int buf_len, int step_len)
{
    if (!buf || buf_len <= 0)
        return;
    int i = 0;
    int mode = buf_len % step_len;
    for (i = 0; i < buf_len - mode; i += step_len) {
        if(isalpha(buf[i]) || isdigit(buf[i])) {
            if (buf[i] == 'A')
                buf[i] = 'Z';
            else if (buf[i] == 'a')
                buf[i] = 'z';
            else if (buf[i] == '0')
                buf[i] = '9';
            else
                buf[i] -= 1;
        }
        else
            buf[i] -= 1;
    }
}
void replace_encode_char(char * buf, int buf_len, int step_len)
{
    if (!buf || buf_len <= 0)
        return;
    int i = 0;
    int mode = buf_len % step_len;
    for (i = 0; i < buf_len - mode; i += step_len) {
        if(isalpha(buf[i]) || isdigit(buf[i])) {
            if (buf[i] == 'A')
                buf[i] = 'Z';
            else if (buf[i] == 'a')
                buf[i] = 'z';
            else if (buf[i] == '0')
                buf[i] = '9';
            else
                buf[i] -= 1;
        }
    }

}

void head_tail_swap(char * buf,int buf_len, int swap_len)
{
    if (!buf || buf_len < (swap_len * 2 + 2))
        return;
    int i = 0;
    char tmp = 0;
    for (i = 0; i < swap_len; i++) {
        tmp = buf[i];
        buf[i] = buf[buf_len- 3 -i];
        buf[buf_len- 3 -i] = tmp;
    }
}

const char *params[] = {"Ah","AppName","Aw","Category","Client", \
    "Density","DeviceNum","Format","Ip","Lat", \
        "Long","Net","Operator","Os_version","Ph", \
        "Pw","Key","TimeStamp","Uuid","Sign", \
        "AdSpaceType","DeviceType","SdkVersion","AppId", \
        "AssUri","AssIp" ,"AdNum",\
        0}; 

#define PARAMSIZE (sizeof(params) / sizeof(params[0]))
static int  ais_get_mask(const char * pp)
{
    const char **p = params;
    int i;
    for( i=0;p[i];i++)
    {
        if (ais_strcasecmp(p[i],pp) ==0)
        {
            break;
        }
    }
    return i;
}
/*
#define AIS_OFLPARAMS_KEY_MASK        1 << 0
#define AID_OFLPARAMS_ADID_MASK       1 << 1
#define AIS_OFLPARAMS_APPNAME_MASK    1 << 2
#define AIS_OFLPARAMS_URL                    1 << 13
#define AIS_OFLPARAMS_ASSURI_MASK     1 << 15
#define AIS_OFLPARAMS_ASSIP_MASK      1 << 16

*/
const char *params_offline[] = { \
    "key", \
    "adid", \
    "adspace", \
    "url",\
    "ver",\
    "sign",\
    "AssUri",\
    "AssIp",\
    0};

#define PARAMSSIZE_OFFLINE (sizeof(params_offline) / sizeof(params_offline[0]))

static int ais_getoffline_mask(const char* key)
{
    const char **p = params_offline;
    int i;
    for( i=0;p[i];i++)
    {
        //DEBUG_TRACE(("p[%d]=%s : key=%s\n", i, p[i], key));
        if (ais_strcasecmp(p[i],key) ==0)
        {
            break;
        }
    }
    DEBUG_TRACE(("ais_getoffline_mask=%d", i));
    return i;
}

const char *params_report[] = {"format", \
    "key", \
        "startdate", \
        "enddate", \
        "sign",\
        "TimeStamp",\
        "groupby"\
        ,0};


const char *params_callback[] = {"format", \
    "sessionid", \
        "callbacktype", \
        "Schema", \
        "AssUri","AssIp","id", "device", "p"\
        ,0};

#define PARAMSIZE_CALLBACK  (sizeof(params_callback) / sizeof(params_callback[0]))
#define PARAMSIZE_REPORT  (sizeof(params_report) / sizeof(params_report[0]))

static int  ais_getcallback_mask(const char * pp)
{
    const char **p = params_callback;
    int i;
    for( i=0;p[i];i++)
    {
        if (ais_strcasecmp(p[i],pp) ==0)
        {
            break;
        }
    }
    return i;
}

static int  ais_getreport_mask(const char * pp)
{
    const char **p = params_report;
    int i;
    for( i=0;p[i];i++)
    {
        if (ais_strcasecmp(p[i],pp) ==0)
        {
            break;
        }
    }
    return i;
}



static int comp_dict(const void *a,const void *b)
{

    return ais_strcasecmp(*(char **)a,*(char **)b);

    //return *(char *)a-*(char *)b;
}

int ais_offline_set(struct ais_connection *conn, const char **parray, int *where)
{
    struct ais_connection *c = conn;
    struct ais_adparams_offline *apo =&(c->apo);
    char *sep = "&"; /*just &*/
    int var_len;
    char *var,*pk,*pv,*s=c->buf;

    char key[256] = {0,};
    int i = 0;
    int j = 0;
    var = (char *)malloc(2048);
    if (var == NULL)    return AIS_ERROR;
    DEBUG_TRACE( ("\r\n\r\nais_offline_set begin [%s]",s)); 
    apo->flags=0;
    while ((pk = mystrtok(&s, sep)) != 0){
        if (( pv = strchr(pk,'=')) !=0){ 
            //*pv = 0;pv++;
            memset(key,0,sizeof(key));
            strncpy(key, pk,((pv-pk)>=255)?255:(pv-pk));
            if (1){//(strlen(key) >= 4 && key[0] == 't' && key[1] == 'x' && key[2] == 't' )
                if ((i = ais_getoffline_mask(key) )== PARAMSSIZE_OFFLINE-1 ){
                    DEBUG_TRACE(("unknown args for [%s]", key));
                    var_len = ais_get_var( pk, strlen(pk), key, var, 2048);    
                    if (var_len == -1)
                    {
                        *where = PARAMETER_CONTAINING_ILLEGAL_CHARACTERS;
                        goto    ais_error;
                    }
                    var[var_len] = '\0'; // Null-terminate the destination
                    if (var_len > 0 && j < 64 && i != 5 && i != 6 && i != 7) { parray[j] =  pk ; j++;}
                    continue;
                    //goto ais_error;
                }

                var_len = ais_get_var( pk, strlen(pk), key, var, 2048);    
                if (var_len == -1)
                {
                    *where = PARAMETER_CONTAINING_ILLEGAL_CHARACTERS;
                    goto    ais_error;
                }
                var[var_len] = '\0'; // Null-terminate the destination
                if (var_len > 0 && j < 64 && i != 5 && i != 6 && i != 7) { parray[j] =  pk ; j++;}

                DEBUG_TRACE( ("key: [%s] Value: [%s] Value size: [%d]", key,var,var_len)); 
                if (apo->flags & 1<<i)
                {
                    DEBUG_TRACE(("repeat args for %s",key));
                    goto    ais_error;
                }
                
#define  DOSOMETHING2(m)        if (c->adparams_buf+c->buf_size - c->adparams_buf_last <var_len +2)\
                {\
                    DEBUG_TRACE(("ad params buf is full"));\
                    goto    ais_error;\
                }\
                apo->m = c->adparams_buf_last;\
                c->adparams_buf_last = my_strlcpy(apo->m,var,var_len);
#define     ALLDIGORERROR(var)        if (!alldig(var)) \
                { \
                    goto    ais_error;\
                }
#define     LENORERROR(var_len)        if (!var_len) \
                { \
                    goto    ais_error;\
                }
#define     OVERLONGERROR(var_len, len) if ((var_len) > (len))\
                {\
                    goto    ais_error;\
                }

                *where = PARAMETRER_NOTFULL;
                switch(i)
                {
                    case 0://key
                        *where = KEY_EMPTY_PARAMETRER_ILLEGAL;
                        LENORERROR(var_len)
                            if (c->adparams_buf+c->buf_size - c->adparams_buf_last <var_len +2)
                            {
                                DEBUG_TRACE(("ad params buf is full"));
                                goto    ais_error;
                            }
                        apo->key = c->adparams_buf_last;
                        c->adparams_buf_last = my_strlcpy(apo->key,var,var_len);
                        break;

                    case 1:  //adid
                        *where = ADID_EMPTY_PARAMETRER_ILLEGAL;
                        LENORERROR(var_len)
                            if (c->adparams_buf+c->buf_size - c->adparams_buf_last <var_len +2)
                            {
                                DEBUG_TRACE(("ad params buf is full"));
                                goto    ais_error;
                            }
                        apo->adid = c->adparams_buf_last;
                        c->adparams_buf_last = my_strlcpy(apo->adid,var,var_len);
                        break;

                    case 2:   //adspace
                        *where = APPNAME_EMPTY_PARAMETRER_ILLEGAL;
                        LENORERROR(var_len)
                        if (c->adparams_buf+c->buf_size - c->adparams_buf_last <var_len +2)
                            {
                                DEBUG_TRACE(("ad params buf is full"));
                                goto    ais_error;
                            }
                        apo->adspace= c->adparams_buf_last;
                        c->adparams_buf_last = my_strlcpy(apo->adspace,var,var_len);
                        *where = APPNAME_NAN_PARAMETRER_ILLEGAL;
                        OVERLONGERROR(var_len, APPNAME_LEN)
                        break;

                    case 3: //url
                        if (c->adparams_buf+c->buf_size - c->adparams_buf_last <var_len +2)
                        {
                            DEBUG_TRACE(("ad params buf is full"));
                            goto    ais_error;
                        }
                        apo->url = c->adparams_buf_last;
                        c->adparams_buf_last = my_strlcpy(apo->url,var,var_len);
                        *where = URL_NAN_PARAMETRER_ILLEGAL;
                        //OVERLONGERROR(var_len, DEVICENUM_LEN);
                        break;

                    case 4: //ver
                        *where = VER_NAN_PARAMETRER_ILLEGAL;
                        if (c->adparams_buf+c->buf_size - c->adparams_buf_last <var_len +2)
                        {
                            DEBUG_TRACE(("ad params buf is full"));
                            goto    ais_error;
                        }
                        apo->ver = c->adparams_buf_last;
                        c->adparams_buf_last = my_strlcpy(apo->ver,var,var_len);
                        
                        break;
                    case 5: //sign
                        *where = SIGN_EMPTY_PARAMETRER_ILLEGAL;
                        LENORERROR(var_len)
                        if (c->adparams_buf+c->buf_size - c->adparams_buf_last <var_len +2)
                        {
                            DEBUG_TRACE(("ad params buf is full"));
                            goto	ais_error;
                        }
                        apo->sign= c->adparams_buf_last;
                        c->adparams_buf_last = my_strlcpy(apo->sign,var,var_len);
                        break;
                    case 6://AssUri
                        LENORERROR(var_len)
                        if (c->adparams_buf+c->buf_size - c->adparams_buf_last <var_len +2)
                        {
                            DEBUG_TRACE(("ad params buf is full"));
                            goto    ais_error;
                        }
                        apo->uri= c->adparams_buf_last;
                        c->adparams_buf_last = my_strlcpy(apo->uri,var,var_len);
                        break;
                    case 7://AssIp
                        LENORERROR(var_len)
                        if (c->adparams_buf+c->buf_size - c->adparams_buf_last <var_len +2)
                        {
                            DEBUG_TRACE(("ad params buf is full"));
                            goto ais_error;
                        }
                        apo->nip= c->adparams_buf_last;
                        c->adparams_buf_last = my_strlcpy(apo->nip,var,var_len);
                        break;

                    default:
                        DEBUG_TRACE(("unable reach  here!!!!!!!"));
                        goto    ais_error;

                }
                apo->flags|= 1<<i;        
            }
        }
    }
    if (*( conn->ctx->config[OPEN_SIGN] ) == 'y' ){
    	qsort(parray,j,sizeof( char * ),comp_dict);
    }
    DEBUG_TRACE( ("ais_offline_set end\r\n\r\n")); 
    free(var);
    return AIS_OK;
ais_error:
    free(var);
    return AIS_ERROR;
}

int ais_adreport_set(struct ais_connection * conn ,const char ** parray ,int *where)
{

    struct ais_connection *c = conn;
    struct ais_adparams_report*ap =&(c->apr);
    char *sep = "&"; /*just &*/
    int var_len;
    char *var,*pk,*pv,*s=c->buf;

    char key[256] = {0,};
    int i = 0;
    int j = 0;
    var = (char *)malloc(2048);
    if (var == NULL)	return AIS_ERROR;
    DEBUG_TRACE( ("\r\n\r\nais_adreport_set begin [%s]",s)); 
    ap->flags=0;
    while ((pk = mystrtok(&s, sep)) != 0){
        if (( pv = strchr(pk,'=')) !=0){ 
            //*pv = 0;pv++;
            memset(key,0,sizeof(key));
            strncpy(key, pk,((pv-pk)>=255)?255:(pv-pk));
            if (1){//(strlen(key) >= 4 && key[0] == 't' && key[1] == 'x' && key[2] == 't' )
                if ((i = ais_getreport_mask(key) )== PARAMSIZE_REPORT-1 ){
                    DEBUG_TRACE(("unknown args for [%s]", key));
                    var_len = ais_get_var( pk, strlen(pk), key, var, 2048);	
                    if (var_len == -1)
                    {
                        *where = PARAMETER_CONTAINING_ILLEGAL_CHARACTERS;
                        goto	ais_error;
                    }
                    var[var_len] = '\0'; // Null-terminate the destination
                    	if (var_len > 0 && j < 64 && i != 6 && i != 7 && i != 4) { parray[j] =  pk ; j++;}
                    continue;
                    //goto ais_error;
                }

                var_len = ais_get_var( pk, strlen(pk), key, var, 2048);	
                if (var_len == -1)
                {
                    *where = PARAMETER_CONTAINING_ILLEGAL_CHARACTERS;
                    goto	ais_error;
                }
                var[var_len] = '\0'; // Null-terminate the destination
                	if (var_len > 0 && j < 64 && i != 6 && i != 7 && i != 4) { parray[j] =  pk ; j++;}

                DEBUG_TRACE( ("key: [%s] Value: [%s] Value size: [%d]", key,var,var_len)); 
                if (ap->flags & 1<<i)
                {
                    DEBUG_TRACE(("repeat args for %s",key));
                    goto	ais_error;
                }
                /*

                   const char *params[] = {"txtSdkID","txtAppPackage","txtAppName","txtUuid",\
                   "txtClient","txtOperator","txtNet","txtIp",\
                   "txtLong","txtLat","txtCategory","txtOs_version",\
                   "txtDensity","txtAw","txtAh","txtPw",\
                   "txtPh","txtDevice",\
                   0}; */
#define  DOSOMETHING(m)		if (c->adparams_buf+c->buf_size - c->adparams_buf_last <var_len +2)\
                {\
                    DEBUG_TRACE(("ad params buf is full"));\
                    goto	ais_error;\
                }\
                ap->m = c->adparams_buf_last;\
                c->adparams_buf_last = my_strlcpy(ap->m,var,var_len);	
#define 	ALLDIGORERROR(var)		if (!alldig(var)) \
                { \
                    goto	ais_error;\
                }
#define 	LENORERROR(var_len)		if (!var_len) \
                { \
                    goto	ais_error;\
                }


                /*
                   char * Format ;
                   char * Schema ;
                   char * sessionid ;
                   char * callbacktype ;
                   */
                *where = PARAMETRER_NOTFULL;
                switch(i)
                {
                    case 0:
                        // LENORERROR(var_len)
                        DOSOMETHING(Format);
                        break;





                    case 1://txtUuid
                        *where = KEY_EMPTY_PARAMETRER_ILLEGAL;
                        LENORERROR(var_len)
                            if (c->adparams_buf+c->buf_size - c->adparams_buf_last <var_len +2)
                            {
                                DEBUG_TRACE(("ad params buf is full"));
                                goto	ais_error;
                            }
                        ap->Key = c->adparams_buf_last;//atoi(var);
                        c->adparams_buf_last = my_strlcpy(ap->Key,var,var_len);
                        break;

                    case 2:
                        *where = STARTDATE_EMPTY;
                        LENORERROR(var_len)
                            //	ALLDIGORERROR(var)	
                            //ap->callbacktype= atoi(var); 
                            DOSOMETHING(Startdate);
                        break;
                        /*
                           char * Schema ;
                           char * CId ;
                           char * Ip ;
                           char * Category ;
                           char * DeviceType ;
                           char * SdkVersion ;
                           char * AppId ;

*/
                    case 3:
                        *where=ENDDATE_EMPTY;
                        LENORERROR(var_len)
                            //ap->callbacktype= atoi(var); 
                            DOSOMETHING(Enddate);
                        break;

                    case 4://AssUri
                        *where=SIGN_EMPTY_PARAMETRER_ILLEGAL;
                        LENORERROR(var_len)
                            if (c->adparams_buf+c->buf_size - c->adparams_buf_last <var_len +2)
                            {
                                DEBUG_TRACE(("ad params buf is full"));
                                goto	ais_error;
                            }
                        ap->Sign= c->adparams_buf_last;
                        c->adparams_buf_last = my_strlcpy(ap->Sign,var,var_len);
                        break;
                    case 5://AssUri
                        *where=TIMESTAMP_EMPTY_PARAMETRER_ILLEGAL;
                        LENORERROR(var_len)
                            if (c->adparams_buf+c->buf_size - c->adparams_buf_last <var_len +2)
                            {
                                DEBUG_TRACE(("ad params buf is full"));
                                goto	ais_error;
                            }
                        ap->TimeStamp= c->adparams_buf_last;
                        c->adparams_buf_last = my_strlcpy(ap->TimeStamp,var,var_len);
                        break;
			case 6:
				DOSOMETHING(groupby);
                        break;
                    default:
                        DEBUG_TRACE(("unable reach  here!!!!!!!"));
                        goto	ais_error;

                }
                ap->flags|= 1<<i;		
            }
        }
    }
    if (*( conn->ctx->config[OPEN_SIGN] ) == 'y' ){
    	qsort(parray,j,sizeof( char * ),comp_dict);}
    DEBUG_TRACE( ("ais_adreport_set end\r\n\r\n")); 
    free(var);
    return AIS_OK;
ais_error:
    free(var);
    return AIS_ERROR;
}


int ais_adcallback_set(struct ais_connection * conn ,const char ** parray ,int *where)
{

    struct ais_connection *c = conn;
    struct ais_adparams_callback*ap =&(c->apc);
    char *sep = "&"; /*just &*/
    int var_len;
    char *var,*pk,*pv,*s=c->buf;

    char key[256] = {0,};
    int i = 0;
    var = (char *)malloc(2048);
    if (var == NULL)	return AIS_ERROR;
    DEBUG_TRACE( ("\r\n\r\nais_adcallback_set begin [%s]",s)); 
    while ((pk = mystrtok(&s, sep)) != 0){
        if (( pv = strchr(pk,'=')) !=0){ 
            //*pv = 0;pv++;
            memset(key,0,sizeof(key));
            strncpy(key, pk,((pv-pk)>=255)?255:(pv-pk));
            if (1){//(strlen(key) >= 4 && key[0] == 't' && key[1] == 'x' && key[2] == 't' )
                if ((i = ais_getcallback_mask(key) )== PARAMSIZE_CALLBACK-1 ){
                    DEBUG_TRACE(("unknown args for [%s]", key));
                    var_len = ais_get_var( pk, strlen(pk), key, var, 2048);	
                    if (var_len == -1)
                    {
                        *where = PARAMETER_CONTAINING_ILLEGAL_CHARACTERS;
                        goto	ais_error;
                    }
                    var[var_len] = '\0'; // Null-terminate the destination
                    //	if (var_len > 0 && j < 64 && i != 26 && i != 27 && i != 2) { parray[j] =  pk ; j++;}
                    continue;
                    //goto ais_error;
                }

                var_len = ais_get_var( pk, strlen(pk), key, var, 2048);	
                if (var_len == -1)
                {
                    *where = PARAMETER_CONTAINING_ILLEGAL_CHARACTERS;
                    goto	ais_error;
                }
                var[var_len] = '\0'; // Null-terminate the destination
                //	if (var_len > 0 && j < 64 && i != 26 && i != 27 && i != 2) { parray[j] =  pk ; j++;}

                DEBUG_TRACE( ("key: [%s] Value: [%s] Value size: [%d]", key,var,var_len)); 
                if (ap->flags & 1<<i)
                {
                    DEBUG_TRACE(("repeat args for %s",key));
                    goto	ais_error;
                }
                /*

                   const char *params[] = {"txtSdkID","txtAppPackage","txtAppName","txtUuid",\
                   "txtClient","txtOperator","txtNet","txtIp",\
                   "txtLong","txtLat","txtCategory","txtOs_version",\
                   "txtDensity","txtAw","txtAh","txtPw",\
                   "txtPh","txtDevice",\
                   0}; */
#define  DOSOMETHING(m)		if (c->adparams_buf+c->buf_size - c->adparams_buf_last <var_len +2)\
                {\
                    DEBUG_TRACE(("ad params buf is full"));\
                    goto	ais_error;\
                }\
                ap->m = c->adparams_buf_last;\
                c->adparams_buf_last = my_strlcpy(ap->m,var,var_len);	
#define 	ALLDIGORERROR(var)		if (!alldig(var)) \
                { \
                    goto	ais_error;\
                }
#define 	LENORERROR(var_len)		if (!var_len) \
                { \
                    goto	ais_error;\
                }


                /*
                   char * Format ;
                   char * Schema ;
                   char * sessionid ;
                   char * callbacktype ;
                   */
                *where = PARAMETRER_NOTFULL;
                switch(i)
                {
                    case 0:
                        // LENORERROR(var_len)
                        DOSOMETHING(Format);
                        break;

                    case 1://txtUuid
                        *where = SESSIONID_EMPTY_PARAMETRER_ILLEGAL;
                        LENORERROR(var_len)
                            if (c->adparams_buf+c->buf_size - c->adparams_buf_last <var_len +2)
                            {
                                DEBUG_TRACE(("ad params buf is full"));
                                goto	ais_error;
                            }
                        ap->sessionid = c->adparams_buf_last;//atoi(var);
                        c->adparams_buf_last = my_strlcpy(ap->sessionid,var,var_len);
                        break;

                    case 2:
                        *where = CALLBACKTYPE_EMPTY_PARAMETRER_ILLEGAL;
                        LENORERROR(var_len)
                            //	ALLDIGORERROR(var)	
                            //ap->callbacktype= atoi(var); 
                            DOSOMETHING(callbacktype);
                        break;
                        /*
                           char * Schema ;
                           char * CId ;
                           char * Ip ;
                           char * Category ;
                           char * DeviceType ;
                           char * SdkVersion ;
                           char * AppId ;

*/
                    case 3:

                        LENORERROR(var_len)
                            //ap->callbacktype= atoi(var); 
                            DOSOMETHING(Schema);
                        break;

                    case 4://AssUri
                        LENORERROR(var_len)
                            if (c->adparams_buf+c->buf_size - c->adparams_buf_last <var_len +2)
                            {
                                DEBUG_TRACE(("ad params buf is full"));
                                goto	ais_error;
                            }
                        ap->uri= c->adparams_buf_last;
                        c->adparams_buf_last = my_strlcpy(ap->uri,var,var_len);
                        break;
                    case 5://AssIp
                        LENORERROR(var_len)
                            if (c->adparams_buf+c->buf_size - c->adparams_buf_last <var_len +2)
                            {
                                DEBUG_TRACE(("ad params buf is full"));
                                goto	ais_error;
                            }
                        ap->nip= c->adparams_buf_last;
                        c->adparams_buf_last = my_strlcpy(ap->nip,var,var_len);
                        break;
                    case 6://id
                        LENORERROR(var_len)
                            if (c->adparams_buf+c->buf_size - c->adparams_buf_last <var_len +2)
                            {
                                DEBUG_TRACE(("ad params buf is full"));
                                goto	ais_error;
                            }
                        ap->id= c->adparams_buf_last;
                        c->adparams_buf_last = my_strlcpy(ap->id,var,var_len);
                        break;
                    case 7://device
                        //LENORERROR(var_len)
                        //ap->callbacktype= atoi(var); 
                        DOSOMETHING(device);
                        break;
                    case 8://pid
                        //LENORERROR(var_len)
                        ALLDIGORERROR(var);
                        conn->app.PlatformProviderID = c->adparams_buf_last;
                        c->adparams_buf_last = my_strlcpy(conn->app.PlatformProviderID, var, var_len);
                        break;
                    default:
                        DEBUG_TRACE(("unable reach  here!!!!!!!"));
                        goto	ais_error;

                }
                ap->flags|= 1<<i;		
            }
        }
    }
    //if (*( conn->ctx->config[OPEN_SIGN] ) == 'y' ){
    //	qsort(parray,j,sizeof( char * ),comp_dict);}
    DEBUG_TRACE( ("ais_adcallback_set end\r\n\r\n")); 
    free(var);
    return AIS_OK;
ais_error:
    free(var);
    return AIS_ERROR;
}

int is_ip_adress(char *s)
{	
    if (!s)return -1;
    char *t = s;
    while(*t)
    {
        if ( ISDIGIT(*t) != 0 || *t == '.' )
        {
            ;
        }
        else
        {
            return -1;
        }
        t++;
    }
    int a,b,c,d;
    if (4==sscanf(s,"%d.%d.%d.%d",&a,&b,&c,&d)) {
        if (0<=a && a<=255
                && 0<=b && b<=255
                && 0<=c && c<=255
                && 0<=d && d<=255) {
            return 0;
        } else {
            return -1;
        }
    } else {
        return -1;
    }

}
int  ais_ad_set(struct ais_connection * conn,const char ** parray , int * where, char *req_session)
{
    struct timeval start_time;
    struct timeval end_time;
    long cost_time = 0;
    gettimeofday(&start_time, NULL);

    struct ais_connection *c = conn;
    struct ais_adparams *ap =&(c->ap);
    char *sep = "&"; /*just &*/
    int var_len;	
    char var[2048],*pk,*pv,*s=c->buf;
    char key[256] = {0,};
    int i = 0;
    int j = 0;
    //char *parray[MAX_DEMli] ={NULL,};
    //var = malloc(2048);
    //	if (var == NULL)	return AIS_ERROR;
    /*	var_len = url_decode(c->bufr,c->bufr_len,ss,10240,1);
        if (var_len == -1)
        {
     *where = PARAMETER_CONTAINING_ILLEGAL_CHARACTERS;
     goto	ais_error;
     }*/

    DEBUG_TRACE( ("\r\n\r\nais_ad_set begin [%s]",s)); 
    while ((pk = mystrtok(&s, sep)) != 0){
        /*----------------------------*/
        gettimeofday(&end_time, NULL);
        cost_time = (end_time.tv_sec - start_time.tv_sec)* 1000000 + end_time.tv_usec - start_time.tv_usec;
        COST_TIME_ERROR_LOG( ("++++++>[%s] %d mystrtok cost_time = [%ld], %lu:%lu", req_session, i, cost_time, end_time.tv_sec, end_time.tv_usec));
        gettimeofday(&start_time, NULL);
        if (( pv = strchr(pk,'=')) !=0){ 
            /*---------------------------------*/
            gettimeofday(&end_time, NULL);
            cost_time = (end_time.tv_sec - start_time.tv_sec)* 1000000 + end_time.tv_usec - start_time.tv_usec;
            COST_TIME_ERROR_LOG( ("++++++>[%s] %d strchr cost_time = [%ld], %lu:%lu", req_session, i, cost_time, end_time.tv_sec, end_time.tv_usec));
            gettimeofday(&start_time, NULL);

            //*pv = 0;pv++;
            memset(key,0,sizeof(key));
            strncpy(key, pk,((pv-pk)>=255)?255:(pv-pk));
            if (1){//(strlen(key) >= 4 && key[0] == 't' && key[1] == 'x' && key[2] == 't' )
                if ((i = ais_get_mask(key) )== PARAMSIZE-1 ){
                    /*---------------------------------*/
                    gettimeofday(&end_time, NULL);
                    cost_time = (end_time.tv_sec - start_time.tv_sec)* 1000000 + end_time.tv_usec - start_time.tv_usec;
                    COST_TIME_ERROR_LOG( ("++++++>[%s] %d ais_get_mask cost_time = [%ld], %lu:%lu", req_session, i, cost_time, end_time.tv_sec, end_time.tv_usec));
                    gettimeofday(&start_time, NULL);
                    DEBUG_TRACE(("unknown args for [%s]", key));

                    /*---------------------------------*/
                    var_len = ais_get_var( pk, strlen(pk), key, var, 2048);
                    gettimeofday(&end_time, NULL);
                    cost_time = (end_time.tv_sec - start_time.tv_sec)* 1000000 + end_time.tv_usec - start_time.tv_usec;
                    COST_TIME_ERROR_LOG( ("++++++>[%s] %d ais_get_var1 cost_time = [%ld], %lu:%lu", req_session, i, cost_time, end_time.tv_sec, end_time.tv_usec));
                    gettimeofday(&start_time, NULL);
                    if (var_len == -1)
                    {
                        *where = PARAMETER_CONTAINING_ILLEGAL_CHARACTERS;
                        goto	ais_error;
                    }
                    var[var_len] = '\0'; // Null-terminate the destination
                    if (var_len > 0 && j < 64 && i != 24 && i != 25 && i != 19) { parray[j] =  pk ; j++;}
                    continue;
                    //goto ais_error;
                }

                var_len = ais_get_var( pk, strlen(pk), key, var, 2048);
                gettimeofday(&end_time, NULL);
                cost_time = (end_time.tv_sec - start_time.tv_sec)* 1000000 + end_time.tv_usec - start_time.tv_usec;
                COST_TIME_ERROR_LOG( ("++++++>[%s] %d ais_get_var2 cost_time = [%ld], %lu:%lu", req_session, i, cost_time, end_time.tv_sec, end_time.tv_usec));
                gettimeofday(&start_time, NULL);
                if (var_len == -1)
                {
                    *where = PARAMETER_CONTAINING_ILLEGAL_CHARACTERS;
                    goto	ais_error;
                }
                var[var_len] = '\0'; // Null-terminate the destination
                if (var_len > 0 && j < 64 && i != 24 && i != 25 && i != 19) { parray[j] = pk ; j++;}

                DEBUG_TRACE( ("key: [%s] Value: [%s] Value size: [%d]", key,var,var_len)); 
                if (ap->flags & 1<<i)
                {
                    DEBUG_TRACE(("repeat args for %s",key));
                    goto	ais_error;
                }

#define  DOSOMETHING(m)		if (c->adparams_buf+c->buf_size - c->adparams_buf_last <var_len +2)\
                {\
                    DEBUG_TRACE(("ad params buf is full"));\
                    goto	ais_error;\
                }\
                ap->m = c->adparams_buf_last;\
                c->adparams_buf_last = my_strlcpy(ap->m,var,var_len);	
#define 	ALLDIGORERROR(var)		if (!alldig(var)) \
                { \
                    goto	ais_error;\
                }
#define 	LENORERROR(var_len)		if (!var_len) \
                { \
                    goto	ais_error;\
                }
#define     OVERLONGERROR(var_len, len) if ((var_len) > (len))\
                {\
                    goto    ais_error;\
                }

                gettimeofday(&end_time, NULL);
                cost_time = (end_time.tv_sec - start_time.tv_sec)* 1000000 + end_time.tv_usec - start_time.tv_usec;
                COST_TIME_ERROR_LOG( ("++++++>[%s] %d cost_time = [%ld], %lu:%lu", req_session, i, cost_time, end_time.tv_sec, end_time.tv_usec));
                gettimeofday(&start_time, NULL);
                                
                *where = PARAMETRER_NOTFULL;
                switch(i)
                {

                    case 0: //txtAh	
                        *where = AH_EMPTY_PARAMETRER_ILLEGAL;
                        LENORERROR(var_len)
                            *where = AH_NAN_PARAMETRER_ILLEGAL;
                        OVERLONGERROR(var_len, AH_LEN);
                        ALLDIGORERROR(var);
                        DOSOMETHING(Ah);
                        break;

                    case 1://txtAppName
                        //LENORERROR(var_len)
                        if (c->adparams_buf+c->buf_size - c->adparams_buf_last <var_len +2)
                        {
                            DEBUG_TRACE(("ad params buf is full"));
                            goto	ais_error;
                        }
                        ap->AppName = c->adparams_buf_last;//atoi(var);
                        c->adparams_buf_last = my_strlcpy(ap->AppName,var,var_len);
                        *where = APPNAME_NAN_PARAMETRER_ILLEGAL;
                        OVERLONGERROR(var_len, APPNAME_LEN);
                        break;

                    case 2: //txtAw
                        *where = AW_EMPTY_PARAMETRER_ILLEGAL;
                        LENORERROR(var_len)
                            *where = AW_NAN_PARAMETRER_ILLEGAL;
                        OVERLONGERROR(var_len, AW_LEN);
                        ALLDIGORERROR(var);
                        //ap->aw = atoi(var);
                        DOSOMETHING(Aw);
                        break;

                    case 3: //txtCategory
                        *where = CATEGORY_EMPTY_PARAMETRER_ILLEGAL;
                        LENORERROR(var_len)
                            *where = CATEGORY_NAN_PARAMETRER_ILLEGAL;
                        OVERLONGERROR(var_len, CATEGORY_LEN);
                        ALLDIGORERROR(var)	
                            //ap->category= atoi(var);
                        DOSOMETHING(Category);
                        if (NULL==ap->Category)
                        {
                            ap->Category = DEF_CATEGORY;
                        }
                        break;

                    case 4: //txtClient
                        *where = CLIENT_EMPTY_PARAMETRER_ILLEGAL;
                        LENORERROR(var_len)
                            *where = CLIENT_NAN_PARAMETRER_ILLEGAL;
                        OVERLONGERROR(var_len, CLIENT_LEN);
                        ALLDIGORERROR(var)	
                            //ap->client= atoi(var); 
                            DOSOMETHING(Client);
                        break;

                    case 5://txtDensity
                        //LENORERROR(var_len)
                        //ap->density= atoi(var);
                        *where = DENSITY_NAN_PARAMETRER_ILLEGAL;
                        OVERLONGERROR(var_len, DENSITY_LEN);
                        DOSOMETHING(Density);
                        break;

                    case 6://txtDevice
                        if (c->adparams_buf+c->buf_size - c->adparams_buf_last <var_len +2)
                        {
                            DEBUG_TRACE(("ad params buf is full"));
                            goto	ais_error;
                        }
                        ap->DeviceNum = c->adparams_buf_last;
                        c->adparams_buf_last = my_strlcpy(ap->DeviceNum,var,var_len);
                        *where = DEVICENUM_NAN_PARAMETRER_ILLEGAL;
                        OVERLONGERROR(var_len, DEVICENUM_LEN);
                        break;

                    case 7://"Format"
                        if (c->adparams_buf+c->buf_size - c->adparams_buf_last <var_len +2)
                        {
                            DEBUG_TRACE(("ad params buf is full"));
                            goto	ais_error;
                        }
                        ap->Format = c->adparams_buf_last;
                        c->adparams_buf_last = my_strlcpy(ap->Format,var,var_len);
                        ap->Format = const_cast<char *>((ap->Format && ais_strcasecmp( ap->Format, "xml") == 0) ? "xml" : "json");
                        *where = FORMAT_NAN_PARAMETRER_ILLEGAL;
                        OVERLONGERROR(var_len, FORMAT_LEN);
                        break;


                    case 8://txtIp
                        *where = IP_EMPTY_PARAMETRER_ILLEGAL;
                        LENORERROR(var_len)
                            if (c->adparams_buf+c->buf_size - c->adparams_buf_last <var_len +2)
                            {
                                DEBUG_TRACE(("ad params buf is full"));
                                goto	ais_error;
                            }
                        if (is_ip_adress(var))
                        {	
                            DEBUG_TRACE(("is_ip_adress error"));
                            goto	ais_error;
                        }
                        ap->Ip = c->adparams_buf_last;//atoi(var);

                        c->adparams_buf_last = my_strlcpy(ap->Ip,var,var_len);
                        *where = IP_NAN_PARAMETRER_ILLEGAL;
                        OVERLONGERROR(var_len, IP_LEN);
                        break;

                    case 9://txtLat
                        //ap->lat = atoi(var);
                        *where = LAT_NAN_PARAMETRER_ILLEGAL;
                        OVERLONGERROR(var_len, LAT_LEN);
                        DOSOMETHING(Lat);
                        //sscanf(var,"%lf",&(ap->lat));
                        break;
                        /*------------------------------------------------------------------------------*/


                    case 10://txtLong
                        //ap->longg= atoi(var);
                        *where = LONG_NAN_PARAMETRER_ILLEGAL;
                        OVERLONGERROR(var_len, LONG_LEN);
                        DOSOMETHING(Long);
                        //sscanf(var,"%lf",&(ap->longg));
                        break;

                    case 11://txtNet
                        //LENORERROR(var_len)	
                        if (!var_len )
                        {
                            ap->Net  = "3";
                        }
                        else
                        {
                            *where = NET_NAN_PARAMETRER_ILLEGAL;
                            OVERLONGERROR(var_len, NET_LEN);
                            ALLDIGORERROR(var)
                                DOSOMETHING(Net);
                        }//ap->net = atoi(var);

                        break;

                    case 12://txtOperator operator key words
                        *where = OPERATOR_EMPTY_PARAMETRER_ILLEGAL;
                        LENORERROR(var_len)
                            *where = OPERATOR_NAN_PARAMETRER_ILLEGAL;
                        OVERLONGERROR(var_len,  OPERATOR_LEN);
                        ALLDIGORERROR(var)	
                            //ap->oprter = atoi(var);
                            DOSOMETHING(Operator);
                        break;

                    case 13: //txtOs_version
                        //LENORERROR(var_len)
                        if (!var_len)
                        {
                            ap->Os_version = unknown;
                        }
                        else
                        {
                            if (c->adparams_buf+c->buf_size - c->adparams_buf_last <var_len +2)
                            {
                                DEBUG_TRACE(("ad params buf is full"));
                                goto	ais_error;
                            }
                            ap->Os_version = c->adparams_buf_last;
                            c->adparams_buf_last = my_strlcpy(ap->Os_version,var,var_len);
                            *where = OS_VERSION_NAN_PARAMETRER_ILLEGAL;
                            OVERLONGERROR(var_len,  OS_VERSION_LEN);
                        }
                        break;


                    case 14://txtPh
                        //ap->ph = atoi(var);
                        *where = PH_NAN_PARAMETRER_ILLEGAL;
                        if (var_len)
                        {
                            OVERLONGERROR(var_len, PH_LEN);
                            ALLDIGORERROR(var);
                        }
                        DOSOMETHING(Ph);
                        break;

                        /*------------------------------------------------------------------------------*/

                    case 15://txtPw
                        //ap->pw = atoi(var);
                        *where = PW_NAN_PARAMETRER_ILLEGAL;
                        if (var_len)
                        {
                            OVERLONGERROR(var_len, PW_LEN);
                            ALLDIGORERROR(var);
                        }
                        DOSOMETHING(Pw);
                        break;

                    case 16:
                        *where = KEY_EMPTY_PARAMETRER_ILLEGAL;
                        LENORERROR(var_len)
                            DOSOMETHING(Key);
                        break;


                    case 17:
                        *where = TIMESTAMP_EMPTY_PARAMETRER_ILLEGAL;
                        LENORERROR(var_len)
                            if (c->adparams_buf+c->buf_size - c->adparams_buf_last <var_len +2)
                            {
                                DEBUG_TRACE(("ad params buf is full"));
                                goto	ais_error;
                            }
                        ap->TimeStamp= c->adparams_buf_last;
                        c->adparams_buf_last = my_strlcpy(ap->TimeStamp,var,var_len);
                        break;


                    case 18://txtUuid
                        *where = UUID_EMPTY_PARAMETRER_ILLEGAL;
                        LENORERROR(var_len)
                            if (c->adparams_buf+c->buf_size - c->adparams_buf_last <var_len +2)
                            {
                                DEBUG_TRACE(("ad params buf is full"));
                                goto	ais_error;
                            }
                        ap->Uuid = c->adparams_buf_last;//atoi(var);
                        c->adparams_buf_last = my_strlcpy(ap->Uuid,var,var_len);
                        *where = UUID_NAN_PARAMETRER_ILLEGAL;
                        OVERLONGERROR(var_len, UUID_LEN);
                        break;

                    case 19:
                        *where = SIGN_EMPTY_PARAMETRER_ILLEGAL;
                        LENORERROR(var_len)
                            if (c->adparams_buf+c->buf_size - c->adparams_buf_last <var_len +2)
                            {
                                DEBUG_TRACE(("ad params buf is full"));
                                goto	ais_error;
                            }
                        ap->Sign= c->adparams_buf_last;
                        c->adparams_buf_last = my_strlcpy(ap->Sign,var,var_len);
                        break;

                        /*------------------------------------------------------------------------------*/
                    case 20:
                        *where = ADSPACETYPE_EMPTY;
                        LENORERROR(var_len)
                            *where = ADSPACETYPE_NAN_PARAMETRER_ILLEGAL;
                        OVERLONGERROR(var_len, ADSPACETYPE_LEN);
                        DOSOMETHING(AdSpaceType);
						if(ap->AdSpaceType){
							if(atoi(ap->AdSpaceType) == 0)
								goto ais_error;
						}
                        break;

                    case 21:
                        if (!var_len) 
                        { 
                            ap->DeviceType = "1";
                        }
                        else
                        {
                            *where = DEVICETYPE_NAN_PARAMETRER_ILLEGAL;
                            OVERLONGERROR(var_len, DEVICETYPE_LEN);
                            DOSOMETHING(DeviceType);
                        }
                        break;

                    case 22:
                        if (!var_len) 
                        { 
                            ap->SdkVersion = unknown;
                        }
                        else
                        {
                            *where = SDKVERSION_NAN_PARAMETRER_ILLEGAL;
                            OVERLONGERROR(var_len, SDKVERSION_LEN);
                            DOSOMETHING(SdkVersion);
                        }
                        break;

                    case 23:
                        *where=APPID_EMPTY;
                        LENORERROR(var_len);
                        *where = APPID_NAN_PARAMETRER_ILLEGAL;
                        OVERLONGERROR(var_len, APPID_LEN);
                        DOSOMETHING(AppId);
                        break;

                    case 24://AssUri
                        LENORERROR(var_len)
                            if (c->adparams_buf+c->buf_size - c->adparams_buf_last <var_len +2)
                            {
                                DEBUG_TRACE(("ad params buf is full"));
                                goto	ais_error;
                            }
                        ap->uri= c->adparams_buf_last;
                        c->adparams_buf_last = my_strlcpy(ap->uri,var,var_len);
                        break;
                    case 25://AssIp
                        LENORERROR(var_len)
                            if (c->adparams_buf+c->buf_size - c->adparams_buf_last <var_len +2)
                            {
                                DEBUG_TRACE(("ad params buf is full"));
                                goto	ais_error;
                            }
                        ap->nip= c->adparams_buf_last;
                        c->adparams_buf_last = my_strlcpy(ap->nip,var,var_len);
                        break;
                    case 26://AssIp
                        DOSOMETHING(AdNum);
                        break;
                    default:
                        DEBUG_TRACE(("unable reach  here!!!!!!!"));
                        goto	ais_error;

                }
            gettimeofday(&end_time, NULL);
            cost_time = (end_time.tv_sec - start_time.tv_sec)* 1000000 + end_time.tv_usec - start_time.tv_usec;
            COST_TIME_ERROR_LOG( ("++++++>[%s] %d switch cost_time = [%ld], %lu:%lu", req_session, i, cost_time, end_time.tv_sec, end_time.tv_usec));
            gettimeofday(&start_time, NULL);
                ap->flags|= 1<<i;		
            }
        }

    }





    ap->Format = const_cast<char *>((ap->Format && ais_strcasecmp( ap->Format , "xml" ) == 0) ? "xml" : "json");
    if (ap->Net == NULL) {ap->Net = "0";}
    if (ap->DeviceType == NULL) {ap->DeviceType = "1";}
    if (ap->Os_version == NULL) {ap->Os_version = unknown;}
    if (ap->SdkVersion == NULL) {ap->SdkVersion = unknown;}		

/*if ap->Ip is a lan ip, replace ap->Ip with ap->nip, which is a wan ip set by Nginx4AIS*/
    if ( NULL==ap->Ip ){
        ap->Ip = ap->nip;
    } else {
        char *lan_ip_pre1 = "10.";
        char *lan_ip_pre2 = "192.168";
        char *lan_ip_pre3 = "127";

        if ( ( 0==strncmp(lan_ip_pre1, ap->Ip, strlen(lan_ip_pre1)) )
        || ( 0==strncmp(lan_ip_pre2, ap->Ip, strlen(lan_ip_pre2)) )
        || ( 0==strncmp(lan_ip_pre3, ap->Ip, strlen(lan_ip_pre3)) ) )
        {
            ap->Ip = ap->nip;
        }
    }

    //ERROR_LOG(("ap->Ip:%s, ap->nip:%s", ap->Ip, ap->nip));

    if (*( conn->ctx->config[OPEN_SIGN] ) == 'y' ){
        qsort(parray,j,sizeof( char * ),comp_dict);}
    DEBUG_TRACE( ("------------------")); 
    //free(var);
    return AIS_OK;
ais_error:
        gettimeofday(&end_time, NULL);
        cost_time = (end_time.tv_sec - start_time.tv_sec)* 1000000 + end_time.tv_usec - start_time.tv_usec;
        COST_TIME_ERROR_LOG( ("++++++>[%s] %d switch ais_error, cost_time = [%ld], %lu:%lu", req_session, i, cost_time, end_time.tv_sec, end_time.tv_usec));
        gettimeofday(&start_time, NULL);
    //	free(var);
    return AIS_ERROR;
}

#if 0
//{"Request":"\/api\/1.0\/get","Status":"500","Msg":"不存在sdk信息","Data":null}


void ais_error(struct ais_connection *conn, char * error,int code)
{
    struct json_object* error_object;
    const char *tp;
    error_object = json_object_new_object();
    json_object_object_add(error_object, "Request", json_object_new_string(IFELSE(conn->ap.uri,"")));
    json_object_object_add(error_object, "Status", json_object_new_int(code));
    json_object_object_add(error_object, "Msg",json_object_new_string(error));
    json_object_object_add(error_object, "Data",NULL);
    tp = json_object_to_json_string(error_object);
    //生成返回值
    //	conn->num_bytes_sent += ais_printf(conn,"%.*s\x0a",strlen(tp),tp);
    out_string(conn->c, tp);
    json_object_put(error_object);
    return;

}
#endif
