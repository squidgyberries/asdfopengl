#ifndef ASDFOPENGL_TYPES_H
#define ASDFOPENGL_TYPES_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

// -------- NUMERICAL TYPES --------
typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;
typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef float f32;
typedef double f64;

// Please don't kill me
#define GET_STRING(x)                                                          \
  _Generic((x), FileOpenErr                                                    \
           : getStringFileOpenErr, MemAllocErr                                 \
           : getStringMemAllocErr, ShaderTypeE                                 \
           : getStringShaderTypeE, ShaderCompilationErr                        \
           : getStringShaderCompilationErr, ShaderLinkErr                      \
           : getStringShaderLinkErr, ShaderFromFileErr                         \
           : getStringShaderFromFileErr, GLFWErr                               \
           : getStringGLFWErr, StateInitErr                                    \
           : getStringStateInitErr)

// I'm geniuninely sorry
// Need default or error
#define SHADER_FROM_FILE_ERR_FROM(x)                                           \
  _Generic((x), FileOpenErr                                                    \
           : shaderFromFileErrFromFileOpenErr, MemAllocErr                     \
           : shaderFromFileErrFromMemAllocErr, ShaderCompilationErr            \
           : shaderFromFileErrFromShaderCompilationErr, ShaderLinkErr          \
           : shaderFromFileErrFromShaderLinkErr, default                       \
           : NULL)

#define STATE_INIT_ERR_FROM(x)                                                 \
  _Generic((x), GLFWErr                                                        \
           : stateInitErrFromGLFWErr, ShaderFromFileErr                        \
           : stateInitErrFromShaderFromFileErr, default                        \
           : NULL)

#define FROM(x, y)                                                             \
  _Generic((x), ShaderFromFileErr                                              \
           : SHADER_FROM_FILE_ERR_FROM(y), StateInitErr                        \
           : STATE_INIT_ERR_FROM(y), default                                   \
           : NULL)

// FileOpenErr
typedef struct FileOpenErr {
  const char *filePath;
} FileOpenErr;

void getStringFileOpenErr(char *buf, size_t bufSize, FileOpenErr err);

// MemAllocErr
typedef struct MemAllocErr {
  size_t size;
} MemAllocErr;

void getStringMemAllocErr(char *buf, size_t bufSize, MemAllocErr err);

// ShaderTypeE
typedef enum ShaderTypeE {
  ShaderVertex,
  ShaderFragment,
  ShaderGeometry,
  ShaderTessControl,
  ShaderTessEval,
  ShaderCompute
} ShaderTypeE;

void getStringShaderTypeE(char *buf, size_t bufSize, ShaderTypeE type);

// ShaderCompilationErr
typedef struct ShaderCompilationErr {
  ShaderTypeE type;
  const char *infoLog;
} ShaderCompilationErr;

void getStringShaderCompilationErr(char *buf, size_t bufSize,
                                   ShaderCompilationErr err);

// ShaderLinkErr
typedef struct ShaderLinkErr {
  const char *infoLog;
} ShaderLinkErr;

void getStringShaderLinkErr(char *buf, size_t bufSize, ShaderLinkErr err);

// ShaderFromFileErr
typedef enum ShaderFromFileErrE {
  SFFEEFileOpenErr,
  SFFEEMemAllocErr,
  SFFEEShaderCompilationErr,
  SFFEEShaderLinkErr
} ShaderFromFileErrE;

typedef union ShaderFromFileErrU {
  FileOpenErr foe;
  MemAllocErr mae;
  ShaderCompilationErr sce;
  ShaderLinkErr sle;
} ShaderFromFileErrU;

typedef struct ShaderFromFileErr {
  ShaderFromFileErrE type;
  ShaderFromFileErrU inner;
} ShaderFromFileErr;

ShaderFromFileErr shaderFromFileErrFromFileOpenErr(FileOpenErr err);
ShaderFromFileErr shaderFromFileErrFromMemAllocErr(MemAllocErr err);
ShaderFromFileErr
shaderFromFileErrFromShaderCompilationErr(ShaderCompilationErr err);
ShaderFromFileErr shaderFromFileErrFromShaderLinkErr(ShaderLinkErr err);

void getStringShaderFromFileErr(char *buf, size_t bufSize,
                                ShaderFromFileErr err);

// GLFWErr
typedef struct GLFWErr {
  int code;
  const char *description;
} GLFWErr;

void getStringGLFWErr(char *buf, size_t bufSize, GLFWErr err);

// StateInitErr
typedef enum StateInitErrE { SIEEGLFWErr, SIEEShaderFromFileErr } StateInitErrE;

typedef union StateInitErrU {
  GLFWErr ge;
  ShaderFromFileErr sffe;
} StateInitErrU;

typedef struct StateInitErr {
  StateInitErrE type;
  StateInitErrU inner;
} StateInitErr;

StateInitErr stateInitErrFromGLFWErr(GLFWErr err);
StateInitErr stateInitErrFromShaderFromFileErr(ShaderFromFileErr err);

void getStringStateInitErr(char *buf, size_t bufSize, StateInitErr err);

#endif // ASDFOPENGL_TYPES_H
