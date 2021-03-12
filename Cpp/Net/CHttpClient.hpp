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
#include "CBaseThreadPool.hpp"
#include "CNetRequest.hpp"

ZJ_NAMESPACE_BEGIN

class CHttpClient: CBaseThreadPool
{
public:
    typedef std::function<void(void *userData, const std::string strContent)> OnHttpClientCompletionCB;
    typedef std::function<void(void *userData, long statusCode, const std::string strError)> OnHttpClientFailureCB;
    typedef std::function<void(void *userData, unsigned long long totalSize, unsigned long long currentSize)> OnHttpClientProgressCB;
    
public:
    CHttpClient();
    ~CHttpClient();
    DECLARE_SINGLETON_CLASS(CHttpClient);
    
    CNetRequest *Build(const std::string url, OnHttpClientCompletionCB comCB, OnHttpClientFailureCB failCB, OnHttpClientProgressCB progressCB = nullptr);
    
    void Request(CNetRequest *request);
    
private:
    void *m_pThreadPool = nullptr;
    
};

ZJ_NAMESPACE_END
#endif /* CHttpClient_hpp */
