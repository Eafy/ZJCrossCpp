//
//  CDefType.h
//  ZJCrossCpp
//
//  Created by eafy on 2020/10/4.
//  Copyright © 2020 ZJ. All rights reserved.
//

#ifndef CDefType_h
#define CDefType_h

#ifndef DeleteP
#define DeleteP(p) { if (p) delete p; p = nullptr; }
#endif

#ifndef DeleteA
#define DeleteA(a) { if (a) { delete[] a; a = nullptr; }}
#endif

#ifndef FreeC
#define FreeC(a) { if (a) { free(a); a = nullptr; }}
#endif

#if defined(__ANDROID__) || defined(ANDROID)    //Android
#define IS_ANDROID_PLATFORM 1
#define IS_IOS_PLATFORM 0
#elif defined(__APPLE__)  //iOS
#define IS_ANDROID_PLATFORM 0
#define IS_IOS_PLATFORM 1
#else
#define IS_ANDROID_PLATFORM 0
#define IS_IOS_PLATFORM 0
#endif

#define ZJ_NAMESPACE_BEGIN \
namespace ZJ {
#define ZJ_NAMESPACE_END \
};  /*ZJ*/

#define CLock(a) std::lock_guard<std::mutex> zjlk(a);

#include <ZJLog/ZJPrintfLog.h>

#endif /* CDefType_h */
