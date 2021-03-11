//
//  CGLShaderYUV.cpp
//  ZJCrossCpp
//
//  Created by eafy on 2020/10/14.
//  Copyright © 2020 ZJ. All rights reserved.
//

#ifndef CGLShaderYUV_hpp
#define CGLShaderYUV_hpp

#include <stdio.h>
#include "CDefType.hpp"
#include "CGLShader.hpp"

ZJ_NAMESPACE_BEGIN

class CGLShaderYUV : public CGLShader
{
public:
    
    /*需要传入YUV着色器代码文件路径,loadAll:是否加载所有属性(默认加载)*/
    CGLShaderYUV(std::string filePath="", bool loadAll = true);
    ~CGLShaderYUV();
    
    bool UpdateAllUniforms();
    
public:
    GLint m_AttPosition;
    GLint m_AttTexcoord;

    GLint m_UniTexY;
    GLint m_UniTexU;
    GLint m_UniTexV;
    GLint m_UniModelViewMatrix3;
    GLint m_UniPreferredRotation;
    
    const GLfloat *m_pColorConversion;
};

ZJ_NAMESPACE_END
#endif /* CGLShaderYUV_hpp */
