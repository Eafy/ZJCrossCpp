//
//  CHttpClient.cpp
//  JMSmartFTPUtils
//
//  Created by eafy on 2021/3/12.
//  Copyright © 2021 ZJ. All rights reserved.
//

#include "CHttpClient.hpp"
#include "CThreadPool.hpp"
#include "CTimer.hpp"

ZJ_NAMESPACE_BEGIN

CHttpClient::CHttpClient()
{
    m_pThreadPool = new CThreadPool();
}

CHttpClient::~CHttpClient()
{
    ((CThreadPool *)m_pThreadPool)->CancelAll();
    delete ((CThreadPool *)m_pThreadPool);
    m_pThreadPool = nullptr;
}

void CHttpClient::CancelAll()
{
    CThreadPool::Instance()->OnceTask([](CHttpClient *self) {
        for (std::map<uint64_t, CNetRequest *>::iterator iter = self->m_TaskMap.begin();
             iter != self->m_TaskMap.end(); ++iter) {
            iter->second->Cancel();
        }
        ((CThreadPool *)self->m_pThreadPool)->Reset();
    }, this);
    ((CThreadPool *)m_pThreadPool)->CancelAll();
    m_TaskMap.clear();
}

void CHttpClient::Cancel(uint64_t tag)
{
    std::map<uint64_t, CNetRequest *>::iterator iter = m_TaskMap.find(tag);
    if (iter != m_TaskMap.end()) {
        iter->second->Cancel();
    }
    ((CThreadPool *)m_pThreadPool)->CancelTask(tag);
}

uint64_t CHttpClient::Request(CNetRequest::METHOD_TYPE httpMethod, const std::string url, neb::CJsonObject paramJson, OnHttpClientCompletionCB comCB, OnHttpClientFailureCB failCB, OnHttpClientProgressCB progressCB)
{
    uint64_t tag = (uint64_t)&url + (uint64_t)&paramJson + CTimer::Timestamp();
    
    tag = ((CThreadPool *)m_pThreadPool)->AddTask(tag, [tag, httpMethod, url, paramJson, comCB, failCB, progressCB](CHttpClient *self) {
        CNetRequest request(self);
        request.SetMethodType(httpMethod);
        request.AddParameter(paramJson);
        request.SetResponseCallback([comCB, failCB](void *userData, long statusCode, std::string strRecvHeader, std::string strRecvBody, const std::string strError) {
            if (statusCode == 0) {
                if (comCB) comCB(userData, strRecvBody);
            } else {
                if (failCB) failCB(userData, statusCode, strError);
            }
        });
        request.SetProgressCallback([progressCB](void *userData, long long dlTotal, long long dlNow, long long ulTotal, long long ulNow) {
            if (progressCB) {
                if (dlTotal > 0) progressCB(userData, dlTotal, dlNow);
                else if (ulTotal > 0) progressCB(userData, ulTotal, ulNow);
            }
        });
        
        CNetRequest *p = &request;
        self->m_TaskMap.insert(std::pair<uint64_t, CNetRequest*>(tag, p));
        request.Request(url);
        self->m_TaskMap.erase(tag);
    }, this);
    
    return tag;
}

uint64_t CHttpClient::Download(const std::string url, const std::string fileDir, const std::string fileName, OnHttpClientCompletionCB comCB, OnHttpClientFailureCB failCB, OnHttpClientProgressCB progressCB)
{
    uint64_t tag = (uint64_t)&url + (uint64_t)&fileDir + CTimer::Timestamp();
    
    tag = ((CThreadPool *)m_pThreadPool)->AddTask(tag, [tag, url, fileDir, fileName, comCB, failCB, progressCB](CHttpClient *self) {
        CNetRequestDownload request(self);
        request.SetResponseCallback([comCB, failCB](void *userData, long statusCode, std::string strRecvHeader, std::string strRecvBody, const std::string strError) {
            if (statusCode == 0) {
                if (comCB) comCB(userData, strRecvBody);
            } else {
                if (failCB) failCB(userData, statusCode, strError);
            }
        });
        
        request.SetProgressCallback([progressCB, self](const std::string url, long long dlTotal, long long dlNow) {
            if (progressCB) {
               progressCB(self, dlTotal, dlNow);
            }
        });
        
        CNetRequest *p = &request;
        self->m_TaskMap.insert(std::pair<uint64_t, CNetRequest*>(tag, p));
        request.Start(url, fileDir, fileName);
        self->m_TaskMap.erase(tag);
    }, this);
    
    return tag;
}

uint64_t CHttpClient::Upload(const std::string url, const std::string filePath, OnHttpClientCompletionCB comCB, OnHttpClientFailureCB failCB, OnHttpClientProgressCB progressCB)
{
    uint64_t tag = (uint64_t)&url + (uint64_t)&filePath + CTimer::Timestamp();
    
    tag = ((CThreadPool *)m_pThreadPool)->AddTask(tag, [tag, url, filePath, comCB, failCB, progressCB](CHttpClient *self) {
        CNetRequestDownload request(self);
        request.SetResponseCallback([comCB, failCB](void *userData, long statusCode, std::string strRecvHeader, std::string strRecvBody, const std::string strError) {
            if (statusCode == 0) {
                if (comCB) comCB(userData, strRecvBody);
            } else {
                if (failCB) failCB(userData, statusCode, strError);
            }
        });
        request.SetProgressCallback([progressCB, self](const std::string url, long long ulTotal, long long ulNow) {
            if (progressCB) {
               progressCB(self, ulTotal, ulNow);
            }
        });
        
        CNetRequest *p = &request;
        self->m_TaskMap.insert(std::pair<uint64_t, CNetRequest*>(tag, p));
        request.Start(url, filePath);
        self->m_TaskMap.erase(tag);
    }, this);
    
    return tag;
}

ZJ_NAMESPACE_END
