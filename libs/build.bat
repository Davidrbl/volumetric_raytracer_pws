mkdir ..\build\libs

gcc gl.c -c -I include -o ../build/libs/gl.c.o
g++ cimgui.cpp -c -I include -o ../build/libs/cimgui.cpp.o
g++ imgui.cpp -c -I include -o ../build/libs/imgui.cpp.o
g++ imgui_demo.cpp -c -I include -o ../build/libs/imgui_demo.cpp.o
g++ imgui_draw.cpp -c -I include -o ../build/libs/imgui_draw.cpp.o
g++ imgui_impl_glfw.cpp -c -I include -o ../build/libs/imgui_impl_glfw.cpp.o
g++ imgui_impl_opengl3.cpp -c -I include -o ../build/libs/imgui_impl_opengl3.cpp.o
g++ imgui_tables.cpp -c -I include -o ../build/libs/imgui_tables.cpp.o
g++ imgui_widgets.cpp -c -I include -o ../build/libs/imgui_widgets.cpp.o
