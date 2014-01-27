#include "SDUtility.h"

#include <time.h>
#include <sys/time.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <net/if.h>//#include <linux/if.h> //编译时加上-D__i386__
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <netdb.h>

#include <cstdio>
#include <cstring>
#include <cstdlib>

#include <sstream>

#include <locale>
#include <iconv.h>
#include <cmath>

using namespace std;


const char SDUtility::HEX_DATA_MAP[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};

string SDUtility::str2hex(const char * buf,int size)
{
	string ret_str;

	char tmpbuf[10] = {0};
	const char * buffer = buf;
	int buffer_size = size;
	for (int i = 0 ;i < buffer_size; i++) {
		sprintf(tmpbuf,"%02X",(unsigned char)buffer[i]);
		ret_str += tmpbuf;
	}

	return ret_str;
}

string SDUtility::
	str2hex(const string& buffer)
{
	string ret_str;
	char buf[40] = {0};
	sprintf(buf,"[%d]",buffer.length());
	ret_str += buf;

	const char* ptr = buffer.c_str();
	for(int i = 0, size = buffer.length(); i < size; i++)
	{
		sprintf(buf,"%02X ",(unsigned char)ptr[i]);		
		ret_str += buf;
	}	
	return ret_str;
}


string SDUtility::format_time(time_t unix_time)
{
//	struct timeval tv;
	struct timezone tz;

	gettimeofday(NULL, &tz);

	time_t local_time = (time_t)(unix_time - tz.tz_minuteswest * 60);

	struct tm lt;
	gmtime_r(&local_time, &lt);

	char timebuf[16];
	sprintf(timebuf, "%02d:%02d:%02d", lt.tm_hour, lt.tm_min, lt.tm_sec);

	return (string)timebuf;
}

string SDUtility::format(int n)
{
	char buf[10];
	sprintf(buf, "%d", n);

	return (string)buf;
}

string SDUtility::format(uint32_t n)
{
	char buf[10];
	sprintf(buf, "%u", n);

	return (string)buf;
}

string SDUtility::format(long n)
{
	char buf[32];
	sprintf(buf, "%ld", n);

	return (string)buf;
}

uint32_t SDUtility::atouint32(const char* ptr, int32_t len)
{
	uint32_t value = 0;
	while((len >= 0) && isdigit(*ptr )){
		value = 10 * value + (uint32_t)(*ptr - '0');
		ptr ++;
		len --;
	}
	return value;
}

uint32_t SDUtility::atouint64(char* ptr, int32_t len)
{
	uint64_t value = 0;
	while((len >= 0) && isdigit(*ptr )){
		value = 10 * value + (uint32_t)(*ptr - '0');
		ptr ++;
		len --;
	}
	return value;
}


string SDUtility::ip_ntoa(uint32_t ip)
{
	struct in_addr in;
	in.s_addr = in_addr_t(ip);
	return string(inet_ntoa(in));
}

uint32_t SDUtility::ip_aton(const  string & ip)
{
	struct in_addr in;
	in.s_addr = 0;
	inet_aton(ip.c_str(), &in);
	return uint32_t(in.s_addr);
}

string SDUtility::get_local_datestring()
{
	struct timeval tv;
	struct timezone tz;

	gettimeofday(&tv, &tz);

	time_t local_time = (time_t)(tv.tv_sec - tz.tz_minuteswest * 60);
	struct tm lt;
	gmtime_r(&local_time, &lt);

	char timebuf[32];
	sprintf(timebuf, "%04d%02d%02d", (lt.tm_year + 1900), (lt.tm_mon + 1), lt.tm_mday);

	return (string)timebuf;
}

vector<string> SDUtility::resolve(string hostname)
{
	vector<string> addresses;
	if(hostname.empty())
	{
	}
	else if(inet_addr(hostname.c_str()) != INADDR_NONE)
	{
		addresses.push_back(hostname);
	}
	else
	{
		struct hostent* ent = gethostbyname(hostname.c_str());		
		if(ent && ent->h_addrtype == AF_INET)
		{
			in_addr addr;  
			for(int i = 0; ent->h_addr_list[i]; ++i)
			{
				addr.s_addr = *((long*)ent->h_addr_list[i]);
				addresses.push_back(inet_ntoa(addr));
			}       
		}
	}

	return addresses;
}


// 文本编码由gbk转换utf-8，如转换失败返回空字符串
string SDUtility::gbk2utf8(const char* text)
{
	if(text == NULL)
	{
		return "";
	}

    size_t inlen = strlen(text);
    if(inlen == 0)
    {
        return "";
    }

	iconv_t ic = iconv_open("utf-8", "gbk");
	if(ic == (iconv_t)-1)
	{
		return "";
	}

	size_t outlen = inlen * 2;
	std::vector<char> result(outlen + 1);
	char* inbuf = const_cast<char*>(text);
	char* outbuf = &result[0];
	if(iconv(ic, &inbuf, &inlen, &outbuf, &outlen) == (size_t)-1)
	{
		iconv_close(ic);
		return "";
	}
	iconv_close(ic);
	return &result[0];
}


// 文本编码由utf-8转换gbk，如转换失败返回空字符串
string SDUtility::utf82gbk(const char* text)
{
	if(text == NULL)
	{
		return "";
	}

    size_t inlen = strlen(text);
    if(inlen == 0)
    {
        return "";
    }

	iconv_t ic = iconv_open("gbk", "utf-8");
	if(ic == (iconv_t)-1)
	{
		return "";
	}

	size_t outlen = inlen * 2;
	std::vector<char> result(outlen + 1);
	char* inbuf = const_cast<char*>(text);
	char* outbuf = &result[0];
	if(iconv(ic, &inbuf, &inlen, &outbuf, &outlen) == (size_t)-1)
	{
		iconv_close(ic);
		return "";
	}
	iconv_close(ic);
	return &result[0];


}


vector<string> SDUtility::split_v(const  string & str,const std :: string & sp)
{
	vector<string> splits;

	string::size_type begin = str.empty() ? string::npos : 0;
	string::size_type end = str.find(sp, begin);
	while (begin != string::npos) {
		if (end == string::npos) {
			splits.push_back(str.substr(begin));
			begin = end;
		} else {
			splits.push_back(str.substr(begin, (end - begin)));
			begin = end;
			if ((begin + sp.length()) >= str.length()) {
				begin = string::npos;
			} else {
				begin += sp.length();
			}
		}
		end = str.find(sp, begin);
	}

	return splits;
}

unsigned int SDUtility::utf8_decode( char *s, unsigned int *pi )
{
    unsigned int c;
    int i = *pi;
    /* one digit utf-8 */
    if ((s[i] & 128)== 0 ) {
        c = (unsigned int) s[i];
        i += 1;
    } else if ((s[i] & 224)== 192 ) { /* 110xxxxx & 111xxxxx == 110xxxxx */
        c = (( (unsigned int) s[i] & 31 ) << 6) +
            ( (unsigned int) s[i+1] & 63 );
        i += 2;
    } else if ((s[i] & 240)== 224 ) { /* 1110xxxx & 1111xxxx == 1110xxxx */
        c = ( ( (unsigned int) s[i] & 15 ) << 12 ) +
            ( ( (unsigned int) s[i+1] & 63 ) << 6 ) +
            ( (unsigned int) s[i+2] & 63 );
        i += 3;
    } else if ((s[i] & 248)== 240 ) { /* 11110xxx & 11111xxx == 11110xxx */
        c =  ( ( (unsigned int) s[i] & 7 ) << 18 ) +
            ( ( (unsigned int) s[i+1] & 63 ) << 12 ) +
            ( ( (unsigned int) s[i+2] & 63 ) << 6 ) +
            ( (unsigned int) s[i+3] & 63 );
        i+= 4;
    } else if ((s[i] & 252)== 248 ) { /* 111110xx & 111111xx == 111110xx */
        c = ( ( (unsigned int) s[i] & 3 ) << 24 ) +
            ( ( (unsigned int) s[i+1] & 63 ) << 18 ) +
            ( ( (unsigned int) s[i+2] & 63 ) << 12 ) +
            ( ( (unsigned int) s[i+3] & 63 ) << 6 ) +
            ( (unsigned int) s[i+4] & 63 );
        i += 5;
    } else if ((s[i] & 254)== 252 ) { /* 1111110x & 1111111x == 1111110x */
        c = ( ( (unsigned int) s[i] & 1 ) << 30 ) +
            ( ( (unsigned int) s[i+1] & 63 ) << 24 ) +
            ( ( (unsigned int) s[i+2] & 63 ) << 18 ) +
            ( ( (unsigned int) s[i+3] & 63 ) << 12 ) +
            ( ( (unsigned int) s[i+4] & 63 ) << 6 ) +
            ( (unsigned int) s[i+5] & 63 );
        i += 6;
    } else {
        c = '?';
        i++;
    }
    *pi = i;
    return c;
}


string SDUtility::UrlEncode(const string& src)
{
    static    char hex[] = "0123456789ABCDEF";
    std::string dst;
    
    for (size_t i = 0; i < src.size(); i++)
    {
        unsigned char ch = src[i];
        if (isalnum(ch))
        {
            dst += ch;
        }
        else
            if (src[i] == ' ')
            {
                dst += '+';
            }
            else
            {
                unsigned char c = static_cast<unsigned char>(src[i]);
                dst += '%';
                dst += hex[c / 16];
                dst += hex[c % 16];
            }
    }
    return dst;
}

string SDUtility::UrlDecode(const std::string& src)
{
    std::string dst, dsturl;

    int srclen = src.size();

    for (size_t i = 0; i < srclen; i++)
    {
        if (src[i] == '%')
        {
            if(isxdigit(src[i + 1]) && isxdigit(src[i + 2]))
            {
                char c1 = src[++i];
                char c2 = src[++i];
                c1 = c1 - 48 - ((c1 >= 'A') ? 7 : 0) - ((c1 >= 'a') ? 32 : 0);
                c2 = c2 - 48 - ((c2 >= 'A') ? 7 : 0) - ((c2 >= 'a') ? 32 : 0);
                dst += (unsigned char)(c1 * 16 + c2);
            }
        }
        else
            if (src[i] == '+')
            {
                dst += ' ';
            }
            else
            {
                dst += src[i];
            }
    }

    int len = dst.size();
    
    for(unsigned int pos = 0; pos < len;)
    {
        unsigned int nvalue = utf8_decode((char *)dst.c_str(), &pos);
        dsturl += (unsigned char)nvalue;
    }

    return dsturl;
}


