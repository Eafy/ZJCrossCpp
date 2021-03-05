//
//  CNetRequestUpload.cpp
//  ZJCrossCpp
//
//  Created by eafy on 2021/3/5.
//  Copyright © 2021 ZJ. All rights reserved.
//

#include "CNetRequestUpload.hpp"
#include <iostream>
#include <sys/stat.h>
#include <stdio.h>
#include "curl.h"
#include "CPrintfLog.hpp"

CNetRequestUpload::CNetRequestUpload(void *pUserData): CNetRequest(pUserData)
{
    m_RequestTimeout = 0;   //设置下载时间无限
}

CNetRequestUpload::~CNetRequestUpload() {
    
}

void CNetRequestUpload::Start(const std::string url, const std::string filePath)
{
    CURL *cUrl = (CURL *)GetHandleWithURL(url, true);
    if (cUrl) {
        FILE *file = fopen(filePath.c_str(), "rb");
        if (file == nullptr) {
            OnRequestResult(m_pUserData, ERR_FILE_PATH_INVALID, "", "", "");
            return;
        }
        fseek(file, 0L, SEEK_END);
        int64_t fileSize = ftell(file);
        fseek(file, 0L, SEEK_SET);
        
        curl_easy_setopt(cUrl, CURLOPT_UPLOAD, 1L);
        curl_easy_setopt(cUrl, CURLOPT_RESUME_FROM_LARGE, fileSize);
//        curl_easy_setopt(cUrl, CURLOPT_WRITEFUNCTION, OnWriterRecvData);
//        curl_easy_setopt(cUrl, CURLOPT_WRITEDATA, file);
        
        curl_easy_setopt(m_pCurl, CURLOPT_READFUNCTION, OnReaderSendData);
        curl_easy_setopt(m_pCurl, CURLOPT_READDATA, file);
        
        CPrintfD("Upload local path: %s", filePath.c_str());
        Request();
        fclose(file);
    }
}

size_t CNetRequestUpload::OnWriterRecvData(char *ptr, size_t size, size_t nmemb, void *stream)
{
    return size * nmemb;
}

size_t CNetRequestUpload::OnReaderSendData(char *ptr, size_t size, size_t nmemb, void *stream)
{
    if (ferror((FILE*)stream)) return CURL_READFUNC_ABORT;
    return fread(ptr, size, nmemb, f) * size;
}

void CNetRequestUpload::OnRequestResult(void *userData, long statusCode, std::string strRecvHeader, std::string strRecvBody, const std::string strError)
{
    CPrintfW("Result: %ld, body: %s", statusCode, strRecvBody.c_str());
}

void CNetRequestUpload::OnProgressResult(void *userData, long long dlTotal, long long dlNow, long long ulTotal, long long ulNow)
{
    CPrintfW("Upload: %lld:%lld", ulTotal, ulNow);
}
