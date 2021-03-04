//
//  CGLShaderYUV.cpp
//  ZJCrossCpp
//
//  Created by eafy on 2020/10/14.
//  Copyright © 2020 ZJ. All rights reserved.
//

#include "CGLShaderYUV.hpp"
#include "CLog.hpp"

ZJ_NAMESPACE_TOOL_BEGIN

// BT.601, which is the standard for SDTV.
static const GLfloat kColorConversion601[] = {
    1.164,  1.164, 1.164,
    0.0, -0.392, 2.017,
    1.596, -0.813,   0.0,
};

// BT.709, which is the standard for HDTV.
static const GLfloat kColorConversion709[] = {
    1.164,  1.164, 1.164,
    0.0, -0.213, 2.112,
    1.793, -0.533,   0.0,
};

// BT.601, which is the full rang for SDTV.
static const GLfloat kColorConversion601FullRange[] = {
    1.0, 1.0, 1.0,
    0.0, -0.343, 1.765,
    1.4, -0.711, 0.0,
};

static const GLchar *shaderSourceFSH = (const GLchar*)"varying lowp vec2 texcoordVarying;\
precision mediump float;\
uniform sampler2D SamplerY;\
uniform sampler2D SamplerU;\
uniform sampler2D SamplerV;\
uniform mat3 colorConversionMatrix;\
void main(void)\
{\
    mediump vec3 yuv;\
    lowp vec3 rgb;\
    yuv.x = texture2D(SamplerY, texcoordVarying).r;\
    yuv.y = texture2D(SamplerU, texcoordVarying).r - 0.5;\
    yuv.z = texture2D(SamplerV, texcoordVarying).r - 0.5;\
    rgb = colorConversionMatrix * yuv;\
    gl_FragColor = vec4(rgb, 1);\
}";

static const GLchar *shaderSourceVSH = (const GLchar*)"\
attribute vec4 position;\
attribute vec2 texcoord;\
varying vec2 texcoordVarying;\
uniform float preferredRotation;\
void main()\
{\
    mat4 rotationMatrix = mat4(cos(preferredRotation), -sin(preferredRotation), 0.0, 0.0,\
    sin(preferredRotation),  cos(preferredRotation), 0.0, 0.0,\
    0.0, 0.0, 1.0, 0.0,\
    0.0, 0.0, 0.0, 1.0);\
    gl_Position = position * rotationMatrix;\
    texcoordVarying = texcoord;\
}";

CGLShaderYUV::CGLShaderYUV(std::string filePath, bool loadAll)
{
    m_AttPosition =
    m_AttTexcoord =
    m_UniTexY =
    m_UniTexU =
    m_UniTexV =
    m_UniModelViewMatrix3 = -1;
    
    if (!filePath.empty()) {
//        std::list<std::string> uniformsList;
//        uniformsList.push_back("modelViewProjectionMatrix");
//        uniformsList.push_back("s_texture_y");
//        uniformsList.push_back("s_texture_u");
//        uniformsList.push_back("s_texture_v");
//
//        std::list<std::string> attributesList;
//        attributesList.push_back("position");
//        attributesList.push_back("texcoord");
//
//        bool loadSuccess = LoadShaders(filePath, uniformsList, attributesList);
    } else {
        std::list<std::string> attributesList;
        attributesList.push_back("position");
        attributesList.push_back("texcoord");
        
        std::list<std::string> uniformsList;
        uniformsList.push_back("SamplerY");
        uniformsList.push_back("SamplerU");
        uniformsList.push_back("SamplerV");
        uniformsList.push_back("colorConversionMatrix");
        uniformsList.push_back("preferredRotation");
        
        bool loadSuccess = LoadShaders(shaderSourceVSH, attributesList, shaderSourceFSH, uniformsList);
        if (loadSuccess) {
            CLogD("Success to load shaders");
            m_AttPosition = GetAttributes()[0];
            m_AttTexcoord = GetAttributes()[1];
            
            m_UniTexY = GetUniforms()[0];
            m_UniTexU = GetUniforms()[1];
            m_UniTexV = GetUniforms()[2];
            m_UniModelViewMatrix3 = GetUniforms()[3];
            m_UniPreferredRotation = GetUniforms()[4];
            
            m_pColorConversion = kColorConversion601FullRange;
            
            if (loadAll) {
                UpdateAllUniforms();
            }
        } else {
            CLogW("Failed to load gl shaders");
        }
    }
}

CGLShaderYUV::~CGLShaderYUV()
{
    
}

bool CGLShaderYUV::UpdateAllUniforms()
{
    if (!isValid()) {
        CLogW("Failed to update yuv uniforms");
        return false;
    }
    glUseProgram(GetProgram());
    
    glUniform1i(m_UniTexY, 0);
    glUniform1i(m_UniTexU, 1);
    glUniform1i(m_UniTexV, 2);
    
    if (m_UniPreferredRotation >= 0) {
        glUniform1i(m_UniPreferredRotation, 0);
    }
    
    if (m_UniModelViewMatrix3 >= 0) {
        glUniformMatrix3fv(m_UniModelViewMatrix3, 1, GL_FALSE, m_pColorConversion);
    }
    
    return true;
}

ZJ_NAMESPACE_TOOL_END
