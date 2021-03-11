//
//  CThreadPool.cpp
//  ZJCrossCpp
//
//  Created by eafy on 2020/10/5.
//  Copyright © 2020 ZJ. All rights reserved.
//

#include "CThreadPool.hpp"

ZJ_NAMESPACE_BEGIN

void CThreadPool::_RunOperate(CThreadTask *task)
{
    task->m_Func();  //执行任务
}

void CThreadPool::CancelTask(uint64_t tag)
{
    Lock();

    CThreadTask *task = GetTask();
    while (task) {
        if (task->m_nTag == tag) {
            if (!task->m_bIsRunning) {
                SetTask(task->GetNextTask());
                DeleteP(task);     //执行完任务释放内存
            }
            break;
        } else {
            task = task->GetNextTask();
        }
    }

    Unlock();
}

ZJ_NAMESPACE_END
