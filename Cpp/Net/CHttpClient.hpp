//
//  CHttpClient.hpp
//  JMSmartFTPUtils
//
//  Created by eafy on 2021/3/12.
//  Copyright © 2021 ZJ. All rights reserved.
//

#ifndef CHttpClient_hpp
#define CHttpClient_hpp

#include <stdio.h>
#include <mutex>
#include <map>
#include "CSingleton.hpp"
#include "CNetRequest.hpp"

ZJ_NAMESPACE_BEGIN

class CHttpClient
{
public:
    typedef std::function<void(void *userData, const std::string strContent)> OnHttpClientCompletionCB;
    typedef std::function<void(void *userData, long statusCode, const std::string strError)> OnHttpClientFailureCB;
    typedef std::function<void(void *userData, unsigned long long totalSize, unsigned long long currentSize)> OnHttpClientProgressCB;
    
public:
    CHttpClient();
    ~CHttpClient();
    DECLARE_SINGLETON_CLASS(CHttpClient);
    
    void SetUserData(void *pUserData) {
        m_pUserData = pUserData;
    }
    
    /// 取消所有请求
    void CancelAll();
    
    /// 取消请求
    /// @param tag 请求标签
    void Cancel(uint64_t tag);
    
    /// 请求
    /// @param httpMethod 请求方式
    /// @param url 请求地址
    /// @param paramJson 参数字典
    /// @param comCB 成功回调
    /// @param failCB 失败回调
    /// @param progressCB 进度回调
    /// @return 请求标签，可用作取消，==0标识创建失败
    uint64_t Request(CNetRequest::METHOD_TYPE httpMethod, const std::string url, neb::CJsonObject paramJson, OnHttpClientCompletionCB comCB, OnHttpClientFailureCB failCB, OnHttpClientProgressCB progressCB = nullptr);
    
private:
    void *m_pUserData = nullptr;
    void *m_pThreadPool = nullptr;
    
    std::map<uint64_t, CNetRequest*> m_TaskMap;
};

ZJ_NAMESPACE_END
#endif /* CHttpClient_hpp */
