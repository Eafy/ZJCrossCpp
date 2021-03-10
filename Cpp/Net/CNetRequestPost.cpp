//
//  CNetRequestPost.cpp
//  ZJCrossCpp
//
//  Created by eafy on 2021/3/2.
//  Copyright © 2021 ZJ. All rights reserved.
//

#include "CNetRequestPost.hpp"
#include "curl.h"
#include "CPrintfLog.hpp"

CNetRequestPost::CNetRequestPost(void *pUserData): CNetRequest(pUserData) {

}

CNetRequestPost::~CNetRequestPost() {
    
}

void CNetRequestPost::Start(const std::string url, const std::string paraMap, std::function<void(long statusCode, std::string strRecvBody, const std::string strError)> pCallback)
{
    CURL *cUrl = (CURL *)ConfigURL(url);
    if (cUrl) {
        AddParameterString(paraMap);
        curl_easy_setopt(cUrl, CURLOPT_POST, true);
        Request(pCallback);
    }
}
