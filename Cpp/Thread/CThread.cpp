//
//  CThread.cpp
//  ZJCrossCpp
//
//  Created by eafy on 2020/10/4.
//  Copyright © 2020 ZJ. All rights reserved.
//

#include "CThread.hpp"

ZJ_NAMESPACE_TOOL_BEGIN

CThread::CThread():m_bIsRunning(false) {
}

CThread::~CThread() {
    Join();
    m_ThreadFuncTask = nullptr;
}

void CThread::Start()
{
    if (m_ThreadFuncTask && !m_Thread) {
        m_bIsRunning = true;
        std::function<void()> task = m_ThreadFuncTask;
        m_Thread = new std::thread([this, task]() {
            if (m_bIsRunning) {
                task();
            }
        });
    }
}

void CThread::Cancel()
{
    m_bIsRunning = false;
}

bool CThread::IsRunning()
{
    return m_bIsRunning;
}

void CThread::Sleep(long msTime)
{
    if (msTime > 0) {
        m_Condition.wait_for(m_Lock, std::chrono::milliseconds(msTime));
    }
}

void CThread::Wakeup()
{
    m_Condition.notify_one();
}

void CThread::WakeupAll()
{
    m_Condition.notify_all();
}

void CThread::Join()
{
    m_bIsRunning = false;
    WakeupAll();
    if (m_Thread) {
        if (m_Thread->joinable()) {
            m_Thread->join();
        }
        delete m_Thread;
        m_Thread = nullptr;
    }
}

void CThread::Lock()
{
    m_Lock.lock();
}

void CThread::Unlock()
{
    m_Lock.unlock();
}

ZJ_NAMESPACE_TOOL_END
