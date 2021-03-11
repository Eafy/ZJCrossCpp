//
//  CGLShader.cpp
//  ZJCrossCpp
//
//  Created by eafy on 2020/10/14.
//  Copyright © 2020 ZJ. All rights reserved.
//

#ifndef CGLShader_hpp
#define CGLShader_hpp

#include <stdio.h>
#include <string>
#include <list>
#include "CDefType.hpp"
#if defined(__ANDROID__) || defined(ANDROID)    //Android
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#elif defined(__APPLE__)  //iOS
#include <OpenGLES/ES2/gl.h>
#include <OpenGLES/ES2/glext.h>
#endif

ZJ_NAMESPACE_BEGIN

class CGLShader
{
public:
    CGLShader();
    CGLShader(std::string filePath, std::list<std::string> attributesList, std::list<std::string> uniformsList);
    virtual ~CGLShader();
    
    /**
     加载着色器代码文件，并进行编译

     @param filePath 着色器代码文件路径，不能带后缀名，且帧着色器代码文件后缀名为.frag，顶点着色器代码问价后缀名.vert。比如2个文件名为“yuvYex.frag“和”yuvTex.vert“，则filePath传入的参数为:文件夹路径/yuvYex。
     @param attributesList attribute名称列表
     @param uniformsList uniform名称列表
     @return 成功：true
     */
    bool LoadShaders(std::string filePath, std::list<std::string> attributesList, std::list<std::string> uniformsList);
    
    /**
     加载着色器代码，并进行编译

     @param vertShaderSource 顶点着色器代码
     @param attributesList attribute名称列表
     @param fragShaderSource 片段着色器代码
     @param uniformsList uniform名称列表
     @return 成功：true
     */
    bool LoadShaders(const GLchar *vertShaderSource, std::list<std::string> attributesList, const GLchar *fragShaderSource, std::list<std::string> uniformsList);
    
    /**
     编译着色器程序

     @param shader 着色器
     @param type 类型
     @param source 着色器代码
     @return 成功:true
     */
    bool CompileShader(GLuint &shader, GLenum type, const GLchar *source);
    
    bool isValid() { return m_Program > 0; };
    GLuint GetProgram() { return m_Program; };
    GLint *GetAttributes() { return m_pAttributes; };
    GLint *GetUniforms() { return m_pUniforms; };
    
private:
    bool LinkProgram(GLuint prog);
    bool ValidateProgram(GLuint prog);
    
private:
    GLuint m_Program = 0;
    GLint *m_pAttributes = nullptr;
    GLint *m_pUniforms = nullptr;
};

ZJ_NAMESPACE_END
#endif /* CGLShader_hpp */
