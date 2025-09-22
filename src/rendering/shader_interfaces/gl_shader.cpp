#include "gl_shader.hpp"
#include "printing.hpp"
#include "colors.hpp"
#include "glad/glad.h"

#include <glm/gtc/type_ptr.hpp>
#include <vector>

#pragma message("(FIXME) Replace with GLShader code from Levitate")
bool GLShader::CompileShader(const std::string& vertex_shader_code, const std::string& fragment_shader_code)
{
    const char *v_shader_code = vertex_shader_code.c_str();
    const char *f_shader_code = fragment_shader_code.c_str();

    unsigned int vertex, fragment;
    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &v_shader_code, nullptr);
    glCompileShader(vertex);
    PRINT_DEBUG("Checking for Vertex Shader errors")
    if(!GLShaderErrorHandler(vertex))
    {
        PRINT_ERROR("Vertex shader failed to compile!")
        PRINT_DEBUG("Shader Code:{}\n{}", sty::Reset, v_shader_code)
        return false;
    }

    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &f_shader_code, nullptr);
    glCompileShader(fragment);
    PRINT_DEBUG("Checking for Fragment Shader errors")
    if(!GLShaderErrorHandler(fragment))
    {
        PRINT_ERROR("Fragment shader failed to compile! Shader code:")
        PRINT_DEBUG("Shader Code:{}\n{}", sty::Reset, f_shader_code)
        return false;
    }

    m_ID = glCreateProgram();
    glAttachShader(m_ID, vertex);
    glAttachShader(m_ID, fragment);
    glLinkProgram(m_ID);
    PRINT_DEBUG("Checking for Shader Program errors")
    if(!GLShaderErrorHandler(m_ID, true))
    {
        PRINT_ERROR("Shader Program failed to compile!")
        PRINT_DEBUG("Vertex Shader Code:{}\n{}", sty::Reset, v_shader_code)
        PRINT_DEBUG("Fragment Shader Code:{}\n{}", sty::Reset, f_shader_code)
        return false;
    }

    glDeleteShader(vertex);
    glDeleteShader(fragment);

    m_IsCompiled = true;
    return true;
}

bool GLShader::IsValid() const
{ return m_IsCompiled; }

void GLShader::SetUniform(const std::string& name, int value) const
{ glProgramUniform1i(m_ID, glGetUniformLocation(m_ID, name.c_str()), value); }

void GLShader::SetUniform(const std::string& name, float value) const
{ glProgramUniform1f(m_ID, glGetUniformLocation(m_ID, name.c_str()), value); }

void GLShader::SetUniform(const std::string& name, glm::vec2 value) const
{ glProgramUniform2fv(m_ID, glGetUniformLocation(m_ID, name.c_str()), 1, glm::value_ptr(value)); }

void GLShader::SetUniform(const std::string& name, glm::vec3 value) const
{ glProgramUniform3fv(m_ID, glGetUniformLocation(m_ID, name.c_str()), 1, glm::value_ptr(value)); }

void GLShader::SetUniform(const std::string& name, glm::vec4 value) const
{ glProgramUniform4fv(m_ID, glGetUniformLocation(m_ID, name.c_str()), 1, glm::value_ptr(value)); }

void GLShader::SetUniform(const std::string& name, glm::mat3 value) const
{ glProgramUniformMatrix3fv(m_ID, glGetUniformLocation(m_ID, name.c_str()), 1, GL_FALSE, glm::value_ptr(value)); }

void GLShader::SetUniform(const std::string& name, glm::mat4 value) const
{ glProgramUniformMatrix4fv(m_ID, glGetUniformLocation(m_ID, name.c_str()), 1, GL_FALSE, glm::value_ptr(value)); }

bool GLShader::GLShaderErrorHandler(unsigned int shaderm_ID, bool is_program_linking) const
{
    #pragma message("(FIXME) This is from GraphX and iirc there's some finnicky business with the if statements")

    // https://stackoverflow.com/a/63420289
    int v_result = GL_FALSE;
    int info_log_length;
    std::string shader_error_type = "Shader Compilation";
    if(is_program_linking)
    {
        shader_error_type = "Program Linking";
        glGetProgramiv(shaderm_ID, GL_LINK_STATUS, &v_result);
        glGetProgramiv(shaderm_ID, GL_INFO_LOG_LENGTH, &info_log_length);
    }
    else
    {
        glGetShaderiv(shaderm_ID, GL_COMPILE_STATUS, &v_result);
        glGetShaderiv(shaderm_ID, GL_INFO_LOG_LENGTH, &info_log_length);
    }
    if(info_log_length > 0)
    {
        std::vector<char> shader_error_message(info_log_length + 1);
        if(is_program_linking)
        { glGetProgramInfoLog(shaderm_ID, info_log_length, nullptr, shader_error_message.data()); }
        else
        { glGetShaderInfoLog(shaderm_ID, info_log_length, nullptr, shader_error_message.data()); }

        PRINT_ERROR("GLSL %s Error(s):\n{}", shader_error_type, shader_error_message.data())
        return false;
    }

    return true;
}
