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

ZJ_NAMESPACE_BEGIN

CNetRequestUpload::CNetRequestUpload(void *pUserData, MODE_TYPE mode): CNetRequest(pUserData, mode)
{
    SetRequestTimeout(0);   //设置上传时间无限
}

CNetRequestUpload::~CNetRequestUpload() {
    
}

int64_t CNetRequestUpload::QueryFileUploadedSize(const std::string url)
{
    int64_t length = 0;
    CURL *cUrl = (CURL *)ConfigURL(url);
    if (cUrl) {
        curl_easy_setopt(cUrl, CURLOPT_HEADER, 1L);     //接收请求头数据
        curl_easy_setopt(cUrl, CURLOPT_NOBODY, 1L);     //查询不附带文件内容
        CURLcode ret = curl_easy_perform(cUrl);
        if (ret != CURLE_OK) {
            CLogW("Error[%d] reading uploaded file size.", ret);
        }
        length = m_nContentLength;
        Cancel();
    }
    
    return length;
}

void CNetRequestUpload::Start(const std::string url, const std::string filePath, OnNetRequestResponseCB pCallback)
{
    if (IsRunning()) return;
    m_nFirstProgress = QueryFileUploadedSize(url);
    
    CURL *cUrl = (CURL *)ConfigURL(url, true);
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
        curl_easy_setopt(cUrl, CURLOPT_READFUNCTION, OnReaderSendData);
        curl_easy_setopt(cUrl, CURLOPT_READDATA, file);
        curl_easy_setopt(cUrl, CURLOPT_HEADER, 0L);    //上传不附带信息头
        curl_easy_setopt(cUrl, CURLOPT_NOBODY, 0L);    //上传需要附带文件内容
        
        m_nTotalProgress = fileSize;
        if (m_nContentLength >= fileSize) {
            OnRequestResult(m_pUserData, ERR_NoERROR, "", "", "");
            Cancel();
            return;
        } else if (m_nContentLength > 0) {
            fseek(file, (long)m_nFirstProgress, SEEK_SET);
            curl_easy_setopt(cUrl, CURLOPT_APPEND, 1L);
        } else {
            curl_easy_setopt(cUrl, CURLOPT_APPEND, 0L);
        }
        
        CLogD("Upload local path: %s", filePath.c_str());
        Request(pCallback);
        fclose(file);
    }
}

#pragma mark -

size_t CNetRequestUpload::OnReaderSendData(char *ptr, size_t size, size_t nmemb, void *stream)
{
    if (ferror((FILE*)stream)) return CURL_READFUNC_ABORT;
    return fread(ptr, size, nmemb, (FILE*)stream) * size;
}

void CNetRequestUpload::OnProgressResult(void *userData, long long dlTotal, long long dlNow, long long ulTotal, long long ulNow)
{
    if (ulNow == 0) return;
    
    m_nCurrentProgress = m_nFirstProgress + ulNow;
    if (m_pProgressCallback) {
        m_pProgressCallback(GetUrl(), m_nTotalProgress, m_nCurrentProgress);
    } else {
        CLogW("Upload: %s -> %lld:%lld", GetUrl().c_str(), m_nTotalProgress, m_nCurrentProgress);
    }
}

ZJ_NAMESPACE_END
