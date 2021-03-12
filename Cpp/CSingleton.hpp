//
//  CSingleton.hpp
//  ZJCrossCpp
//
//  Created by eafy on 2020/10/5.
//  Copyright © 2020 ZJ. All rights reserved.
//

#ifndef CSingleton_hpp
#define CSingleton_hpp

#include <stdio.h>
#include <pthread.h>
#include <memory>
#include "CDefType.hpp"

ZJ_NAMESPACE_BEGIN

class CSinGuard {
public:
    CSinGuard() { m_lGrdCnt = 0; pthread_mutex_init(&m_cs, nullptr); }
    ~CSinGuard() { pthread_mutex_destroy(&m_cs); }
    
    int IsGuarded() const { return (m_lGrdCnt > 0); }
    
public:
    class CGuard {
    public:
        CGuard(CSinGuard& rg):m_rg(rg) { m_rg.Guard(); };
        ~CGuard() { m_rg.Unguard(); }
        
    private:
        CSinGuard& m_rg;
    };
    
private:
    void Guard() { pthread_mutex_lock(&m_cs); m_lGrdCnt++; }    //引用+1
    void Unguard() { m_lGrdCnt--; pthread_mutex_unlock(&m_cs); }    //引用-1
    
    friend class CSinGuard::CGuard;
    
private:
    pthread_mutex_t m_cs;   //线程锁
    long m_lGrdCnt;     //引用计数
};

template <class T>
class CSingleton
{
public:
    static inline T* Instance();
    static inline void Release();
    
private:
    CSingleton(void) {}
    virtual ~CSingleton(void) {}
    CSingleton(const CSingleton&) {}
    CSingleton & operator= (const CSingleton &) {}
    
    static std::unique_ptr<T> m_Instance; //智能指针，会自动释放
    static CSinGuard m_sg;   //单例安全锁
};

//声明单例(对象)模板
template <class T>
std::unique_ptr<T> CSingleton<T>::m_Instance;

//声明单例对象安全锁模板
template <class T>
CSinGuard CSingleton<T>::m_sg;

//获取单例
template <class T>
inline T* CSingleton<T>::Instance()
{
    if (m_Instance.get() == nullptr) {
        CSinGuard::CGuard gd(m_sg);      //上锁，防止多线程同时访问
        if (m_Instance.get() == nullptr) {
            m_Instance.reset(new T);
        }
    }
    
    return m_Instance.get();
}

//释放单例
template <class T>
inline void CSingleton<T>::Release()
{
    CSinGuard::CGuard gd(m_sg);
    m_Instance.release();
}

#define DECLARE_SINGLETON_CLASS(Type) \
friend class std::unique_ptr<Type>; \
friend class CSingleton<Type>; \
static Type* Instance() { Type *shareClass = CSingleton<Type>::Instance(); return shareClass; } \
static void Release() { \
    Type *shareClass = CSingleton<Type>::Instance(); \
    if (shareClass) { \
        delete shareClass; \
        shareClass = nullptr; \
    } \
    CSingleton<Type>::Release(); \
}

ZJ_NAMESPACE_END
#endif /* CSingleton_hpp */
