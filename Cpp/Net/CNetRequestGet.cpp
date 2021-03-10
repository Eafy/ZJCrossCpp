//
//  CNetRequestGet.cpp
//  ZJCrossCpp
//
//  Created by eafy on 2021/3/2.
//  Copyright © 2021 ZJ. All rights reserved.
//

#include "CNetRequestGet.hpp"
#include "curl.h"
#include "CPrintfLog.hpp"

CNetRequestGet::CNetRequestGet(void *pUserData): CNetRequest(pUserData) {

}

CNetRequestGet::~CNetRequestGet() {
    
}

void CNetRequestGet::Start(const std::string url, std::function<void(long statusCode, std::string strRecvBody, const std::string strError)> pCallback)
{
    CURL *cUrl = (CURL *)ConfigURL(url, true);
    if (cUrl) {
        Request(pCallback);
    }
}
