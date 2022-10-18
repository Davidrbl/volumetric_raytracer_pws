#ifdef CIMGUI_USE_GLFW

typedef struct GLFWwindow GLFWwindow;
typedef struct GLFWmonitor GLFWmonitor;
struct GLFWwindow;
struct GLFWmonitor;CIMGUI_API bool igImplGlfw_InitForOpenGL(GLFWwindow* window,bool install_callbacks);
CIMGUI_API bool igImplGlfw_InitForVulkan(GLFWwindow* window,bool install_callbacks);
CIMGUI_API bool igImplGlfw_InitForOther(GLFWwindow* window,bool install_callbacks);
CIMGUI_API void igImplGlfw_Shutdown(void);
CIMGUI_API void igImplGlfw_NewFrame(void);
CIMGUI_API void igImplGlfw_InstallCallbacks(GLFWwindow* window);
CIMGUI_API void igImplGlfw_RestoreCallbacks(GLFWwindow* window);
CIMGUI_API void igImplGlfw_WindowFocusCallback(GLFWwindow* window,int focused);
CIMGUI_API void igImplGlfw_CursorEnterCallback(GLFWwindow* window,int entered);
CIMGUI_API void igImplGlfw_CursorPosCallback(GLFWwindow* window,double x,double y);
CIMGUI_API void igImplGlfw_MouseButtonCallback(GLFWwindow* window,int button,int action,int mods);
CIMGUI_API void igImplGlfw_ScrollCallback(GLFWwindow* window,double xoffset,double yoffset);
CIMGUI_API void igImplGlfw_KeyCallback(GLFWwindow* window,int key,int scancode,int action,int mods);
CIMGUI_API void igImplGlfw_CharCallback(GLFWwindow* window,unsigned int c);
CIMGUI_API void igImplGlfw_MonitorCallback(GLFWmonitor* monitor,int event);

#endif
#ifdef CIMGUI_USE_OPENGL3
CIMGUI_API bool igImplOpenGL3_Init(const char* glsl_version);
CIMGUI_API void igImplOpenGL3_Shutdown(void);
CIMGUI_API void igImplOpenGL3_NewFrame(void);
CIMGUI_API void igImplOpenGL3_RenderDrawData(ImDrawData* draw_data);
CIMGUI_API bool igImplOpenGL3_CreateFontsTexture(void);
CIMGUI_API void igImplOpenGL3_DestroyFontsTexture(void);
CIMGUI_API bool igImplOpenGL3_CreateDeviceObjects(void);
CIMGUI_API void igImplOpenGL3_DestroyDeviceObjects(void);

#endif
#ifdef CIMGUI_USE_OPENGL2
CIMGUI_API bool ImGui_ImplOpenGL2_Init(void);
CIMGUI_API void ImGui_ImplOpenGL2_Shutdown(void);
CIMGUI_API void ImGui_ImplOpenGL2_NewFrame(void);
CIMGUI_API void ImGui_ImplOpenGL2_RenderDrawData(ImDrawData* draw_data);
CIMGUI_API bool ImGui_ImplOpenGL2_CreateFontsTexture(void);
CIMGUI_API void ImGui_ImplOpenGL2_DestroyFontsTexture(void);
CIMGUI_API bool ImGui_ImplOpenGL2_CreateDeviceObjects(void);
CIMGUI_API void ImGui_ImplOpenGL2_DestroyDeviceObjects(void);

#endif
#ifdef CIMGUI_USE_SDL

typedef struct SDL_Window SDL_Window;
typedef struct SDL_Renderer SDL_Renderer;
struct SDL_Window;
struct SDL_Renderer;
typedef union SDL_Event SDL_Event;CIMGUI_API bool ImGui_ImplSDL2_InitForOpenGL(SDL_Window* window,void* sdl_gl_context);
CIMGUI_API bool ImGui_ImplSDL2_InitForVulkan(SDL_Window* window);
CIMGUI_API bool ImGui_ImplSDL2_InitForD3D(SDL_Window* window);
CIMGUI_API bool ImGui_ImplSDL2_InitForMetal(SDL_Window* window);
CIMGUI_API bool ImGui_ImplSDL2_InitForSDLRenderer(SDL_Window* window,SDL_Renderer* renderer);
CIMGUI_API void ImGui_ImplSDL2_Shutdown(void);
CIMGUI_API void ImGui_ImplSDL2_NewFrame(void);
CIMGUI_API bool ImGui_ImplSDL2_ProcessEvent(const SDL_Event* event);

#endif
