#include "log.h"
#include "shader.h"
#include "types.h"

#include <math.h>

#include "gl.h"

#include <GLFW/glfw3.h>

#define NK_INCLUDE_FIXED_TYPES
#define NK_INCLUDE_STANDARD_BOOL
#define NK_INCLUDE_STANDARD_IO
#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_INCLUDE_VERTEX_BUFFER_OUTPUT
#define NK_INCLUDE_FONT_BAKING
#define NK_INCLUDE_DEFAULT_FONT
#define NK_KEYSTATE_BASED_INPUT
#include "nuklear/nuklear.h"
#include "nuklear/nuklear_glfw_gl3.h"

#include "stb_image/stb_image.h"
#include <cglm/cglm.h>

#define WINDOW_WIDTH 1600
#define WINDOW_HEIGHT 1000

// Nuklear stuff
#define MAX_VERTEX_BUFFER 512 * 1024
#define MAX_ELEMENT_BUFFER 128 * 1024

// clang-format off
const f32 vertices[] = {
  -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
   0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
   0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
   0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
  -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
  -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,

  -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
   0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
   0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
   0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
  -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
  -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

  -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
  -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
  -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
  -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
  -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
  -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

   0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
   0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
   0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
   0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
   0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
   0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

  -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
   0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
   0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
   0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
  -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
  -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

  -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
   0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
   0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
   0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
  -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
  -0.5f,  0.5f,  0.5f,  0.0f, 0.0f
};
vec3 cubePositions[] = {
  { 0.0f,  0.0f,  0.0f },
  { 2.0f,  5.0f, -15.0f},
  {-1.5f, -2.2f, -2.5f },
  {-3.8f, -2.0f, -12.3f},
  { 2.4f, -0.4f, -3.5f },
  {-1.7f,  3.0f, -7.5f },
  { 1.3f, -2.0f, -2.5f },
  { 1.5f,  2.0f, -2.5f },
  { 1.5f,  0.2f, -1.5f },
  {-1.3f,  1.0f, -1.5f }
};
// clang-format on

typedef struct State {
  // Camera
  vec3 cameraPos;
  vec3 cameraFront;
  vec3 cameraUp;
  f32 yaw;
  f32 pitch;
  f32 fov;
  f32 nearZ;
  f32 farZ;

  // Frame rate stuff
  f32 deltaTime;
  f32 lastFrame;

  // Input stuff
  f32 lastX;
  f32 lastY;
  f32 sensitivity;
  bool firstMouse;

  // Other
  bool wireframe;
  bool cursor;
  f32 mixValue;
  i32 rotateSpeed;

  // GLFW stuff
  GLFWwindow *window;

  // // Nuklear stuff
  // struct nk_glfw nkGlfw;
  // struct nk_context *nkCtx;

  // OpenGL stuff
  u32 shaderProgram;
  u32 vao;
  u32 vbo;
  u32 texture1;
  u32 texture2;
} State;

static inline f32 clampf(f32 v, f32 min, f32 max) {
  const f32 t = v < min ? min : v;
  return t > max ? max : t;
}

void framebufferSizeCallback(GLFWwindow *window, int width, int height);
void cursorPosCallback(GLFWwindow *window, double xposIn, double yposIn);
void keyCallback(GLFWwindow *window, int key, int scancode, int action,
                 int mods);
void processInput(GLFWwindow *window, State *state);
bool getGlfwError(GLFWErr *r_err);
void setOpenGLState(void);

bool stateInit(State *r_state, StateInitErr *r_err) {
  // -------- MISC --------
  stbi_set_flip_vertically_on_load(1);

  // -------- WINDOWING AND CONTEXT CREATION --------
  GLFWErr glfwErr;
  glfwInit();
  if (getGlfwError(&glfwErr)) {
    *r_err = FROM(*r_err, glfwErr)(glfwErr);
    return true;
  }
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif // __APPLE__
  glfwWindowHint(GLFW_SAMPLES, 4);

  GLFWwindow *window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT,
                                        "HELLO YES I AM A TITLE", NULL, NULL);
  if (getGlfwError(&glfwErr)) {
    *r_err = FROM(*r_err, glfwErr)(glfwErr);
    return true;
  }

  glfwMakeContextCurrent(window);
  if (getGlfwError(&glfwErr)) {
    *r_err = FROM(*r_err, glfwErr)(glfwErr);
    return true;
  }

  // User pointer for state
  glfwSetWindowUserPointer(window, r_state);

  // Input callbacks
  glfwSetCursorPosCallback(window, cursorPosCallback);
  glfwSetKeyCallback(window, keyCallback);

  // Viewport resize callback
  glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);

  // Initialize GLAD
  int version = gladLoadGL(glfwGetProcAddress);
  logMsg(LOG_INFO, "Using OpenGL version %d.%d", GLAD_VERSION_MAJOR(version),
         GLAD_VERSION_MINOR(version));

  // Set swap interval to monitor refresh rate
  glfwSwapInterval(1);

  // // Nuklear setup
  // struct nk_glfw nkGlfw = {0};
  // struct nk_context *nkCtx;

  // nkCtx = nk_glfw3_init(&nkGlfw, window, NK_GLFW3_INSTALL_CALLBACKS);
  // {
  //   struct nk_font_atlas *atlas;
  //   nk_glfw3_font_stash_begin(&nkGlfw, &atlas);
  //   struct nk_font *jetbrains_mono = nk_font_atlas_add_from_file(
  //       atlas, "resources/JetBrainsMonoNL-Regular.ttf", 16.0f, NULL);
  //   nk_glfw3_font_stash_end(&nkGlfw);
  //   // nk_style_load_all_cursors(nkCtx, atlas->cursors);
  //   nk_style_set_font(nkCtx, &jetbrains_mono->handle);
  // }

  // -------- OPENGL SETUP --------
  // Configure global state
  setOpenGLState();

  // Shaders
  u32 shaderProgram;
  ShaderFromFileErr shaderFromFileErr;
  if (shaderFromFile("resources/shaders/shader.vert",
                     "resources/shaders/shader.frag", &shaderProgram,
                     &shaderFromFileErr)) {
    *r_err = FROM(*r_err, shaderFromFileErr)(shaderFromFileErr);
    return true;
  }

  // Buffers
  u32 vao, vbo;
  glGenVertexArrays(1, &vao);
  glGenBuffers(1, &vbo);

  glBindVertexArray(vao);

  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  const GLsizei stride = 5 * sizeof(f32);

  // Position
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void *)0);
  glEnableVertexAttribArray(0);

  // Texture coords
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride,
                        (void *)(sizeof(f32) * 3));
  glEnableVertexAttribArray(1);

  // Texture 1
  u32 texture1;

  glGenTextures(1, &texture1);
  glBindTexture(GL_TEXTURE_2D, texture1);

  f32 borderColor[] = {0.0f, 0.0f, 0.0f, 1.0f};
  glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

  // Mipmap options for minification:
  // GL_NEAREST_MIPMAP_NEAREST, nearest mipmap level and nearest neighbor
  // interpolation GL_LINEAR_MIPMAP_NEAREST, nearest mipmap level and linear
  // interpolation GL_NEAREST_MIPMAP_LINEAR, linearly interpolates mipmaps and
  // nearest neighbor interpolation GL_LINEAR_MIPMAP_LINEAR, linearly
  // interpolates mipmaps and linear interpolation
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                  GL_NEAREST_MIPMAP_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

  int width, height;
  // Force 4 components, rgba
  unsigned char *data =
      stbi_load("resources/textures/awesomeface.png", &width, &height, NULL, 4);

  if (!data) {
    logMsg(LOG_ERROR, "Loading image %s failed: %s",
           "resources/textures/awesomeface.png", stbi_failure_reason());
    glfwTerminate();
    return EXIT_FAILURE;
  }

  // Second param is mipmap level to create texture for, sixth is legacy stuff
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA,
               GL_UNSIGNED_BYTE, data);
  glGenerateMipmap(GL_TEXTURE_2D);

  stbi_image_free(data);

  // Texture 2
  u32 texture2;

  glGenTextures(1, &texture2);
  glBindTexture(GL_TEXTURE_2D, texture2);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

  // Force 4 components, rgba
  data =
      stbi_load("resources/textures/container.jpg", &width, &height, NULL, 4);

  if (!data) {
    logMsg(LOG_ERROR, "Loading image %s failed: %s",
           "resources/textures/container.jpg", stbi_failure_reason());
    glfwTerminate();
    return EXIT_FAILURE;
  }

  // Second param is mipmap level to create texture for, sixth is legacy stuff
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA,
               GL_UNSIGNED_BYTE, data);
  glGenerateMipmap(GL_TEXTURE_2D);

  stbi_image_free(data);

  glUseProgram(shaderProgram);
  glUniform1i(glGetUniformLocation(shaderProgram, "texture1"), 0);
  glUniform1i(glGetUniformLocation(shaderProgram, "texture2"), 1);

  // clang-format off
  *r_state = (State){
    .cameraPos = {0.0f, 0.0f, 5.0f},
    .cameraFront = {0.0f, 0.0f, -1.0f},
    .cameraUp = {0.0f, 1.0f, 0.0f},
    .yaw = -90.0f,
    .pitch = 0.0f,
    .fov = 60.0f,
    .nearZ = 0.1f,
    .farZ = 100.0f,

    .deltaTime = 0.0f,
    .lastFrame = 0.0f,

    .lastX = (f32)WINDOW_HEIGHT / 2,
    .lastY = (f32)WINDOW_WIDTH / 2,
    .sensitivity = 0.07f,
    .firstMouse = true,

    .wireframe = false,
    .cursor = true,
    .mixValue = 0.8f,
    .rotateSpeed = 60,

    window,

    // nkGlfw,
    // nkCtx,

    shaderProgram,
    vao,
    vbo,
    texture1,
    texture2
  };
  // clang-format on

  return false;
}

int main(void) {
  State state;
  StateInitErr stateInitErr;
  if (stateInit(&state, &stateInitErr)) {
    char buf[1024];
    GET_STRING(stateInitErr)(buf, 1024, stateInitErr);
    logMsg(LOG_ERROR, "%s", buf);
    glfwTerminate();
    return EXIT_SUCCESS;
  }

  // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // Wireframe

  // Render loop
  while (!glfwWindowShouldClose(state.window)) {
    f32 currentFrame = (f32)glfwGetTime();
    state.deltaTime = currentFrame - state.lastFrame;
    state.lastFrame = currentFrame;

    // Input
    processInput(state.window, &state);

    if (state.wireframe) {
      glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    } else {
      glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }

    // Rendering
    setOpenGLState();

    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, state.texture1);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, state.texture2);

    glUseProgram(state.shaderProgram);

    glUniform1f(glGetUniformLocation(state.shaderProgram, "mixValue"),
                state.mixValue);

    mat4 projection;
    glm_perspective(glm_rad(state.fov), (f32)WINDOW_WIDTH / (f32)WINDOW_HEIGHT,
                    state.nearZ, state.farZ, projection);
    glUniformMatrix4fv(
        glGetUniformLocation(state.shaderProgram, "projection"), 1, GL_FALSE,
        (const f32 *)projection); // cglm uses column-major layout

    mat4 view;
    vec3 dir;
    glm_vec3_add(state.cameraPos, state.cameraFront, dir);
    glm_lookat(state.cameraPos, dir, state.cameraUp, view);

    glUniformMatrix4fv(glGetUniformLocation(state.shaderProgram, "view"), 1,
                       GL_FALSE, (const f32 *)view);

    glBindVertexArray(state.vao);
    for (i32 i = 0; i < 10; i++) {
      mat4 model = GLM_MAT4_IDENTITY_INIT;
      glm_translate(model, cubePositions[i]);
      f32 angle = 20.0f * i;
      if (i % 3 == 0) {
        angle +=
            fmod(glfwGetTime() * state.rotateSpeed,
                 360.0f); // Mod for smoother motion when changing rotateSpeed
      }
      glm_rotate(model, glm_rad(angle), (vec3){1.0f, 0.3f, 0.5f});
      glUniformMatrix4fv(glGetUniformLocation(state.shaderProgram, "model"), 1,
                         GL_FALSE, (const f32 *)model);

      glDrawArrays(GL_TRIANGLES, 0, 36);
    }

    // nk_glfw3_new_frame(&state.nkGlfw);

    // // GUI
    // glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); // Turn off for gui

    // if (nk_begin(state.nkCtx, "Demo", nk_rect(10, 10, 400, 400),
    //              NK_WINDOW_BORDER | NK_WINDOW_MOVABLE | NK_WINDOW_CLOSABLE))
    //              {
    //   // sensitivity
    //   nk_layout_row_dynamic(state.nkCtx, 25, 1);
    //   nk_property_float(state.nkCtx, "Sensitivity:", 0.0f,
    //   &state.sensitivity, 1.0f, 0.01f,
    //                     0.01f);

    //   // mixValue
    //   nk_layout_row_dynamic(state.nkCtx, 25, 1);
    //   nk_property_float(state.nkCtx, "Mix Value:", 0.0f,
    //   &state.mixValue, 1.0f, 0.1f,
    //                     0.005f);

    //   // rotateSpeed
    //   nk_layout_row_dynamic(state.nkCtx, 25, 1);
    //   nk_property_int(state.nkCtx, "Rotate Speed:", -360, &state.rotateSpeed,
    //   360, 2, 1);

    //   // fov
    //   nk_layout_row_dynamic(state.nkCtx, 25, 1);
    //   nk_property_float(state.nkCtx, "FOV:", 30.0f, &state.fov,
    //   110.0f, 1.0f, 1.0f);

    //   // nearZ
    //   nk_layout_row_dynamic(state.nkCtx, 25, 1);
    //   nk_property_float(state.nkCtx, "Near Z:", 0.0f, &state.nearZ, 1000.0f,
    //   0.01f, 0.05f);

    //   // farZ
    //   nk_layout_row_dynamic(state.nkCtx, 25, 1);
    //   nk_property_float(state.nkCtx, "Far Z:", 0.0f, &state.farZ,
    //   1000.0f, 1.0f, 1.0f);

    //   // wireframe
    //   nk_layout_row_dynamic(state.nkCtx, 25, 1);
    //   nk_checkbox_label(state.nkCtx, "Wireframe", &state.wireframe);
    // }
    // nk_end(state.nkCtx);
    // nk_glfw3_render(&state.nkGlfw, NK_ANTI_ALIASING_ON, MAX_VERTEX_BUFFER,
    //                 MAX_ELEMENT_BUFFER);

    // Check and call events and swap the buffers
    glfwSwapBuffers(state.window); // Double buffered for window applications
    glfwPollEvents();
  }

  glDeleteVertexArrays(1, &state.vao);
  glDeleteBuffers(1, &state.vbo);
  glDeleteShader(state.shaderProgram);

  // nk_glfw3_shutdown(&state.nkGlfw);
  glfwTerminate();
  return EXIT_SUCCESS;
}

void framebufferSizeCallback(GLFWwindow *window, int width, int height) {
  glViewport(0, 0, width, height);
}

void cursorPosCallback(GLFWwindow *window, double xposIn, double yposIn) {
  // Get state
  State *state = glfwGetWindowUserPointer(window);

  if (state->cursor) {
    return;
  }

  f32 xpos = (f32)xposIn;
  f32 ypos = (f32)yposIn;

  if (state->firstMouse) {
    state->lastX = xpos;
    state->lastY = ypos;
    state->firstMouse = false;
  }

  f32 xOffset = xpos - state->lastX;
  f32 yOffset =
      state->lastY - ypos; // Reversed because y coords go from bottom to top

  state->lastX = xpos;
  state->lastY = ypos;

  xOffset *= state->sensitivity;
  yOffset *= state->sensitivity;

  state->yaw += xOffset;
  state->pitch += yOffset;

  state->pitch = clampf(state->pitch, -89.0f, 89.0f);
  // const f32 t = state->pitch < -89.0f ? -89.0f : state->pitch;
  // state->pitch = t > 89.0f ? 89.0f : t;

  vec3 front;
  front[0] = cos(glm_rad(state->yaw)) * cos(glm_rad(state->pitch));
  front[1] = sin(glm_rad(state->pitch));
  front[2] = sin(glm_rad(state->yaw)) * cos(glm_rad(state->pitch));
  glm_normalize(front);
  glm_vec3_copy(front, state->cameraFront);
}

void keyCallback(GLFWwindow *window, int key, int scancode, int action,
                 int mods) {
  // Get state
  State *state = glfwGetWindowUserPointer(window);

  // Space for capturing mouse
  if (key == GLFW_KEY_SPACE && action == GLFW_PRESS) {
    if (state->cursor) {
      glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    } else {
      glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
      state->firstMouse = true;
    }
    state->cursor = !state->cursor;
  }
}

void processInput(GLFWwindow *window, State *state) {
  // Esc for quit
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
    glfwSetWindowShouldClose(window, GLFW_TRUE);
  }

  // Camera move
  const f32 cameraSpeed = 0.05f;
  if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
    vec3 add;
    glm_vec3_scale(state->cameraFront, cameraSpeed, add);
    glm_vec3_add(state->cameraPos, add, state->cameraPos);
  }
  if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
    vec3 sub;
    glm_vec3_scale(state->cameraFront, cameraSpeed, sub);
    glm_vec3_sub(state->cameraPos, sub, state->cameraPos);
  }
  if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
    vec3 cross;
    glm_vec3_cross(state->cameraFront, state->cameraUp, cross);
    glm_vec3_normalize(cross);
    vec3 sub;
    glm_vec3_scale(cross, cameraSpeed, sub);
    glm_vec3_sub(state->cameraPos, sub, state->cameraPos);
  }
  if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
    vec3 cross;
    glm_vec3_cross(state->cameraFront, state->cameraUp, cross);
    glm_vec3_normalize(cross);
    vec3 add;
    glm_vec3_scale(cross, cameraSpeed, add);
    glm_vec3_add(state->cameraPos, add, state->cameraPos);
  }
}

bool getGlfwError(GLFWErr *r_err) {
  int code;
  const char *desc;
  code = glfwGetError(&desc);
  if (code != GLFW_NO_ERROR) {
    r_err->code = code;
    r_err->description = desc;
    return true;
  }
  return false;
}

void setOpenGLState(void) {
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);
  glEnable(GL_MULTISAMPLE);
}
