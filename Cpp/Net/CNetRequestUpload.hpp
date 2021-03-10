//
//  CNetRequestUpload.hpp
//  ZJCrossCpp
//
//  Created by eafy on 2021/3/5.
//  Copyright © 2021 ZJ. All rights reserved.
//

#ifndef CNetRequestUpload_hpp
#define CNetRequestUpload_hpp

#include <stdio.h>
#include "CNetRequest.hpp"

class CNetRequestUpload: public CNetRequest {
public:
    CNetRequestUpload(void *pUserData = nullptr, MODE_TYPE mode = MODE_TYPE_HTTP);
    ~CNetRequestUpload();
    
    /// 开始上传
    /// @param url 远程服务器上传地址
    /// @param filePath 本地文件路径
    /// @param pCallback 上传进度
    void Start(const std::string url, const std::string filePath, std::function<void(long statusCode, std::string strRecvBody, const std::string strError)> pCallback = nullptr);
    
    /// 设置进度回调
    /// @param pCallback 回调
    void SetProgessCallback(std::function<void(const std::string url, long long dlTotal, long long dlNow)> pCallback) {
        m_pProgessCallback = pCallback;
    }
    
    /// 是否正在上传
    bool IsUploading() {
        return IsRunning();
    }
    
private:
    static size_t OnReadUploadedLenght(char *data, size_t size, size_t nmemb, void *stream);
    static size_t OnReaderSendData(char *ptr, size_t size, size_t nmemb, void *stream);
    void OnProgressResult(void *userData, long long dlTotal, long long dlNow, long long ulTotal, long long ulNow);
    
    int64_t QueryFileUploadedSize(const std::string url);
    
    int64_t m_nFirstProgress = 0;       //已上传进度
    int64_t m_nCurrentProgress = 0;     //当前进度
    int64_t m_nTotalProgress = 0;       //总进度
    std::function<void(const std::string url, long long dlTotal, long long dlNow)> m_pProgessCallback = nullptr;
};


#endif /* CNetRequestUpload_hpp */
