#include "gl_shader.hpp"
#include "core/printing.hpp"
#include "common/colors.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glad/glad.h>

GLShader::GLShader(const std::string& vertex_shader_code, const std::string& fragment_shader_code)
{ CompileShader(vertex_shader_code, fragment_shader_code); }

bool GLShader::CompileShader(const std::string& vertex_shader_code, const std::string& fragment_shader_code)
{
    const char* v_shader_code{vertex_shader_code.data()};
    const char* f_shader_code{fragment_shader_code.data()};

    uint vertex{glCreateShader(GL_VERTEX_SHADER)};
    glShaderSource(vertex, 1, &v_shader_code, nullptr);
    glCompileShader(vertex);
    if(!GLShaderErrorHandler(vertex))
    {
        print_debug("Vertex Shader Code:{}\n{}", Sty::Reset, v_shader_code);
        return print_errorv(VERBOSE0, "Vertex shader failed to compile!");
    }

    uint fragment{glCreateShader(GL_FRAGMENT_SHADER)};
    glShaderSource(fragment, 1, &f_shader_code, nullptr);
    glCompileShader(fragment);
    if(!GLShaderErrorHandler(fragment))
    {
        print_debug("Fragment Shader Code:{}\n{}", Sty::Reset, f_shader_code);
        return print_errorv(VERBOSE0, "Fragment shader failed to compile!");
    }

    mID = glCreateProgram();
    glAttachShader(mID(), vertex);
    glAttachShader(mID(), fragment);
    glLinkProgram(mID());
    if(!GLShaderErrorHandler(mID, true))
    {
        print_debug("Vertex Shader Code:{}\n{}", Sty::Reset, v_shader_code);
        print_debug("Fragment Shader Code:{}\n{}", Sty::Reset, f_shader_code);
        return print_errorv(VERBOSE0, "Shader Program failed to compile!");
    }
#pragma message("TODO: Should these be called even when an error occurs? I'm thinking they could free memory, which you'd want to do (unless the memory isn't used up on an error occuring)")
    glDeleteShader(vertex);
    glDeleteShader(fragment);
    return mIsCompiled = true;
}

void GLShader::Bind() const
{ glUseProgram(mID()); }

void GLShader::Unbind() const
{ glUseProgram(0); }

void GLShader::SetUniform(FARG(std::string) name, FARG(int) value) const
{ glProgramUniform1i(mID(), glGetUniformLocation(mID(), name.data()), value); }

void GLShader::SetUniform(FARG(std::string) name, FARG(float) value) const
{ glProgramUniform1f(mID(), glGetUniformLocation(mID(), name.data()), value); }

void GLShader::SetUniform(FARG(std::string) name, FARG(glm::vec2) value) const
{ glProgramUniform2fv(mID(), glGetUniformLocation(mID(), name.data()), 1, glm::value_ptr(value)); }

void GLShader::SetUniform(FARG(std::string) name, FARG(glm::vec3) value) const
{ glProgramUniform3fv(mID(), glGetUniformLocation(mID(), name.data()), 1, glm::value_ptr(value)); }

void GLShader::SetUniform(FARG(std::string) name, FARG(glm::vec4) value) const
{ glProgramUniform4fv(mID(), glGetUniformLocation(mID(), name.data()), 1, glm::value_ptr(value)); }

void GLShader::SetUniform(FARG(std::string) name, FARG(glm::mat3) value) const
{ glProgramUniformMatrix3fv(mID(), glGetUniformLocation(mID(), name.data()), 1, GL_FALSE, glm::value_ptr(value)); }

void GLShader::SetUniform(FARG(std::string) name, FARG(glm::mat4) value) const
{ glProgramUniformMatrix4fv(mID(), glGetUniformLocation(mID(), name.data()), 1, GL_FALSE, glm::value_ptr(value)); }

#pragma message("NOTE: I did a lot of 'optimization' here, so make sure it isn't broken")
bool GLShader::GLShaderErrorHandler(FARG(ID) shader, bool is_program_linking) const
{
    // https://stackoverflow.com/a/63420289
    int v_result{GL_FALSE};
    int info_log_length{};
    std::string shader_error_type =
        (is_program_linking)
            ? "Program Linking"
            : "Shader Compilation";
    int pname =
        (is_program_linking)
            ? GL_LINK_STATUS
            : GL_COMPILE_STATUS;
    void (*glGetProgramOrShaderiv)(uint,uint,int*) =
        (is_program_linking)
            ? glad_glGetProgramiv
            : glad_glGetShaderiv;
    void (*glGetProgramOrShaderInfoLog)(uint, int, int*, char*) =
        (is_program_linking)
            ? glad_glGetProgramInfoLog
            : glad_glGetShaderInfoLog;

    glGetProgramOrShaderiv(shader(), pname, &v_result);
    glGetProgramOrShaderiv(shader(), GL_INFO_LOG_LENGTH, &info_log_length);

    if(info_log_length <= 0)
        { return true; }

    std::vector<char> shader_error_message(info_log_length + 1);
    glGetProgramOrShaderInfoLog(shader(), info_log_length, nullptr, shader_error_message.data());
    return print_errorv(VERBOSE0, "GLSL %s Error(s):\n{}", shader_error_type, shader_error_message.data());
}
