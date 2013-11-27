#ifndef SD_UTILITY_H
#define SD_UTILITY_H

#include <string>
#include <vector>
#include <set>
#include <stdint.h>
#include <unistd.h>


class SDUtility
{
	const static char HEX_DATA_MAP[];

public:
	static std::string str2hex(const char* buf, int size);
	static std::string str2hex(const std::string& buffer);
	static std::string format_time(time_t unix_time);
	static std::string format(int n);
	static std::string format(uint32_t n);
	static std::string format(long n);
	static uint32_t  atouint32(const char* ptr, int32_t len);
	static uint32_t  atouint64(char* ptr, int32_t len);
	
	static std::string ip_ntoa(uint32_t ip);
	static uint32_t ip_aton(const std::string& ip);

	static std::string get_local_datestring();
	
	static std::vector<std::string> resolve(std::string hostname);

	static std::string data_to_string(char * buf, int nbytes);

	static std::string gbk2utf8(const char* text);
	
	static std::string utf82gbk(const char* text);

	static std::vector<std::string> split_v(const std::string& str, const std::string& sp);

	static unsigned int utf8_decode( char *s, unsigned int *pi );
	static std::string UrlEncode(const std::string& src);
	static std::string UrlDecode(const std::string& src);
	
};

#endif /* end SD_UTILITY_H */

