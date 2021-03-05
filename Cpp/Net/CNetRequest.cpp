//
//  CNetRequest.cpp
//  ZJCrossCpp
//
//  Created by eafy on 2021/3/1.
//  Copyright © 2021 ZJ. All rights reserved.
//

#include "CNetRequest.hpp"
#include <unistd.h>
#include "curl.h"
#include "CPrintfLog.hpp"
#include "CString.hpp"

static size_t OnProgressResultCallback(void *userData, long long dlTotal, long long dlNow, long long ulTotal, long long ulNow) {
    CNetRequest *request = static_cast<CNetRequest *>(userData);
    if (request) {
        request->OnProgressResult(request->m_pUserData, dlTotal, dlNow, ulTotal, ulNow);
    }
    return 0;
}

static size_t OnGetContentLengthCallback(void *ptr, size_t size, size_t nmemb, void *stream)
{
    long len = 0;
    int r = sscanf((const char*)ptr, "Content-Length: %ld\n", &len);
    if (r) {
        *((long *)stream) = len;
    }
    return size * nmemb;
}

#pragma mark -

CNetRequest::CNetRequest(void *pUserData)
{
    m_pUserData = pUserData;
    m_pErrorBuf = new char[CURL_ERROR_SIZE]();
    m_bRequesting.store(false);
}

CNetRequest::~CNetRequest()
{
    if (m_bRequesting.load()) {
        Cancel();
        while (m_pCurl) {
            usleep(5);
        }
    }
        
    RemoveCurl(&m_pCurl);
    RemoveHeader();
    if (m_pErrorBuf) {
        delete [] m_pErrorBuf;
        m_pErrorBuf = nullptr;
    }
}

void CNetRequest::Clear()
{
    RemoveCurl(&m_pCurl);
    RemoveHeader();
    m_oParaJsonObj.Clear();
    m_sParaString = "";
    m_bRequesting.store(false);
}

void CNetRequest::RemoveCurl(void **curl)
{
    if (curl != nullptr && *curl != nullptr) {
        curl_easy_cleanup(*curl);
        *curl = nullptr;
    }
    curl = nullptr;
}

#pragma mark -

CURL *CNetRequest::GetHandleWithURL(const std::string url, bool bAppendUrl, bool bProgress)
{
    if (m_pCurl) return m_pCurl;
    m_sUrl = url;
    
    m_pCurl = curl_easy_init();
    if (m_pCurl == nullptr) {
        CPrintfE("Failed to create CURL connection");
        return nullptr;
    }
    
    std::string sURL = url;
    BuildParameters();
    if (bAppendUrl) {
        if (m_sParaString.length() > 0) {
            if (sURL.find("?") != std::string::npos) {   //链接已附带参数
                sURL += "&" + m_sParaString;
            } else {
                sURL += "?" + m_sParaString;
            }
        }
    } else {
        BuildParameters();
        curl_easy_setopt(m_pCurl, CURLOPT_POSTFIELDS, m_sParaString.c_str());
        curl_easy_setopt(m_pCurl, CURLOPT_POSTFIELDSIZE, m_sParaString.length());
    }
    
    curl_easy_setopt(m_pCurl, CURLOPT_ERRORBUFFER, m_pErrorBuf);
    CURLcode code = curl_easy_setopt(m_pCurl, CURLOPT_URL, sURL.c_str());
    if (code != CURLE_OK) {
        CPrintfE("Failed to set url:%s, error:%s", sURL.c_str(), m_pErrorBuf);
        RemoveCurl(&m_pCurl);
        return nullptr;
    }
    
    curl_easy_setopt(m_pCurl, CURLOPT_NOSIGNAL, true);     //毫秒超时需要设置
    curl_easy_setopt(m_pCurl, CURLOPT_CONNECTTIMEOUT_MS, m_ConnectTimeout); //连接超时
    curl_easy_setopt(m_pCurl, CURLOPT_TIMEOUT_MS, m_RequestTimeout);    //读取超时
    curl_easy_setopt(m_pCurl, CURLOPT_HEADER, true);   //返回附带信息头
//    curl_easy_setopt(curl, CURLOPT_VERBOSE, true);    //会打印头信息
//    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, true);
    
    /*SLL CER证书*/
    if (m_sSSLFilePath.length() > 0) {
        CURLcode code = curl_easy_setopt(m_pCurl, CURLOPT_CAINFO, m_sSSLFilePath.c_str());
        curl_easy_setopt(m_pCurl, CURLOPT_SSL_VERIFYPEER, code == CURLE_OK); // 对认证证书来源的检查
        curl_easy_setopt(m_pCurl, CURLOPT_SSL_VERIFYPEER, code == CURLE_OK); // 对认证证书来源的检查
    }
    
    /*进度*/
    if (bProgress) {
        curl_easy_setopt(m_pCurl, CURLOPT_XFERINFOFUNCTION, OnProgressResultCallback);
        curl_easy_setopt(m_pCurl, CURLOPT_XFERINFODATA, this);
        curl_easy_setopt(m_pCurl, CURLOPT_NOPROGRESS, 0);
    }
    
    /*头信息中返回的内容大小*/
    curl_easy_setopt(m_pCurl, CURLOPT_HEADERFUNCTION, OnGetContentLengthCallback);
    curl_easy_setopt(m_pCurl, CURLOPT_HEADERDATA, &m_nContentLength);
    
    curl_easy_setopt(m_pCurl, CURLOPT_WRITEFUNCTION, OnWriterRecvData);
    curl_easy_setopt(m_pCurl, CURLOPT_WRITEDATA, &m_sResponseBuf);
    
    if (m_pRequestHeader) {
        int code = curl_easy_setopt(m_pCurl, CURLOPT_HTTPHEADER, m_pRequestHeader);    //设置请求头
        if (code != CURLE_OK) {
            CPrintfE("Failed to set header field:%s", m_pErrorBuf);
        }
    }
    
    return m_pCurl;
}

long CNetRequest::ParseResponseData(std::string &header, std::string &boby)
{
    if (!m_pCurl) return ERR_OBJ_HANDLE_INVALID;

    long responseCode = 0;
    CURLcode code = curl_easy_getinfo(m_pCurl, CURLINFO_RESPONSE_CODE, &responseCode);
    if (code == CURLE_OK && responseCode == 200) {
        long headerSize = 0;
        code = curl_easy_getinfo(m_pCurl, CURLINFO_HEADER_SIZE, &headerSize);
        if (code == CURLE_OK && m_sResponseBuf.size() >= headerSize) {
            header = m_sResponseBuf.substr(0, headerSize);
            boby = m_sResponseBuf.substr(headerSize);
        }
    }
    ShowRequestAfterInfo();
    
    return responseCode;
}

#pragma mark -

/// 显示请求前信息
void CNetRequest::ShowRequestBeforeInfo()
{
    if (m_bIsShowRequestLog) {
        CPrintfI("Result Start URL:  %s\n", m_sUrl.c_str());   //请求链接
        for (curl_slist *header = (curl_slist *)m_pRequestHeader; header != nullptr; ) {    //请求头
            CPrintfI("%s\n", header->data);
            header = header->next;
        }
    }
}

/// 显示请求后信息
void CNetRequest::ShowRequestAfterInfo()
{
    if (m_bIsShowRequestLog) {
        CPrintfI("Result End URL: %s\n", m_sUrl.c_str());
        
        char *infoStr = NULL;
        long port = 0;
        CURLcode code = curl_easy_getinfo(m_pCurl, CURLINFO_PRIMARY_IP, &infoStr);
        code = curl_easy_getinfo(m_pCurl, CURLINFO_PRIMARY_PORT, &port);
        if (code == CURLE_OK && infoStr) {
            CPrintfI("Host IP:%s Port:%ld\n", infoStr, port);
        }
        
        code = curl_easy_getinfo(m_pCurl, CURLINFO_LOCAL_IP, &infoStr);
        code = curl_easy_getinfo(m_pCurl, CURLINFO_LOCAL_PORT, &port);
        if (code == CURLE_OK && infoStr) {
            CPrintfI("Local IP:%s Port:%ld\n", infoStr, port);
        }
        
        CPrintfI("Response: \n%s\n", m_sResponseBuf.c_str());
    }
}

#pragma mark -

void CNetRequest::RemoveHeader()
{
    if (m_pRequestHeader) {
        curl_slist_free_all((curl_slist *)m_pRequestHeader);
        m_pRequestHeader = nullptr;
    }
}

void CNetRequest::SetHeaders(std::list<std::string> headersList)
{
    RemoveHeader();
    for (std::list<std::string>::iterator iter = headersList.begin(); iter != headersList.end(); ++iter) {
        if (iter->length() > 0) {
            m_pRequestHeader = curl_slist_append((curl_slist *)m_pRequestHeader, iter->c_str());
        }
    }
    if (m_pCurl && m_pRequestHeader) {
        int code = curl_easy_setopt(m_pCurl, CURLOPT_HTTPHEADER, m_pRequestHeader);
        if (code != CURLE_OK) {
            CPrintfE("Failed to set header field:%s", m_pErrorBuf);
        }
    }
}

void CNetRequest::AddHeader(std::string headerStr)
{
    m_pRequestHeader = curl_slist_append((curl_slist *)m_pRequestHeader, headerStr.c_str());
    if (m_pCurl && m_pRequestHeader) {
        int code = curl_easy_setopt(m_pCurl, CURLOPT_HTTPHEADER, m_pRequestHeader);
        if (code != CURLE_OK) {
            CPrintfE("Failed to set header field:%s", m_pErrorBuf);
        }
    }
}

void CNetRequest::SetContentType(CONTENT_TYPE type)
{
    RemoveHeader();
    m_nContentType = type;
    if (m_nContentType == CONTENT_TYPE_X_WWW_FORM_URLENCODED) {
        AddHeader("Content-Type: application/x-www-form-urlencoded;charset=utf-8");
    } else if (m_nContentType == CONTENT_TYPE_JSON || m_nContentType == CONTENT_TYPE_JSON_RAW) {
        AddHeader("Accept: application/json");
        AddHeader("Content-Type: application/json;charset=UTF-8");
    }
}

std::string CNetRequest::BuildParameters()
{
    if (m_nContentType == CONTENT_TYPE_JSON_RAW) {  //转Json
        if (m_sParaString.length() > 0) {
            std::vector<std::string> map = ZJ::Tool::CString::Split(m_sParaString, "&");
            for (std::string str : map) {
                std::vector<std::string> kv = ZJ::Tool::CString::Split(str, "=");
                if (kv.size() >= 2) {
                    m_oParaJsonObj.Add(kv.at(0), kv.at(1));
                }
            }
        }
        m_sParaString = m_oParaJsonObj.ToString();
    } else {    //表单字符串
        std::string keyT;
        for (; m_oParaJsonObj.GetKey(keyT); ) {
            int type = m_oParaJsonObj.Type(keyT);
            if (type == cJSONZJ_Array || type == cJSONZJ_Object || type == cJSONZJ_NULL) {
                continue;
            }
            std::stringstream sValue;
            if (m_sParaString.length() > 0)
                m_sParaString += "&";
            m_sParaString += keyT + "=";
                
            if (type == cJSONZJ_Int) {
                int64_t value = 0;
                m_oParaJsonObj.Get(keyT, value);
                
                sValue << value;
            } else if (type == cJSONZJ_False || type == cJSONZJ_True) {
                bool value = false;
                m_oParaJsonObj.Get(keyT, value);
                
                sValue << value;
            } else if (type == cJSONZJ_Double) {
                double value = 0;
                m_oParaJsonObj.Get(keyT, value);
                
                sValue << value;
            } else if (type == cJSONZJ_String) {
                std::string value = "";
                m_oParaJsonObj.Get(keyT, value);
                
                sValue << value;
            } else if (type == cJSONZJ_Object || type == cJSONZJ_Array) {
                neb::CJsonObject obj;
                m_oParaJsonObj.Get(keyT, obj);
                sValue << ZJ::Tool::CString::EncodeToUTF8(obj.ToString());
            }
            m_sParaString += sValue.str();
        }
    }
    
    return m_sParaString;
}

#pragma mark -

void CNetRequest::Request(std::function<void()> pCallback)
{
    if (m_bRequesting.load()) {
        return;
    } else if (m_pCurl) {
        m_bRequesting.store(true);
        m_sResponseBuf = "";
        ShowRequestBeforeInfo();
        
        long code = curl_easy_perform(m_pCurl);
        if (m_pCurl && m_bRequesting.load()) {
            std::string header = "";
            std::string boby = "";
            if (code == CURLE_OK) {
                code = ParseResponseData(header, boby);
            }
            std::string errorStr(m_pErrorBuf);
            if (code == CURLE_OPERATION_TIMEDOUT || code == CURLE_OBSOLETE32) {  //超时
                OnRequestResult(m_pUserData, ERR_TIMEOUT, header, boby, errorStr);
            } else if (code == CURLE_COULDNT_RESOLVE_HOST || code == 404 || code == 302) {
                OnRequestResult(m_pUserData, ERR_HOST_NOT_FIND, header, boby, errorStr);
            } else if (code == CURLE_RECV_ERROR || code == CURLE_SEND_ERROR) {
                OnRequestResult(m_pUserData, ERR_NET_CONN_LOST, header, boby, errorStr);
            } else {
                OnRequestResult(m_pUserData, code, header, boby, errorStr);
            }
        } else {
            CPrintfD("Cancel Result: %s", m_sUrl.c_str());
        }
        
        if (pCallback) pCallback();
        Clear();
    }
}

void CNetRequest::Cancel()
{
    m_bRequesting.store(false);
    if (m_pCurl) {
        curl_easy_setopt(m_pCurl, CURLOPT_CONNECTTIMEOUT_MS, 1);
        curl_easy_setopt(m_pCurl, CURLOPT_TIMEOUT_MS, 1);
    }
}

#pragma mark -

size_t CNetRequest::OnWriterRecvData(char *data, size_t size, size_t nmemb, void *stream)
{
    std::string *str = dynamic_cast<std::string*>((std::string *)stream);
    if (stream == nullptr || data == nullptr) {
        return -1;
    } else if (str->length() >= 33554432) {     //32M，防止下载的进入这个回调函数
        return -1;
    }
    
    size_t sizes = size * nmemb;
    str->append(data, sizes);
    
    return sizes;
}

void CNetRequest::OnRequestResult(void *userData, long statusCode, std::string strRecvHeader, std::string strRecvBody, const std::string strError) {
    CPrintfW("No set receive proxy interface!");
}

void CNetRequest::OnProgressResult(void *userData, long long dlTotal, long long dlNow, long long ulTotal, long long ulNow) {
    CPrintfW("No set progress proxy interface, download: %lld/%lld upload: %lld/%lld", dlTotal, dlNow, ulTotal, ulNow);
}
