//
//  CCondition.hpp
//  ZJCrossCpp
//
//  Created by eafy on 2020/10/5.
//  Copyright © 2020 ZJ. All rights reserved.
//

#ifndef CCondition_hpp
#define CCondition_hpp

#include <stdio.h>
#include <pthread.h>
#include "CDefType.hpp"

ZJ_NAMESPACE_TOOL_BEGIN

class CCondition
{
public:
    CCondition();
    virtual ~CCondition();
    
    int Lock();         //加锁
    int TryLock();
    int Unlock();       //解锁
    int Wait();         //等待(内部会加锁)
    int WaitTime(long msTime);  //等待time毫秒（内部会进行加锁处理）
    int Signal();       //唤醒一个睡眠线程
    int Broadcast();    //唤醒所有睡眠线程
    bool IsWaiting();   //是否在等待或休眠
private:
    pthread_mutex_t m_Mutex;
    pthread_cond_t m_Cond;
};

ZJ_NAMESPACE_TOOL_END
#endif /* CCondition_hpp */
