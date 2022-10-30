#ifndef LEARNOPENGL_SHADER_H
#define LEARNOPENGL_SHADER_H

#include "types.h"

#include "gl.h"

#include <stddef.h>

// returns:
//   false if no error occured
//   true if error occured and error was returned
bool shaderCheckCompileErrors(u32 shader, ShaderTypeE type, ShaderCompilationErr *r_err);

// returns:
//   false if no error occured
//   true if error occured and error was returned
bool shaderCheckLinkErrors(u32 shader, ShaderLinkErr *r_err);

// returns:
//   false if no error occured and result was returned
//   true if error occured and error was returned
bool shaderFromFile(const char *vertexPath, const char *fragmentPath, u32 *r_id,
                   ShaderFromFileErr *r_err);

#endif // LEARNOPENGL_SHADER_H
