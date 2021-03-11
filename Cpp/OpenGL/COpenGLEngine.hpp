//
//  COpenGLEngine.hpp
//  ZJCrossCpp
//
//  Created by eafy on 2020/10/14.
//  Copyright © 2020 ZJ. All rights reserved.
//

#ifndef COpenGLEngine_hpp
#define COpenGLEngine_hpp

#include <stdio.h>
#include "CDefType.hpp"
#if defined(__ANDROID__) || defined(ANDROID)    //Android
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#elif defined(__APPLE__)  //iOS
#include <OpenGLES/ES2/gl.h>
#include <OpenGLES/ES2/glext.h>
#endif

ZJ_NAMESPACE_BEGIN

class CGLShaderYUV;
class CGLTextureYUV;
class CGLVideoVBO;

class COpenGLEngine
{
public:
    COpenGLEngine();
    ~COpenGLEngine();

    bool isValid();
    void LoadYUVData(int width, int height, const unsigned char *y, const unsigned char *u, const unsigned char *v);
    void Render(int width = -1, int height = -1);   //窗口的大小
    void UpdateSize(int width, int height);     //窗口的大小
    
public:
    CGLTextureYUV *m_pTextureYUV = nullptr;
    CGLShaderYUV *m_pShaderYUV = nullptr;
    CGLVideoVBO *m_pVideoVBO = nullptr;
    
private:
    int m_ViewWidth = 0;
    int m_ViewHeight = 0;
};

ZJ_NAMESPACE_END
#endif /* COpenGLEngine_hpp */
