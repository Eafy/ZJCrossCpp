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
    enum ERR_CODE {
        ERR_NoERROR = 200,     //无错误
        
        ERR_OBJ_HANDLE_INVALID = -100,        //句柄对象无效
        ERR_PARA_ERROR = -101,          //参数无效
        ERR_FILE_PATH_INVALID = -101,   //文件路径无效
        
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
    
public:
    CNetRequest(void *pUserData = nullptr);
    ~CNetRequest();
    
    /// 获取请求CURL句柄
    /// @param url 请求地址
    /// @param bAppendUrl 参数是否追加到Url链接上
    /// @param bProgress 是否开启进度
    void *GetHandleWithURL(const std::string url, bool bAppendUrl = false, bool bProgress = false);
    
    /// 设置内容格式
    /// @param type CONTENT_TYPE
    void SetContentType(CONTENT_TYPE type);
    
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
    /// @param pCallback 清楚前的回调
    void Request(std::function<void()> pCallback = nullptr);
    /// 取消
    void Cancel();

    
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
    /// 清除数据
    void Clear();
    /// 移除CURL
    /// @param curl curl对象
    void RemoveCurl(void **curl);
    /// 移除请求头
    void RemoveHeader();
    
    /// 解析回复内容
    /// @param header 回复的头
    /// @param boby 回复的内容
    long ParseResponseData(std::string &header, std::string &boby);
    
    /// 显示请求前信息
    void ShowRequestBeforeInfo();
    
    /// 显示强求后信息
    void ShowRequestAfterInfo();
    
public:
    void *m_pUserData = nullptr;
    bool m_bIsShowRequestLog = true;        //显示请求信息
    long m_ConnectTimeout = 10000;      //连接超时时间(毫秒)
    long m_RequestTimeout = 30000;      //请求超时时间(毫秒)
    
protected:
    static size_t OnWriterRecvData(char *data, size_t size, size_t nmemb, void *stream);
    
private:
    std::string m_sUrl = "";          //请求连接
    void *m_pCurl = nullptr;
    std::string m_sSSLFilePath = "";    //Cer证书路径
    CONTENT_TYPE m_nContentType = CONTENT_TYPE_NONE;   //post请求类型
    void *m_pRequestHeader = nullptr;   //请求头
    neb::CJsonObject m_oParaJsonObj;       //请求参数，字典
    std::string m_sParaString;          //请求参数，字符串
    
    std::string m_sResponseBuf = "";             //接收buffer，仅get、post
    char *m_pErrorBuf = nullptr;            //错误信息buffer
    std::atomic_bool m_bRequesting;         //是否正在请求
    int64_t m_nContentLength = 0;   
};

#endif /* CNetRequestInfo_hpp */
