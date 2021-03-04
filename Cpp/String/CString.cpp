//
//  CString.cpp
//  ZJCrossCppDemo
//
//  Created by lzj on 2021/3/4.
//

#include "CString.hpp"
#ifdef _WIN32
#include <windows.h>
#endif

#if defined(__ANDROID__) || defined(ANDROID) || defined(__APPLE__)
#define _arc4random() arc4random()
#else
#define _arc4random() rand()
#endif

ZJ_NAMESPACE_TOOL_BEGIN


void UTF8ToUnicode(wchar_t* pOut, const char *pText)
{
    char* uchar = (char *)pOut;
    uchar[1] = ((pText[0] & 0x0F) << 4) + ((pText[1] >> 2) & 0x0F);
    uchar[0] = ((pText[1] & 0x03) << 6) + (pText[2] & 0x3F);
    return;
}

void UnicodeToUTF8(char* pOut, const wchar_t* pText)
{
    // 注意 WCHAR高低字的顺序,低字节在前，高字节在后
    const char* pchar = (const char *)pText;
    pOut[0] = (0xE0 | ((pchar[1] & 0xF0) >> 4));
    pOut[1] = (0x80 | ((pchar[1] & 0x0F) << 2)) + ((pchar[0] & 0xC0) >> 6);
    pOut[2] = (0x80 | (pchar[0] & 0x3F));
    return;
}

char CharToInt(char ch)
{
    if (ch >= '0' && ch <= '9')return (char)(ch - '0');
    if (ch >= 'a' && ch <= 'f')return (char)(ch - 'a' + 10);
    if (ch >= 'A' && ch <= 'F')return (char)(ch - 'A' + 10);
    return -1;
}

char StrToBin(const char *str)
{
    char tempWord[2];
    char chn;
    tempWord[0] = CharToInt(str[0]);    //make the B to 11 -- 00001011
    tempWord[1] = CharToInt(str[1]);    //make the 0 to 0 -- 00000000
    chn = (tempWord[0] << 4) | tempWord[1];     //to change the BO to 10110000
    return chn;
}

#if defined(_WIN32)

void UnicodeToGB2312(char* pOut, wchar_t uData)
{
    WideCharToMultiByte(CP_ACP, NULL, &uData, 1, pOut, sizeof(wchar_t), NULL, NULL);
}
void Gb2312ToUnicode(wchar_t* pOut, const char *gbBuffer)
{
    MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, gbBuffer, 2, pOut, 1);
}

#endif

#pragma mark -

std::string CString::EncodeToUTF8(const std::string &str) {
    std::string output;
    size_t len = str.size();
    for (size_t i = 0; i < len; i++) {
        if (isalnum((uint8_t)str[i])) {
            char tempbuff[2];
            sprintf(tempbuff, "%c", str[i]);
            output.append(tempbuff);
        } else if (isspace((uint8_t)str[i])) {
            output.append("+");
        } else {
            char tempbuff[4];
            sprintf(tempbuff, "%%%X%X", (uint8_t)str[i] >> 4,(uint8_t)str[i] % 16);
            output.append(tempbuff);
        }
    }
    
    return output;
}

std::string CString::DecodeFromUTF8(const std::string &str)
{
    std::string output = "";
    char tmp[2];
    unsigned long i = 0;
    unsigned long len = str.length();
    while (i < len) {
        if (str[i] == '%') {
            tmp[0] = str[i + 1];
            tmp[1] = str[i + 2];
            output += StrToBin(tmp);
            i = i + 3;
        } else if (str[i] == '+') {
            output += ' ';
            i++;
        } else {
            output += str[i];
            i++;
        }
    }
    return output;
}

#ifndef _WIN32
std::string CString::EncodeToGB2312(const std::string &str)
{
    std::string output;
    size_t len = str.size();
    for (size_t i = 0; i<len; i++) {
        if (isalnum((uint8_t)str[i])) {
            char tempbuff[2];
            sprintf(tempbuff, "%c", str[i]);
            output.append(tempbuff);
        } else if (isspace((uint8_t)str[i])) {
            output.append("+");
        } else {
            char tempbuff[4];
            sprintf(tempbuff, "%%%X%X", (uint8_t)str[i] >> 4, (uint8_t)str[i] % 16);
            output.append(tempbuff);
        }
    }
    return output;
}

std::string CString::DecodeFromGB2312(const std::string &str)
{
    std::string output = "";
    char tmp[2];
    unsigned long i = 0;
    unsigned long len = str.length();
    while (i<len) {
        if (str[i] == '%') {
            tmp[0] = str[i + 1];
            tmp[1] = str[i + 2];
            output += StrToBin(tmp);
            i = i + 3;
        }
        else if (str[i] == '+') {
            output += ' ';
            i++;
        }
        else {
            output += str[i];
            i++;
        }
    }
    return output;
}

#else

std::string CString::EncodeToGB2312(const std::string &str) {
    auto len = str.size();
    auto pText = str.data();
    char Ctemp[4] = {0};
    char *pOut = new char[len + 1];
    memset(pOut, 0, len + 1);

    int i = 0, j = 0;
    while (i < len) {
        if (pText[i] >= 0) {
            pOut[j++] = pText[i++];
        } else {
            wchar_t Wtemp;
            UTF8ToUnicode(&Wtemp, pText + i);
            UnicodeToGB2312(Ctemp, Wtemp);
            pOut[j] = Ctemp[0];
            pOut[j + 1] = Ctemp[1];
            i += 3;
            j += 2;
        }
    }
    string ret = pOut;
    delete[] pOut;
    return ret;
}

std::string CString::DecodeFromGB2312(const std::string &str) {
    auto len = str.size();
    auto pText = str.data();
    char buf[4] = { 0 };
    int nLength = len * 3;
    char* pOut = new char[nLength];
    memset(pOut, 0, nLength);
    int i = 0, j = 0;
    while (i < len) {
        if (*(pText + i) >= 0) {    //英文直接复制
            pOut[j++] = pText[i++];
        } else {
            wchar_t pbuffer;
            Gb2312ToUnicode(&pbuffer, pText + i);
            UnicodeToUTF8(buf, &pbuffer);
            pOut[j] = buf[0];
            pOut[j + 1] = buf[1];
            pOut[j + 2] = buf[2];
            j += 3;
            i += 2;
        }
    }
    string ret = pOut;
    delete[] pOut;
    return ret;
}
#endif

#pragma mark -

std::string& CString::ReplaceAll(std::string& str, const std::string& old_value, const std::string& new_value)
{
    while(true) {
        std::string::size_type pos(0);
        if ((pos = str.find(old_value)) != std::string::npos)
            str.replace(pos, old_value.length(), new_value);
        else
            break;
    }

    return str;
}

std::string& CString::ReplaceFirst(std::string& str, const std::string& old_value, const std::string& new_value)
{
    std::string::size_type pos = str.find(old_value);
    if (pos != std::string::npos) {
        str.replace(pos, old_value.length(), new_value);
    }

    return str;
}

std::string CString::GetRandomStr(int count)
{
    char *data = new char[count + 1]();
    for (int i=0; i<count; i++) {
#if !defined(__ANDROID__) && !defined(ANDROID) && !defined(__APPLE__)
        srand(time(NULL));
#endif
        int type = _arc4random() % 3;
        if (type == 0) {    //整形
            data[i] = (char)('0' + (_arc4random() % 9) + 1);
        } else if (type == 1) {     //小写字母
            data[i] = (char)('a' + (_arc4random() % 25) + 1);
        } else {    //大写字母
            data[i] = (char)('A' + (_arc4random() % 25) + 1);
        }
    }

    return std::string(data);
}

std::vector<std::string> CString::Split(const std::string &str, const std::string &pattern)
{
    std::vector<std::string> resultVec;
    std::string::size_type pos1 = 0;
    std::string::size_type pos2 = str.find(pattern);

    while(std::string::npos != pos2) {
        resultVec.push_back(str.substr(pos1, pos2-pos1));
        pos1 = pos2 + pattern.size();
        pos2 = str.find(pattern, pos1);
    }
    if (pos1 != str.length()) {
        resultVec.push_back(str.substr(pos1));
    }

    return resultVec;
}


ZJ_NAMESPACE_TOOL_END
