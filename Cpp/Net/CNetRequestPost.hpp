//
//  CNetRequestPost.hpp
//  ZJCrossCpp
//
//  Created by eafy on 2021/3/2.
//  Copyright © 2021 ZJ. All rights reserved.
//

#ifndef CNetRequestPost_hpp
#define CNetRequestPost_hpp

#include <stdio.h>
#include "CNetRequest.hpp"

ZJ_NAMESPACE_BEGIN

class CNetRequestPost: public CNetRequest {
public:
    CNetRequestPost(void *pUserData = nullptr);
    ~CNetRequestPost();
    
    /// 开始请求
    /// @param url 请求链接
    /// @param paraMap 请求参数字符串，比如uuid=1231241&userid=123
    void Start(const std::string url, const std::string paraMap = "", std::function<void(long statusCode, std::string strRecvBody, const std::string strError)> pCallback = nullptr);
};

ZJ_NAMESPACE_END
#endif /* CNetRequestPost_hpp */
