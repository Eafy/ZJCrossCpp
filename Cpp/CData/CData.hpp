//
//  CData.hpp
//  ZJCrossCpp
//
//  Created by eafy on 2021/07/22.
//  Copyright © 2021 ZJ. All rights reserved.
//

#ifndef CData_hpp
#define CData_hpp

#include <stdio.h>
#include <mutex>
#include <string>
#include "CDefType.hpp"

ZJ_NAMESPACE_BEGIN

class CData
{
public:
    CData(){};
    CData(uint64_t lenght);
    CData(char *data, u_int64_t lenght);
    CData(unsigned char *data, u_int64_t lenght);
    CData(std::string data);
    CData(const CData &c) {
        m_nLenght = c.m_nLenght;
        if (m_nLenght > 0 && !m_pData) {
            m_pData = new char[m_nLenght]();
            memcpy(m_pData, c.m_pData, m_nLenght);
        }
    }
    ~CData();
    
    char *Begin() { return m_pData; }
    char *End() { return m_pData + m_nLenght; }
    char *Data(int length = 0);  //获取length>0长度的数据，需要手动释放
    uint64_t Size() { return m_nLenght; }

    /// 删除数据区数据
    void clear();

    /// 删除数据区数据
    /// @param start 数据区需要删除的起始位置
    /// @param lenght 需要删除的长度
    bool remove(uint64_t start, uint64_t lenght);

    /// 打印数据区
    /// @param bPrintf 是否打印
    /// @return 返还数据区十六进制字符串
    std::string Printf(bool bPrintf = true);

    template <class T>
    /// 向数据区插入基础数据类型数据
    /// @param index 数据区的起始位置
    /// @param bits 基础数据类型占用的字节大小
    bool Push(T value, uint64_t index = 0, unsigned int bits = 0) {
        if (!bits) bits = sizeof(T);
        char *temp = new char[bits]();
        for (int i=0; i<bits; i++) {
            temp[i] = (value >> ((bits - i - 1) * 8)) & 0xFF;
        }
        bool ret = Insert(temp, 0, bits, index);

        delete [] temp;
        temp = nullptr;
        return ret;
    }

    template <class T>
    /// 向数据区末尾插入基础数据类型数据
    /// @param bits 基础数据类型占用的字节大小
    bool PushBack(T value, unsigned char bits = 0) {
        return Push<T>(value, m_nLenght, bits);
    }
    bool PushBack(std::string str) {
        return Insert(str, 0, str.length(), m_nLenght);
    }
    bool PushBack(char *ch, int length) {
        return Insert(ch, 0, length, m_nLenght);
    }
    bool PushBack(const char *ch, int length) {
        if (!ch || length <= 0) {
            return false;
        }
        return Insert(ch, 0, length, m_nLenght);
    }

    template <class T>
    /// 向数据区首位插入基础数据类型数据
    /// @param bits 基础数据类型占用的字节大小
    bool PushFront(T value, unsigned char bits = 0) {
        return Push<T>(value, 0, bits);
    }

    /**
    向数据区插入数组或字符串

    @param data 新增的数据
    @param start 新增数据的需要插入起始位置
    @param lenght  新增数据的需要插入的长度
    @param index  数据区的起始位置
    @return 是否插入成功
    */
    bool Insert(char *data, uint64_t start, uint64_t lenght, uint64_t index = 0);
    bool Insert(char *data, uint64_t lenght);
    bool Insert(std::string str, uint64_t start, uint64_t lenght, uint64_t index = 0);
    bool Insert(std::string str);

    /// 替换数据中某个值
    /// @param ch 数据值
    /// @param index 索引
    bool Replace(char ch, uint64_t index);
    
public:
    CData *operator + (const CData &dataT);
    CData &operator += (const CData &dataT);
    CData &operator += (const std::string &dataT);
    CData &operator = (const CData &dataT);
    
private:
    std::mutex m_Lock;
    
    char *m_pData = nullptr;
    uint64_t m_nLenght = 0;
};

ZJ_NAMESPACE_END
#endif /* CData_hpp */
