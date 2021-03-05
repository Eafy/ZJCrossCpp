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

void CNetRequestPost::Start(const std::string url, const std::string paraMap)
{
    CURL *cUrl = (CURL *)GetHandleWithURL(url);
    if (cUrl) {
        AddParameterString(paraMap);
        curl_easy_setopt(cUrl, CURLOPT_POST, true);
        Request();
    }
}

void CNetRequestPost::OnRequestResult(void *userData, long statusCode, std::string strRecvHeader, std::string strRecvBody, const std::string strError)
{
    
}
