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
#include <dirent.h>
#include "curl.h"
#include "CString.hpp"

ZJ_NAMESPACE_BEGIN

CNetRequestDownload::CNetRequestDownload(void *pUserData, MODE_TYPE mode): CNetRequest(pUserData, mode)
{
    SetRequestTimeout(0);   //设置下载时间无限
}

CNetRequestDownload::~CNetRequestDownload() {
    
}

int64_t CNetRequestDownload::QueryFileSize(const std::string url)
{
    int64_t length = 0;
    CURL *cUrl = (CURL *)ConfigURL(url);
    if (cUrl) {
        curl_easy_setopt(cUrl, CURLOPT_HEADER, 0L);     //接收请求头数据
        curl_easy_setopt(cUrl, CURLOPT_NOBODY, 1L);     //查询不附带文件内容
        CURLcode ret = curl_easy_perform(cUrl);
        if (ret != CURLE_OK) {
            CPrintfW("Error[%d] reading uploaded file size.", ret);
        }
        length = m_nContentLength;
        Cancel();
    }
    
    return length;
}

void CNetRequestDownload::Start(const std::string url, const std::string fileDir, const std::string fileName, OnNetRequestResponseCB pCallback)
{
    if (IsRunning()) return;
    if (fileDir.length() == 0) {
        OnRequestResult(m_pUserData, ERR_FILE_PATH_INVALID, "", "", "");
        return;
    }
    m_nTotalProgress = QueryFileSize(url);
    
    CURL *cUrl = (CURL *)ConfigURL(url, true);
    if (cUrl) {
        DIR *dir = opendir(fileDir.c_str());
        if (!dir) {     //判断文件夹是否存在，不存在则创建
            if (mkdir(fileDir.c_str(), 0777) == -1) {
                OnRequestResult(m_pUserData, ERR_FILE_PATH_INVALID, "", "", "");
                Cancel();
                return;
            }
        } else {
            closedir(dir);
        }
        
        std::string fileNameT = fileName;
        if (fileName.length() == 0) {   //文件名不存在，则使用服务器文件名称
            std::vector<std::string> list = ZJ::CString::Split(url, "/");
            fileNameT = list.back();
        }
        std::string filePath = ZJ::CString::AppendComponentForPath(fileDir, fileNameT);
        FILE *file = fopen(filePath.c_str(), "ab+");
        if (file == nullptr) {
            OnRequestResult(m_pUserData, ERR_FILE_PATH_INVALID, "", "", "");
            Cancel();
            return;
        }
        
        struct stat fileInfo;
        if (stat(filePath.c_str(), &fileInfo) == 0) {
            m_nFirstProgress = fileInfo.st_size;
            if (m_nFirstProgress >= m_nTotalProgress && m_nTotalProgress > 0) {
                OnRequestResult(m_pUserData, ERR_NoERROR, "", "", "");
                Cancel();
                return;
            }
        } else {
            m_nFirstProgress = 0;
        }
        
        curl_easy_setopt(cUrl, CURLOPT_WRITEFUNCTION, OnWriteRecvDataToFile);
        curl_easy_setopt(cUrl, CURLOPT_WRITEDATA, file);
        curl_easy_setopt(cUrl, CURLOPT_RESUME_FROM_LARGE, m_nFirstProgress);
        curl_easy_setopt(cUrl, CURLOPT_HEADER, 0L);      //不接收请求头数据
        curl_easy_setopt(cUrl, CURLOPT_NOBODY, 0L);      //附带文件内容
//        curl_easy_setopt(cUrl, CURLOPT_MAX_RECV_SPEED_LARGE, 819200L);      //最大速度，单位bytes/s
        
        CPrintfD("Download local path: %s", filePath.c_str());
        Request(pCallback);
        fclose(file);
    }
}

size_t CNetRequestDownload::OnWriteRecvDataToFile(char *ptr, size_t size, size_t nmemb, void *stream)
{
    return fwrite(ptr, size, nmemb, (FILE*)stream);
}

void CNetRequestDownload::OnProgressResult(void *userData, long long dlTotal, long long dlNow, long long ulTotal, long long ulNow)
{
    if (dlNow == 0) return;
    if (m_nTotalProgress == 0 && dlTotal > 0) {
        m_nTotalProgress = m_nFirstProgress + dlTotal;
    }
    
    m_nCurrentProgress = m_nFirstProgress + dlNow;
    if (m_pProgessCallback) {
        m_pProgessCallback(GetUrl(), m_nTotalProgress, m_nCurrentProgress);
    } else {
        CPrintfW("Download: %s -> %lld:%lld", GetUrl().c_str(), m_nTotalProgress, m_nCurrentProgress);
    }
}

ZJ_NAMESPACE_END
