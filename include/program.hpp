#ifndef PROGRAM_HPP
#define PROGRAM_HPP

#include "pch.hpp"

class Program {
public:
    Program(const std::string &vertexShaderFilepath,
            const std::string &fragmentShaderFilepath);
    Program(const std::string &vertexShaderFilepath,
            const std::string &fragmentShaderFilepath, bool spirv);
    Program(const std::string &vertexShaderFilepath,
            const std::string &geometryShaderFilepath,
            const std::string &fragmentShaderFilepath);
    ~Program();

    void Bind() const;
    void Unbind() const;

    /**
     * Validate the current state of the current bound program. If there are no
     * errors, the program is guaranteed to execute.
     */
    void Validate() const;

    /**
     * Map the sampler name in the shader to a texture slot
     */
    void SetInt(const std::string &name, int value);

    GLuint GetProgramID() const { return m_ProgramID; }

private:
    std::string LoadShaderFile(const std::string &filepath);
    void CompileShader(const GLuint shaderID, const std::string &shaderSrc);
    void CompileBinShader(const GLuint shaderID, const std::string &shaderSrc);
    void LinkProgram();

private:
    GLuint m_ProgramID;

    // perhaps can change this into a vector of shaders
    GLuint m_VertexShaderID;
    GLuint m_GeometryShaderID = 0;
    GLuint m_FragmentShaderID;
};

#endif