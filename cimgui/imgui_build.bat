@echo -----------------------------BEGIN-----------------------------

g++ imgui.cpp -I ../include -c -o imgui.o
g++ imgui_demo.cpp -I ../include -c -o imgui_demo.o
g++ imgui_draw.cpp -I ../include -c -o imgui_draw.o
g++ imgui_impl_glfw.cpp -I ../include -c -o imgui_impl_glfw.o
g++ imgui_impl_opengl3.cpp -I ../include -c -o imgui_impl_opengl3.o
g++ imgui_tables.cpp -I ../include -c -o imgui_tables.o
g++ imgui_widgets.cpp -I ../include -c -o imgui_widgets.o
g++ cimgui.cpp -I ../include -c -o cimgui.o

@echo ------------------------------END------------------------------
:: g++ cimgui_impl_wrap.cpp -c -o cimgui_impl_wrap.o

:: imgui.cpp imgui_demo.cpp imgui_draw.cpp imgui_impl_glfw.cpp imgui_impl_opengl3.cpp imgui_tables.cpp imgui_widgets.cpp 