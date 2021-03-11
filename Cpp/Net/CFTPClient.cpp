//
//  CFTPClient.cpp
//  ZJCrossCpp
//
//  Created by eafy on 2021/3/8.
//  Copyright © 2021 ZJ. All rights reserved.
//

#include "CFTPClient.hpp"
#include "CString.hpp"
#include <string.h>
#include "CThreadPool.hpp"

CFTPClient::CFTPClient(void *pUserData)
{
    m_pUserData = pUserData;
    m_pThreadPool = new ZJ::Tool::CThreadPool(1);
}

CFTPClient::~CFTPClient()
{
    if (m_pThreadPool) {
        ((ZJ::Tool::CThreadPool *)m_pThreadPool)->CancelAll();
        delete (ZJ::Tool::CThreadPool *)m_pThreadPool;
        m_pThreadPool = nullptr;
    }
}

bool CFTPClient::ConfigUserInfo(const std::string domain, int port, const std::string userName, const std::string password)
{
    if (m_sUrl.length() > 0) return false;
    m_sDomain = domain;
    m_nPort = port;
    m_sUserName = userName;
    m_sPassword = password;
    
    if (domain.find("ftp://") != 0 && domain.find("FTP://") != 0) {
        std::stringstream sUrl;
        sUrl << "ftp://" << domain << ":" << port;
        m_sUrl = sUrl.str();
    } else {
        m_sUrl = domain;
    }
    return true;
}

void CFTPClient::CheckFilesInfo(std::string sPath, std::string strFileListStr, std::list<FileInfo> &fileInfoList)
{
    FileInfo fileInfo = {0};
    std::vector<std::string> list = ZJ::Tool::CString::Split(strFileListStr,"\n");
    if (list.size() > 0) {
        std::vector<std::string>::iterator iterFile = list.begin();
        for (; iterFile != list.end(); iterFile++) {
            if (iterFile->size() == 0) continue;
            
            if (isUseListCmd) {
                std::vector<std::string> listItem = ZJ::Tool::CString::Split(*iterFile, " ");
                if (iterFile->length() > 0 && iterFile->c_str()[0] == 'd') {    //判断是否是文件夹
                    fileInfo.isDir = true;
                }
                
                if (listItem.size() >= 9) {
                    std::string sAuthority = listItem.at(0);
                    std::string sSize = listItem.at(4);
                    std::string sDate = listItem.at(5) + " " + listItem.at(6) + " " + listItem.at(7);
                    std::string sName = listItem.at(8);
                    for (int i=9; i<listItem.size(); i++) {
                        sName += " " + listItem.at(i);
                    }
                    
                    if (sAuthority.c_str()[0] == 'd') {    //判断是否是文件夹
                        fileInfo.isDir = true;
                    }
                    fileInfo.time = sDate;
                    fileInfo.fileSize = atol(sSize.c_str());
                    fileInfo.fileName = sName;
                    fileInfo.filePath = sPath + sName;
                    
                    fileInfoList.push_back(fileInfo);
                }
            } else {
                std::vector<std::string> listItem = ZJ::Tool::CString::Split(*iterFile, ";");
                for (std::string itemStr : listItem) {
                    std::vector<std::string> values = ZJ::Tool::CString::Split(itemStr, "=");
                    if (values.size() == 2) {
                        if (values.front() == "Size") {
                            fileInfo.fileSize = atol(values.back().c_str());
                        } else if (values.front() == "Modify") {
                            fileInfo.time = values.back();
                        } else if (values.front() == "Type") {
                            fileInfo.isDir = values.back() == "dir";
                        }
                    } else if (values.size() == 1 && itemStr == listItem.back()) {
                        if (listItem.back().size() > 1 && listItem.back().c_str()[0] == ' ') {
                            fileInfo.fileName = listItem.back().substr(1,  listItem.back().size()-1);
                        } else {
                            fileInfo.fileName = listItem.back();
                        }
                        fileInfo.filePath = sPath + fileInfo.fileName;
                    }
                }
                fileInfoList.push_back(fileInfo);
            }
        }
    }
}

#pragma mark -

void CFTPClient::QueryAllFiles(const std::string path, std::function<void(std::list<CFTPClient::FileInfo> list)> pCallback)
{
    ((ZJ::Tool::CThreadPool *)m_pThreadPool)->AddTask([path, pCallback](CFTPClient *self) {
        CNetRequest request(self, (CNetRequest::MODE_TYPE)(CNetRequest::MODE_TYPE_FTP | CNetRequest::MODE_TYPE_CUSTOMR));
        request.ConfigURL(self->m_sUrl);
        request.ConfigUserInfo(self->m_sUserName, self->m_sPassword);
        
        std::string sPath = ZJ::Tool::CString::AppendComponentForPath(path, "");
        request.AddHeader((self->isUseListCmd ? "LIST " : "MLSD ") + sPath);
        
        long ret = 0;
        request.Request([&ret](long code, std::string ctx, std::string err) {
            ret = code;
        });
        
        std::list<FileInfo> list;
        if (ret == 0) {
            self->CheckFilesInfo(sPath, request.GetBoby(), list);
        } else {
            CPrintfW("Failed to query: %s", sPath.c_str());
        }
        pCallback(list);
    }, this);
}

bool CFTPClient::IsExist(const std::string path, bool isDir)
{
    CNetRequest request(this, (CNetRequest::MODE_TYPE)(CNetRequest::MODE_TYPE_FTP | CNetRequest::MODE_TYPE_CUSTOMR));
    request.ConfigURL(m_sUrl);
    request.ConfigUserInfo(m_sUserName, m_sPassword);
    
    std::string lastName = "";
    std::string sPath = ZJ::Tool::CString::RemoveLastComponentForPath(path, lastName);
    request.AddHeader(ZJ::Tool::CString::AppendComponentForPath(isUseListCmd ? "LIST " : "MLSD ", sPath));
    
    long ret = 0;
    request.Request([&ret](long code, std::string ctx, std::string err) {
        ret = code;
    });
    
    if (ret == 0) {
        if (lastName.length() > 0) {
            std::list<FileInfo> list;
            CheckFilesInfo(path, request.GetBoby(), list);
            for (FileInfo info : list) {
                if (info.fileName == lastName) {
                    return info.isDir == isDir;
                }
            }
        } else {
            return true;
        }
    }
    
    return false;
}

bool CFTPClient::CreateDirectory(const std::string path)
{
    CNetRequest request(this, (CNetRequest::MODE_TYPE)(CNetRequest::MODE_TYPE_FTP | CNetRequest::MODE_TYPE_CUSTOMR));
    
    bool isCreate = false;
    std::string subPath = "";
    std::vector<std::string> list = ZJ::Tool::CString::Split(path, "/");
    for (std::string str : list) {
        if (str.length() > 0) {
            request.ConfigUserInfo(m_sUserName, m_sPassword);
            request.ConfigURL(m_sUrl);
            subPath = ZJ::Tool::CString::AppendComponentForPath(subPath, str);
            request.AddHeader("MKD " + subPath);
            long ret = 0;
            request.Request([&ret](long code, std::string ctx, std::string err) {
                ret = code;
            });
            
            isCreate = false;
            if (ret != 0) {
                if (request.GetError().find("550") != std::string::npos) {  //未执行请求的操作，即存在路径
                    isCreate = true;
                    continue;
                } else if (request.GetError().find("257") != std::string::npos) {   //显示目前的路径名称，即创建成功
                    isCreate = true;
                    continue;
                }
            }
        }
    }

    return isCreate;
}

bool CFTPClient::DeleteDirectory(const std::string path)
{
    CNetRequest request(this, (CNetRequest::MODE_TYPE)(CNetRequest::MODE_TYPE_FTP | CNetRequest::MODE_TYPE_CUSTOMR));
    request.ConfigUserInfo(m_sUserName, m_sPassword);
    request.ConfigURL(m_sUrl);
    request.AddHeader("RMD " + path);
    long ret = 0;
    request.Request([&ret](long code, std::string ctx, std::string err) {
        ret = code;
    });
    if (ret != 0 && (request.GetError().find("250") != std::string::npos ||     //请求的文件操作完成
                     request.GetError().find("550") != std::string::npos)) {     //未执行请求的操作，即路径不存在
        ret = 0;
    }

    return ret == 0;
}

bool CFTPClient::DeleteFile(const std::string path)
{
    CNetRequest request(this, (CNetRequest::MODE_TYPE)(CNetRequest::MODE_TYPE_FTP | CNetRequest::MODE_TYPE_CUSTOMR));
    request.ConfigUserInfo(m_sUserName, m_sPassword);
    request.ConfigURL(m_sUrl);
    request.AddHeader("DELE " + path);
    long ret = 0;
    request.Request([&ret](long code, std::string ctx, std::string err) {
        ret = code;
    });
    if (ret != 0 && request.GetError().find("250") != std::string::npos) {
        ret = 0;
    }

    return ret == 0;
}

bool CFTPClient::MoveFile(const std::string fromPath, const std::string toPath)
{
    CNetRequest request(this, (CNetRequest::MODE_TYPE)(CNetRequest::MODE_TYPE_FTP | CNetRequest::MODE_TYPE_QUOTE));
    request.ConfigUserInfo(m_sUserName, m_sPassword);
    request.ConfigURL(m_sUrl);
    request.AddHeader("RNFR " + fromPath);
    request.AddHeader("RNTO " + toPath);
    long ret = 0;
    request.Request([&ret](long code, std::string ctx, std::string err) {
        ret = code;
    });
    if (ret != 0 && request.GetError().find("250") != std::string::npos) {
        ret = 0;
    }

    return ret == 0;
}

bool CFTPClient::DownloadFile(const std::string url, const std::string fileDir, const std::string fileName, int timeout, std::function<void(long statusCode, std::string strRecvBody, const std::string strError)> pReqsCallback, std::function<void(const std::string url, long long dlTotal, long long dlNow)> pProgessCallback)
{
    if (!m_pDownloader) {
        m_pDownloader = new CNetRequestDownload(this, CNetRequest::MODE_TYPE_FTP);
    } else if (m_pDownloader->IsDownloading()) {
        return false;
    }
    m_pDownloader->ConfigUserInfo(m_sUserName, m_sPassword);
    m_pDownloader->SetRequestTimeout(timeout);
    m_pDownloader->SetProgessCallback(pProgessCallback);
    
    m_pDownloader->Start(ZJ::Tool::CString::AppendComponentForPath(m_sUrl, url), fileDir, fileName, pReqsCallback);
    return true;
}

bool CFTPClient::UploadFile(const std::string url, const std::string filePath, int timeout, std::function<void(long statusCode, std::string strRecvBody, const std::string strError)> pReqsCallback, std::function<void(const std::string url, long long dlTotal, long long dlNow)> pProgessCallback)
{
    if (!m_pUploader) {
        m_pUploader = new CNetRequestUpload(this, CNetRequest::MODE_TYPE_FTP);
    } else if (m_pUploader->IsUploading()) {
        return false;
    }
    m_pUploader->ConfigUserInfo(m_sUserName, m_sPassword);
    m_pUploader->SetRequestTimeout(timeout);
    m_pUploader->SetProgessCallback(pProgessCallback);
    
    m_pUploader->Start(ZJ::Tool::CString::AppendComponentForPath(m_sUrl, url), filePath, pReqsCallback);
    return true;
}