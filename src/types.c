#include "types.h"

#include <stdio.h>

// FileOpenErr
void getStringFileOpenErr(char *buf, size_t bufSize, FileOpenErr err) {
  snprintf(buf, bufSize, "Opening file %s failed", err.filePath);
}

// MemAllocErr
void getStringMemAllocErr(char *buf, size_t bufSize, MemAllocErr err) {
  snprintf(buf, bufSize, "Allocating memory of size %zu failed", err.size);
}

// ShaderType
void getStringShaderTypeE(char *buf, size_t bufSize, ShaderTypeE type) {
  switch (type) {
  case ShaderVertex:
    snprintf(buf, bufSize, "vertex");
    break;
  case ShaderFragment:
    snprintf(buf, bufSize, "fragment");
    break;
  case ShaderGeometry:
    snprintf(buf, bufSize, "geometry");
    break;
  case ShaderTessControl:
    snprintf(buf, bufSize, "tessellation control");
    break;
  case ShaderTessEval:
    snprintf(buf, bufSize, "tessellation evaluation");
    break;
  case ShaderCompute:
    snprintf(buf, bufSize, "compute");
    break;
  }
}

// ShaderCompilationErr
void getStringShaderCompilationErr(char *buf, size_t bufSize,
                                   ShaderCompilationErr err) {
  char typeBuf[24];
  GET_STRING(err.type)(typeBuf, 24, err.type);
  snprintf(buf, bufSize, "Compiling %s shader failed:\n\n%s", typeBuf,
           err.infoLog);
}

// ShaderLinkErr
void getStringShaderLinkErr(char *buf, size_t bufSize, ShaderLinkErr err) {
  snprintf(buf, bufSize, "Linking shader failed:\n\n%s", err.infoLog);
}

// ShaderFromFileErr
ShaderFromFileErr shaderFromFileErrFromFileOpenErr(FileOpenErr err) {
  return (ShaderFromFileErr){SFFEEFileOpenErr,
                             (ShaderFromFileErrU){.foe = err}};
}
ShaderFromFileErr shaderFromFileErrFromMemAllocErr(MemAllocErr err) {
  return (ShaderFromFileErr){SFFEEMemAllocErr,
                             (ShaderFromFileErrU){.mae = err}};
}
ShaderFromFileErr
shaderFromFileErrFromShaderCompilationErr(ShaderCompilationErr err) {
  return (ShaderFromFileErr){SFFEEShaderCompilationErr,
                             (ShaderFromFileErrU){.sce = err}};
}
ShaderFromFileErr shaderFromFileErrFromShaderLinkErr(ShaderLinkErr err) {
  return (ShaderFromFileErr){SFFEEShaderLinkErr,
                             (ShaderFromFileErrU){.sle = err}};
}

void getStringShaderFromFileErr(char *buf, size_t bufSize,
                                ShaderFromFileErr err) {
  switch (err.type) {
  case SFFEEFileOpenErr:
    GET_STRING(err.inner.foe)(buf, bufSize, err.inner.foe);
    break;
  case SFFEEMemAllocErr:
    GET_STRING(err.inner.mae)(buf, bufSize, err.inner.mae);
    break;
  case SFFEEShaderCompilationErr:
    GET_STRING(err.inner.sce)(buf, bufSize, err.inner.sce);
    break;
  case SFFEEShaderLinkErr:
    GET_STRING(err.inner.sle)(buf, bufSize, err.inner.sle);
    break;
  }
}

// GLFWErr
void getStringGLFWErr(char *buf, size_t bufSize, GLFWErr err) {
  snprintf(buf, bufSize, "GLFW error %d: %s", err.code, err.description);
}

// StateInitErr
StateInitErr stateInitErrFromGLFWErr(GLFWErr err) {
  return (StateInitErr){SIEEGLFWErr, (StateInitErrU){.ge = err}};
}
StateInitErr stateInitErrFromShaderFromFileErr(ShaderFromFileErr err) {
  return (StateInitErr){SIEEShaderFromFileErr, (StateInitErrU){.sffe = err}};
}

void getStringStateInitErr(char *buf, size_t bufSize, StateInitErr err) {
  switch (err.type) {
  case SIEEGLFWErr:
    GET_STRING(err.inner.ge)(buf, bufSize, err.inner.ge);
    break;
  case SIEEShaderFromFileErr:
    GET_STRING(err.inner.sffe)(buf, bufSize, err.inner.sffe);
    break;
  }
}
