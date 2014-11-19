#ifndef __CGI_UTIL_H_20140506__
#define __CGI_UTIL_H_20140506__

#include <map>
#include <string>
#include <iterator>
#include <string.h>
#include <stdlib.h>

using namespace std;

#define _NAME 0
#define _VALUE 1
#define FREE(p)  do { if (p != NULL) { free(p); p= NULL; } } while (0)


char x2c(char *what);
void unescape_url(char *url);

int Map_Cgi_Params(map<string, string> &param, char *buffer);
void Traverse_Map(map<string, string> &param);
bool isalldigit(string &pid);


#endif//__CGI_UTIL_H_20140506__
