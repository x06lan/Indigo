#include "program.hpp"

#include <fstream>
#include <sstream>

#include "log.hpp"
#include "exception.hpp"

Program::Program(const std::string &vertexShaderFilepath,
                 const std::string &fragmentShaderFilepath) {
    m_ProgramID = glCreateProgram();
    LOG_TRACE("Creating Program {}", m_ProgramID);

    m_VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
    m_FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

    CompileShader(m_VertexShaderID, vertexShaderFilepath);
    CompileShader(m_FragmentShaderID, fragmentShaderFilepath);
    // CompileBinShader(m_FragmentShaderID, fragmentShaderFilepath);

    LinkProgram();
}
Program::Program(const std::string &vertexShaderFilepath,
                 const std::string &fragmentShaderFilepath, bool spirv) {
    m_ProgramID = glCreateProgram();
    LOG_TRACE("Creating Program {}", m_ProgramID);

    m_VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
    m_FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

    if (spirv) {
        CompileBinShader(m_VertexShaderID, vertexShaderFilepath);
        CompileBinShader(m_FragmentShaderID, fragmentShaderFilepath);
    } else {
        CompileShader(m_VertexShaderID, vertexShaderFilepath);
        CompileShader(m_FragmentShaderID, fragmentShaderFilepath);
    }

    LinkProgram();
}

Program::Program(const std::string &vertexShaderFilepath,
                 const std::string &geometryShaderFilepath,
                 const std::string &fragmentShaderFilepath) {
    m_ProgramID = glCreateProgram();
    LOG_TRACE("Creating Program {}", m_ProgramID);

    LOG_DEBUG("Varying Info");
    GLint info;
    glGetIntegerv(GL_MAX_VARYING_FLOATS, &info);
    LOG_DEBUG("  GL_MAX_VARYING_FLOATS: {}", info);
    glGetIntegerv(GL_MAX_VARYING_COMPONENTS, &info);
    LOG_DEBUG("  GL_MAX_VARYING_COMPONENTS: {}", info);
    glGetIntegerv(GL_MAX_VARYING_VECTORS, &info);
    LOG_DEBUG("  GL_MAX_VARYING_VECTORS: {}", info);
    glGetIntegerv(GL_MAX_COLOR_ATTACHMENTS, &info);
    LOG_DEBUG("  GL_MAX_COLOR_ATTACHMENTS: {}", info);
    glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &info);
    LOG_DEBUG("  GL_MAX_TEXTURE_IMAGE_UNITS: {}", info);

    m_VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
    m_GeometryShaderID = glCreateShader(GL_GEOMETRY_SHADER);
    m_FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

    CompileShader(m_VertexShaderID, vertexShaderFilepath);
    CompileShader(m_GeometryShaderID, geometryShaderFilepath);
    CompileShader(m_FragmentShaderID, fragmentShaderFilepath);

    LinkProgram();
}

Program::~Program() {
    LOG_TRACE("Deleting Program {}", m_ProgramID);
    glDeleteProgram(m_ProgramID);
}

void Program::Bind() const {
    glUseProgram(m_ProgramID);
}

void Program::Unbind() const {
    glUseProgram(0);
}

void Program::SetInt(const std::string &name, int value) {
    GLuint location = glGetUniformLocation(m_ProgramID, name.c_str());
    glUniform1i(location, value);
}

void Program::Validate() const {
    LOG_TRACE("Validating Program {}", m_ProgramID);

    GLint status;

    glValidateProgram(m_ProgramID);
    glGetProgramiv(m_ProgramID, GL_VALIDATE_STATUS, &status);
    if (status != GL_TRUE) {
        int infoLogLength;
        glGetProgramiv(m_ProgramID, GL_INFO_LOG_LENGTH, &infoLogLength);

        std::vector<char> errMessage(infoLogLength + 1);
        glGetProgramInfoLog(m_ProgramID, infoLogLength, NULL, &errMessage[0]);

        LOG_ERROR("Validation Failed:\n{}", &errMessage[0]);
    }
}

std::string Program::LoadShaderFile(const std::string &filepath) {
    std::string source;
    std::ifstream stream(filepath, std::ios::in);

    LOG_TRACE("Loading File: '{}'", filepath);

    if (stream.is_open()) {
        std::stringstream sstr;
        sstr << stream.rdbuf();
        source = sstr.str();
        stream.close();
    } else {
        throw FileNotFoundException(filepath);
    }

    return source;
}

void Program::CompileBinShader(const GLuint shaderID,
                               const std::string &shaderFilepath) {
    std::string shaderSrc = LoadShaderFile(shaderFilepath);
    const char *srcPtr = shaderSrc.c_str();

    LOG_TRACE("Compiling Shader: '{}'", shaderFilepath);
    // glShaderSource(shaderID, 1, &srcPtr, NULL);
    // glCompileShader(shaderID);

    glShaderBinary(1, &shaderID, GL_SHADER_BINARY_FORMAT_SPIR_V, srcPtr,
                   shaderSrc.size());

    // Specialize the shader (specify the entry point)
    glSpecializeShader(shaderID, "main", 0, 0, 0);

    GLint status = GL_FALSE;

    glGetShaderiv(shaderID, GL_COMPILE_STATUS, &status);
    if (status != GL_TRUE) {
        int infoLogLength;
        glGetShaderiv(shaderID, GL_INFO_LOG_LENGTH, &infoLogLength);

        std::vector<char> errMessage(infoLogLength + 1);
        glGetShaderInfoLog(shaderID, infoLogLength, NULL, &errMessage[0]);

        LOG_ERROR("Failed to Compile Shader: '{}'\n{}", shaderFilepath,
                  &errMessage[0]);
    }
    LOG_TRACE("Compiling Successful");
}
void Program::CompileShader(const GLuint shaderID,
                            const std::string &shaderFilepath) {
    std::string shaderSrc = LoadShaderFile(shaderFilepath);
    const char *srcPtr = shaderSrc.c_str();

    LOG_TRACE("Compiling Shader: '{}'", shaderFilepath);
    glShaderSource(shaderID, 1, &srcPtr, NULL);
    glCompileShader(shaderID);

    GLint status = GL_FALSE;

    glGetShaderiv(shaderID, GL_COMPILE_STATUS, &status);
    if (status != GL_TRUE) {
        int infoLogLength;
        glGetShaderiv(shaderID, GL_INFO_LOG_LENGTH, &infoLogLength);

        std::vector<char> errMessage(infoLogLength + 1);
        glGetShaderInfoLog(shaderID, infoLogLength, NULL, &errMessage[0]);

        LOG_ERROR("Failed to Compile Shader: '{}'\n{}", shaderFilepath,
                  &errMessage[0]);
    }
    LOG_TRACE("Compiling Successful");
}

void Program::LinkProgram() {
    LOG_TRACE("Linking Program {}", m_ProgramID);

    glAttachShader(m_ProgramID, m_VertexShaderID);
    if (m_GeometryShaderID != 0)
        glAttachShader(m_ProgramID, m_GeometryShaderID);
    glAttachShader(m_ProgramID, m_FragmentShaderID);
    glLinkProgram(m_ProgramID);

    /*
     * The error handling here seems redundant but
     * the linking process doesn't seem to happen until
     * the program enters the rendering loop. So it's
     * better to handle it here for better tracing.
     * Also, glGetProgramInfoLog produces better error
     * messages than the general OpenGLDebugMessageCallback
     */
    GLint status = GL_FALSE;

    glGetProgramiv(m_ProgramID, GL_LINK_STATUS, &status);
    if (status != GL_TRUE) {
        int infoLogLength;
        glGetProgramiv(m_ProgramID, GL_INFO_LOG_LENGTH, &infoLogLength);

        std::vector<char> errMessage(infoLogLength + 1);
        glGetProgramInfoLog(m_ProgramID, infoLogLength, NULL, &errMessage[0]);

        LOG_ERROR("Failed to Link Program:\n{}", &errMessage[0]);
    }
    LOG_TRACE("Linking Successful");

    GLint info;
    glGetProgramiv(m_ProgramID, GL_ACTIVE_UNIFORMS, &info);
    LOG_DEBUG("[{}] Active Uniforms: {}", m_ProgramID, info);
    glGetProgramiv(m_ProgramID, GL_ACTIVE_UNIFORM_BLOCKS, &info);
    LOG_DEBUG("[{}] Active Uniform Blocks: {}", m_ProgramID, info);

    glDetachShader(m_ProgramID, m_VertexShaderID);
    if (m_GeometryShaderID != 0)
        glDetachShader(m_ProgramID, m_GeometryShaderID);
    glDetachShader(m_ProgramID, m_FragmentShaderID);

    glDeleteShader(m_VertexShaderID);
    if (m_GeometryShaderID != 0)
        glDeleteShader(m_GeometryShaderID);
    glDeleteShader(m_FragmentShaderID);
}