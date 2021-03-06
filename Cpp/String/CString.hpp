//
//  CString.hpp
//  ZJCrossCppDemo
//
//  Created by eafy on 2020/10/2.
//  Copyright © 2021 ZJ. All rights reserved.
//

#ifndef CString_hpp
#define CString_hpp

#include <stdio.h>
#include <string>
#include <vector>
#include <sstream>
#include "CDefType.hpp"

ZJ_NAMESPACE_BEGIN

class CString {
public:
    /// utf8 编码
    /// @param str 字符串
    static std::string EncodeToUTF8(const std::string &str);
    
    ///  utf8 解码
    /// @param str 字符串
    static std::string DecodeFromUTF8(const std::string &str);
    
    /// gb2312 编码
    /// @param str 字符串
    static std::string EncodeToGB2312(const std::string &str);
    
    /// gb2312 解码
    /// @param str 字符串
    static std::string DecodeFromGB2312(const std::string &str);
    
    /// 替换字符串
    /// @param str 原始字符串
    /// @param old_value 被替换值(老字符串)
    /// @param new_value 需替换值(新字符串)
    static std::string& ReplaceAll(std::string& str, const std::string& old_value, const std::string& new_value);
    
    /// 替换字符串(替换第一个)
    /// @param str 原始字符串
    /// @param old_value 被替换值(老字符串)
    /// @param new_value 需替换值(新字符串)
    static std::string& ReplaceFirst(std::string& str, const std::string& old_value, const std::string& new_value);

    /// 获取随机字符串
    /// @param count 字符串个数
    static std::string GetRandomStr(int count);

    /// 分割字符串
    /// @param str 需要分割的字符串
    /// @param pattern 分割字符串标识
    static std::vector<std::string> Split(const std::string &str, const std::string &pattern);
    
#pragma - File Path
    /// 路径上添加下一级路径
    /// @param path 路径
    /// @param component 下一级组成部分名称
    static std::string AppendComponentForPath(const std::string path, const std::string component);
    
    /// 获取路径的最后组成部分
    /// @param path 文件路径
    static std::string LastPathComponent(const std::string path);
    
    /// 移除最后一个路径的组成部分
    /// @param path 文件路径
    /// @param lastComName 获取最后一个组成部分的名称
    static std::string RemoveLastComponentForPath(const std::string path, std::string &lastComName);
};

ZJ_NAMESPACE_END

#endif /* CString_hpp */
