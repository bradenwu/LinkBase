#ifndef __QPUB_BASE64_H__
#define __QPUB_BASE64_H__

#include <string>

/**
 * 线程安全的base64编码
 * @return base64编码数据, 失败返回empty()
 */
std::string qbase64_encode(const char* data, size_t data_length);

/**
 * 线程安全的base64解码
 * @return 源数据, 失败返回empty()
 */
std::string qbase64_decode(const char* data, size_t data_length);

/**
 * 线程安全的base64编码
 * @return base64编码数据, 失败返回empty()
 */
inline std::string qbase64_encode(const std::string& data)
{
	return qbase64_encode(data.c_str(), data.size());
}

/**
 * 线程安全的base64解码
 * @return 源数据, 失败返回empty()
 */
inline std::string qbase64_decode(const std::string& data)
{
	return qbase64_decode(data.c_str(), data.size());
}

#endif

