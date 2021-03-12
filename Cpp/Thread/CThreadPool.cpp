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
    task->Run();  //执行任务
}

void CThreadPool::CancelTask(uint64_t tag)
{
    Lock();

    CThreadTask *task = GetFirstTask();
    while (task) {
        if (task->m_nTag == tag) {
            if (!task->IsRunning()) {
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
