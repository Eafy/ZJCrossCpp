//
//  CNetRequestDownload.hpp
//  ZJCrossCpp
//
//  Created by eafy on 2021/3/4.
//  Copyright © 2021 ZJ. All rights reserved.
//

#ifndef CNetRequestDownload_hpp
#define CNetRequestDownload_hpp

#include <stdio.h>
#include "CNetRequest.hpp"

ZJ_NAMESPACE_BEGIN

class CNetRequestDownload: public CNetRequest {
public:
    CNetRequestDownload(void *pUserData = nullptr, MODE_TYPE mode = MODE_TYPE_HTTP);
    ~CNetRequestDownload();
    
    /// 开始下载
    /// @param url 下载地址
    /// @param fileDir 下载目录
    /// @param fileName 文件名称
    /// @param pCallback 下载进度
    void Start(const std::string url, const std::string fileDir, const std::string fileName = "", OnNetRequestResponseCB pCallback = nullptr);
    
    /// 设置进度回调
    /// @param pCallback 回调
    void SetProgressCallback(std::function<void(const std::string url, long long dlTotal, long long dlNow)> pCallback) {
        m_pProgressCallback = pCallback;
    }
    
    /// 是否正在下载
    bool IsDownloading() {
        return IsRunning();
    }
    
private:
    static size_t OnWriteRecvDataToFile(char *data, size_t size, size_t nmemb, void *stream);
    void OnProgressResult(void *userData, long long dlTotal, long long dlNow, long long ulTotal, long long ulNow);
    
    int64_t QueryFileSize(const std::string url);
    
    int64_t m_nFirstProgress = 0;       //已下载进度
    int64_t m_nCurrentProgress = 0;     //当前进度
    int64_t m_nTotalProgress = 0;       //总进度
    std::function<void(const std::string url, long long dlTotal, long long dlNow)> m_pProgressCallback = nullptr;
};

ZJ_NAMESPACE_END
#endif /* CNetRequestDownload_hpp */

