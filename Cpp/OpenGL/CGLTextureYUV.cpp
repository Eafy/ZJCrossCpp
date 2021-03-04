//
//  CGLTextureYUV.cpp
//  ZJCrossCpp
//
//  Created by eafy on 2020/10/14.
//  Copyright © 2020 ZJ. All rights reserved.
//

#include "CGLTextureYUV.hpp"
#include "CLog.hpp"
#include <string>

ZJ_NAMESPACE_TOOL_BEGIN

CGLTextureYUV::CGLTextureYUV()
{
    memset(m_Texture, 0, sizeof(GLuint)*3);
    
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    
    glGenTextures(3, m_Texture);  //生成纹理
    
    if (!m_Texture[0] || !m_Texture[1] || !m_Texture[2]) {
        CLogW("Failed to create gl textures");
        return;
    }
    
    for (int i=0; i<3; i++) {
        //选择当前活跃单元
        glActiveTexture(GL_TEXTURE0 + i);
        //绑定Y纹理
        glBindTexture(GL_TEXTURE_2D, m_Texture[i]);
        //纹理过滤函数
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);//放大过滤
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);//缩小过滤
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);//水平方向
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);//垂直方向
    }

    CLogD("Success to bing yuv texture");
}

CGLTextureYUV::~CGLTextureYUV()
{
    if (m_Texture[0]) {
        glDeleteTextures(3, m_Texture);   //删除纹理
        memset(m_Texture, 0, sizeof(GLuint)*3);
    }
}

bool CGLTextureYUV::isValid()
{
    if (!m_Texture[0] || !m_Texture[1] || !m_Texture[2]) {
        return false;
    }

    return true;
}

ZJ_NAMESPACE_TOOL_END
