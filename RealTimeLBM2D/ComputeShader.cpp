#include "ComputeShader.h"

ComputeShader::ComputeShader(const char* computePath)
{
  std::string computeCode;
  std::ifstream cShaderFile;
  cShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

  try
  {
    cShaderFile.open(computePath);
    std::stringstream cShaderStream;
    cShaderStream << cShaderFile.rdbuf();
    computeCode = cShaderStream.str();
  }
  catch(std::ifstream::failure e)
  {
    std::cout << "ERROR::COMPUTE_SHADER::FILE_NOT_SUCCESSFULLY_READ" << std::endl;
  }
  
  const char* cShaderCode = computeCode.c_str();

  unsigned int compute;

  compute = glCreateShader(GL_COMPUTE_SHADER);
  glShaderSource(compute, 1, &cShaderCode, NULL);
  glCompileShader(compute);
  checkCompileErrors(compute, "COMPUTE");

  ID = glCreateProgram();
  glAttachShader(ID, compute);
  glLinkProgram(ID);
  checkProgramLinkErrors(ID);
  glDeleteShader(compute);
}

void checkCompileErrors(const unsigned int& shaderID, const char* keyword)
{
  int success;
  char infoLog[512];
  glGetShaderiv(shaderID, GL_COMPILE_STATUS, &success);
  if (!success)
  {
    glGetShaderInfoLog(shaderID, 512, NULL, infoLog);
    std::cout << "ERROR::COMPUTE_SHADER::" << keyword << "::COMPILATION_FAILED\n" << infoLog << std::endl;
  }
}

void checkProgramLinkErrors(const unsigned int& programID)
{
  int success;
  char infoLog[512];
  glGetProgramiv(programID, GL_LINK_STATUS, &success);
  if (!success)
  {
    glGetShaderInfoLog(programID, 512, NULL, infoLog);
    std::cout << "ERROR::COMPUTE_SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
  }
}

