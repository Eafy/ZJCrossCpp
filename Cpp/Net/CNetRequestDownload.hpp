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

class CNetRequestDownload: public CNetRequest {
public:
    CNetRequestDownload(void *pUserData = nullptr);
    ~CNetRequestDownload();
    
    /// 开始下载
    /// @param url 下载地址
    /// @param fileDir 目录，可填写完整带.的本地下载路径，则不使用后面的文件名称参数
    /// @param fileName 文件名称
    void Start(const std::string url, const std::string fileDir, const std::string fileName = "");
    
private:
    static size_t OnWriterRecvData(char *data, size_t size, size_t nmemb, void *stream);
    void OnRequestResult(void *userData, long statusCode, std::string strRecvHeader, std::string strRecvBody, const std::string strError);
    void OnProgressResult(void *userData, long long dlTotal, long long dlNow, long long ulTotal, long long ulNow);
    
    int64_t m_nCurrentProgress = 0;     //当前已下载长度
    int64_t m_nTotalProgress = 0;       //文件总长度
};

#endif /* CNetRequestDownload_hpp */
