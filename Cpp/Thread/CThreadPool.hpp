//
//  CThreadPool.hpp
//  ZJCrossCpp
//
//  Created by eafy on 2020/10/5.
//  Copyright © 2020 ZJ. All rights reserved.
//

#ifndef CThreadPool_hpp
#define CThreadPool_hpp

#include <stdio.h>
#include "CDefType.hpp"
#include "CSingleton.hpp"
#include "CBaseThreadPool.hpp"

ZJ_NAMESPACE_BEGIN

class CThreadPool: public CBaseThreadPool
{
public:
    CThreadPool(u_int32_t nThreads = 32): CBaseThreadPool(nThreads) {};   //初始化线程池(默认线程池最大64个线程)
    virtual ~CThreadPool() {};
    DECLARE_SINGLETON_CLASS(CThreadPool);
    
    template<typename callable, typename... arguments>
    uint64_t AddTask(callable&& func, arguments&&... args) {
        std::function<typename std::result_of<callable(arguments...)>::type()> task(std::bind(std::forward<callable>(func), std::forward<arguments>(args)...));

        if (!IsExit()) {
            CThreadTask *threadTask = new CThreadTask();  //产生一个新的任务
            threadTask->m_Func = task;
            threadTask->m_nTag = (uint64_t)&func;
            _AddTask(threadTask);
            
            return threadTask->m_nTag;
        }
        return 0;
    }
    
    template<typename callable, typename... arguments>
    uint64_t AddTask(uint64_t tag, callable&& func, arguments&&... args) {
        std::function<typename std::result_of<callable(arguments...)>::type()> task(std::bind(std::forward<callable>(func), std::forward<arguments>(args)...));

        if (!IsExit()) {
            CThreadTask *threadTask = new CThreadTask();  //产生一个新的任务
            threadTask->m_Func = task;
            threadTask->m_nTag = tag;
            _AddTask(threadTask);
            
            return threadTask->m_nTag;
        }
        return 0;
    }
    
    template<typename callable, typename... arguments>
    uint64_t OnceTask(callable&& func, arguments&&... args) {
        std::function<typename std::result_of<callable(arguments...)>::type()> task(std::bind(std::forward<callable>(func), std::forward<arguments>(args)...));

        if (!IsExit()) {
            CThreadTask *threadTask = new CThreadTask();  //产生一个新的任务
            threadTask->m_Func = task;
            _AddTask(threadTask);
            
            return threadTask->m_nTag;
        }
        
        return 0;
    }

    /**
     取消未运行的任务(已执行的任务无法取消)

     @param func 函数接口
     */
    template<typename callable>
    void CancelTask(callable&& func) {
        uint64_t tag = &func;
        CancelTask(tag);
    }
    
    /// 取消任务，需要和AddTask第一个参数对应
    /// @param tag 任务Tag
    void CancelTask(uint64_t tag);
    
private:
    void _RunOperate(CThreadTask *task);
};

ZJ_NAMESPACE_END
#endif /* CThreadPool_hpp */
