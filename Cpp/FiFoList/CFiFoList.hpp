//
//  CFiFoList.hpp
//  ZJCrossCpp
//
//  Created by eafy on 2020/10/10.
//  Copyright © 2020 ZJ. All rights reserved.
//

#ifndef CFiFoList_hpp
#define CFiFoList_hpp

#include <stdio.h>
#include <mutex>
#include <string>
#include <deque>
#include "CDefType.hpp"

ZJ_NAMESPACE_BEGIN

template<class T>
class CFiFoNode
{
public:
    CFiFoNode() {}
    
    /// 使用数据实例化节点
    /// @param pBuffer 数据源
    /// @param size 数据源大小
    template<class T1 = char, unsigned char>
    CFiFoNode(T1 *pBuffer, uint64_t size) {
        if (pBuffer && size > 0) {
            m_nSize = size;
            m_pBuffer = new uint8_t[size]();
            memcpy(m_pBuffer, pBuffer, m_nSize);
        }
    }
    
    ~CFiFoNode() {
        if (m_pBuffer) {
            delete [] m_pBuffer;
            m_pBuffer = nullptr;
        }
        m_nSize = 0;
    }
    
    /// 获取数据地址
    uint8_t *Buffer() { return m_pBuffer; }
    
    /// 获取数据大小
    uint64_t Size() { return m_nSize; }
    
    /// 设置下一个节点地址
    /// @param p 节点
    void SetNext(T *p) { m_pNext = p; }
    
    /// 获取下一个节点
    T *Next() { return m_pNext; }
    
protected:
    uint8_t *m_pBuffer = nullptr;  //数据源
    uint64_t m_nSize = 0;
    T *m_pNext = nullptr;
};

template<class T>
class CFiFoList
{
public:
    CFiFoList() {}
    ~CFiFoList() {
        Clear();
    }

    /// 添加节点
    /// @param pBuffer 数据
    /// @param size 大小
    T *Put(char *pBuffer, uint32_t size) {
        std::lock_guard<std::mutex> lk(m_Lock);
        T *pNode = new T(pBuffer, size);

        if (m_pFirst == nullptr) {
            m_pLast = m_pFirst = pNode;
        } else {
            m_pLast->SetNext(pNode);    //重置下一个节点
            m_pLast = pNode;
        }

        m_nDepth ++;
        m_nSize += size;

        return pNode;
    }
    
    /// 添加节点（直接链接）
    /// @param pNode 节点地址
    uint64_t Put(T *pNode) {
        if (!pNode)
             return m_nDepth;
         std::lock_guard<std::mutex> lk(m_Lock);
         if (m_pFirst == nullptr) {
             m_pLast = m_pFirst = pNode;
         } else {
             m_pLast->SetNext(pNode);    //重置下一个节点
             m_pLast = pNode;
         }

         m_nDepth ++;
         m_nSize += pNode->Size();

         return m_nDepth;
    }
    
    /// 获取节点（需要手动释放）
    T *Get() {
        std::lock_guard<std::mutex> lk(m_Lock);
        T *pNode = m_pFirst;
        if (!pNode) return nullptr;

        m_pFirst = pNode->Next();
        m_nDepth--;
        m_nSize -= pNode->Size();

        if(m_pFirst == nullptr) {
            m_pLast = nullptr;
        }

        pNode->SetNext(nullptr);

        return pNode;
    }
    
    /// 清除所有节点
    void Clear() {
        std::lock_guard<std::mutex> lk(m_Lock);
          T *pNode = m_pFirst;
          T *pNext = nullptr;
          for (; pNode;) {
              pNext = pNode->Next();
              delete pNode;
              pNode = pNext;
          }

          m_pLast = m_pFirst = nullptr;
          m_nDepth = 0;
          m_nSize = 0;
    }
    
    /// 保留多少节点数
    /// @param count 保留的节点数据长度
    void Keep(int count) {
        std::lock_guard<std::mutex> lk(m_Lock);
        T *pNode = m_pFirst;
        T *pNext = nullptr;
        for (; pNode;) {
            pNext = pNode->Next();

            if (!pNode || m_nDepth <= count) {
                m_pFirst = pNode;
                break;
            } else {
                m_nSize -= pNode->Size();
                m_nDepth --;
                delete pNode;
                pNode = pNext;
            }
        }
    }

    /// 获取列表的总数据量大小
    uint64_t Size() { return m_nSize; }

    /// 获取列表的个数
    uint64_t Count() { return m_nDepth; }
    
protected:
    std::mutex m_Lock;
    T *m_pFirst = nullptr;    //首节点
    T *m_pLast = nullptr;   //末节点地址

    uint64_t m_nDepth = 0;     //节点深度
    uint64_t m_nSize = 0;      //列表数据总大小
};

ZJ_NAMESPACE_END
#endif /* CFiFoList_hpp */
