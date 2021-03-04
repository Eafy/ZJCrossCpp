//
//  CCondition.cpp
//  ZJCrossCpp
//
//  Created by eafy on 2020/10/5.
//  Copyright © 2020 ZJ. All rights reserved.
//

#include "CCondition.hpp"
#include <time.h>
#include <sys/time.h>
#include <sys/errno.h>

ZJ_NAMESPACE_TOOL_BEGIN

CCondition::CCondition()
{
    pthread_mutex_init(&m_Mutex, nullptr);
    pthread_cond_init(&m_Cond, nullptr);
}

CCondition::~CCondition()
{
    pthread_mutex_destroy(&m_Mutex);
    pthread_cond_destroy(&m_Cond);
}

int CCondition::Lock()
{
    return pthread_mutex_lock(&m_Mutex);
}

int CCondition::TryLock()
{
    return pthread_mutex_trylock(&m_Mutex);
}

int CCondition::Unlock()
{
    return pthread_mutex_unlock(&m_Mutex);
}

int CCondition::Wait()
{
    Lock();
    int ret = pthread_cond_wait(&m_Cond, &m_Mutex);
    Unlock();
    return ret;
}

int CCondition::WaitTime(long msTime)
{
    if (msTime <= 0) return EAGAIN;
    Lock();
    int ret = 0;

#if defined(__APPLE__)  //iOS
    if (__builtin_available(iOS 10.0, *)) {
        struct timespec abstime;
        clock_gettime(CLOCK_REALTIME, &abstime);

        if (msTime >= 1000) {
            abstime.tv_sec += msTime / 1000;
            abstime.tv_nsec += msTime % 1000 * 1000000;
        } else {
            abstime.tv_nsec += msTime * 1000000;
        }
        
        ret = pthread_cond_timedwait(&m_Cond, &m_Mutex, &abstime);
    } else {
        struct timeval valtime;
        gettimeofday(&valtime, NULL);
        
        if (msTime >= 1000) {
            valtime.tv_sec += msTime / 1000;
            valtime.tv_usec += msTime % 1000 * 1000;
        } else {
            valtime.tv_usec += msTime * 1000;
        }
        
        struct timespec abstime;
        abstime.tv_sec = valtime.tv_sec;
        abstime.tv_nsec = valtime.tv_usec * 1000;
        
        ret = pthread_cond_timedwait(&m_Cond, &m_Mutex, &abstime);
    }
#else
    struct timespec abstime;
    clock_gettime(CLOCK_REALTIME, &abstime);

    if (msTime >= 1000) {
        abstime.tv_sec += msTime / 1000;
        abstime.tv_nsec += msTime % 1000 * 1000000;
    } else {
        abstime.tv_nsec += msTime * 1000000;
    }
    
    ret = pthread_cond_timedwait(&m_Cond, &m_Mutex, &abstime);
#endif
    Unlock();
    return ret;
}

int CCondition::Signal()
{
    return pthread_cond_signal(&m_Cond);
}

int CCondition::Broadcast()
{
    return pthread_cond_broadcast(&m_Cond);
}

ZJ_NAMESPACE_TOOL_END
