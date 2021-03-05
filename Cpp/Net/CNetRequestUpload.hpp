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
    CNetRequestUpload(void *pUserData = nullptr);
    ~CNetRequestUpload();
    
    /// 开始上传
    /// @param url 远程服务器上传地址
    /// @param filePath 本地文件路径
    void Start(const std::string url, const std::string filePath);
    
private:
    static size_t OnWriterRecvData(char *data, size_t size, size_t nmemb, void *stream);
    static size_t OnReaderSendData(char *ptr, size_t size, size_t nmemb, void *stream)
    void OnRequestResult(void *userData, long statusCode, std::string strRecvHeader, std::string strRecvBody, const std::string strError);
    void OnProgressResult(void *userData, long long dlTotal, long long dlNow, long long ulTotal, long long ulNow);
};


#endif /* CNetRequestUpload_hpp */
