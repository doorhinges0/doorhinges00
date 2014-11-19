#include <stdio.h>
#include <time.h>
#include <sys/time.h>
#include "cgi_util.h"

class CTimeStamp
{
public:
    CTimeStamp() { gettimeofday( &t_start, NULL ); }
    ~CTimeStamp() {}
    unsigned int getElapsedMs() ; //毫秒
    unsigned int getElapsedUs() ; //微秒
private:
    struct timeval t_start, t_end ;
};
unsigned int CTimeStamp::getElapsedMs()
{
    gettimeofday( &t_end, NULL ) ;
    return ( (t_end.tv_sec*1000 + t_end.tv_usec/1000) - (t_start.tv_sec*1000 + t_start.tv_usec/1000) ) ;
}

unsigned int CTimeStamp::getElapsedUs()
{
    gettimeofday( &t_end, NULL ) ;
    return ( (t_end.tv_sec*1000*1000 + t_end.tv_usec) - (t_start.tv_sec*1000*1000+t_start.tv_usec) ) ;
}

void Test_Map_Cgi_Params(char *p)
{
    map<string, string> param;

    CTimeStamp allTimeStamp ;
    int ret = Map_Cgi_Params(param, p);  
    printf("ret=%d, buffer:%s, use alltime %u us\n", ret, p, allTimeStamp.getElapsedUs());

    Traverse_Map(param);
}

int main()
{
/*null case*/
    char* buffer1 = "url302=http::/site.lomark.cn/201405/zhxa/&format=json&pid=9&timestamp=1401255648";
    Test_Map_Cgi_Params(buffer1); 
#if 0
/*empty case*/
    char* buffer2 = "";
    Test_Map_Cgi_Params(buffer2); 

/*simple case*/
    char* buffer3 = "a=1&b=2jkl;pooiuy";
    Test_Map_Cgi_Params(buffer3); 

/*long case*/
    char* buffer4 = "name=8684公交&format=json&timestamp=1399357758&ah=50&appname=donson&aw=320&category=1&client=1&density=&devicenum=&ip=61.144.238.128&lat=&long=&net=&operator=1&os_version=4.5&ph=&pw=&uuid=UuidXXWDsd&key=0b0c1c5523aa40c3a5dcde4402947693&adspacetype=1&devicetype=&sdkversion=&appid=XAWDFWXF&sign=0316956e385882848f8708e1d187baf7";
    Test_Map_Cgi_Params(buffer4); 


/*encoded url case*/
    char* buffer5 = "url=https%3a%2f%2fwww.google.com.hk%2fsearch%3fnewwindow%3d1%26safe%3dstrict%26espv%3d2%26es_sm%3d93%26q%3dmap%2bbegin%26oq%3dmap%2bbegin%26gs_l%3dserp.3...263287.265217.0.265405.9.7.0.0.0.0.0.0..0.0....0...1c.1.42.serp..9.0.0.CjrgSPfxHPw&name=编码url作为参数";
    Test_Map_Cgi_Params(buffer5); 

    char* buffer6 = "a=1&b";
    Test_Map_Cgi_Params(buffer6); 

    char* buffer7 = "a=1&b=";
    Test_Map_Cgi_Params(buffer7); 

    char* buffer8 = "?a=1234&b=donson.com.cn";
    Test_Map_Cgi_Params(buffer8); 

    char* buffer9 = "Format=json&Ip=124.161.204.127&Client=2&SiteType=1&Key=bbb1182edea1408a8497dc84bdefab79&AppId=china_iphone_ron_app&AdSpaceType=1&Operator=2&Lat=31.149999618530273&Long=104.38899993896484&Uuid=D12C0CF5-DA95-4496-B8BD-C6C084910ACB&DeviceType=1&AppName=00000000-0008-77ae-0000-00000001bd03&Category=8&Aw=728&Ah=90&Timestamp=1399360649&Sign=2167b524e2e460a9ba9545508f75b2f6";
    Test_Map_Cgi_Params(buffer9); 

#endif
    return 0;
}
