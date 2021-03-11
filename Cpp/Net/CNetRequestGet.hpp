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

ZJ_NAMESPACE_BEGIN

class CNetRequestGet: public CNetRequest {
public:
    CNetRequestGet(void *pUserData = nullptr);
    ~CNetRequestGet();
    
    /// 开始请求
    /// @param url 请求链接
    void Start(const std::string url, std::function<void(long statusCode, std::string strRecvBody, const std::string strError)> pCallback = nullptr);
};

ZJ_NAMESPACE_END
#endif /* CNetRequestGet_hpp */

