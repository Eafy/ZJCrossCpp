//
//  CHttpClient.cpp
//  JMSmartFTPUtils
//
//  Created by eafy on 2021/3/12.
//  Copyright © 2021 ZJ. All rights reserved.
//

#include "CHttpClient.hpp"
#include "CThreadPool.hpp"

ZJ_NAMESPACE_BEGIN

CHttpClient::CHttpClient()
{
    
}

CHttpClient::~CHttpClient()
{
    
}

CNetRequest *CHttpClient::Build(const std::string url, OnHttpClientCompletionCB comCB, OnHttpClientFailureCB failCB, OnHttpClientProgressCB progressCB)
{
    return nullptr;
}

void CHttpClient::Request(CNetRequest *request)
{
    
}

ZJ_NAMESPACE_END
