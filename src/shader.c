#include "shader.h"
#include "types.h"

#include <stdio.h>
#include <stdlib.h>

// returns:
//   false if no error occured
//   true if error occured and error was returned
bool shaderCheckCompileErrors(u32 shader, ShaderTypeE type, ShaderCompilationErr *r_err) {
  i32 success;
  char infoLog[512];
  glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(shader, 512, NULL, infoLog);
    r_err->type = type;
    r_err->infoLog = infoLog;
    return true;
  }
  return false;
}

// returns:
//   false if no error occured
//   true if error occured and error was returned
bool shaderCheckLinkErrors(u32 shader, ShaderLinkErr *r_err) {
  i32 success;
  char infoLog[512];
  glGetProgramiv(shader, GL_LINK_STATUS, &success);
  if (!success) {
    glGetProgramInfoLog(shader, 512, NULL, infoLog);
    r_err->infoLog = infoLog;
    return true;
  }
  return false;
}

// returns:
//   false if no error occured and result was returned
//   true if error occured and error was returned
bool shaderFromFile(const char *vPath, const char *fPath, u32 *r_id, ShaderFromFileErr *r_err) {
  // Read vertex shader into memory
  FILE *vShaderFile = fopen(vPath, "r");
  if (!vShaderFile) {
    FileOpenErr err = {vPath};
    *r_err = FROM(*r_err, err)(err);
    return true;
  }

  char *vShaderS = malloc(sizeof(char));
  if (!vShaderS) {
    MemAllocErr err = {sizeof(char)};
    *r_err = FROM(*r_err, err)(err);
    return true;
  }
  i32 i = 0;
  while (!feof(vShaderFile)) {
    vShaderS[i] = fgetc(vShaderFile);
    vShaderS = realloc(vShaderS, (i + 2) * sizeof(char));
    if (!vShaderS) {
      MemAllocErr err = {(i + 2) * sizeof(char)};
      *r_err = FROM(*r_err, err)(err);
      return true;
    }
    i++;
  }
  vShaderS[i - 1] = '\0';
  fclose(vShaderFile);

  // Compile vertex shader
  u32 vShader;

  vShader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vShader, 1, (const char *const *)&vShaderS, NULL);
  glCompileShader(vShader);
  free(vShaderS);

  // Check for errors
  ShaderCompilationErr shaderCompilationErr;
  if (shaderCheckCompileErrors(vShader, ShaderVertex, &shaderCompilationErr)) {
    *r_err = FROM(*r_err, shaderCompilationErr)(shaderCompilationErr);
    return true;
  }

  // Read fragment shader into memory
  FILE *fShaderFile = fopen(fPath, "r");
  if (!fShaderFile) {
    FileOpenErr err = {fPath};
    *r_err = FROM(*r_err, err)(err);
    return true;
  }

  char *fShaderS = malloc(sizeof(char));
  if (!fShaderS) {
    MemAllocErr err = {sizeof(char)};
    *r_err = FROM(*r_err, err)(err);
    return true;
  }
  i = 0;
  while (!feof(fShaderFile)) {
    fShaderS[i] = fgetc(fShaderFile);
    fShaderS = realloc(fShaderS, (i + 2) * sizeof(char));
    if (!fShaderS) {
      MemAllocErr err = {(i + 2) * sizeof(char)};
      *r_err = FROM(*r_err, err)(err);
      return true;
    }
    i++;
  }
  fShaderS[i - 1] = '\0';
  fclose(fShaderFile);

  // Compile fragment shader
  u32 fShader;

  fShader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fShader, 1, (const char *const *)&fShaderS, NULL);
  glCompileShader(fShader);
  free(fShaderS);

  // Check for errors
  if (shaderCheckCompileErrors(fShader, ShaderFragment, &shaderCompilationErr)) {
    *r_err = FROM(*r_err, shaderCompilationErr)(shaderCompilationErr);
    return true;
  }

  *r_id = glCreateProgram();
  glAttachShader(*r_id, vShader);
  glAttachShader(*r_id, fShader);
  glLinkProgram(*r_id);

  ShaderLinkErr shaderLinkErr;
  if (shaderCheckLinkErrors(*r_id, &shaderLinkErr)) {
    *r_err = FROM(*r_err, shaderLinkErr)(shaderLinkErr);
    return true;
  }

  glDeleteShader(vShader);
  glDeleteShader(fShader);

  return false;
}
