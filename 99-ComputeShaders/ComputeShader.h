#pragma once

#include "Shader.h"

class ComputeShader : public Shader
{
public:
  ComputeShader(const char* computePath);
};

void checkCompileErrors(const unsigned int& shaderID, const char* keyword);
void checkProgramLinkErrors(const unsigned int& programID);
