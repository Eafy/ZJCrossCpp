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
#include "CString.hpp"

ZJ_NAMESPACE_BEGIN

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

CNetRequest::CNetRequest(void *pUserData, MODE_TYPE mode)
{
    m_nModeType = mode;
    m_pUserData = pUserData;
    m_pErrorBuf = new char[CURL_ERROR_SIZE]();
    m_bRequestState.store(REQUEST_STATE_NONE);
}

CNetRequest::~CNetRequest()
{
    if (IsRunning()) {
        m_pResponseCallback = nullptr;
        m_pProgressCallback = nullptr;
        Cancel();
        while (m_bRequestState.load() != REQUEST_STATE_END) {
            usleep(5);
        }
    } else {
        Clear();
    }
        
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
    m_sUserName = "";
    m_sPassword = "";
    
    m_pResponseCallback = nullptr;
    m_pProgressCallback = nullptr;
    m_bRequestState.store(REQUEST_STATE_END);
}

void CNetRequest::RemoveCurl(void **curl)
{
    if (curl != nullptr && *curl != nullptr) {
        curl_easy_cleanup(*curl);
        *curl = nullptr;
    }
    curl = nullptr;
}

bool CNetRequest::IsRunning() {
    if (m_pCurl) {
        REQUEST_STATE state = m_bRequestState.load();
        if (state == REQUEST_STATE_REQUESTING || state == REQUEST_STATE_PAUSE) {
            return true;
        }
    }
    return false;
}

#pragma mark -

/// 显示请求前信息
void CNetRequest::ShowRequestBeforeInfo()
{
    if (m_bIsShowRequestLog) {
        CLogI("Result Start URL:  %s\n", m_sUrl.c_str());   //请求链接
        for (curl_slist *header = (curl_slist *)m_pRequestHeader; header != nullptr; ) {    //请求头
            CLogI("%s\n", header->data);
            header = header->next;
        }
    }
}

/// 显示请求后信息
void CNetRequest::ShowRequestAfterInfo()
{
    if (m_bIsShowRequestLog) {
        CLogI("Result End URL: %s\n", m_sUrl.c_str());
        
        char *infoStr = NULL;
        long port = 0;
        CURLcode code = curl_easy_getinfo(m_pCurl, CURLINFO_PRIMARY_IP, &infoStr);
        code = curl_easy_getinfo(m_pCurl, CURLINFO_PRIMARY_PORT, &port);
        if (code == CURLE_OK && infoStr) {
            CLogI("Host IP:%s Port:%ld\n", infoStr, port);
        }
        
        code = curl_easy_getinfo(m_pCurl, CURLINFO_LOCAL_IP, &infoStr);
        code = curl_easy_getinfo(m_pCurl, CURLINFO_LOCAL_PORT, &port);
        if (code == CURLE_OK && infoStr) {
            CLogI("Local IP:%s Port:%ld\n", infoStr, port);
        }
        
        CLogI("Response: \n%s\n", m_sResponseBuf.c_str());
    }
}

#pragma mark -

void CNetRequest::ConfigUserInfo(const std::string userName, const std::string password)
{
    m_sUserName = userName;
    m_sPassword = password;
    if (m_pCurl && m_sUserName.length() > 0 && m_sPassword.length() > 0 && m_bRequestState.load() == REQUEST_STATE_LOADED) {
        std::string userInfo = m_sUserName + ":" + m_sPassword;
        curl_easy_setopt(m_pCurl, CURLOPT_USERPWD, userInfo.c_str());
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
    ConfigHeaders();
}

void CNetRequest::AddHeader(std::string headerStr)
{
    m_pRequestHeader = curl_slist_append((curl_slist *)m_pRequestHeader, headerStr.c_str());
    if (m_pCurl && m_bRequestState.load() == REQUEST_STATE_LOADED) {
        ConfigHeaders();
    }
}

void CNetRequest::RemoveHeader()
{
    if (m_pRequestHeader) {
        curl_slist_free_all((curl_slist *)m_pRequestHeader);
        m_pRequestHeader = nullptr;
    }
}

void CNetRequest::ConfigHeaders()
{
    if (m_pCurl && m_pRequestHeader) {
        void *headCmd = m_pRequestHeader;
        
        CURLoption type = CURLOPT_HTTPHEADER;
        if (m_nModeType & MODE_TYPE_QUOTE) {
            type = CURLOPT_QUOTE;
        } else if (m_nModeType & MODE_TYPE_CUSTOMR) {
            type = CURLOPT_CUSTOMREQUEST;
            for (curl_slist *header = (curl_slist *)m_pRequestHeader; header != nullptr; ) {    //请求头
                headCmd = header->data;
                header = header->next;
                break;
            }
        }
        
        int code = curl_easy_setopt(m_pCurl, type, headCmd);
        if (code != CURLE_OK) {
            CLogE("Failed to config header field:%s", m_pErrorBuf);
        }
    }
}

void CNetRequest::SetMethodType(METHOD_TYPE type)
{
    m_nMethodType = type;
    if (!m_pCurl) return;
    
    if (m_nMethodType == METHOD_TYPE_POST) {
        curl_easy_setopt(m_pCurl, CURLOPT_POST, true);
    } else {
        curl_easy_setopt(m_pCurl, CURLOPT_POST, false);
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

void CNetRequest::SetConnectTimeout(long msTime)
{
    m_nConnectTimeout = msTime;
    if (m_pCurl) {
        curl_easy_setopt(m_pCurl, CURLOPT_CONNECTTIMEOUT_MS, m_nConnectTimeout); //连接超时
    }
}

void CNetRequest::SetRequestTimeout(long msTime)
{
    m_nRequestTimeout = msTime;
    if (m_pCurl) {
        curl_easy_setopt(m_pCurl, CURLOPT_TIMEOUT_MS, m_nRequestTimeout);    //读取超时
    }
}

std::string CNetRequest::BuildParameters()
{
    if (m_nContentType == CONTENT_TYPE_JSON_RAW) {  //转Json
        if (m_sParaString.length() > 0) {
            std::vector<std::string> map = CString::Split(m_sParaString, "&");
            for (std::string str : map) {
                std::vector<std::string> kv = CString::Split(str, "=");
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
                sValue << CString::EncodeToUTF8(obj.ToString());
            }
            m_sParaString += sValue.str();
        }
    }
    
    return m_sParaString;
}


std::string CNetRequest::BuildURL(const std::string url)
{
    std::string sURL = url;
    BuildParameters();
    
    if (m_nMethodType == METHOD_TYPE_GET) {
        if (m_sParaString.length() > 0) {
            if (sURL.find("?") != std::string::npos) {   //链接已附带参数
                sURL += "&" + m_sParaString;
            } else {
                sURL += "?" + m_sParaString;
            }
        }
    } else {
        int64_t length = m_sParaString.length();
        if (length > 0) {
            curl_easy_setopt(m_pCurl, CURLOPT_POSTFIELDS, m_sParaString.c_str());
            curl_easy_setopt(m_pCurl, CURLOPT_POSTFIELDSIZE, length);
        }
    }
    
    curl_easy_setopt(m_pCurl, CURLOPT_ERRORBUFFER, m_pErrorBuf);
    CURLcode code = curl_easy_setopt(m_pCurl, CURLOPT_URL, sURL.c_str());
    if (code != CURLE_OK) {
        CLogE("Failed to set url:%s, error:%s", sURL.c_str(), m_pErrorBuf);
        RemoveCurl(&m_pCurl);
        OnRequestResult(m_pUserData, ERR_URL_INVALID, "", "", sURL);
        return "";
    }
    
    /*配置请求方式*/
    if (m_nMethodType == METHOD_TYPE_POST) {
        curl_easy_setopt(m_pCurl, CURLOPT_POST, true);
    }
    
    return sURL;
}

#pragma mark -

long CNetRequest::ParseResponseData()
{
    if (!m_pCurl) return ERR_OBJ_NOT_EXIST;
    if (m_nModeType & MODE_TYPE_FTP) {  //FTP模式不检测头
        m_sResponseBody = m_sResponseBuf;
        return ERR_NoERROR;
    }

    long responseCode = ERR_NoERROR;
    CURLcode code = curl_easy_getinfo(m_pCurl, CURLINFO_RESPONSE_CODE, &responseCode);
    if (code == CURLE_OK && responseCode == 200) {
        long headerSize = 0;
        code = curl_easy_getinfo(m_pCurl, CURLINFO_HEADER_SIZE, &headerSize);
        if (code == CURLE_OK && m_sResponseBuf.size() >= headerSize) {
            m_sResponseHeader = m_sResponseBuf.substr(0, headerSize);
            m_sResponseBody = m_sResponseBuf.substr(headerSize);
        }
        responseCode = ERR_NoERROR;
    }
    ShowRequestAfterInfo();
    
    return responseCode;
}

CURL *CNetRequest::ConfigURL(const std::string url, bool bProgress)
{
    if (IsRunning()) return m_pCurl;
    
    if (!m_pCurl) {
        m_pCurl = curl_easy_init();
        if (m_pCurl == nullptr) {
            CLogE("Failed to create CURL connection");
            OnRequestResult(m_pUserData, ERR_OBJ_INIT_FAILED, "", "", "");
            return nullptr;
        }
    }
    
    m_bRequestState.store(REQUEST_STATE_LOADED);
    m_sUrl = BuildURL(url);
    if (m_sUrl.length() == 0) {
        m_bRequestState.store(REQUEST_STATE_NONE);
        return nullptr;
    }
    ConfigHeaders();
    
    curl_easy_setopt(m_pCurl, CURLOPT_NOSIGNAL, 1L);     //毫秒超时需要设置
    curl_easy_setopt(m_pCurl, CURLOPT_CONNECTTIMEOUT_MS, m_nConnectTimeout); //连接超时
    curl_easy_setopt(m_pCurl, CURLOPT_TIMEOUT_MS, m_nRequestTimeout);    //读取超时
    curl_easy_setopt(m_pCurl, CURLOPT_HEADER, 1L);      //返回附带信息头
    curl_easy_setopt(m_pCurl, CURLOPT_READFUNCTION, nullptr);
//    curl_easy_setopt(curl, CURLOPT_VERBOSE, true);    //会打印头信息
//    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, true);
    
    /*SLL CER证书*/
    if (m_sSSLFilePath.length() > 0) {
        CURLcode code = curl_easy_setopt(m_pCurl, CURLOPT_CAINFO, m_sSSLFilePath.c_str());
        curl_easy_setopt(m_pCurl, CURLOPT_SSL_VERIFYPEER, code == CURLE_OK); // 对认证证书来源的检查
        curl_easy_setopt(m_pCurl, CURLOPT_SSL_VERIFYHOST, code == CURLE_OK); // 从证书中检查SSL加密算法是否存在
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
    
    /*FTP模式参数*/
    if (m_nModeType & MODE_TYPE_FTP) {
        curl_easy_setopt(m_pCurl, CURLOPT_FTPPORT, "-");
        curl_easy_setopt(m_pCurl, CURLOPT_FTP_CREATE_MISSING_DIRS, 1L);    //目录不存在时，上传文件时，先创建目录
    }
    
    /*配置账号信息*/
    if (m_sUserName.length() > 0 && m_sPassword.length() > 0) {
        std::string userInfo = m_sUserName + ":" + m_sPassword;
        curl_easy_setopt(m_pCurl, CURLOPT_USERPWD, userInfo.c_str());
    }
    
    return m_pCurl;
}

void CNetRequest::Request(OnNetRequestResponseCB pCallback)
{
    if (IsRunning()) {
        return;
    } else if (m_pCurl) {
        m_bRequestState.store(REQUEST_STATE_REQUESTING);
        if (pCallback) m_pResponseCallback = pCallback;
        m_sResponseBuf = "";
        m_sResponseHeader = "";
        m_sResponseBody = "";
        ShowRequestBeforeInfo();
        
        if (m_pProgressCallback) {
            curl_easy_setopt(m_pCurl, CURLOPT_XFERINFOFUNCTION, OnProgressResultCallback);
            curl_easy_setopt(m_pCurl, CURLOPT_XFERINFODATA, this);
            curl_easy_setopt(m_pCurl, CURLOPT_NOPROGRESS, 0);
        }
        
        m_nRespCode = curl_easy_perform(m_pCurl);
        
        long code = m_nRespCode;
        std::string errorStr(m_pErrorBuf);
        if (m_pCurl && IsRunning()) {
            if (code == CURLE_OK) {
                code = ParseResponseData();
            }
            if (code == CURLE_OPERATION_TIMEDOUT || code == CURLE_OBSOLETE32) {  //超时
                code = ERR_TIMEOUT;     //超时
            } else if (code == CURLE_COULDNT_RESOLVE_HOST || code == 404 || code == 302) {
                code = ERR_HOST_NOT_FIND;   //Host未找到
            } else if (code == CURLE_COULDNT_CONNECT || code == CURLE_RECV_ERROR || code == CURLE_SEND_ERROR) {
                code = ERR_NET_CONN_LOST;   //连接断开(无网)
            } else if (code == CURLE_LOGIN_DENIED) {
                code = ERR_USER_INFO_ERROR;     //用户账号信息错误
            } else if (code == CURLE_URL_MALFORMAT) {
                code = ERR_URL_INVALID;         //URL无效
            }
            OnRequestResult(m_pUserData, code, m_sResponseHeader, m_sResponseBody, errorStr);
        } else {
            CLogD("Cancel Result: %s", m_sUrl.c_str());
            code = ERR_CANCEL;
        }
        Clear();
    }
}

void CNetRequest::Request(const std::string url, OnNetRequestResponseCB pCallback)
{
    if (pCallback) m_pResponseCallback = pCallback;
    
    CURL *cUrl = ConfigURL(url);
    if (cUrl) {
        Request();
    }
}

void CNetRequest::Cancel()
{
    if (IsRunning()) {
        m_bRequestState.store(REQUEST_STATE_CANCEL);
        if (m_pCurl) {
            curl_easy_setopt(m_pCurl, CURLOPT_CONNECTTIMEOUT_MS, 1);
            curl_easy_setopt(m_pCurl, CURLOPT_TIMEOUT_MS, 1);
        }
    } else if (m_bRequestState.load() == REQUEST_STATE_LOADED && m_pCurl) {
        curl_easy_reset(m_pCurl);
        m_bRequestState.store(REQUEST_STATE_CANCEL);
    }
}

void CNetRequest::Pause() {
    if (m_pCurl && m_bRequestState.load() == REQUEST_STATE_REQUESTING) {
        m_bRequestState.store(REQUEST_STATE_PAUSE);
        curl_easy_pause(m_pCurl, CURLPAUSE_ALL);
    }
}

void CNetRequest::Resume() {
    if (m_pCurl && m_bRequestState.load() == REQUEST_STATE_PAUSE) {
        m_bRequestState.store(REQUEST_STATE_REQUESTING);
        curl_easy_pause(m_pCurl, CURLPAUSE_CONT);  
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
    if (m_pResponseCallback) {
        m_pResponseCallback(userData, statusCode, strRecvHeader, strRecvBody, strError);
    } else {
        CLogW("No set receive proxy interface: %s, StatusCode: %ld, RespCode:%ld, Header: %s, Body: %s, Error: %s", m_sUrl.c_str(), statusCode, m_nRespCode, strRecvHeader.c_str(), strRecvBody.c_str(), strError.c_str());
    }
}

void CNetRequest::OnProgressResult(void *userData, long long dlTotal, long long dlNow, long long ulTotal, long long ulNow) {
    if (m_pProgressCallback) {
        m_pProgressCallback(userData, dlTotal, dlNow, ulTotal, ulNow);
    } else {
        CLogW("No set progress proxy interface: %s, download: %lld/%lld upload: %lld/%lld", m_sUrl.c_str(), dlTotal, dlNow, ulTotal, ulNow);
    }
}

ZJ_NAMESPACE_END
