//
//  CGLShader.cpp
//  ZJCrossCpp
//
//  Created by eafy on 2020/10/14.
//  Copyright © 2020 ZJ. All rights reserved.
//

#include "CGLShader.hpp"
#include <fstream>
#include <sstream>

ZJ_NAMESPACE_BEGIN

CGLShader::CGLShader():m_Program(0)
{
  
}

CGLShader::CGLShader(std::string filePath, std::list<std::string> attributesList, std::list<std::string> uniformsList):m_Program(0)
{
    LoadShaders(filePath, attributesList, uniformsList);
}

CGLShader::~CGLShader()
{
    if (m_Program) {
        glDeleteProgram(m_Program);
        m_Program = 0;
    }
    if (m_pAttributes != nullptr) {
        delete [] m_pAttributes;
        m_pAttributes = NULL;
    }
    if (m_pUniforms != nullptr) {
        delete [] m_pUniforms;
        m_pUniforms = NULL;
    }
}

bool CGLShader::LoadShaders(std::string filePath, std::list<std::string> attributesList, std::list<std::string> uniformsList)
{
    std::ifstream vertStream(filePath + ".vert");
    std::stringstream vertBuffer;
    vertBuffer << vertStream.rdbuf();
    std::string vertContentStr = vertBuffer.str();
    const GLchar *vertShaderSource = (GLchar *)vertContentStr.c_str();
    if (!vertShaderSource) {
        CLogW("Failed to load vertex shader");
        return false;
    }
    
    std::ifstream fragStream(filePath + ".frag");
    std::stringstream fragBuffer;
    fragBuffer << fragStream.rdbuf();
    std::string fragContentStr = fragBuffer.str();
    const GLchar *fragShaderSource = (GLchar *)fragContentStr.c_str();
    if (!fragShaderSource) {
        CLogW("Failed to load fragment shader");
        return false;
    }
    
    return LoadShaders(vertShaderSource, attributesList, fragShaderSource, uniformsList);
}

bool CGLShader::LoadShaders(const GLchar *vertShaderSource, std::list<std::string> attributesList, const GLchar *fragShaderSource, std::list<std::string> uniformsList)
{
//    CLogD("Load vertex shader: %s", vertShaderSource);
//    CLogD("Load fragment shader: %s", fragShaderSource);
    
    GLuint vertShader, fragShader;
    if (m_Program) {
        glDeleteProgram(m_Program);
    }
    m_Program = glCreateProgram();  //创建着色程序容器
    if (m_Program <= 0) {
        CLogW("Failed to create gl program");
        return false;
    }
    
    if (!CompileShader(vertShader, GL_VERTEX_SHADER, vertShaderSource)) { //创建并编译顶点着色器
        CLogW("Failed to compile vertex shader");
        if (m_Program) {
            glDeleteProgram(m_Program);
            m_Program = 0;
        }
        return false;
    }
    
    if (!CompileShader(fragShader, GL_FRAGMENT_SHADER, fragShaderSource)) {   //创建和编译帧着色器
        CLogW("Failed to compile fragment shader");
        if (m_Program) {
            glDeleteProgram(m_Program);
            m_Program = 0;
        }
        return false;
    }
    
    glAttachShader(m_Program, vertShader);  //绑定顶点着色器
    glAttachShader(m_Program, fragShader);  //绑定帧着色器
    
    //绑定属性，可通过VBO进行绑定
//    for (std::list<std::string>::iterator iter = attributesList.begin(); iter != attributesList.end(); iter++) {
//        glBindAttribLocation(m_Program, index, (*iter).c_str());
//        index ++;
//    }
    
    if (!LinkProgram(m_Program)) {
        CLogW("Failed to link program: %d", m_Program);
        if (vertShader) {
            glDeleteShader(vertShader);
        }
        if (fragShader) {
            glDeleteShader(fragShader);
        }
        if (m_Program) {
            glDeleteProgram(m_Program);
            m_Program = 0;
        }
        
        return false;
    }
    
    int index = 0;
    if (m_pUniforms) {
        delete [] m_pUniforms;
        m_pUniforms = nullptr;
    }
    m_pUniforms = new GLint[uniformsList.size()]();
    memset(m_pUniforms, -1 , attributesList.size());
    for (std::list<std::string>::iterator iter = uniformsList.begin(); iter != uniformsList.end(); iter++) {
        m_pUniforms[index] = glGetUniformLocation(m_Program, (*iter).c_str());
        if (m_pUniforms[index] == (GLuint)-1) {
            CLogW("uniform with name %s is disabled", (*iter).c_str());
        }
        index ++;
    }
    
    index = 0;
    if (m_pAttributes) {
        delete [] m_pAttributes;
        m_pAttributes = nullptr;
    }
    m_pAttributes = new GLint[attributesList.size()]();
    memset(m_pAttributes, -1 , attributesList.size());
    for (std::list<std::string>::iterator iter = attributesList.begin(); iter != attributesList.end(); iter++) {
        m_pAttributes[index] = glGetAttribLocation(m_Program, (*iter).c_str());
        if (m_pAttributes[index] == (GLuint)-1) {
            CLogW("attributes with name %s is disabled", (*iter).c_str());
        }
        index ++;
    }
    
    if (vertShader) {
        glDeleteShader(vertShader);
    }
    if (fragShader) {
        glDeleteShader(fragShader);
    }
    
    return true;
}

//编译着色器
bool CGLShader::CompileShader(GLuint &shader, GLenum type, const GLchar *source)
{
    shader = glCreateShader(type);  //创建着色器
    glShaderSource(shader, 1, &source, NULL);   //绑定着色器代码
    glCompileShader(shader);    //编译着色器
    
    GLint status;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &status);  //检测着色器有效性
    if (status == GL_FALSE) {
        GLint logLength;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLength);
        if (logLength > 0) {
            GLchar *log = (GLchar *)malloc(logLength);
            glGetShaderInfoLog(shader, logLength, &logLength, log);
            CLogD("Shader compile log:\n%s", log);
            free(log);
        }
        
        glDeleteShader(shader);
        return false;
    }
    
    return true;
}

//链接着色程序容器
bool CGLShader::LinkProgram(GLuint prog)
{
    glLinkProgram(prog);
    
    GLint status;
    glGetProgramiv(prog, GL_LINK_STATUS, &status);
    if (status == 0) {
        GLint logLength;
        glGetProgramiv(prog, GL_INFO_LOG_LENGTH, &logLength);
        if (logLength > 0) {
            GLchar *log = (GLchar *)malloc(logLength);
            glGetProgramInfoLog(prog, logLength, &logLength, log);
            CLogD("Program link log:\n%s", log);
            free(log);
        }
        
        return false;
    }
    
    return true;
}

//验证着色器容器是否有效
bool CGLShader::ValidateProgram(GLuint prog)
{
    glValidateProgram(prog);
    
#ifdef DEBUG
    GLint logLength;
    glGetProgramiv(prog, GL_INFO_LOG_LENGTH, &logLength);
    if (logLength > 0) {
        GLchar *log = (GLchar *)malloc(logLength);
        glGetProgramInfoLog(prog, logLength, &logLength, log);
        CLogD("Program validate log:\n%s", log);
        free(log);
    }
#endif
    
    GLint status;
    glGetProgramiv(prog, GL_VALIDATE_STATUS, &status);
    if (status == 0)
        return false;
    
    return true;
}

ZJ_NAMESPACE_END
