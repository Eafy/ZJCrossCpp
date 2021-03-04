//
//  CGLTextureYUV.cpp
//  ZJCrossCpp
//
//  Created by eafy on 2020/10/14.
//  Copyright © 2020 ZJ. All rights reserved.
//

#ifndef CGLTextureYUV_hpp
#define CGLTextureYUV_hpp

#include <stdio.h>
#include "CDefType.hpp"
#if defined(__ANDROID__) || defined(ANDROID)    //Android
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#elif defined(__APPLE__)  //iOS
#include <OpenGLES/ES2/gl.h>
#include <OpenGLES/ES2/glext.h>
#endif

ZJ_NAMESPACE_TOOL_BEGIN

class CGLTextureYUV
{
public:
    CGLTextureYUV();
    ~CGLTextureYUV();

    bool isValid();

    GLuint m_Texture[3];
};

ZJ_NAMESPACE_TOOL_END
#endif /* CGLTextureYUV_hpp */
