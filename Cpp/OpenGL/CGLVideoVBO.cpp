//
//  CGLVideoVBO.cpp
//  ZJCrossCpp
//
//  Created by eafy on 2020/10/14.
//  Copyright © 2020 ZJ. All rights reserved.
//

#include "CGLVideoVBO.hpp"

ZJ_NAMESPACE_BEGIN

#define BUFFER_OFFSET(i) ((char *)NULL + (i))

static const GLfloat QuadVertex[] =
{
    -1.0f, -1.0f,       0.0f, 1.0f,
     1.0f, -1.0f,       1.0f, 1.0f,
    -1.0f,  1.0f,       0.0f, 0.0f,
     1.0f,  1.0f,       1.0f, 0.0f,
};

CGLVideoVBO::CGLVideoVBO(GLuint position, GLuint texcoord)
{
    glGenBuffers(1, &m_VBOHandle);
    glBindBuffer(GL_ARRAY_BUFFER, m_VBOHandle);
    glBufferData(GL_ARRAY_BUFFER, sizeof(QuadVertex), QuadVertex, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glClearColor(0, 0, 0, 1.0);
    
    if (position > 0 || texcoord > 0) {
        SetAttribute(position, texcoord);
        Update();
    } else {
        CLogW("Failed to load video VBO");
    }
}

CGLVideoVBO::~CGLVideoVBO()
{
    if (m_VBOHandle) {
        glDeleteBuffers(1, &m_VBOHandle);
        m_VBOHandle = 0;
    }
}

void CGLVideoVBO::SetAttribute(GLuint position, GLuint texcoord)
{
    m_AttPosition = position;
    m_AttTexcoord = texcoord;
}

bool CGLVideoVBO::isValid()
{
    if (m_VBOHandle > 0 && (m_AttPosition > 0 || m_AttTexcoord > 0)) {
        return true;
    }
    
    return false;
}

void CGLVideoVBO::Update()
{
    if (isValid()) {
        glBindBuffer(GL_ARRAY_BUFFER, m_VBOHandle);
        glEnableVertexAttribArray(m_AttPosition);
        glVertexAttribPointer(m_AttPosition, 2, GL_FLOAT, GL_FALSE, 16/*单组(每行)的位数*/, BUFFER_OFFSET(0)/*数据偏移*/);
        glEnableVertexAttribArray(m_AttTexcoord);
        glVertexAttribPointer(m_AttTexcoord, 2, GL_FLOAT, GL_FALSE, 16, BUFFER_OFFSET(8));
    }
}

ZJ_NAMESPACE_END
