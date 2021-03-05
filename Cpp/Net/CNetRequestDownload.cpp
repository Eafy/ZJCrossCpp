//
//  CNetRequestDownload.cpp
//  ZJCrossCpp
//
//  Created by eafy on 2021/3/4.
//  Copyright © 2021 ZJ. All rights reserved.
//

#include "CNetRequestDownload.hpp"
#include <iostream>
#include <sys/stat.h>
#include <stdio.h>
#include "curl.h"
#include "CPrintfLog.hpp"
#include "CString.hpp"

CNetRequestDownload::CNetRequestDownload(void *pUserData): CNetRequest(pUserData)
{
    m_RequestTimeout = 0;   //设置下载时间无限
}

CNetRequestDownload::~CNetRequestDownload() {
    
}

void CNetRequestDownload::Start(const std::string url, const std::string fileDir, const std::string fileName)
{
    if (fileDir.length() == 0) {
        OnRequestResult(m_pUserData, ERR_FILE_PATH_INVALID, "", "", "");
        return;
    }
    
    CURL *cUrl = (CURL *)GetHandleWithURL(url, true);
    if (cUrl) {
        std::string filePath = fileDir;
        std::vector<std::string> list = ZJ::Tool::CString::Split(fileDir, "/");
        if (list.back().find(".") != std::string::npos) {    //不包含.符号，包含的时候，默认完整路径
            if (*fileDir.rbegin() != '/') {
                filePath += "/";
            }
            if (fileName.length() == 0) {
                std::vector<std::string> list = ZJ::Tool::CString::Split(url, "/");
                std::string fileName =  list.back();
                filePath += fileName;
            } else {
                filePath += fileName;
            }
        }
        
        struct stat fileInfo;
        if (stat(filePath.c_str(), &fileInfo) == 0) {
            m_nCurrentProgress = fileInfo.st_size;
        } else {
            m_nCurrentProgress = 0;
        }
        FILE *file = fopen(filePath.c_str(), "ab+");
        if (file == nullptr) {
            OnRequestResult(m_pUserData, ERR_FILE_PATH_INVALID, "", "", "");
            return;
        }
        
        curl_easy_setopt(cUrl, CURLOPT_WRITEFUNCTION, OnWriterRecvData);
        curl_easy_setopt(cUrl, CURLOPT_WRITEDATA, file);
        curl_easy_setopt(cUrl, CURLOPT_RESUME_FROM_LARGE, m_nCurrentProgress);
        curl_easy_setopt(cUrl, CURLOPT_HEADER, 0);      //不写入请求头文件数据
        
        CPrintfD("Download local path: %s", filePath.c_str());
        Request();
        fclose(file);
    }
}

size_t CNetRequestDownload::OnWriterRecvData(char *ptr, size_t size, size_t nmemb, void *stream)
{
    return fwrite(ptr, size, nmemb, (FILE*)stream);
}

void CNetRequestDownload::OnRequestResult(void *userData, long statusCode, std::string strRecvHeader, std::string strRecvBody, const std::string strError)
{
    CPrintfW("Result: %ld, body: %s", statusCode, strRecvBody.c_str());
}

void CNetRequestDownload::OnProgressResult(void *userData, long long dlTotal, long long dlNow, long long ulTotal, long long ulNow)
{
    if (m_nTotalProgress == 0 && dlTotal > 0) {
        m_nTotalProgress = m_nCurrentProgress + dlTotal;
    }
    m_nCurrentProgress += dlNow;
    CPrintfW("Download: %lld:%lld", m_nTotalProgress, m_nCurrentProgress);
}
