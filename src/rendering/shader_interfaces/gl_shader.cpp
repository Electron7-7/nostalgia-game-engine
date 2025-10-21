#include "gl_shader.hpp"
#include "common/printing.hpp"
#include "common/colors.hpp"
#include "glad/glad.h"

#include <glm/gtc/type_ptr.hpp>
#include <vector>

bool GLShader::CompileShader(const std::string& vertex_shader_code, const std::string& fragment_shader_code)
{
    const char* v_shader_code = vertex_shader_code.c_str();
    const char* f_shader_code = fragment_shader_code.c_str();
    unsigned int vertex, fragment;
    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &v_shader_code, nullptr);
    glCompileShader(vertex);
    if(!GLShaderErrorHandler(vertex))
    {
        print_error("Vertex shader failed to compile!");
        print_debug("Shader Code:{}\n{}", sty::Reset, v_shader_code);
        return false;
    }
    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &f_shader_code, nullptr);
    glCompileShader(fragment);
    if(!GLShaderErrorHandler(fragment))
    {
        print_error("Fragment shader failed to compile! Shader code:");
        print_debug("Shader Code:{}\n{}", sty::Reset, f_shader_code);
        return false;
    }
    m_ID = glCreateProgram();
    glAttachShader(m_ID, vertex);
    glAttachShader(m_ID, fragment);
    glLinkProgram(m_ID);
    if(!GLShaderErrorHandler(m_ID, true))
    {
        print_error("Shader Program failed to compile!");
        print_debug("Vertex Shader Code:{}\n{}", sty::Reset, v_shader_code);
        print_debug("Fragment Shader Code:{}\n{}", sty::Reset, f_shader_code);
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

bool GLShader::GLShaderErrorHandler(unsigned int shader, bool is_program_linking) const
{
    // https://stackoverflow.com/a/63420289
    int v_result = GL_FALSE;
    int info_log_length;
    std::string shader_error_type = "Shader Compilation";
    if(is_program_linking)
    {
        shader_error_type = "Program Linking";
        glGetProgramiv(shader, GL_LINK_STATUS, &v_result);
        glGetProgramiv(shader, GL_INFO_LOG_LENGTH, &info_log_length);
    }
    else
    {
        glGetShaderiv(shader, GL_COMPILE_STATUS, &v_result);
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &info_log_length);
    }
    if(info_log_length > 0)
    {
        std::vector<char> shader_error_message(info_log_length + 1);
        if(is_program_linking)
            { glGetProgramInfoLog(shader, info_log_length, nullptr, shader_error_message.data()); }
        else
            { glGetShaderInfoLog(shader, info_log_length, nullptr, shader_error_message.data()); }
        print_error("GLSL %s Error(s):\n{}", shader_error_type, shader_error_message.data());
        return false;
    }

    return true;
}
