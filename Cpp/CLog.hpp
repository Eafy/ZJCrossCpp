//
//  CLog.hpp
//  ZJCrossCpp
//
//  Created by eafy on 2020/10/2.
//  Copyright © 2020 ZJ. All rights reserved.
//

#ifndef CLog_hpp
#define CLog_hpp

#include <stdio.h>
#include <string>

#if defined(__ANDROID__) || defined(ANDROID)    //Android

#include <android/log.h>
#define LOG_TAG "ZJLog"

extern void Java_com_jimi_jmlog_JMlogJni_PrintAndroid(int prio, const char* tag, const char* fmt, ...);

#define CLog(...) Java_com_eafy_zjlog_ZJlogJni_PrintAndroid(ANDROID_LOG_DEBUG,LOG_TAG,__VA_ARGS__)
#define CLogV(...) Java_com_eafy_zjlog_ZJlogJni_PrintAndroid(ANDROID_LOG_VERBOSE,LOG_TAG,__VA_ARGS__)
#define CLogD(...) Java_com_eafy_zjlog_ZJlogJni_PrintAndroid(ANDROID_LOG_DEBUG,LOG_TAG,__VA_ARGS__)
#define CLogI(...) Java_com_eafy_zjlog_ZJlogJni_PrintAndroid(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)
#define CLogW(...) Java_com_eafy_zjlog_ZJlogJni_PrintAndroid(ANDROID_LOG_WARN,LOG_TAG,__VA_ARGS__)
#define CLogE(...) Java_com_eafy_zjlog_ZJlogJni_PrintAndroid(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)
#define CLogLine() CPrintfE("%s---->%d", __func__, __LINE__)

#elif defined(__APPLE__)  //iOS

#include <ZJLog/ZJPrintfLog.h>

#ifndef CLog
#define CLog CPrintf
#define CLogV CPrintfV
#define CLogD CPrintfD
#define CLogI CPrintfI
#define CLogW CPrintfW
#define CLogE CPrintfE
#define CLogLine() CPrintfLine
#endif

#else

#ifndef CLog
#define CLog printf
#define CLogV printf
#define CLogD printf
#define CLogI printf
#define CLogW printf
#define CLogE printf
#define CLogLine() printf("%s---->%d", __func__, __LINE__)
#endif

#endif  //end


#endif /* CLog_hpp */
