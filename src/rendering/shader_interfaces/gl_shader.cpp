#include "gl_shader.hpp"
#include "opengl_includes.hpp"
#include "printing.hpp"

#include <glm/gtc/type_ptr.hpp>
#include <vector>

// FIXME: Replace with GLShader code from Levitate

bool GLShader::CompileShader(const std::string& vertex_shader_code, const std::string& fragment_shader_code)
{
    const char *v_shader_code = vertex_shader_code.c_str();
    const char *f_shader_code = fragment_shader_code.c_str();

    unsigned int vertex, fragment;
    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &v_shader_code, nullptr);
    glCompileShader(vertex);
    PRINTDEBUG("Checking for Vertex Shader errors")
    if(!GLShaderErrorHandler(vertex))
    {
        PRINT_DEBUG("Vertex shader failed to compile! Shader code:\n'{}'", v_shader_code)
        return false;
    }

    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &f_shader_code, nullptr);
    glCompileShader(fragment);
    PRINTDEBUG("Checking for Fragment Shader errors")
    if(!GLShaderErrorHandler(fragment))
    {
        PRINT_DEBUG("Fragment shader failed to compile! Shader code:\n'{}'", f_shader_code)
        return false;
    }

    _id = glCreateProgram();
    glAttachShader(_id, vertex);
    glAttachShader(_id, fragment);
    glLinkProgram(_id);
    PRINTDEBUG("Checking for Shader Program errors")
    if(!GLShaderErrorHandler(_id, true))
        return false;

    glDeleteShader(vertex);
    glDeleteShader(fragment);

    is_compiled = true;
    return true;
}

bool GLShader::IsValid() const
{ return is_compiled; }

void GLShader::Bind()
{
    if(is_bound)
        return;
    is_bound = true;
    glUseProgram(_id);
}

void GLShader::Unbind()
{
    if(!is_bound)
        return;
    is_bound = false;
}

void GLShader::Delete()
{ glDeleteShader(_id); }

void GLShader::SetUniform(const std::string& name, int value) const
{ glProgramUniform1i(_id, glGetUniformLocation(_id, name.c_str()), value); }

void GLShader::SetUniform(const std::string& name, float value) const
{ glProgramUniform1f(_id, glGetUniformLocation(_id, name.c_str()), value); }

void GLShader::SetUniform(const std::string& name, glm::vec2 value) const
{ glProgramUniform2fv(_id, glGetUniformLocation(_id, name.c_str()), 1, glm::value_ptr(value)); }

void GLShader::SetUniform(const std::string& name, glm::vec3 value) const
{ glProgramUniform3fv(_id, glGetUniformLocation(_id, name.c_str()), 1, glm::value_ptr(value)); }

void GLShader::SetUniform(const std::string& name, glm::mat4 value) const
{ glProgramUniformMatrix4fv(_id, glGetUniformLocation(_id, name.c_str()), 1, GL_FALSE, glm::value_ptr(value)); }

bool GLShader::GLShaderErrorHandler(unsigned int shader_id, bool is_program_linking) const
{
    // FIXME: This is from GraphX and iirc there's some finnicky business with the if statements

    // https://stackoverflow.com/a/63420289
    int v_result = GL_FALSE;
    int info_log_length;
    std::string shader_error_type = "Shader Compilation";
    if(is_program_linking)
    {
        shader_error_type = "Program Linking";
        glGetProgramiv(shader_id, GL_LINK_STATUS, &v_result);
        glGetProgramiv(shader_id, GL_INFO_LOG_LENGTH, &info_log_length);
    }
    else
    {
        glGetShaderiv(shader_id, GL_COMPILE_STATUS, &v_result);
        glGetShaderiv(shader_id, GL_INFO_LOG_LENGTH, &info_log_length);
    }
    if(info_log_length > 0)
    {
        std::vector<char> shader_error_message(info_log_length + 1);
        if(is_program_linking)
            glGetProgramInfoLog(shader_id, info_log_length, nullptr, shader_error_message.data());
        else
            glGetShaderInfoLog(shader_id, info_log_length, nullptr, shader_error_message.data());

        PRINT_ERROR("GLSL %s Error(s):\n{}", shader_error_type, shader_error_message.data())
        return false;
    }

    return true;
}
