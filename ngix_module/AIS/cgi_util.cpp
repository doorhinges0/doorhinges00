#include "cgi_util.h"
#include <stdio.h>

/* x2c() and unescape_url() stolen from NCSA code */
char x2c(char *what)
{
    register char digit;

    digit = (what[0] >= 'A' ? ((what[0] & 0xdf) - 'A')+10 : (what[0] - '0'));
    digit *= 16;
    digit += (what[1] >= 'A' ? ((what[1] & 0xdf) - 'A')+10 : (what[1] - '0'));
    return(digit);
}

void unescape_url(char *url)
{
    register int x,y;

    for (x=0,y=0; url[y]; ++x,++y) {
        if((url[x] = url[y]) == '%') {
            url[x] = x2c(&url[y+1]);
            y+=2;
        }
    }
    url[x] = '\0';
}

int Map_Cgi_Params(map<string, string> &param, char *buffer)
{
    if (NULL==buffer)
        return -1;

    int i, j, num, token;
    int len = strlen(buffer);
    char *lexeme;
    string name, value;

    if ((lexeme = (char *)malloc(sizeof(char) * len + 1)) == NULL)
        return -2;

    i = 0;
    num = 0;
    token = _NAME;
    while (i < len) {
        j = 0;
        if (_NAME==token)
        {
            while ( (i < len)
                &&((buffer[i] == '=') || (buffer[i] == '&')))
            {
                i++;
            }
        }
        while ( (buffer[i] != '=') && (buffer[i] != '&') && (i < len) ) {
            lexeme[j] = (buffer[i] == '+') ? ' ' : buffer[i];
            //lexeme[j] = buffer[i];
            i++;
            j++;
        }
        lexeme[j] = '\0';
//printf("lexeme:%s, buffer[i]=%c, i=%d, len=%d\n", lexeme, buffer[i], i, len);
        if (token == _NAME) {
            unescape_url(lexeme);
            name = lexeme;
            if ( (buffer[i] != '=') || (i == len - 1) ) {
                if (!name.empty())
                    param[name] = "";

                if (i != len - 1)
                { /* error in expression */
                    FREE(lexeme);
                    return -3;
                }
            } else {
                token = _VALUE;
            }
        }
        else {
            unescape_url(lexeme);
            value = lexeme;
            if (!name.empty()){
                param[name] = value;
            }

            token = _NAME;
        }
        i++;
        j = 0;
    }
    FREE(lexeme);
    return 0;
}

void Traverse_Map(map<string, string> &param)
{

    map<string, string>::iterator it=param.begin();
    printf("---------------Traverse cgi params---------------\n");
    for(; it!=param.end(); it++) 
    {
        printf("%s=>%s\n", it->first.c_str(), it->second.c_str());
    }
    printf("\n\n");
}

bool isalldigit(string &pid)
{
    char *p = const_cast<char *>(pid.c_str());
    for (; p<pid.c_str()+pid.size(); p++)
    {
        if (!isdigit(*p))
            return false;
    }
    return true;
}


