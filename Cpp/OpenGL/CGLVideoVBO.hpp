//
//  CGLVideoVBO.cpp
//  ZJCrossCpp
//
//  Created by eafy on 2020/10/14.
//  Copyright © 2020 ZJ. All rights reserved.
//

#ifndef CGLVideoVBO_hpp
#define CGLVideoVBO_hpp

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

class CGLVideoVBO
{
public:
    CGLVideoVBO(GLuint position = 0, GLuint texcoord = 0);
    ~CGLVideoVBO();
    
    void SetAttribute(GLuint position, GLuint texcoord);    //设置VAO属性
    bool isValid(); //VBO和VAO是否能设置有效
    void Update();  //更新VAO
    
private:
    GLuint m_VBOHandle = 0;
    GLuint m_AttPosition = 0;
    GLuint m_AttTexcoord = 0;
};
    
ZJ_NAMESPACE_TOOL_END
#endif /* CGLVideoVBO_hpp */
