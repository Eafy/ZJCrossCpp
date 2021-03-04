//
//  CGLVideoFBO.cpp
//  ZJCrossCpp
//
//  Created by eafy on 2020/10/14.
//  Copyright © 2020 ZJ. All rights reserved.
//

#ifndef CGLVideoFBO_hpp
#define CGLVideoFBO_hpp

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

class CGLVideoFBO
{
public:
    enum AttribEnum {
        ATTRIB_VERTEX,
        ATTRIB_TEXCOORD,
        NUM_ATTRIBUTES
    };
    
public:
    CGLVideoFBO();
    ~CGLVideoFBO();
    
    bool BindBuffers();
    GLuint GetRenderBufferID() { return m_RenderBufferColor; };
    
private:
    GLuint m_RenderBufferColor = 0;
    GLuint m_FrameBuffer = 0;
    
    void SetupBuffers();
    
public:
    int m_FboWidth = 0;
    int m_FboHeight = 0;
};

ZJ_NAMESPACE_TOOL_END
#endif /* CGLVideoFBO_hpp */
