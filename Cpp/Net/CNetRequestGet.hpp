//
//  CNetRequestGet.hpp
//  ZJCrossCpp
//
//  Created by eafy on 2021/3/2.
//  Copyright © 2021 ZJ. All rights reserved.
//

#ifndef CNetRequestGet_hpp
#define CNetRequestGet_hpp

#include <stdio.h>
#include "CNetRequest.hpp"

class CNetRequestGet: public CNetRequest {
public:
    CNetRequestGet(void *pUserData = nullptr);
    ~CNetRequestGet();
    
    /// 开始请求
    /// @param url 请求链接
    void Start(const std::string url);
    
private:
    void OnRequestResult(void *userData, long statusCode, std::string strRecvHeader, std::string strRecvBody, const std::string strError);
};

#endif /* CNetRequestGet_hpp */
