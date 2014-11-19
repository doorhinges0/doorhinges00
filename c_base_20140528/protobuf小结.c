
安装 protobuf 

安装步骤如下所示：
 tar -xzf protobuf-XXX.tar.gz 
 cd protobuf-XXX 
 ./configure --prefix=$INSTALL_DIR 
 make 
 make check 
 make install


打算使用 Protobuf 和 C++ 开发一个十分简单的例子程序。
该程序由两部分组成。第一部分被称为 Writer，第二部分叫做 Reader。
Writer 负责将一些结构化的数据写入一个磁盘文件，Reader 则负责从该磁盘文件中读取结构化数据并打印到屏幕上。
准备用于演示的结构化数据是 HelloWorld，它包含两个基本数据：
ID，为一个整数类型的数据
Str，这是一个字符串


package lc;

message helloworld
{
        required int32  id = 1;
        required string str = 2;
        optional int32  opt = 3;
}


protoc -I=./ --cpp_out=./ ./lc.helloworld.proto



命令将生成两个文件：
lc.helloworld.pb.h ， 定义了 C++ 类的头文件
lc.helloworld.pb.cc ， C++ 类的实现文件
在生成的头文件中，定义了一个 C++ 类 helloworld，后面的 Writer 和 Reader 将使用这个类来对消息进行操作。诸如对消息的成员进行赋值，将消息序列化等等都有相应的方法。


如前所述，Writer 将把一个结构化数据写入磁盘，以便其他人来读取。假如我们不使用 Protobuf，其实也有许多的选择。一个可能的方法是将数据转换为字符串，然后将字符串写入磁盘。转换为字符串的方法可以使用 sprintf()，这非常简单。数字 123 可以变成字符串”123”。
这样做似乎没有什么不妥，但是仔细考虑一下就会发现，这样的做法对写 Reader 的那个人的要求比较高，Reader 的作者必须了 Writer 的细节。比如”123”可以是单个数字 123，但也可以是三个数字 1,2 和 3，等等。这么说来，我们还必须让 Writer 定义一种分隔符一样的字符，以便 Reader 可以正确读取。但分隔符也许还会引起其他的什么问题。最后我们发现一个简单的 Helloworld 也需要写许多处理消息格式的代码。
如果使用 Protobuf，那么这些细节就可以不需要应用程序来考虑了。

使用 Protobuf，Writer 的工作很简单，需要处理的结构化数据由 .proto 文件描述，经过上一节中的编译过程后，该数据化结构对应了一个 C++ 的类，并定义在 lc.helloworld.pb.h 中。对于本例，类名为 lc::helloworld。

Writer 需要 include 该头文件，然后便可以使用这个类了。
现在，在 Writer 代码中，将要存入磁盘的结构化数据由一个 lc::helloworld 类的对象表示，它提供了一系列的 get/set 函数用来修改和读取结构化数据中的数据成员，或者叫 field。
当我们需要将该结构化数据保存到磁盘上时，类 lc::helloworld 已经提供相应的方法来把一个复杂的数据变成一个字节序列，我们可以将这个字节序列写入磁盘。
对于想要读取这个数据的程序来说，也只需要使用类 lc::helloworld 的相应反序列化方法来将这个字节序列重新转换会结构化数据。不过 Protobuf 想的远远比我们那个粗糙的字符串转换要全面，因此，我们不如放心将这类事情交给 Protobuf 吧。


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

Msg1 是一个 helloworld 类的对象，set_id() 用来设置 id 的值。SerializeToOstream 将对象序列化后写入一个 fstream 流。



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


结果：


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
	
	
	
	
	
	

//C数组的序列化和序列化API
bool ParseFromArray(const void* data, int size);
bool SerializeToArray(void* data, int size) const;
//使用
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


//C++string序列化和序列化API
bool SerializeToString(string* output) const;
bool ParseFromString(const string& data);
//使用：
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


 //文件描述符的序列化和序列化API
 bool SerializeToFileDescriptor(int file_descriptor) const;
 bool ParseFromFileDescriptor(int file_descriptor);

 //使用:
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



//C++ stream 序列化/反序列化API
bool SerializeToOstream(ostream* output) const;
bool ParseFromIstream(istream* input);

//使用：
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
	
	
