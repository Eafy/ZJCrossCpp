//
//  CBaseThreadPool.hpp
//  ZJCrossCpp
//
//  Created by eafy on 2020/10/5.
//  Copyright © 2020 ZJ. All rights reserved.
//

#ifndef CBaseThreadPool_hpp
#define CBaseThreadPool_hpp

#include <stdio.h>
#include "CDefType.hpp"
#include <functional>
#include <string>

ZJ_NAMESPACE_BEGIN
               
class CCondition;

class CThreadTask
{
public:
    CThreadTask(){};
    ~CThreadTask() {
        if (m_pData) {
            delete [] m_pData;
            m_pData = nullptr;
        }
        m_nSize = 0;
    }
    
public:     //外部访问
    //数据类
    std::string m_cMsgName = "";     //消息名称
    char *m_pData = nullptr;        //消息内容（存数据类型）
    uint64_t m_nSize = 0;           //消息大小
    std::string m_sMsgContent = "";       //消息内容（存字符串类型）
    int64_t m_nMsgTag = 0;       //消息标签
    
public:     //内部使用
    bool m_bIsRunning = false;          //正在运行
    void *m_pDispatcher = nullptr;      //发送者
    uint64_t m_nTag = 0;                //任务标签
    std::function<void()> m_Func;   //函数指针，需要执行的任务
    
    CThreadTask *GetNextTask() { return m_pNext; }
    void SetNextTask(CThreadTask *task) { m_pNext = task; }
private:
    CThreadTask *m_pNext = nullptr;         //内部处理，外部请勿使用
};
               
class CBaseThreadPool
{
public:
    CBaseThreadPool(u_int32_t nThreads = 32);   //初始化线程池(默认线程池最大64个线程)
    virtual ~CBaseThreadPool();
    
    virtual void CancelAll();
    virtual void Reset() { m_bQuit = false; }
    
    void Lock();
    void Unlock();
    CThreadTask *GetTask() { return m_pTaskFirst; }
    void SetTask(CThreadTask *task) { m_pTaskLast = task; }
    bool IsExit() { return m_bQuit; }
    
protected:
    void _AddTask(CThreadTask *task);
    virtual void _RunOperate(CThreadTask *task){};       //执行任务的操作
    virtual void _CancelPreOperate(CThreadTask *task){};    //删除之前的操作
    virtual void _DeleteTask(CThreadTask *task);    //删除任务的内存，部分继承了JMThreadTask的类需要重写此接口
    
private:
    static void *didRoutineThread(void *arg);
    
    int m_MaxThread = 32;
    int m_ThreadCount;                //线程池中已有线程数
    int m_ThreadIdle;                 //线程池中空闲线程数
    bool m_bQuit;                //是否退出标志
    CThreadTask *m_pTaskFirst = nullptr;           //任务队列中第一个任务
    CThreadTask *m_pTaskLast = nullptr;            //任务队列中最后一个任务
    CCondition *m_TrigCondition = nullptr;        //状态量
};
            
ZJ_NAMESPACE_END
#endif /* CBaseThreadPool_hpp */
