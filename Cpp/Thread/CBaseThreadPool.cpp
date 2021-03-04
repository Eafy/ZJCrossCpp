//
//  CBaseThreadPool.cpp
//  ZJCrossCpp
//
//  Created by eafy on 2020/10/5.
//  Copyright © 2020 ZJ. All rights reserved.
//

#include "CBaseThreadPool.hpp"
#include "CCondition.hpp"
#include <thread>

ZJ_NAMESPACE_TOOL_BEGIN

CBaseThreadPool::CBaseThreadPool(u_int32_t nThreads)
{
    m_TrigCondition = new CCondition();
    m_ThreadCount = 0;
    m_ThreadIdle = 0;
    m_bQuit = false;
}

CBaseThreadPool::~CBaseThreadPool()
{
    CancelAll();
    DeleteP(m_TrigCondition);
}

#pragma mark -

void CBaseThreadPool::_AddTask(CThreadTask *task)
{
    if (m_bQuit) return;
    
    m_TrigCondition->Lock();
    if (m_pTaskFirst == nullptr) {
        m_pTaskFirst = task;
    } else {
        m_pTaskLast->SetNextTask(task);   //连接末尾任务
    }
    m_pTaskLast = task;  //将新任务指定为末尾任务

    if (m_ThreadIdle > 0) {   //线程池中有线程空闲则唤醒
        m_TrigCondition->Signal();
    } else if (m_ThreadCount < m_MaxThread) {    //创建新线程
        m_ThreadCount ++;
        pthread_t tid;
        pthread_create(&tid, nullptr, didRoutineThread, this);
    }

    m_TrigCondition->Unlock();
}

void *CBaseThreadPool::didRoutineThread(void *arg)
{
    CBaseThreadPool *self = static_cast<CBaseThreadPool*>(arg);

    bool waitTimeout = false;
    while (1) {
        //等待队列有任务到来 或者 收到线程池销毁通知
        self->m_ThreadIdle ++;
        while (self->m_pTaskFirst == nullptr && !self->m_bQuit) {       //等待队列有任务到来/收到线程池销毁通知
            if (self->m_ThreadIdle > 1 ||
                self->m_TrigCondition->WaitTime(1000) == ETIMEDOUT) {
                waitTimeout = true;
                break;
            }
        }
        self->m_ThreadIdle --;

        if (self->m_pTaskFirst != nullptr) {   //运行下一个任务
            if (self->m_bQuit) {    //线程正在等待退出，不再执行任务
                self->m_TrigCondition->Lock();
                CThreadTask *task = self->m_pTaskFirst;
                if (task) {
                    self->_CancelPreOperate(task);
                    self->m_pTaskFirst = task->GetNextTask();
                    self->_DeleteTask(task);    //执行完任务释放任务l内存
                }
                if (self->m_pTaskFirst == nullptr) {
                    self->m_pTaskLast = nullptr;
                }
                self->m_TrigCondition->Unlock();
            } else {
                self->m_TrigCondition->Lock();
                CThreadTask *task = self->m_pTaskFirst;
                if (task) {
                    self->m_pTaskFirst = task->GetNextTask();
                }
                if (self->m_pTaskFirst == nullptr) {
                    self->m_pTaskLast = nullptr;
                }
                self->m_TrigCondition->Unlock();   //休眠前需要解锁，允许其他任务进入线程池

                if (task && !self->m_bQuit && !task->m_bIsRunning) {
                    task->m_bIsRunning = true;
                    self->_RunOperate(task);
                    task->m_bIsRunning = false;
                }
                self->_DeleteTask(task);    //执行完任务释放任务内存
            }
        }

        if (waitTimeout ||     //等待超时，跳出线程
            (self->m_bQuit && self->m_pTaskFirst == nullptr)) {     //被主动全部退出线程池，self->m_pFirst为了让所有任务都被移除
            self->m_ThreadCount--;    //线程数减1
            if (self->m_ThreadCount <= 0) {
                self->m_TrigCondition->Signal();
            }
            break;
        }
    }

    return NULL;
}

void CBaseThreadPool::CancelAll()
{
    if (m_bQuit) return;
    m_bQuit = true;

    if (m_ThreadIdle > 0) {
        m_TrigCondition->Broadcast();
    }

    while (m_ThreadCount > 0) {
        m_TrigCondition->Wait();
    }
}

void CBaseThreadPool::Lock()
{
    m_TrigCondition->Lock();
}

void CBaseThreadPool::Unlock()
{
    m_TrigCondition->Unlock();
}

#pragma mark -

void CBaseThreadPool::_DeleteTask(CThreadTask *task)
{
    DeleteP(task);
}


ZJ_NAMESPACE_TOOL_END
