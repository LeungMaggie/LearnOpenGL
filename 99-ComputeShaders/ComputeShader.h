#pragma once

#include <glad/glad.h>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

class ComputeShader
{
public: 
  unsigned int ID;
  ComputeShader(const char* computePath);
  void use();
  void delete_program();
  void set_bool(const std::string& name, bool value) const;
  void set_int(const std::string& name, int value) const;
  void set_float(const std::string& name, float value) const;

};

void checkCompileErrors(const unsigned int& shaderID, const char* keyword);
void checkProgramLinkErrors(const unsigned int& programID);
