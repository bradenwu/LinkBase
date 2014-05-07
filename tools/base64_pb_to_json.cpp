#include <iostream>
#include <fstream>
#include <string>
#include <stdlib.h>
#include <stdio.h>
#include <map>
#include <vector>

#include "qbase64.h"
#include "linkbase.pb.h"
//TODO: 1. cpp 测试用例 ;. 抽象供进一步使用-> 实现定期删除过期死链数据; .make file 3. 优化cpp性能
using namespace std;


#define MAX_BUFFER_LEN 1024*1024
#define TEMP_BUFFER_LEN 20
#define CRAWL_HIS_BUFFER_LEN 1024
static const char int2CharTable[] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f' };

void Usage() {
    cerr << "Usage: <base64StringFieldNumber(begin with 0)>" << endl;
}

inline void InsertInt32String(map<string,string>& attr2Value, const char * fieldName, char * buffer, int32_t & value) {
        bzero(buffer,TEMP_BUFFER_LEN);
        sprintf(buffer, "%d", value);
        attr2Value.insert(make_pair(fieldName, buffer));
}

inline void InsertInt64String(map<string,string>& attr2Value, const char * fieldName, char * buffer, int64_t & value) {
        bzero(buffer,TEMP_BUFFER_LEN);
        sprintf(buffer, "%lld", value);
        attr2Value.insert(make_pair(fieldName, buffer));
}

inline void InsertUInt32String(map<string, string>& attr2Value, const char * fieldName, char * buffer, uint32_t & value) {
        bzero(buffer,TEMP_BUFFER_LEN);
        sprintf(buffer, "%u", value);
        attr2Value.insert(make_pair(fieldName, buffer));
}
inline void InsertMD5String(map<string, string>& attr2Value, const char * fieldName, char * buffer, vector<uint64_t>& value) {
    //TODO:
}

//TODO: use sscanf and sprintf
inline string ConvertLongUInt64ToMD5String(uint64_t& firstPart, uint64_t& secondPart) {
//#ifdef DEBUG
//    cerr << "high long:" << firstPart << endl;
//    cerr << "low long:" << secondPart << endl;
//#endif
    char buffer[32] = {'\0'};

    char* pDst = &buffer[0];

    char* pSrc = (char *)&firstPart + 7;

    for (int i=0; i < 8; ++i) {
//#ifdef DEBUG
//        cerr << "[" << i << "] byte value:" <<(int)(*pSrc) << endl;
//        char temp = *pSrc >> 4 & 0x0F;
//        cerr << " high index:" << (int)temp <<", high char:"<< int2CharTable[temp] << endl;
//        temp = *pSrc & 0x0F;
//        cerr << " low index:" << (int)temp << ", low char:" << int2CharTable[temp] << endl;
//#endif
        *pDst++ =  int2CharTable[*pSrc >> 4 & 0x0F];
        *pDst++ =  int2CharTable[*pSrc & 0x0F];
        pSrc--;
    }
    pSrc = (char *)&secondPart + 7;
    for (int i=0; i < 8; ++i) {
//#ifdef DEBUG
//        cerr << "[" << i << "] byte value:" <<(int)(*pSrc) << endl;
//        char temp = *pSrc >> 4 & 0x0F;
//        cerr << " high index:" << (int)temp <<", high char:"<< int2CharTable[temp] << endl;
//        temp = *pSrc & 0x0F;
//        cerr << " low index:" << (int)temp << ", low char:" << int2CharTable[temp] << endl;
//        cerr << " high index:" << (int)temp << endl;
//        temp = *pSrc & 0x0F;
//        cerr << " low index:" << (int)temp << endl;
//#endif
        *pDst++ =  int2CharTable[*pSrc  >> 4 & 0x0F  ];
        *pDst++ =  int2CharTable[*pSrc & 0x0F];
        pSrc--;

    }

    string result = string(buffer, 32);
    //TODO: 性能
    return result;
}

int ConvertHistoryItemToString(::LinkBase::LinkBaseAttr_CrawlHistory item, char* pBuffer) {
    int length = 0;
    if(item.has_downloadtime()) {
        uint32_t value = item.downloadtime();
        sprintf(pBuffer, "%u", value);
        length += strlen(pBuffer);
        pBuffer += strlen(pBuffer);
    } 
    *pBuffer++ = ':';
    length += 1;

    if (item.has_httpcode()) {
        int32_t value = item.httpcode();
        sprintf(pBuffer, "%d", value);
        length += strlen(pBuffer);
        pBuffer += strlen(pBuffer);
    } 
    *pBuffer++ = ':';
    length += 1;

    if (item.contentmd5_size() == 2) {
        uint64_t firstPart = item.contentmd5(0);
        uint64_t secondPart = item.contentmd5(1);
        string md5String = ConvertLongUInt64ToMD5String(firstPart, secondPart);
        memcpy(pBuffer, md5String.c_str(), md5String.size());
        pBuffer += md5String.size();
        length += md5String.size();
    }
    *pBuffer++ = ':';
    length += 1;

    if (item.innerlinksmd5_size() == 2) {
        uint64_t firstPart = item.innerlinksmd5(0);
        uint64_t secondPart = item.innerlinksmd5(1);
        string md5String = ConvertLongUInt64ToMD5String(firstPart, secondPart);
        memcpy(pBuffer, md5String.c_str(), md5String.size());
        pBuffer += md5String.size();
        length += md5String.size();
    }
    *pBuffer++ = ':';
    length += 1;

    if (item.has_contentsize()) {
        int32_t value = item.contentsize();
        sprintf(pBuffer, "%d", value);
        length += strlen(pBuffer);
        pBuffer += strlen(pBuffer);
    }
    *pBuffer++ = ':';
    length += 1;

    if (item.has_innerlinksnum()) {
        int32_t value = item.innerlinksnum();
        sprintf(pBuffer, "%d", value);
        length += strlen(pBuffer);
        pBuffer += strlen(pBuffer);
    }
    *pBuffer++ = ':';
    length += 1;

    if (item.has_inneranchorlength()) {
        int32_t value = item.inneranchorlength();
        sprintf(pBuffer, "%d", value);
        length += strlen(pBuffer);
        pBuffer += strlen(pBuffer);
    }
    *pBuffer++ = ':';
    length += 1;

    if (item.has_outerlinksnum()) {
        int32_t value = item.outerlinksnum();
        sprintf(pBuffer, "%d", value);
        length += strlen(pBuffer);
        pBuffer += strlen(pBuffer);
    }
    *pBuffer++ = ':';
    length+=1;

    if (item.has_outeranchorlength()) {
        int32_t value = item.outeranchorlength();
        sprintf(pBuffer, "%d", value);
        length += strlen(pBuffer);
        pBuffer += strlen(pBuffer);
    }
    *pBuffer++ = ':';
    length += 1;

    if (item.etagmd5_size()) {
        uint64_t firstPart = item.etagmd5(0);
        uint64_t secondPart = item.etagmd5(1);
        string md5String = ConvertLongUInt64ToMD5String(firstPart, secondPart);
        memcpy(pBuffer, md5String.c_str(), md5String.size());
        pBuffer += md5String.size();
        length += md5String.size();
    }
    *pBuffer++ = ':';
    length += 1;

    if (item.has_newchildlinksnumforself()) {
        int32_t value = item.newchildlinksnumforself();
        sprintf(pBuffer, "%d", value);
        length += strlen(pBuffer);
        pBuffer += strlen(pBuffer);
    }
    *pBuffer++ = ':';
    length += 1;

    if (item.has_goodchildlinksnumforself()) {
        int32_t value = item.goodchildlinksnumforself();
        sprintf(pBuffer, "%d", value);
        length += strlen(pBuffer);
        pBuffer += strlen(pBuffer);
    }
    *pBuffer++ = ':';
    length += 1;


    if (item.has_newchildlinksnumforall()) {
        int32_t value = item.newchildlinksnumforall();
        sprintf(pBuffer, "%d", value);
        length += strlen(pBuffer);
        pBuffer += strlen(pBuffer);
    }
    *pBuffer++ = ':';
    length += 1;

    if (item.has_goodchildlinksnumforall()) {
        int32_t value = item.goodchildlinksnumforall();
        sprintf(pBuffer, "%d", value);
        length += strlen(pBuffer);
        pBuffer += strlen(pBuffer);
    }

    return length;
}

void SplitString(string& line, vector<string>& fields, const char* splitChar) {
    //TODO:
    if (line.empty()) {
        return;
    }
    unsigned int start =  0;
    string::size_type end = line.find(splitChar, start);
    while(end != string::npos ) {
        fields.push_back(line.substr(start, end-start));
        start = end+1;
        end = line.find(splitChar, start);
    }
    fields.push_back(line.substr(start, line.length()-start));
}
/* TODO:
void SplitString(const char* inputString, vector<string>& fields, const char* splitChar) {
	if (*inputString == '\0' || inputString == NULL) {

		return;
	}        

	const char*p = inputString;
	char 
	while(*p++!=splitChar)
}
*/


void PB2Map(LinkBase::LinkBaseAttr& item, map<string,string>& attr2Value) {
    char buffer[TEMP_BUFFER_LEN];
    char crawl_his_buffer[CRAWL_HIS_BUFFER_LEN];
    if (item.has_deadlinkadjustschedulepriority()) {
        int32_t value = item.deadlinkadjustschedulepriority();
        //TODO: tou //sprintf(&attr2Value["adjust"], "%d", value)
        bzero(buffer,TEMP_BUFFER_LEN);
        sprintf(buffer, "%d", value);
        attr2Value.insert(make_pair("adjust", buffer));
    }
    
    if (item.has_anchor()) {
        string value = item.anchor();
        attr2Value["anchor"] = value;
    }

    if (item.has_linksnumafterbloomfilter()) {
        int32_t value = item.linksnumafterbloomfilter();
        bzero(buffer,TEMP_BUFFER_LEN);
        sprintf(buffer, "%d", value);
        attr2Value.insert(make_pair("bloomLinkNum", buffer));
    }

    if (item.has_browsepv()) {
        int64_t value = item.browsepv();
        bzero(buffer,TEMP_BUFFER_LEN);
        sprintf(buffer, "%lld", value);
        attr2Value.insert(make_pair("browsePV", buffer));
    }
    if (item.has_browseuv()) {
        int64_t value = item.browseuv();
        bzero(buffer,TEMP_BUFFER_LEN);
        sprintf(buffer, "%lld", value);
        attr2Value.insert(make_pair("browseUV", buffer));
    }
    if (item.has_cndepth()) {
        int32_t value = item.cndepth();
        InsertInt32String(attr2Value, "cnDepth", buffer, value);
    }

    if (item.has_contentsize()) {
        int32_t value = item.contentsize();
        InsertInt32String(attr2Value, "contentSize", buffer, value);
    }

    if (item.has_deadlinktime()) {
        uint32_t value = item.deadlinktime();
        InsertUInt32String(attr2Value, "deadLinkTime", buffer, value);
    }

    if (item.has_deadpagetime()) {
        uint32_t value = item.deadpagetime();
        InsertUInt32String(attr2Value, "deadPageTime", buffer, value);
    }
    
    if (item.has_delreason()) {
        string delReason = item.delreason();
        attr2Value["delReason"] = delReason;
    }

    if (item.has_deltime()) {
        uint32_t value = item.deltime();
        InsertUInt32String(attr2Value, "delTime", buffer, value);
    }

    if (item.has_depth()) {
        int32_t value = item.depth();
        InsertInt32String(attr2Value, "depth", buffer, value);
    }

    if (item.has_downloadtime()) {
        uint32_t value = item.downloadtime();
        InsertUInt32String(attr2Value, "downloadTime", buffer, value);
    }
    if (item.has_downloadtype()) {
        string value = item.downloadtype();
        attr2Value["downloadType"] = value;
    }
    if (item.has_durl()) {
        string value = item.durl();
        attr2Value["durl"] = value;
    }
    if (item.has_errno()) {
        int32_t value = item.errno();
        InsertInt32String(attr2Value, "errno", buffer, value);
    }
    if (item.has_extends()) {
        string value = item.extends();
        attr2Value["extends"] = value;
    }
    if (item.has_fanyetype()) {
        int32_t value = item.fanyetype();
        InsertInt32String(attr2Value, "FanyeType", buffer, value);
    }
    if (item.has_linksnumafterlinkfilter()) {
        int32_t value = item.linksnumafterlinkfilter();
        InsertInt32String(attr2Value, "filterLinkNum", buffer, value);
    }
    if (item.has_firstdownloadtime()) {
        uint32_t value = item.firstdownloadtime();
        InsertUInt32String(attr2Value, "firstDownloadTime", buffer, value);
    }
    if (item.has_foundtime()) {
        uint32_t value = item.foundtime();
        InsertUInt32String(attr2Value, "foundTime", buffer, value);
    }
    if (item.has_garbagetype()) {
        string value = item.garbagetype();
        attr2Value["GarbageType"] = value;
    }
    if (item.has_httpcode()) {
        int32_t value = item.httpcode();
        InsertInt32String(attr2Value, "httpCode", buffer, value);
    }
    if (item.has_initparentpage()) {
        string value = item.initparentpage();
        attr2Value["initParent"] = value;
    }

    if (item.has_inneranchorlength()) {
        int32_t value = item.inneranchorlength();
        InsertInt32String(attr2Value, "innerAnchorLength", buffer, value);
    }

    if (item.has_innerlinksnum()) {
        int32_t value = item.innerlinksnum();
        InsertInt32String(attr2Value, "innerLinks", buffer, value);
    }

    if (item.has_latestfoundtime()) {
        uint32_t value = item.latestfoundtime();
        InsertUInt32String(attr2Value, "latestFoundTime", buffer, value);
    }

    if (item.has_bestinnerlinkshrefpattern()){
        string value = item.bestinnerlinkshrefpattern();
        attr2Value["linkBestHref"] = value ;
    }

    if (item.has_manualimporttag()) {
        string value = item.manualimporttag();
        attr2Value["manualImportTag"] = value ;
    }
    if (item.has_isnewlink()) {
        bool value = item.isnewlink(); 
        attr2Value["newLink"] = value ? "true" : "false";
    }

    if (item.has_offlinepagetype()){
        int32_t value = item.offlinepagetype();
        InsertInt32String(attr2Value, "offline_pagetype", buffer, value);
    }
    if (item.has_optype()) {
        string value = item.optype();
        attr2Value["opType"] = value ;
    }
    if (item.has_orglinknum()) {
        int32_t value = item.orglinknum();
        InsertInt32String(attr2Value, "orgLinkNum", buffer, value);
    }

    if (item.has_outeranchorlength()){
        int32_t value = item.outeranchorlength();
        InsertInt32String(attr2Value, "outerAnchorLength", buffer, value);
    }

    if (item.has_outerlinksnum()) {
        int32_t value = item.outerlinksnum();
        InsertInt32String(attr2Value, "outerLinks", buffer, value);
    }

    if (item.has_pagelanguage()) {
        string value = item.pagelanguage();
        attr2Value["pageLang"] = value ;
    }
    if (item.has_pagepublishdate()) {
        uint32_t value = item.pagepublishdate();
        InsertUInt32String(attr2Value, "pagePdate", buffer, value);
    }

    if (item.has_pagerank()) {
        string value = item.pagerank();
        attr2Value["pageRank"] = value ;
    }

    if (item.has_pagesize()) {
        int32_t value = item.pagesize();
        InsertInt32String(attr2Value, "pageSize", buffer, value);
    }
    if (item.has_pagetype()) {
        string value = item.pagetype();
        attr2Value["pageType"] = value ;
    }

    if (item.has_parentbrowseuv()) {
        int64_t value = item.parentbrowseuv();
        InsertInt64String(attr2Value, "parentBrowseUV", buffer, value);
    }
    if (item.has_parentdownloadtime()) {
        uint32_t value = item.parentdownloadtime();
        InsertUInt32String(attr2Value, "parentDownloadTime", buffer, value);
    }
    if (item.has_parentpage()) {
        string value = item.parentpage();
        attr2Value["parentPage"] = value ;
    }
    if (item.has_parentpagedepth()) {
        int32_t value = item.parentpagedepth();
        InsertInt32String(attr2Value, "parentPageDepth", buffer, value);
    }
    if (item.has_parentpagerank()) {
        string value = item.parentpagerank();
        attr2Value["parentPageRank"] = value ;
    }
    if (item.has_parentpagetype()) {
        string value = item.parentpagetype();
        attr2Value["parentPageType"] = value ;
    }
    if (item.has_parentsearchuv()) {
        int64_t value = item.parentsearchuv();
        InsertInt64String(attr2Value, "parentSearchUV", buffer, value);
    }
    if (item.has_linksnumafterqssfilter()) {
        int32_t value = item.linksnumafterqssfilter();
        InsertInt32String(attr2Value, "qssLinkNum", buffer, value);
    }
    if (item.has_linksnumafterrobotsfilter()) {
        int32_t value = item.linksnumafterrobotsfilter();
        InsertInt32String(attr2Value, "robotsLinkNum", buffer, value);
    }
    if (item.has_scheduletime()) {
        uint32_t value = item.scheduletime();
        InsertUInt32String(attr2Value, "scheduleTime", buffer, value);
    }
    if (item.has_searchuv()) {
        int64_t value = item.searchuv();
        InsertInt64String(attr2Value, "searchUV", buffer, value);
    }
    if (item.has_parentseedlevel()) {
        int32_t value = item.parentseedlevel();
        InsertInt32String(attr2Value, "seed_level", buffer, value);
    }
    if (item.has_linksnumafternormalized()) {
        int32_t value = item.linksnumafternormalized();
        //TODO: 可能有错???
        InsertInt32String(attr2Value, "unifiedLinkNum", buffer, value);
    }
    if (item.has_normalizedurl()) {
        string value = item.normalizedurl();
        attr2Value["uniUrl"] = value;
    }

    if (item.hubcrawlhis_size()>0) {
        bzero(crawl_his_buffer, CRAWL_HIS_BUFFER_LEN);
        char * pBuffer = crawl_his_buffer;
        int totalLength = 0;
        
        //TODO: 怪怪的
        for (int i=0; i<item.hubcrawlhis_size(); ++i) {
            const ::LinkBase::LinkBaseAttr_CrawlHistory& value = item.hubcrawlhis(i);
            int length = ConvertHistoryItemToString(value, pBuffer);
            pBuffer += length;
            totalLength += length;
            if (i!=item.hubcrawlhis_size()-1) {
                *pBuffer++ = ';';
                totalLength++;
            }
        }
        attr2Value["hubCrawlHis"] = string(crawl_his_buffer, totalLength);
    }
    if (item.normalcrawlhis_size()>0) {
        bzero(crawl_his_buffer, CRAWL_HIS_BUFFER_LEN);
        char * pBuffer = crawl_his_buffer;
        int totalLength = 0;
        
        //TODO: 怪怪的
        for (int i=0; i<item.normalcrawlhis_size(); ++i) {
            const ::LinkBase::LinkBaseAttr_CrawlHistory& value = item.normalcrawlhis(i);
            int length = ConvertHistoryItemToString(value, pBuffer);
            pBuffer += length;
            totalLength += length;
            if (i!=item.normalcrawlhis_size()-1) {
                *pBuffer++ = ';';
                totalLength++;
            }
        }
        attr2Value["crawlHis"] = string(crawl_his_buffer, totalLength);
    }
    if (item.othercrawlhis_size()>0) {
        bzero(crawl_his_buffer, CRAWL_HIS_BUFFER_LEN);
        char * pBuffer = crawl_his_buffer;
        int totalLength = 0;
        
        //TODO: 怪怪的
        for (int i=0; i<item.othercrawlhis_size(); ++i) {
            const ::LinkBase::LinkBaseAttr_CrawlHistory& value = item.othercrawlhis(i);
            int length = ConvertHistoryItemToString(value, pBuffer);
            pBuffer += length;
            totalLength += length;
            if (i!=item.othercrawlhis_size()-1) {
                *pBuffer++ = ';';
                totalLength++;
            }
        }
        attr2Value["otherCrawlHis"] = string(crawl_his_buffer, totalLength);
    }

    if (item.contentmd5_size() == 2) {
        uint64_t firstPart = item.contentmd5(0);
        uint64_t secondPart = item.contentmd5(1);
        string md5String = ConvertLongUInt64ToMD5String(firstPart, secondPart);
        attr2Value["contentMD5"] = md5String;
    }


    if (item.innerlinksmd5_size()) {
        uint64_t firstPart = item.innerlinksmd5(0);
        uint64_t secondPart = item.innerlinksmd5(1);
        string md5String = ConvertLongUInt64ToMD5String(firstPart, secondPart);
        attr2Value["innerLinksMD5"] = md5String;
    }

    if (item.etagmd5_size()) {
        uint64_t firstPart = item.etagmd5(0);
        uint64_t secondPart = item.etagmd5(1);
        string md5String = ConvertLongUInt64ToMD5String(firstPart, secondPart);
        attr2Value["eTag"] = md5String;
    }

    if (item.has_childnum()) {
        int32_t value = item.childnum();
        InsertInt32String(attr2Value, "Child", buffer, value);
    }
    if (item.has_goodchildnum()) {
        int32_t value = item.goodchildnum();
        InsertInt32String(attr2Value, "gChild", buffer, value);
    }

    if (item.has_goodparentstatnum()) {
        int32_t value = item.goodparentstatnum();
        InsertInt32String(attr2Value, "gParentStatNum", buffer, value);
    }
    if (item.has_isexpired()) {
        bool value = item.isexpired();
        attr2Value["expired"] = value ? "1":"0";
    }
    if (item.has_asseedlevel()) {
        int32_t value = item.asseedlevel();
        InsertInt32String(attr2Value, "seedLevel", buffer, value);
    }

    if (item.has_pagemodifydate()) {
        uint32_t value = item.pagemodifydate();
        InsertUInt32String(attr2Value, "pageModifydate", buffer, value);
    }
    if (item.has_forbid()) {
        int32_t value = item.forbid();
        InsertInt32String(attr2Value, "forbid", buffer, value);
    }

    //TODO:添加测试用例
    if (item.has_isinindex()) {
        bool value = item.isinindex();
        attr2Value["inIndex"] = value ? "1":"0";
    }
}

inline bool GetInt32Value(map<string, string>& attr2Value, const char* fieldName, int32_t & value) {
    map<string, string>::iterator iter = attr2Value.find(fieldName);
    if (iter != attr2Value.end()) {
        sscanf((iter->second).c_str(), "%d", &value);
        return true;
    } else {
        return false;
    }
}

inline bool GetUInt32Value(map<string, string>& attr2Value, const char* fieldName, uint32_t & value) {
    map<string, string>::iterator iter = attr2Value.find(fieldName);
    if (iter != attr2Value.end()) {
        sscanf((iter->second).c_str(), "%u", &value);
        return true;
    } else {
        return false;
    }
}
inline bool GetInt64Value(map<string, string>& attr2Value, const char* fieldName, int64_t & value) {
    map<string, string>::iterator iter = attr2Value.find(fieldName);
    if (iter != attr2Value.end()) {
        sscanf((iter->second).c_str(), "%lld", &value);
        return true;
    } else {
        return false;
    }
}

//note: little endian
int ParseHexChar(const char * p) {
    int result;
    if (*p >= 'a' && *p <= 'f') {
        result = *p - 'a' + 10;
    }

    if (*p >= 'A' && *p <= 'F') {
        result = *p - 'A' + 10;
    }
    if (*p >= '0' && *p <= '9') {
        result = *p - '0';
    }

    //cerr << "DEBUG *p= " << *p << ", result= " << (int)result << endl;
    return result ;
}
void ConvertMD5StringToLongs(string& md5String, uint64_t& firstLong, uint64_t& secondLong) {
    assert(md5String.size()==32);
    const char * pSrc = md5String.c_str();
    char* pDst = (char*)&firstLong + 7;
    for (int i=0;i<8;++i) {
        *pDst = ParseHexChar(pSrc++);
        //TODO 尝试移位操作
        *pDst-- = (char)(*pDst * 16 + ParseHexChar(pSrc++));
    }
    pDst = (char*)&secondLong + 7;
    for (int i=0;i<8;++i) {
        *pDst = ParseHexChar(pSrc++);
        *pDst-- = (char)(*pDst * 16 + ParseHexChar(pSrc++));
    }

//#ifdef DEBUG
//    char * p = (char*)&firstLong;
//    cerr << "md5String=" << md5String << endl;
//    for (int i=0;i<8;++i) {
//        cerr << "firstLong[" << i<<"]=" << (int)*p++ << endl;
//    }
//    p = (char*)&secondLong;
//    for (int i=0;i<8;++i) {
//        cerr << "secondLong[" << i<<"]=" << (int)*p++ << endl;
//    }
//#endif
}
void    SetCrawlHistoryItem(::LinkBase::LinkBaseAttr_CrawlHistory* pCrawlHisItem, vector<string>& attrFields) {
            if (attrFields.size()>=14) {
                if (!attrFields[0].empty()) {
                    uint32_t value;
                    sscanf(attrFields[0].c_str(), "%u", &value);
                    pCrawlHisItem->set_downloadtime(value);
                }
                if (!attrFields[1].empty()) {
                    int32_t value;
                    sscanf(attrFields[1].c_str(), "%d", &value);
                    pCrawlHisItem->set_httpcode(value);
                }
                if (!attrFields[2].empty()) {
                    uint64_t firstLong;
                    uint64_t secondLong;
                    ConvertMD5StringToLongs(attrFields[2], firstLong, secondLong);
                    pCrawlHisItem->add_contentmd5(firstLong);
                    pCrawlHisItem->add_contentmd5(secondLong);
                }
                if (!attrFields[3].empty()) {
                    uint64_t firstLong;
                    uint64_t secondLong;
                    ConvertMD5StringToLongs(attrFields[3], firstLong, secondLong);
                    pCrawlHisItem->add_innerlinksmd5(firstLong);
                    pCrawlHisItem->add_innerlinksmd5(secondLong);
                }

                if (!attrFields[4].empty()) {
                    int32_t value;
                    sscanf(attrFields[4].c_str(), "%d", &value);
                    pCrawlHisItem->set_contentsize(value);
                }
                if (!attrFields[5].empty()) {
                    int32_t value;
                    sscanf(attrFields[5].c_str(), "%d", &value);
                    pCrawlHisItem->set_innerlinksnum(value);
                }
                if (!attrFields[6].empty()) {
                    int32_t value;
                    sscanf(attrFields[6].c_str(), "%d", &value);
                    pCrawlHisItem->set_inneranchorlength(value);
                }
                if (!attrFields[7].empty()) {
                    int32_t value;
                    sscanf(attrFields[7].c_str(), "%d", &value);
                    pCrawlHisItem->set_outerlinksnum(value);
                }
                if (!attrFields[8].empty()) {
                    int32_t value;
                    sscanf(attrFields[8].c_str(), "%d", &value);
                    pCrawlHisItem->set_outeranchorlength(value);
                }
                if (!attrFields[9].empty()) {
                    uint64_t firstLong;
                    uint64_t secondLong;
                    ConvertMD5StringToLongs(attrFields[9], firstLong, secondLong);
                    pCrawlHisItem->add_etagmd5(firstLong);
                    pCrawlHisItem->add_etagmd5(secondLong);
                }
                if (!attrFields[10].empty()) {
                    int32_t value;
                    sscanf(attrFields[10].c_str(), "%d", &value);
                    pCrawlHisItem->set_newchildlinksnumforself(value);
                }
                if (!attrFields[11].empty()) {
                    int32_t value;
                    sscanf(attrFields[11].c_str(), "%d", &value);
                    pCrawlHisItem->set_goodchildlinksnumforself(value);
                }
                if (!attrFields[12].empty()) {
                    int32_t value;
                    sscanf(attrFields[12].c_str(), "%d", &value);
                    pCrawlHisItem->set_newchildlinksnumforall(value);
                }
                if (!attrFields[13].empty()) {
                    int32_t value;
                    sscanf(attrFields[13].c_str(), "%d", &value);
                    pCrawlHisItem->set_goodchildlinksnumforall(value);
                }
            }
}

void Map2PB(map<string,string>& attr2Value, LinkBase::LinkBaseAttr& item) {

    map<string,string>::iterator tempIter ;
    int32_t tempInt32Value;
    uint32_t tempUInt32Value;
    int64_t tempInt64Value;
    
    tempIter=attr2Value.find("parentPage");
    if (tempIter != attr2Value.end()){
        item.set_parentpage(tempIter->second);
    }

    if (GetInt32Value(attr2Value, "depth", tempInt32Value)) {
        item.set_depth(tempInt32Value);
    }

    tempIter=attr2Value.find("anchor");
    if (tempIter!=attr2Value.end()) {
        item.set_anchor(attr2Value["anchor"]);
    }
    
    if (GetUInt32Value(attr2Value, "foundTime", tempUInt32Value)) {
        item.set_foundtime(tempUInt32Value);
    }

    tempIter = attr2Value.find("crawlHis");
    if (tempIter != attr2Value.end()) {
        string crawlHistoryString = tempIter->second;
        vector<string> itemFields;
        SplitString(crawlHistoryString, itemFields, ";");
        for (vector<string>::iterator iter = itemFields.begin(); iter != itemFields.end(); ++iter) {
            if (iter->empty()) {
                continue;
            }
            string& itemString = *iter;
            vector<string> attrFields;
            SplitString(itemString, attrFields, ":");
            ::LinkBase::LinkBaseAttr_CrawlHistory* pCrawlHisItem = item.add_normalcrawlhis();
            SetCrawlHistoryItem(pCrawlHisItem, attrFields);
        }
    }

    tempIter = attr2Value.find("otherCrawlHis");
    if (tempIter != attr2Value.end()) {
        string crawlHistoryString = tempIter->second;
        vector<string> itemFields;
        SplitString(crawlHistoryString, itemFields, ";");
        for (vector<string>::iterator iter = itemFields.begin(); iter != itemFields.end(); ++iter) {
            if (iter->empty()) {
                continue;
            }
            string& itemString = *iter;
            vector<string> attrFields;
            SplitString(itemString, attrFields, ":");
            ::LinkBase::LinkBaseAttr_CrawlHistory* pCrawlHisItem = item.add_othercrawlhis();
            SetCrawlHistoryItem(pCrawlHisItem, attrFields);
        }
    }
    tempIter = attr2Value.find("hubCrawlHis");
    if (tempIter != attr2Value.end()) {
        string crawlHistoryString = tempIter->second;
        vector<string> itemFields;
        SplitString(crawlHistoryString, itemFields, ";");
        for (vector<string>::iterator iter = itemFields.begin(); iter != itemFields.end(); ++iter) {
            if (iter->empty()) {
                continue;
            }
            string& itemString = *iter;
            vector<string> attrFields;
            SplitString(itemString, attrFields, ":");
            ::LinkBase::LinkBaseAttr_CrawlHistory* pCrawlHisItem = item.add_hubcrawlhis();
            SetCrawlHistoryItem(pCrawlHisItem, attrFields);
        }
    }

    if (GetInt32Value(attr2Value, "innerLinks", tempInt32Value)) {
        item.set_innerlinksnum(tempInt32Value);
    }
    if (GetInt32Value(attr2Value, "seed_level", tempInt32Value)) {
        item.set_parentseedlevel(tempInt32Value);
    }
    if (GetUInt32Value(attr2Value, "pagePdate", tempUInt32Value)) {
        item.set_pagepublishdate(tempUInt32Value);
    }
    if (GetInt32Value(attr2Value, "bloomLinkNum", tempInt32Value)) {
        item.set_linksnumafterbloomfilter(tempInt32Value);
    }

    tempIter = attr2Value.find("uniUrl");
    if (tempIter != attr2Value.end()) {
        item.set_normalizedurl(tempIter->second);
    }

    if (GetInt32Value(attr2Value, "filterLinkNum", tempInt32Value)) {
        item.set_linksnumafterlinkfilter(tempInt32Value);
    }

    tempIter = attr2Value.find("durl");
    if (tempIter != attr2Value.end()) {
        item.set_durl(tempIter->second);
    }

    if (GetInt32Value(attr2Value, "outerLinks", tempInt32Value)) {
        item.set_outerlinksnum(tempInt32Value);
    }

    if (GetInt64Value(attr2Value, "browseUV", tempInt64Value)) {
        item.set_browseuv(tempInt64Value);
    }
    if (GetInt64Value(attr2Value, "searchUV", tempInt64Value)) {
        item.set_searchuv(tempInt64Value);
    }

    tempIter = attr2Value.find("newLink");
    if (tempIter!= attr2Value.end()) {
        item.set_isnewlink((strcmp(tempIter->second.c_str(), "true")==0));
    }
    tempIter=attr2Value.find("downloadType");
    if (tempIter != attr2Value.end()){
        item.set_downloadtype(tempIter->second);
    }
    tempIter=attr2Value.find("manualImportTag");
    if (tempIter != attr2Value.end()){
        item.set_manualimporttag(tempIter->second);
    }
    tempIter=attr2Value.find("pageLang");
    if (tempIter != attr2Value.end()){
        item.set_pagelanguage(tempIter->second);
    }

    if (GetInt64Value(attr2Value, "parentBrowseUV", tempInt64Value)) {
        item.set_parentbrowseuv(tempInt64Value);
    }

    tempIter=attr2Value.find("pageRank");
    if (tempIter != attr2Value.end()){
        item.set_pagerank(tempIter->second);
    }

    tempIter=attr2Value.find("parentPageRank");
    if (tempIter != attr2Value.end()){
        item.set_parentpagerank(tempIter->second);
    }
 if (GetUInt32Value(attr2Value, "foundTime", tempUInt32Value)) {
        item.set_foundtime(tempUInt32Value);
    }


    tempIter=attr2Value.find("extends");
    if (tempIter != attr2Value.end()){
        item.set_extends(tempIter->second);
    }
    if (GetInt32Value(attr2Value, "pageSize", tempInt32Value)) {
        item.set_pagesize(tempInt32Value);
    }
    tempIter=attr2Value.find("opType");
    if (tempIter != attr2Value.end()){
        item.set_optype(tempIter->second);
    }
    if (GetInt32Value(attr2Value, "outerAnchorLength", tempInt32Value)) {
        item.set_outeranchorlength(tempInt32Value);
    }
    if (GetUInt32Value(attr2Value, "scheduleTime", tempUInt32Value)) {
        item.set_scheduletime(tempUInt32Value);
    }

    if (GetInt32Value(attr2Value, "errno", tempInt32Value)) {
        item.set_errno(tempInt32Value);
    }

    tempIter=attr2Value.find("linkBestHref");
    if (tempIter != attr2Value.end()){
        item.set_bestinnerlinkshrefpattern(tempIter->second);
    }

    tempIter=attr2Value.find("initParent");
    if (tempIter != attr2Value.end()){
        item.set_initparentpage(tempIter->second);
    }

    tempIter=attr2Value.find("delReason");
    if (tempIter != attr2Value.end()){
        item.set_delreason(tempIter->second);
    }
    if (GetInt32Value(attr2Value, "orgLinkNum", tempInt32Value)) {
        item.set_orglinknum(tempInt32Value);
    }
    if (GetInt64Value(attr2Value, "browsePV", tempInt64Value)) {
        item.set_browsepv(tempInt64Value);
    }
    if (GetInt32Value(attr2Value, "cnDepth", tempInt32Value)) {
        item.set_cndepth(tempInt32Value);
    }

    tempIter=attr2Value.find("parentPageType");
    if (tempIter != attr2Value.end()){
        item.set_parentpagetype(tempIter->second);
    }
    if (GetInt32Value(attr2Value, "httpCode", tempInt32Value)) {
        item.set_httpcode(tempInt32Value);
    }
    if (GetUInt32Value(attr2Value, "parentDownloadTime", tempUInt32Value)) {
        item.set_parentdownloadtime(tempUInt32Value);
    }

    if (GetInt32Value(attr2Value, "innerAnchorLength", tempInt32Value)) {
        item.set_inneranchorlength(tempInt32Value);
    }
    if (GetUInt32Value(attr2Value, "latestFoundTime", tempUInt32Value)) {
        item.set_latestfoundtime(tempUInt32Value);
    }

    //TODO: hun
    if (GetInt32Value(attr2Value, "robotsLinkNum", tempInt32Value)) {
        item.set_linksnumafterrobotsfilter(tempInt32Value);
    }
    if (GetInt32Value(attr2Value, "qssLinkNum", tempInt32Value)) {
        item.set_linksnumafterqssfilter(tempInt32Value);
    }

    if (GetUInt32Value(attr2Value, "deadPageTime", tempUInt32Value)) {
        item.set_deadpagetime(tempUInt32Value);
    }

    if (GetUInt32Value(attr2Value, "deadLinkTime", tempUInt32Value)) {
        item.set_deadlinktime(tempUInt32Value);
    }
    if (GetInt32Value(attr2Value, "unifiedLinkNum", tempInt32Value)) {
        item.set_linksnumafternormalized(tempInt32Value);
    }
    if (GetUInt32Value(attr2Value, "downloadTime", tempUInt32Value)) {
        item.set_downloadtime(tempUInt32Value);
    }
    if (GetInt32Value(attr2Value, "contentSize", tempInt32Value)) {
        item.set_contentsize(tempInt32Value);
    }

    tempIter = attr2Value.find("contentMD5");
    if (tempIter != attr2Value.end()) {
        uint64_t firstLong;
        uint64_t secondLong;
        ConvertMD5StringToLongs(tempIter->second, firstLong, secondLong);
        item.add_contentmd5(firstLong);
        item.add_contentmd5(secondLong);
    }

    if (GetInt64Value(attr2Value, "parentSearchUV", tempInt64Value)) {
        item.set_parentsearchuv(tempInt64Value);
    }
    if (GetUInt32Value(attr2Value, "delTime", tempUInt32Value)) {
        item.set_deltime(tempUInt32Value);
    }
    if (GetInt32Value(attr2Value, "parentPageDepth", tempInt32Value)) {
        item.set_parentpagedepth(tempInt32Value);
    }
    tempIter = attr2Value.find("innerLinksMD5");
    if (tempIter != attr2Value.end()) {
        uint64_t firstLong;
        uint64_t secondLong;
        ConvertMD5StringToLongs(tempIter->second, firstLong, secondLong);
        item.add_innerlinksmd5(firstLong);
        item.add_innerlinksmd5(secondLong);
    }

    tempIter=attr2Value.find("pageType");
    if (tempIter != attr2Value.end()){
        item.set_pagetype(tempIter->second);
    }

    tempIter = attr2Value.find("eTag");
    if (tempIter != attr2Value.end()) {
        uint64_t firstLong;
        uint64_t secondLong;
        ConvertMD5StringToLongs(tempIter->second, firstLong, secondLong);
        item.add_etagmd5(firstLong);
        item.add_etagmd5(secondLong);
    }

    if (GetUInt32Value(attr2Value, "deadLinkTime", tempUInt32Value)) {
        item.set_deadlinktime(tempUInt32Value);
    }

    if (GetInt32Value(attr2Value, "offline_pagetype", tempInt32Value)) {
        item.set_offlinepagetype(tempInt32Value);
    }
    if (GetInt32Value(attr2Value, "FanyeType", tempInt32Value)) {
        item.set_fanyetype(tempInt32Value);
    }

    if (GetUInt32Value(attr2Value, "firstDownloadTime", tempUInt32Value)) {
        item.set_firstdownloadtime(tempUInt32Value);
    }

    if (GetInt32Value(attr2Value, "adjust", tempInt32Value)) {
        item.set_deadlinkadjustschedulepriority(tempInt32Value);
    }

    tempIter=attr2Value.find("GarbageType");
    if (tempIter != attr2Value.end()){
        item.set_garbagetype(tempIter->second);
    }

    if (GetInt32Value(attr2Value, "Child", tempInt32Value)) {
        item.set_childnum(tempInt32Value);
    }
    if (GetInt32Value(attr2Value, "gChild", tempInt32Value)) {
        item.set_goodchildnum(tempInt32Value);
    }
    if (GetInt32Value(attr2Value, "gParentStatNum", tempInt32Value)) {
        item.set_goodparentstatnum(tempInt32Value);
    }

    if (GetInt32Value(attr2Value, "expired", tempInt32Value)) {
        item.set_isexpired(tempInt32Value==1);
    }
    if (GetInt32Value(attr2Value, "seedLevel", tempInt32Value)) {
        item.set_asseedlevel(tempInt32Value);
    }
    if (GetUInt32Value(attr2Value, "pageModifyDate", tempUInt32Value)) {
        item.set_pagemodifydate(tempUInt32Value);
    }
}

void Map2JsonString(map<string,string>& attr2Value, char* pResult) {

    map<string,string>::iterator iter;
    *pResult = '{';

    for(iter=attr2Value.begin(); iter!=attr2Value.end();iter++) {
        pResult++;
        {
            string key = iter->first;
            *pResult++ = '\"';
            const char * pChar = key.c_str();
            while(*pChar!='\0') {
                *pResult ++ = *pChar++;
            }
            *pResult++ = '\"';
            *pResult++ = ':';
        }
        {
            string value = iter->second;
            *pResult++ = '\"';
            const char * pChar = value.c_str();
            while(*pChar!='\0') {
                if (*pChar=='"' || *pChar == '\'') {
                    *pResult ++ = '\\';
                    *pResult ++ = *pChar++;
                } else {
                    *pResult ++ = *pChar++;

                }
            }
            *pResult++ = '\"';
            *pResult = ',';
        }
    }

    //TODO: 内存溢出
    if (attr2Value.size()==0) {
        pResult++;
    }
    *pResult++ = '}';
    *pResult = '\0';

}

void PB2String() {

}
int del_str_line(char *str)
{
    char *p = str;
    int index = 0;
    while(('\n' != *p) && ('\0' != *p))
    {
        p++;
	index++;
	cerr << "index=" << index << endl;
    }

    if(*p = '\n') //最后一行EOF不包含\n
    {
	    *p = '\0';
	    return 0;

    }
 
    return 0;
}
 

int main(int argc, char** argv) {
    if (argc!=2) {
        Usage();
        return -1;
    }

    int fieldNumber = atoi(argv[1]);

    cerr << "[INFO]fieldNumber=" << fieldNumber << endl;


    char buffer_line[MAX_BUFFER_LEN];
    char buffer[MAX_BUFFER_LEN];
    bzero(buffer_line, MAX_BUFFER_LEN);
#ifdef READ_FROM_FILE
    FILE* input = fopen("./test_input", "r");
    while(fgets(buffer_line,MAX_BUFFER_LEN, input)) {
#else
    while(fgets(buffer_line,MAX_BUFFER_LEN, stdin)) {
#endif 
	//del_str_line(buffer_line);
       //cout << "Read from input:" << buffer_line << endl;
        vector<string>fields;
        //TODO:性能!!!
        string line(buffer_line);
        SplitString(line,fields, "\t");
        string base64String = fields.at(fieldNumber);
	//cerr << "base64String.at() (in int) = " << (int)base64String.at(base64String.size()-1) << endl;
        //1.base64_decode 
        //TODO: 研究
        string binData = qbase64_decode(base64String.c_str(), base64String.size() - 1);

        LinkBase::LinkBaseAttr pbItem;
        if (!pbItem.ParseFromString(binData)){
		cerr << "[ERROR] pbItem.ParseFromString return 0" << endl;
            continue;
        }    

        //TODO:2
        map<string, string> attr2Value;
        PB2Map(pbItem, attr2Value);

//#ifdef DEBUG
//        for (map<string, string>::iterator iter =attr2Value.begin(); iter != attr2Value.end(); ++iter) {
//            cerr << "[DEBUG] map [" << iter->first << "] ->" << iter->second << endl;
//        }
//#endif

#ifdef DEBUG
        LinkBase::LinkBaseAttr convertedItem;
        Map2PB(attr2Value, convertedItem);
        string convertedBinData;
        convertedItem.SerializeToString(&convertedBinData);
	string srcDebugString = pbItem.DebugString();
	string dstDebugString = convertedItem.DebugString();
	
        if(convertedBinData.compare(binData) != 0) {

            cerr << "[ERROR]srcDebugString.compare(dstDebugString)="<< srcDebugString.compare(dstDebugString) << endl;
	    cerr << "[ERROR]input line:" << line << endl;
	    cerr << "[ERROR]srcDebugString.size() = "<< srcDebugString.size() << ", dstDebugString.size()="<< dstDebugString.size() << endl; 
	    const char *pSrc = srcDebugString.c_str();
	    const char *pDst = dstDebugString.c_str();
	    if (srcDebugString.size() == dstDebugString.size()) {
		    for (int i=0; i < srcDebugString.size() ; ++i) {
			    if (*pSrc != *pDst) {
				    cerr << "["<< i << "] src=" << (int)*pSrc << ", dst=" << (int)*pDst << endl;
			    }
			    pSrc++;
			    pDst++;

		    }

	    }

            map<string, string> convertedAttr2Value;
            PB2Map(convertedItem, convertedAttr2Value);

            for (map<string, string>::iterator iter =attr2Value.begin(); iter != attr2Value.end(); ++iter) {
                string key = iter->first;
                string value = iter->second;
                map<string, string>::iterator convertedIter = convertedAttr2Value.find(key);
                if (convertedIter == convertedAttr2Value.end()) {
                    cerr <<"ERROR: attr2Value has key " << key << ", but convetedAttr2Value not have " << endl; 
                    continue;
                }

                string& convertedValue = convertedIter -> second;
                if (convertedValue.compare(value) != 0) {
                    cerr <<"ERROR: attr2Value["<<key <<"]="<< value << ", but convertedAttr2Value[" << key << "]="<< convertedValue << endl; 
                    continue;
                }
            }

            for (map<string, string>::iterator convertedIter=convertedAttr2Value.begin(); convertedIter != convertedAttr2Value.end(); ++convertedIter) {
                string key = convertedIter ->first;
                string convertedValue= convertedIter->second;
                map<string, string>::iterator iter = attr2Value.find(key);
                if (iter == attr2Value.end()) {
                    cerr <<"ERROR: convetedAttr2Value has key " << key << ", but attr2Value not have " << endl; 
                    continue;
                }

                string value = iter-> second;
                if (convertedValue.compare(value) != 0) {
                    cerr <<"ERROR: attr2Value["<<key <<"]="<< value << ", but convertedAttr2Value[" << key << "]="<< convertedValue << endl; 
                    continue;
                }
            }



            for (map<string, string>::iterator iter =attr2Value.begin(); iter != attr2Value.end(); ++iter) {
                cerr << "[DEBUG] origin map [" << iter->first << "] ->" << iter->second << endl;
            }

            for (map<string, string>::iterator iter =convertedAttr2Value.begin(); iter != convertedAttr2Value.end(); ++iter) {
                cerr << "[DEBUG] converted map [" << iter->first << "] ->" << iter->second << endl;
            }
	    
            cerr << " *********************** " << endl;
            cerr << "[DEBUG] srcDebugString=" << srcDebugString << endl;
            cerr << " *********************** " << endl;
            cerr << "[DEBUG] dstDebugString=" << dstDebugString << endl;
            cerr << " *********************** " << endl;

        }
#endif

        bzero(buffer,MAX_BUFFER_LEN);

        Map2JsonString(attr2Value, buffer);
        if (fieldNumber!=0) {
            cout << fields[0];
        } else {
            cout << buffer;
        }

        for (int i=1; i<fields.size(); ++i) {
            if (i == fieldNumber) {
                cout << "\t" << buffer;
            } else {
                cout << "\t" << fields[i];
            }
        }
        cout << endl;
    }

#ifdef READ_FROM_FILE
    fclose(input);
#endif 
    return 0;
}
