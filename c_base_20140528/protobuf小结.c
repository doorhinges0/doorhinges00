
��װ protobuf 

��װ����������ʾ��
 tar -xzf protobuf-XXX.tar.gz 
 cd protobuf-XXX 
 ./configure --prefix=$INSTALL_DIR 
 make 
 make check 
 make install


����ʹ�� Protobuf �� C++ ����һ��ʮ�ּ򵥵����ӳ���
�ó�������������ɡ���һ���ֱ���Ϊ Writer���ڶ����ֽ��� Reader��
Writer ����һЩ�ṹ��������д��һ�������ļ���Reader ����Ӹô����ļ��ж�ȡ�ṹ�����ݲ���ӡ����Ļ�ϡ�
׼��������ʾ�Ľṹ�������� HelloWorld�������������������ݣ�
ID��Ϊһ���������͵�����
Str������һ���ַ���


package lc;

message helloworld
{
        required int32  id = 1;
        required string str = 2;
        optional int32  opt = 3;
}


protoc -I=./ --cpp_out=./ ./lc.helloworld.proto



������������ļ���
lc.helloworld.pb.h �� ������ C++ ���ͷ�ļ�
lc.helloworld.pb.cc �� C++ ���ʵ���ļ�
�����ɵ�ͷ�ļ��У�������һ�� C++ �� helloworld������� Writer �� Reader ��ʹ�������������Ϣ���в������������Ϣ�ĳ�Ա���и�ֵ������Ϣ���л��ȵȶ�����Ӧ�ķ�����


��ǰ������Writer ����һ���ṹ������д����̣��Ա�����������ȡ���������ǲ�ʹ�� Protobuf����ʵҲ������ѡ��һ�����ܵķ����ǽ�����ת��Ϊ�ַ�����Ȼ���ַ���д����̡�ת��Ϊ�ַ����ķ�������ʹ�� sprintf()����ǳ��򵥡����� 123 ���Ա���ַ�����123����
�������ƺ�û��ʲô���ף�������ϸ����һ�¾ͻᷢ�֣�������������д Reader ���Ǹ��˵�Ҫ��Ƚϸߣ�Reader �����߱����� Writer ��ϸ�ڡ����硱123�������ǵ������� 123����Ҳ�������������� 1,2 �� 3���ȵȡ���ô˵�������ǻ������� Writer ����һ�ַָ���һ�����ַ����Ա� Reader ������ȷ��ȡ�����ָ���Ҳ��������������ʲô���⡣������Ƿ���һ���򵥵� Helloworld Ҳ��Ҫд��ദ����Ϣ��ʽ�Ĵ��롣
���ʹ�� Protobuf����ô��Щϸ�ھͿ��Բ���ҪӦ�ó����������ˡ�

ʹ�� Protobuf��Writer �Ĺ����ܼ򵥣���Ҫ����Ľṹ�������� .proto �ļ�������������һ���еı�����̺󣬸����ݻ��ṹ��Ӧ��һ�� C++ ���࣬�������� lc.helloworld.pb.h �С����ڱ���������Ϊ lc::helloworld��

Writer ��Ҫ include ��ͷ�ļ���Ȼ������ʹ��������ˡ�
���ڣ��� Writer �����У���Ҫ������̵Ľṹ��������һ�� lc::helloworld ��Ķ����ʾ�����ṩ��һϵ�е� get/set ���������޸ĺͶ�ȡ�ṹ�������е����ݳ�Ա�����߽� field��
��������Ҫ���ýṹ�����ݱ��浽������ʱ���� lc::helloworld �Ѿ��ṩ��Ӧ�ķ�������һ�����ӵ����ݱ��һ���ֽ����У����ǿ��Խ�����ֽ�����д����̡�
������Ҫ��ȡ������ݵĳ�����˵��Ҳֻ��Ҫʹ���� lc::helloworld ����Ӧ�����л�������������ֽ���������ת����ṹ�����ݡ����� Protobuf ���ԶԶ�������Ǹ��ֲڵ��ַ���ת��Ҫȫ�棬��ˣ����ǲ�����Ľ��������齻�� Protobuf �ɡ�


[root@APP01 protobuf_test]# cat Writer.cpp
#include <iostream>
#include <fstream>
#include <string>
#include "lc.helloworld.pb.h"

using namespace std;

int main(int argc, char* argv[])
{
        lc::helloworld mgs1;
        mgs1.set_id(110);
        mgs1.set_str("hello");

        fstream output("./log", ios::out| ios::trunc | ios::binary);

        if (!mgs1.SerializeToOstream(&output)) {
                cerr << "failed to write msg."<<endl;
                return -1;
        }

        return 0;
}


[root@APP01 protobuf_test]# cat Write.sh

g++ -owriter Writer.cpp lc.helloworld.pb.cc -lprotobuf

[root@APP01 protobuf_test]#

Msg1 ��һ�� helloworld ��Ķ���set_id() �������� id ��ֵ��SerializeToOstream ���������л���д��һ�� fstream ����



[root@APP01 protobuf_test]# cat Reader.cpp
#include <iostream>
#include <fstream>
#include <string>
#include "lc.helloworld.pb.h"

using namespace std;



void ListMsg(const lc::helloworld &msg) {
        cout<<msg.id()<<endl;
        cout<<msg.str()<<endl;
}


int main(int argc, char* argv[])
{
        lc::helloworld msg1;

        fstream input("./log", ios::in| ios::binary);


        if (!msg1.ParseFromIstream(&input)) {
                cerr << "failed to write msg."<<endl;
                return -1;
        }

        ListMsg(msg1);

        return 0;
}


[root@APP01 protobuf_test]# 


�����


[root@APP01 protobuf_test]# ./writer
[root@APP01 protobuf_test]# ./reader
110
hello
[root@APP01 protobuf_test]# 




ReqLogin req;
bool bRet = req.ParseFromString(packet);


string sPacket;

RspLogin rsp;
rsp.set_token(s.sKey);
rsp.set_apiresponse(result.toStyledString().c_str());
rsp.SerializeToString(&sPacket);


rsp.SerializeToString(&sPacket);
BUILDACK( 0 , header , "" , sPacket, buffer, length );
	
ReqHeader header;
string s("");
int iRet = (int) header.ParseFromArray(buffer+5, HeaderLen);	
	
	
	
	
	
	

//C��������л������л�API
bool ParseFromArray(const void* data, int size);
bool SerializeToArray(void* data, int size) const;
//ʹ��
void set_people()             
{
    wp.set_name("sealyao");   
    wp.set_id(123456);        
    wp.set_email("sealyaog@gmail.com");
    wp.SerializeToArray(parray,256);
}

void get_people()             
{
    rap.ParseFromArray(parray,256);
    cout << "Get People from Array:" << endl;
    cout << "\t Name : " <<rap.name() << endl;
    cout << "\t Id : " << rap.id() << endl;
    cout << "\t email : " << rap.email() << endl;
}


//C++string���л������л�API
bool SerializeToString(string* output) const;
bool ParseFromString(const string& data);
//ʹ�ã�
void set_people()             
{
    wp.set_name("sealyao");   
    wp.set_id(123456);        
    wp.set_email("sealyaog@gmail.com");
    wp.SerializeToString(&pstring);
}

void get_people()             
{
    rsp.ParseFromString(pstring);  
    cout << "Get People from String:" << endl;
    cout << "\t Name : " <<rsp.name() << endl;
    cout << "\t Id : " << rsp.id() << endl;
    cout << "\t email : " << rsp.email() << endl;
}


 //�ļ������������л������л�API
 bool SerializeToFileDescriptor(int file_descriptor) const;
 bool ParseFromFileDescriptor(int file_descriptor);

 //ʹ��:
void set_people()
{
    fd = open(path,O_CREAT|O_TRUNC|O_RDWR,0644);
    if(fd <= 0){
        perror("open");
        exit(0); 
    }   
    wp.set_name("sealyaog");
    wp.set_id(123456);
    wp.set_email("sealyaog@gmail.com");
    wp.SerializeToFileDescriptor(fd);   
    close(fd);
}

void get_people()
{
    fd = open(path,O_RDONLY);
    if(fd <= 0){
        perror("open");
        exit(0);
    }
    rp.ParseFromFileDescriptor(fd);
    std::cout << "Get People from FD:" << endl;
    std::cout << "\t Name : " <<rp.name() << endl;
    std::cout << "\t Id : " << rp.id() << endl;
    std::cout << "\t email : " << rp.email() << endl;
    close(fd);
}



//C++ stream ���л�/�����л�API
bool SerializeToOstream(ostream* output) const;
bool ParseFromIstream(istream* input);

//ʹ�ã�
void set_people()
{
    fstream fs(path,ios::out|ios::trunc|ios::binary);
    wp.set_name("sealyaog");
    wp.set_id(123456);
    wp.set_email("sealyaog@gmail.com");
    wp.SerializeToOstream(&fs);    
    fs.close();
    fs.clear();
}

void get_people()
{
    fstream fs(path,ios::in|ios::binary);
    rp.ParseFromIstream(&fs);
    std::cout << "\t Name : " <<rp.name() << endl;
    std::cout << "\t Id : " << rp.id() << endl; 
    std::cout << "\t email : " << rp.email() << endl;   
    fs.close();
    fs.clear();
}



http://blog.csdn.net/sealyao/article/details/6940245
	
	
