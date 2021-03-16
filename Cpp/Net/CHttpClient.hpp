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
#include "CNetRequestUpload.hpp"
#include "CNetRequestDownload.hpp"

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
    
    /// 暂停下载
    void Pause(uint64_t tag);
    
    /// 恢复下载
    void Resume(uint64_t tag);
    
    /// 请求
    /// @param httpMethod 请求方式
    /// @param url 请求地址
    /// @param paramJson 参数字典
    /// @param comCB 成功回调
    /// @param failCB 失败回调
    /// @param progressCB 进度回调
    /// @return 请求标签，可用作取消，==0标识创建失败
    uint64_t Request(CNetRequest::METHOD_TYPE httpMethod, const std::string url, neb::CJsonObject paramJson, OnHttpClientCompletionCB comCB, OnHttpClientFailureCB failCB, OnHttpClientProgressCB progressCB = nullptr);
    
    /// 下载
    /// @param url 下载地址
    /// @param fileDir 本地保存的文件夹
    /// @param fileName 本地文件名称
    /// @param comCB 完成回调
    /// @param failCB 失败回调
    /// @param progressCB 进度回调
    uint64_t Download(const std::string url, const std::string fileDir, const std::string fileName, OnHttpClientCompletionCB comCB, OnHttpClientFailureCB failCB, OnHttpClientProgressCB progressCB);
    
    /// 上传
    /// @param url 服务器上传地址
    /// @param filePath 本地上传的文件地址
    /// @param comCB 完成回调
    /// @param failCB 失败回调
    /// @param progressCB 进度回调
    uint64_t Upload(const std::string url, const std::string filePath, OnHttpClientCompletionCB comCB, OnHttpClientFailureCB failCB, OnHttpClientProgressCB progressCB);
    
private:
    void *m_pUserData = nullptr;
    void *m_pThreadPool = nullptr;
    
    std::map<uint64_t, CNetRequest*> m_TaskMap;
};

ZJ_NAMESPACE_END
#endif /* CHttpClient_hpp */
