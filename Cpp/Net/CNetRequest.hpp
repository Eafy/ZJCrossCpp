//
//  CNetRequest.hpp
//  ZJCrossCpp
//
//  Created by eafy on 2021/3/1.
//  Copyright © 2021 ZJ. All rights reserved.
//

#ifndef CNetRequest_hpp
#define CNetRequest_hpp

#include <stdio.h>
#include <string>
#include <list>
#include <map>
#include <functional>
#include "CJsonObject.hpp"
#include "CLog.hpp"

class CNetRequest {
    
public:
    enum MODE_TYPE {
        MODE_TYPE_HTTP = 0,
        MODE_TYPE_FTP = 1 << 1,
        MODE_TYPE_QUOTE = 1 << 2,
        MODE_TYPE_CUSTOMR = 1 << 3,
    };
    
    enum ERR_CODE {
        ERR_CANCEL = 999,       //已取消
        
        ERR_NoERROR = 0,     //无错误
        
        ERR_OBJ_HANDLE_INVALID = -100,        //句柄对象无效
        ERR_FILE_PATH_INVALID = -101,   //文件路径无效
        ERR_USER_INFO_ERROR = -102,     //用户信息错误
        
        ERR_TIMEOUT = -1001,            //访问超时
        ERR_URL_INVALID = -1002,        //URL无效
        ERR_HOST_NOT_FIND = -1003,      //Host错误
        ERR_HOST_NOT_CONNECT = -1004,   //Host不能连接
        ERR_NET_CONN_LOST = -1005,      //网络连接异常
    };
    
    enum CONTENT_TYPE {
        CONTENT_TYPE_NONE = 0,
        CONTENT_TYPE_X_WWW_FORM_URLENCODED,
        CONTENT_TYPE_JSON,
        CONTENT_TYPE_JSON_RAW,  //请求参数为json格式
    };
    
    /// 请求回调
    /// @param userData 用户数据(透传)，默认空
    /// @param statusCode http访问的状态码，正常的查看http状态码，部分异常查看HTTP_ERR_CODE
    /// @param strRecvHeader 接收到的信息头
    /// @param strRecvBody 接收到的主体
    /// @param strError CURL解析到的错误
    virtual void OnRequestResult(void *userData, long statusCode, std::string strRecvHeader, std::string strRecvBody, const std::string strError);

    /// 进度回调
    /// @param userData 用户数据(透传)，默认空
    /// @param dlTotal 下载总长度
    /// @param dlNow 下载进度
    /// @param ulTotal 上传总长度
    /// @param ulNow 上传进度
    virtual void OnProgressResult(void *userData, long long dlTotal, long long dlNow, long long ulTotal, long long ulNow);
    
private:
    enum REQUEST_STATE {
        REQUEST_STATE_NONE = 0,
        REQUEST_STATE_LOADED,       //已记载句柄
        REQUEST_STATE_REQUESTING,   //正在请求
        REQUEST_STATE_PAUSE,        //暂停
        REQUEST_STATE_CANCEL,       //已取消
        REQUEST_STATE_END,          //结束
    };
    
public:
    CNetRequest(void *pUserData = nullptr, MODE_TYPE mode = MODE_TYPE_HTTP);
    ~CNetRequest();
    
    /// 设置模式类型，默认HTTP
    /// @param mode MODE_TYPE
    void SetModeType(MODE_TYPE mode) {
        m_nModeType = mode;
    }
    
    /// 获取请求CURL句柄
    /// @param url 请求地址
    /// @param bAppendUrl 参数是否追加到Url链接上
    /// @param bProgress 是否开启进度
    void *ConfigURL(const std::string url, bool bAppendUrl = false, bool bProgress = false);
    
    /// 设置内容格式
    /// @param type CONTENT_TYPE
    void SetContentType(CONTENT_TYPE type);
    
    /// 连接超时时间(毫秒)
    /// @param msTime 超时时间(毫秒)
    void SetConnectTimeout(long msTime);
    /// 请求超时时间(毫秒)
    /// @param msTime 超时时间(毫秒)
    void SetRequestTimeout(long msTime);
    
    /// 配置账号信息
    /// @param userName 用户名
    /// @param password 用户密码
    void ConfigUserInfo(const std::string userName, const std::string password);
    
    /// 配置SSL证书
    /// @param filePath 文件路径
    void SetSSLFilePath(const std::string filePath) {
        m_sSSLFilePath = filePath;
    }
    
    /// 设置请求头参数
    /// @param headersList 头参数数组
    void SetHeaders(std::list<std::string> headersList);
    
    /// 添加请求头参数
    /// @param headerStr 参数内容
    void AddHeader(std::string headerStr);
    
    /// 添加请求参数
    /// @param key 字段Key
    /// @param value 字段内容
    template <class T> void AddParameter(std::string key, T value) {
        if (m_oParaJsonObj.HasKey(key)) {
            m_oParaJsonObj.Replace(key, value);
        } else {
            m_oParaJsonObj.Add(key, value);
        }
    }
    
    /// 移除请求参数
    /// @param key 字段Key
    template <class T> void removeParameter(std::string key) {
        m_oParaJsonObj.Delete(key);
    }
    
    /// 添加字符串请求参数
    /// @param strPara 请求字符串，比如uuid=1231241&userid=123
    void AddParameterString(std::string strPara) {
        m_sParaString = strPara;
    }
    
    void RemoveParameterString() {
        m_sParaString = "";
    }
    
    /// 组装请求参数
    std::string BuildParameters();
    
    /// 开始请求
    /// @param pCallback 结果的回调，设置null无效
    void Request(std::function<void(long statusCode, std::string strRecvBody, const std::string strError)> pCallback = nullptr);
    
    /// 设置响应回调(一旦代理了OnRequestResult将无效)
    /// @param pCallback 回调(状态码, 接收内容, 错误提示)
    void SetResponseCallback(std::function<void(long statusCode, std::string strRecvBody, const std::string strError)> pCallback) {
        m_pRespCallback = pCallback;
    }
    
    /// 取消
    void Cancel();
    /// 暂停
    void Pause();
    /// 回复暂停
    void Resume();
    
    /// 正在运行
    bool IsRunning();
    
    /// 获取请求状态
    REQUEST_STATE GetRequestState() {
        return m_bRequestState.load();
    }
    
    /// 回复请求的URL链接
    std::string GetUrl() {
        return m_sUrl;
    }
    
    /// 获取回复的内容
    std::string GetBoby() {
        return m_sResponseBody;
    }
    
    /// 获取错误提示名字
    std::string GetError() {
        if (m_pErrorBuf) {
            return std::string(m_pErrorBuf);
        }
        return "";
    }
    
private:
    /// 清除数据
    void Clear();
    /// 移除CURL
    /// @param curl curl对象
    void RemoveCurl(void **curl);
    /// 移除请求头
    void RemoveHeader();
    /// 配置请求头参数
    void ConfigHeaders();
    
    /// 解析回复内容
    long ParseResponseData();
    
    /// 显示请求前信息
    void ShowRequestBeforeInfo();
    
    /// 显示强求后信息
    void ShowRequestAfterInfo();
    
public:
    void *m_pUserData = nullptr;
    bool m_bIsShowRequestLog = true;        //显示请求信息
    
protected:
    static size_t OnWriterRecvData(char *data, size_t size, size_t nmemb, void *stream);
    
    int64_t m_nContentLength = 0;       //内容长度
    
private:
    MODE_TYPE m_nModeType = MODE_TYPE_HTTP;
    std::string m_sUrl = "";          //请求连接
    void *m_pCurl = nullptr;
    std::string m_sSSLFilePath = "";    //Cer证书路径
    CONTENT_TYPE m_nContentType = CONTENT_TYPE_NONE;   //post请求类型
    long m_nConnectTimeout = 15000;      //连接超时时间(毫秒)
    long m_nRequestTimeout = 60000;      //请求超时时间(毫秒)
    
    void *m_pRequestHeader = nullptr;   //请求头
    neb::CJsonObject m_oParaJsonObj;       //请求参数，字典
    std::string m_sParaString;          //请求参数，字符串
    std::string m_sUserName = "";       //用户账号
    std::string m_sPassword = "";       //用户密码
    
    long m_nRespCode = 0;               //响应码
    std::string m_sResponseBuf = "";             //接收buffer，仅get、post
    std::string m_sResponseHeader = "";
    std::string m_sResponseBody = "";
    char *m_pErrorBuf = nullptr;            //错误信息buffer
    std::atomic<REQUEST_STATE> m_bRequestState;         //请求状态
    std::function<void(long statusCode, std::string strRecvBody, const std::string strError)> m_pRespCallback = nullptr;    //结果回调
};

#endif /* CNetRequestInfo_hpp */
