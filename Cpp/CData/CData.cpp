//
//  CData.cpp
//  ZJCrossCpp
//
//  Created by eafy on 2021/07/22.
//  Copyright © 2021 ZJ. All rights reserved.
//

#include "CData.hpp"
#include <stdlib.h>
#include <string.h>

ZJ_NAMESPACE_BEGIN

#ifndef DeleteP
#define DeleteP(p) { if (p) delete p; p = nullptr; }
#endif

#ifndef DeleteA
#define DeleteA(a) { if (a) { delete[] a; a = nullptr; }}
#endif

CData::CData(uint64_t lenght) {
    m_pData = nullptr;
    m_nLenght = 0;
    
    if (lenght > 0) {
        m_pData = new char[lenght]();
        m_nLenght = lenght;
    }
}

CData::CData(char *data, uint64_t lenght) {
    m_pData = nullptr;
    m_nLenght = 0;
    
    if (data && lenght > 0) {
        m_pData = new char[lenght]();
        memcpy(m_pData, data, lenght);
        m_nLenght = lenght;
    } else {
        m_pData = NULL;
        m_nLenght = 0;
    }
}

CData::CData(unsigned char *data, u_int64_t lenght) {
    m_pData = nullptr;
    m_nLenght = 0;

    if (data && lenght > 0) {
        m_pData = new char[lenght]();
        memcpy(m_pData, data, lenght);
        m_nLenght = lenght;
    } else {
        m_pData = NULL;
        m_nLenght = 0;
    }
}

CData::CData(std::string data) {
    m_pData = nullptr;
    m_nLenght = 0;
    
    if (!data.empty()) {
        m_pData = new char[data.size()]();
        memcpy(m_pData, data.c_str(), data.size());
        m_nLenght = data.size();
    }
}

CData::~CData() {
    m_nLenght = 0;
    DeleteA(m_pData);
}

#pragma mark -

char *CData::Data(int length)
{
    if (length == 0) {
        return m_pData;
    }
    std::lock_guard<std::mutex> lk(m_Lock);

    unsigned long size = m_nLenght > length ? length : m_nLenght;
    char *buf = NULL;
    if (size > 0) {
        buf = new char[size]();
        memcpy(buf, m_pData, size);

        unsigned long dataSize = m_nLenght - size;
        if (dataSize > 0) {
            memcpy(m_pData, m_pData + size, dataSize);
        } else {
            DeleteA(m_pData);
        }
        m_nLenght = dataSize;
    }

    return buf;
}

void CData::clear()
{
    std::lock_guard<std::mutex> lk(m_Lock);
    
    m_nLenght = 0;
    DeleteA(m_pData);
}

bool CData::remove(uint64_t start, uint64_t lenght)
{
    std::lock_guard<std::mutex> lk(m_Lock);
    
    if (m_pData && start + lenght <= m_nLenght) {
        char *buf = new char[m_nLenght - lenght]();
        if (start > 0) {
            memcpy(buf, m_pData, start);
        }
        memcpy(buf + start, m_pData + start + lenght, m_nLenght - lenght - start);
        DeleteA(m_pData);
        m_pData = buf;
        m_nLenght = m_nLenght - lenght;
        
        return true;
    }
    
    return false;
}

#pragma mark -

bool CData::Insert(char *data, uint64_t start, uint64_t lenght, uint64_t index)
{
    std::lock_guard<std::mutex> lk(m_Lock);
    if (!data || !lenght || index > m_nLenght) return false;

    char *buf = new char[m_nLenght + lenght]();
    if (index == 0) {
    } else if (index < m_nLenght) {
        memcpy(buf, m_pData, index);
    } else if (m_nLenght > 0) {
        memcpy(buf, m_pData, m_nLenght);
        index = m_nLenght;
    }
    memcpy(buf + index, data + start, lenght);
    if (m_nLenght > index) {
        memcpy(buf + index + lenght, m_pData + index, m_nLenght - index);
    }

    DeleteA(m_pData);
    m_pData = buf;
    m_nLenght = m_nLenght + lenght;

    return true;
}

bool CData::Insert(char *data, uint64_t lenght) {
    return Insert(data, 0, lenght, m_nLenght);
}

bool CData::Insert(std::string str, uint64_t start, uint64_t lenght, uint64_t index) {
    return Insert((char *)str.c_str(), start, lenght, index);
}

bool CData::Insert(std::string str) {
    return Insert((char *)str.c_str(), 0, str.size(), m_nLenght);
}

#pragma mark -

bool CData::Replace(char ch, uint64_t index)
{
    if (index < m_nLenght && m_pData) {
        m_pData[index] = ch;
        return true;
    }

    return false;
}

#pragma mark -

CData *CData::operator + (const CData &dataT)
{
    std::lock_guard<std::mutex> lk(m_Lock);
    CData *data = new CData(m_nLenght+dataT.m_nLenght);
    if (m_pData && m_nLenght) {
        memcpy(data->m_pData, m_pData, m_nLenght);
    }
    memcpy(data->m_pData + m_nLenght, dataT.m_pData, dataT.m_nLenght);
    
    return data;
}

CData &CData::operator += (const CData &dataT)
{
    std::lock_guard<std::mutex> lk(m_Lock);
    if (dataT.m_nLenght > 0) {
        char *pData = new char[m_nLenght + dataT.m_nLenght]();
        if (m_pData && m_nLenght) {
            memcpy(pData, m_pData, m_nLenght);
            memcpy(pData+m_nLenght, dataT.m_pData, dataT.m_nLenght);
        }
        m_nLenght += dataT.m_nLenght;
        DeleteA(m_pData);
        m_pData = pData;
    }
    
    return *this;
}

CData &CData::operator += (const std::string &dataT)
{
    std::lock_guard<std::mutex> lk(m_Lock);
    
    if (!dataT.empty()) {
        char *pData = new char[m_nLenght + dataT.size()]();
        if (m_pData && m_nLenght) {
            memcpy(pData, m_pData, m_nLenght);
            memcpy(pData+m_nLenght, dataT.c_str(), dataT.size());
        }
        m_nLenght += dataT.size();
        DeleteA(m_pData);
        m_pData = pData;
    }
    
    return *this;
}

CData &CData::operator = (const CData & dataT)
{
    DeleteA(m_pData);
    m_nLenght = dataT.m_nLenght;
    if (m_nLenght > 0 && !m_pData) {
        m_pData = new char[m_nLenght]();
        memcpy(m_pData, dataT.m_pData, m_nLenght);
    }

    return *this;
}

#pragma mark -

std::string CData::Printf(bool bPrintf)
{
    if (!m_pData || m_nLenght <= 0) return "";

    std::string str = "";
    char buffer[4];
    for (int i=0; i<m_nLenght; i++) {
        snprintf(buffer, 4, "%02X ", m_pData[i] & 0xFF);
        str += std::string(buffer);
    }
    if (bPrintf) {
        printf("%s", str.c_str());
    }

    return str;
}

ZJ_NAMESPACE_END
