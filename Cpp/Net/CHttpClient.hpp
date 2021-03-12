//
//  CHttpClient.hpp
//  JMSmartFTPUtils
//
//  Created by lzj on 2021/3/12.
//

#ifndef CHttpClient_hpp
#define CHttpClient_hpp

#include <stdio.h>
#include <mutex>
#include <map>
#include "CSingleton.hpp"
#include "CBaseThreadPool.hpp"
#include "CNetRequest.hpp"

ZJ_NAMESPACE_BEGIN

class CHttpClient: CBaseThreadPool, CNetRequest
{
public:
    CHttpClient();
    ~CHttpClient();
    DECLARE_SINGLETON_CLASS(CHttpClient);
    
    
    
private:
    void *m_pThreadPool = nullptr;
    
};

ZJ_NAMESPACE_END
#endif /* CHttpClient_hpp */
