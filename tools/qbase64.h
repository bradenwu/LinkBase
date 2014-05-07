#ifndef __QPUB_BASE64_H__
#define __QPUB_BASE64_H__

#include <string>

/**
 * �̰߳�ȫ��base64����
 * @return base64��������, ʧ�ܷ���empty()
 */
std::string qbase64_encode(const char* data, size_t data_length);

/**
 * �̰߳�ȫ��base64����
 * @return Դ����, ʧ�ܷ���empty()
 */
std::string qbase64_decode(const char* data, size_t data_length);

/**
 * �̰߳�ȫ��base64����
 * @return base64��������, ʧ�ܷ���empty()
 */
inline std::string qbase64_encode(const std::string& data)
{
	return qbase64_encode(data.c_str(), data.size());
}

/**
 * �̰߳�ȫ��base64����
 * @return Դ����, ʧ�ܷ���empty()
 */
inline std::string qbase64_decode(const std::string& data)
{
	return qbase64_decode(data.c_str(), data.size());
}

#endif

