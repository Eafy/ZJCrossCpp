//
//  COpenGLEngine.cpp
//  ZJCrossCpp
//
//  Created by eafy on 2020/10/14.
//  Copyright © 2020 ZJ. All rights reserved.
//

#include "COpenGLEngine.hpp"
#include "CGLTextureYUV.hpp"
#include "CGLShaderYUV.hpp"
#include "CGLVideoVBO.hpp"

ZJ_NAMESPACE_BEGIN

COpenGLEngine::COpenGLEngine()
{
    const GLubyte *vendor = glGetString(GL_VENDOR);
    const GLubyte *renderer = glGetString(GL_RENDERER);
    const GLubyte *version = glGetString(GL_VERSION);
    const GLubyte *glslVersion = glGetString(GL_SHADING_LANGUAGE_VERSION);
    
    CLogI("Graphics card supplier: %s", vendor);
    CLogI("Graphics card model: %s", renderer);
    CLogI("OpenGL version: %s", version);
    CLogI("GLSL version: %s", glslVersion);
    
    m_pTextureYUV = new CGLTextureYUV();
    m_pShaderYUV = new CGLShaderYUV();
//    m_pVideoFBO  = new JMGLVideoFBO();    //非OpenGL视图需要申请内存
    m_pVideoVBO = new CGLVideoVBO(m_pShaderYUV->m_AttPosition, m_pShaderYUV->m_AttTexcoord);
}

COpenGLEngine::~COpenGLEngine()
{
    if (m_pTextureYUV) {
        delete m_pTextureYUV;
        m_pTextureYUV = nullptr;
    }
    if (m_pShaderYUV) {
        delete m_pShaderYUV;
        m_pShaderYUV = nullptr;
    }
    if (m_pVideoVBO) {
        delete m_pVideoVBO;
        m_pVideoVBO = nullptr;
    }
}

bool COpenGLEngine::isValid()
{
    if (m_pTextureYUV->isValid() && m_pShaderYUV->isValid() && m_pVideoVBO->isValid()) {
        return true;
    }

    return false;
}

void COpenGLEngine::Render(int width, int height)
{
    if (!m_pShaderYUV || !m_pShaderYUV->isValid()) {
        return;
    }
    UpdateSize(width, height);
    
    if (m_pVideoVBO && m_pVideoVBO->isValid()) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        m_pVideoVBO->Update();
        
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        
        glViewport(0, 0, m_ViewWidth, m_ViewHeight);
    }
}

void COpenGLEngine::LoadYUVData(int width, int height, const unsigned char *y, const unsigned char *u, const unsigned char *v)
{
    if (width <= 0 || height <= 0 || !y || !u || !v) {
        return;
    }
    
    for (int i = 0; i < 3; ++i) {
        int nw = width;
        int nh = height;
        
        const unsigned char *p = y;
        if (1 == i) {
            p = u;
            nw = width/2;
            nh = height/2;
        } else if (2 == i) {
            p = v;
            nw = width/2;
            nh = height/2;
        }
        
        glBindTexture(GL_TEXTURE_2D, m_pTextureYUV->m_Texture[i]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, nw, nh, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, p);
    }
}

void COpenGLEngine::UpdateSize(int width, int height)
{
    if (width >= 0 && height >= 0) {
        m_ViewWidth = width;
        m_ViewHeight = height;        
    }
}

ZJ_NAMESPACE_END
