#include "gl_shader.hpp"
#include "debugging.hpp"
#include "opengl_includes.hpp"
#include <vector>

GLShader::GLShader(const std::string& vertex_shader_code, const std::string& fragment_shader_code)
{
    const char *v_shader_code = vertex_shader_code.c_str();
    const char *f_shader_code = fragment_shader_code.c_str();

    unsigned int vertex, fragment;
    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &v_shader_code, nullptr);
    glCompileShader(vertex);
    GLShaderErrorHandler(vertex);

    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &f_shader_code, nullptr);
    glCompileShader(fragment);
    GLShaderErrorHandler(fragment);

    _id = glCreateProgram();
    glAttachShader(_id, vertex);
    glAttachShader(_id, fragment);
    glLinkProgram(_id);
    GLShaderErrorHandler(_id, true);

    glDeleteShader(vertex);
    glDeleteShader(fragment);
}

void GLShader::Bind() const
{ glUseProgram(_id); }

void GLShader::SetUniform(const std::string& name, int value) const
{}

void GLShader::SetUniform(const std::string& name, float value) const
{}

void GLShader::SetUniform(const std::string& name, glm::vec2 value) const
{}

void GLShader::SetUniform(const std::string& name, glm::vec3 value) const
{}

void GLShader::SetUniform(const std::string& name, glm::mat4 value) const
{}

void GLShader::GLShaderErrorHandler(unsigned int shader_id, bool is_program_linking) const
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

        PRINTERR("GLSL " + shader_error_type + " Error(s):\n" + shader_error_message.data());
    }
}
