//
//  CThread.hpp
//  ZJCrossCpp
//
//  Created by eafy on 2020/10/4.
//  Copyright © 2020 ZJ. All rights reserved.
//

#ifndef CThread_hpp
#define CThread_hpp

#include <stdio.h>
#include <thread>
#include <condition_variable>
#include <functional>
#include "CDefType.hpp"

ZJ_NAMESPACE_TOOL_BEGIN

class CThread
{
public:
    CThread();
    template<typename callable, typename... arguments>
    CThread(callable&& f, arguments&&... args) {
        std::function<typename std::result_of<callable(arguments...)>::type()> task(std::bind(std::forward<callable>(f), std::forward<arguments>(args)...));
        m_ThreadFuncTask = task;
    }
    
    ~CThread();
    
    void Start();       //启动线程；
    template<typename callable, typename... arguments>
    void Start(callable&& f, arguments&&... args) {
        std::function<typename std::result_of<callable(arguments...)>::type()> task(std::bind(std::forward<callable>(f), std::forward<arguments>(args)...));
        m_ThreadFuncTask = task;
        
        Start();
    }
    void Cancel();      //标志线程准备取消(实际内部若已运行，可能无法取消)
    void Sleep(long msTime);    //休眠等待msTime毫秒
    void Wakeup();      //唤醒
    void WakeupAll();   //唤醒所有
    void Join();        //等待线程取消
    
    void Lock();
    void Unlock();

    bool IsRunning();   //判断线程是否正在运行
    
private:
    std::thread *m_Thread = nullptr;
    std::mutex m_Lock;

    bool m_bIsRunning = false;
    std::condition_variable_any m_Condition;

    std::function<void()> m_ThreadFuncTask = nullptr;
};

ZJ_NAMESPACE_TOOL_END
#endif /* CThread_hpp */
