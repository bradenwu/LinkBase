#include "qbase64.h"
#include <memory>

using namespace std;

#ifdef __linux__
	#ifndef SIZE_T_MAX	
	#include <stdint.h>
	#define SIZE_T_MAX (4294967295U) 
	#endif
#endif 

static const char base64_table[] =
{ 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M',
	'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',
	'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm',
	'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z',
	'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '+', '/', '\0'
};

static const char base64_pad = '=';

static const char base64_reverse_table[256] = {
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 62, -1, -1, -1, 63,
	52, 53, 54, 55, 56, 57, 58, 59, 60, 61, -1, -1, -1, -1, -1, -1,
	-1,  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14,
	15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, -1, -1, -1, -1, -1,
	-1, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
	41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1
};

//数据长度小于MY_BUFFER_SIZE时, 使用栈内存进行编码/解码
#define MY_BUFFER_SIZE (100*1024)

string qbase64_encode (const char* data, size_t data_length)
{
	if ( data_length == 0 )
		return string();
	if ( data_length > (3 * (SIZE_T_MAX >> 2)) )
		return string();

	size_t ret_length = ((data_length + 2) / 3) * 4 + 1;

//modify by ml 2012.03.31 auto_ptr不能用于数组
/*
#if defined(__FreeBSD__) && __FreeBSD__ < 6
	char* result = new char[ret_length];
	auto_ptr<char> x(result);
#else
	char* result;
	auto_ptr<char> x;
	char tmp[MY_BUFFER_SIZE];
	if ( ret_length > MY_BUFFER_SIZE )
	{
		result = new char[ret_length];
		x.reset(result);
	}
	else
		result = tmp;
#endif
*/
	string ret_result = string();
	int len_flag = 0;

	char* result;
	char tmp[MY_BUFFER_SIZE];
	if ( ret_length > MY_BUFFER_SIZE )
	{
		result = new char[ret_length];
		len_flag = 1;
	}
	else
		result = tmp;

	const unsigned char* current = (const unsigned char*)data;
	unsigned char* p = (unsigned char*)result;

	while ( data_length > 2 )
	{ /* keep going until we have less than 24 bits */
		*p++ = base64_table[current[0] >> 2];
		*p++ = base64_table[((current[0] & 0x03) << 4) | (current[1] >> 4)];
		*p++ = base64_table[((current[1] & 0x0f) << 2) | (current[2] >> 6)];
		*p++ = base64_table[current[2] & 0x3f];

		current += 3;
		data_length -= 3; /* we just handle 3 octets of data */
	}

	/* now deal with the tail end of things */
	if ( data_length != 0 )
	{
		*p++ = base64_table[current[0] >> 2];
		if ( data_length > 1 )
		{
			*p++ = base64_table[((current[0] & 0x03) << 4) | (current[1] >> 4)];
			*p++ = base64_table[(current[1] & 0x0f) << 2];
			*p++ = base64_pad;
		}
		else
		{
			*p++ = base64_table[(current[0] & 0x03) << 4];
			*p++ = base64_pad;
			*p++ = base64_pad;
		}
	}
	*p = '\0';

	//return string(result, (char*)p - result);

	ret_result = string(result, (char*)p - result);

	if ( 1 == len_flag )
		delete [] result;
	return ret_result;
}

string qbase64_decode(const char *data, size_t data_length)
{
	if ( data_length < 4 )
		return string();

//modify by ml 2012.03.31 auto_ptr不能用于数组
/*
#if defined(__FreeBSD__) && __FreeBSD__ < 6
	char* presult = new char[data_length];
	auto_ptr<char> x(presult);
#else
	char* presult;
	auto_ptr<char> x;
	char tmp[MY_BUFFER_SIZE];
	if ( data_length > MY_BUFFER_SIZE )
	{
		presult = new char[data_length];
		x.reset(presult);
	}
	else
		presult	= tmp;
#endif
*/
	string ret_result = string();
	int len_flag = 0;

	char* presult;
	char tmp[MY_BUFFER_SIZE];
	if ( data_length > MY_BUFFER_SIZE )
	{
		presult = new char[data_length];
		len_flag = 1;
	}
	else
		presult	= tmp;

	unsigned char* result = (unsigned char*)presult;
	const unsigned char* current = (const unsigned char*)data;
	size_t i = 0, j = 0, k;
	unsigned char ch;

	/* run through the whole string, converting as we go */
	while ( (ch = *current++) != '\0' && data_length-- > 0 )
	{
		if ( ch == base64_pad )
			break;

		/* When Base64 gets POSTed, all pluses are interpreted as spaces.
		   This line changes them back.  It's not exactly the Base64 spec,
		   but it is completely compatible with it (the spec says that
		   spaces are invalid).  This will also save many people considerable
		   headache.  - Turadg Aleahmad <turadg@wise.berkeley.edu>
		*/

		if ( ch == ' ' )
			ch = '+';

		if ( base64_reverse_table[ch] < 0 )
			continue;
		else
			ch = base64_reverse_table[ch];

		switch ( i % 4 )
		{
		case 0:
			result[j] = ch << 2;
			break;
		case 1:
			result[j++] |= ch >> 4;
			result[j] = (ch & 0x0f) << 4;
			break;
		case 2:
			result[j++] |= ch >>2;
			result[j] = (ch & 0x03) << 6;
			break;
		case 3:
			result[j++] |= ch;
			break;
		}
		i++;
	}

	k = j;
	/* mop things up if we ended on a boundary */
	if ( ch == base64_pad )
	{
		switch ( i % 4 )
		{
		case 1:
			return string();
		case 2:
			k++;
		case 3:
			result[k++] = 0;
		}
	}
	result[j] = '\0';
	//return string(presult, j);

	ret_result = string(presult, j);

	if( 1 == len_flag )
	{
		delete [] presult;
	}

	return ret_result;
}

#ifdef __MAIN__
#include <iostream>
#include <cstring>

#include "qcompress.h"
#include <base64.h>

#include <qpub/qtime.h>
using namespace QPub;

int main(int ac, char* av[])
{
	string src;
	if ( ac < 2 )
		src = "asasas";
	else
		src = av[1];

	string result = qbase64_encode( qcompress(src) );
	//string result = qbase64_encode( src );
	cout << result.size() << ": " << result << endl;
	result = qbase64_decode(result);
	cout << result.size() << ": " << src << endl;

	CalcBase64 x;
	int len;
	unsigned char* dst;
	dst = x.base64_encode((const unsigned char*)src.c_str(), src.size(), &len);
	cout << len << ": " << dst << endl;
	cout << x.base64_decode(dst, len, 0) << endl;

	int cnt = 100000;
	Time st = Time::now();
	for ( int i = 0; i < cnt; i++ )
		string s = qbase64_encode(src);
	Time diff = Time::now() - st;
	cout << "len:" << src.size() << " time:" << diff << " qps:" << (cnt*1000000)/diff.getlongTime() << endl;

	st = Time::now();
	for ( int i = 0; i < cnt; i++ )
	{
		dst = x.base64_encode((const unsigned char*)src.c_str(), src.size(), &len);
		string s = string((char*)dst, len);
	}
	diff = Time::now() - st;
	cout << "len:" << src.size() << " time:" << diff << " qps:" << (cnt*1000000)/diff.getlongTime() << endl;
	return 0;
}
#endif
