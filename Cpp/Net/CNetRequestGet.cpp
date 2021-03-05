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

void CNetRequestGet::Start(const std::string url)
{
    CURL *cUrl = (CURL *)GetHandleWithURL(url, true);
    if (cUrl) {
        Request();
    }
}

void CNetRequestGet::OnRequestResult(void *userData, long statusCode, std::string strRecvHeader, std::string strRecvBody, const std::string strError)
{
    
}
