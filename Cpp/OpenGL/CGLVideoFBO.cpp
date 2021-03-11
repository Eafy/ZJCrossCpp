//
//  CGLVideoFBO.cpp
//  ZJCrossCpp
//
//  Created by eafy on 2020/10/14.
//  Copyright © 2020 ZJ. All rights reserved.
//

#include "CGLVideoFBO.hpp"

ZJ_NAMESPACE_BEGIN

CGLVideoFBO::CGLVideoFBO()
{
    glDisable(GL_DEPTH_TEST);
    
    glEnableVertexAttribArray(ATTRIB_VERTEX);
    glVertexAttribPointer(ATTRIB_VERTEX, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), 0);
    
    glEnableVertexAttribArray(ATTRIB_TEXCOORD);
    glVertexAttribPointer(ATTRIB_TEXCOORD, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), 0);
    
    SetupBuffers();
}

CGLVideoFBO::~CGLVideoFBO()
{
    if (m_FrameBuffer) {
        glDeleteFramebuffers(1, &m_FrameBuffer);
        m_FrameBuffer = 0;
    }
    
    if (m_RenderBufferColor) {
        glDeleteRenderbuffers(1, &m_RenderBufferColor);
        m_RenderBufferColor = 0;
    }
}

void CGLVideoFBO::SetupBuffers()
{
    glDisable(GL_DEPTH_TEST);
    
    glEnableVertexAttribArray(ATTRIB_VERTEX);
    glVertexAttribPointer(ATTRIB_VERTEX, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), 0);
    
    glEnableVertexAttribArray(ATTRIB_TEXCOORD);
    glVertexAttribPointer(ATTRIB_TEXCOORD, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), 0);
    
    glGenFramebuffers(1, &m_FrameBuffer);                  //创建帧缓冲区
    glBindFramebuffer(GL_FRAMEBUFFER, m_FrameBuffer);      //绑定帧缓冲区到渲染管线

    glGenRenderbuffers(1, &m_RenderBufferColor);                 //创建绘制缓冲区
    glBindRenderbuffer(GL_RENDERBUFFER, m_RenderBufferColor);    //绑定绘制缓冲区到渲染管线
}

bool CGLVideoFBO::BindBuffers()
{
    if (m_RenderBufferColor > 0) {
        glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_WIDTH, &m_FboWidth);   //获取绘制缓冲区的像素宽度
        glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_HEIGHT, &m_FboHeight); //获取绘制缓冲区的像素宽度
        
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, m_RenderBufferColor);   //绑定绘制缓冲区到帧缓冲区
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
            CLogW("Failed to make complete framebuffer object %x", glCheckFramebufferStatus(GL_FRAMEBUFFER));
            return false;
        }
    } else {
        CLogW("Failed to bind render color buffer:%d", m_RenderBufferColor);
        return false;
    }
    
    return true;
}

ZJ_NAMESPACE_END
