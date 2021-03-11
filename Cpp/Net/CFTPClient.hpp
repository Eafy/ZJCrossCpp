//
//  CFTPClient.hpp
//  ZJCrossCpp
//
//  Created by eafy on 2021/3/8.
//  Copyright © 2021 ZJ. All rights reserved.
//

#ifndef CFTPClient_hpp
#define CFTPClient_hpp

#include <stdio.h>
#include <list>
#include <string>
#include <functional>
#include "CNetRequestUpload.hpp"
#include "CNetRequestDownload.hpp"

class CFTPClient {
    
public:
    struct FileInfo {
        unsigned long fileSize;           //文件大小
        std::string fileName;             //文件名称
        std::string filePath;             //文件完整路径
        std::string time;                 //时间
        bool isDir;                       //是否是目录
    };
    
public:
    CFTPClient(void *pUserData = nullptr);
    ~CFTPClient();
    
    /// 配置用户信息
    /// @param domain FTP服务器域名或IP
    /// @param port 端口
    /// @param userName 用户账号
    /// @param password 用户密码
    bool ConfigUserInfo(const std::string domain, int port, const std::string userName, const std::string password);
    
    /// 查询所有文件
    /// @param path 路径
    void QueryAllFiles(const std::string path, std::function<void(std::list<CFTPClient::FileInfo> list)> pCallback);
    
    /// 是否存在文件或文件夹
    /// @param path 文件路径
    /// @param isDir 是否判断文件夹，默认判断是文件
    bool IsExist(const std::string path, bool isDir = false);
    
    /// 创建目录
    /// @param path 目录路径
    bool CreateDirectory(const std::string path);
    
    /// 删除目录
    /// @param path 目录路径
    bool DeleteDirectory(const std::string path);
    
    /// 删除文件
    /// @param path 文件路径
    bool DeleteFile(const std::string path);
    
    /// 移动文件路径
    /// @param fromPath 原路径
    /// @param toPath 移动路径
    bool MoveFile(const std::string fromPath, const std::string toPath);
    
    /// 下载文件
    /// @param url 服务器下载短链接地址
    /// @param fileDir 本地保存的目录
    /// @param fileName 本地保存的文件名称，传空字符串默认使用服务器名称
    /// @param timeout 下载超时时间，0表示不限制时间
    /// @param pReqsCallback 结果回调
    /// @param pProgessCallback 进度回调
    /// @return true，启动成功，false，有文件正在下载
    bool DownloadFile(const std::string url, const std::string fileDir, const std::string fileName, int timeout = 0, std::function<void(long statusCode, std::string strRecvBody, const std::string strError)> pReqsCallback = nullptr, std::function<void(const std::string url, long long dlTotal, long long dlNow)> pProgessCallback = nullptr);
    
    /// 上传文件
    /// @param url 服务器上传短链接地址
    /// @param filePath 本地文件地址
    /// @param timeout 上传超时时间，0表示不限制时间
    /// @param pReqsCallback 结果回调
    /// @param pProgessCallback 进度回调
    /// @return true，启动成功，false，有文件正在上传
    bool UploadFile(const std::string url, const std::string filePath, int timeout = 0, std::function<void(long statusCode, std::string strRecvBody, const std::string strError)> pReqsCallback = nullptr, std::function<void(const std::string url, long long dlTotal, long long dlNow)> pProgessCallback = nullptr);
 
private:
    
    /// 检测文件信息，并组装为列表
    /// @param sPath 上级目录
    /// @param strFileListStr 文件列表字符串
    /// @param fileInfoList 待接收数组
    void CheckFilesInfo(std::string sPath, std::string strFileListStr, std::list<FileInfo> &fileInfoList);
    
private:
    void *m_pUserData = nullptr;
    std::string m_sDomain = "";
    int m_nPort = 0;
    std::string m_sUserName = "";
    std::string m_sPassword = "";
    std::string m_sUrl = "";
    
    bool isUseListCmd = false;      // 是否使用"LIST"查询文件，默认使用"MLSD"
    void *m_pThreadPool = nullptr;
    CNetRequestUpload *m_pUploader = nullptr;
    CNetRequestDownload *m_pDownloader = nullptr;
};

#endif /* CFTPClient_hpp */
