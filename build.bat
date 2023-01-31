:: gcc src/main.c src/shader.c src/texture.c src/logging.c src/gl.c glfw3.dll -o main.exe -I include -std=c17 -Wall -Wextra -Wpedantic

mkdir build\src

gcc src/main.c -c -I include -std=c17 -Wall -Wextra -Wpedantic -o build/src/main.c.o
gcc src/shader.c -c -I include -std=c17 -Wall -Wextra -Wpedantic -o build/src/shader.c.o
gcc src/texture.c -c -I include -std=c17 -Wall -Wextra -Wpedantic -o build/src/texture.c.o
gcc src/logging.c -c -I include -std=c17 -Wall -Wextra -Wpedantic -o build/src/logging.c.o
gcc src/bmp_imp.c -c -I include -std=c17 -Wall -Wextra -Wpedantic -o build/src/bmp_imp.c.o

g++ ^
build/src/main.c.o ^
build/src/shader.c.o ^
build/src/texture.c.o ^
build/src/logging.c.o ^
build/src/bmp_imp.c.o ^
build/libs/gl.c.o ^
build/libs/cimgui.cpp.o ^
build/libs/imgui.cpp.o ^
build/libs/imgui_demo.cpp.o ^
build/libs/imgui_draw.cpp.o ^
build/libs/imgui_impl_glfw.cpp.o ^
build/libs/imgui_impl_opengl3.cpp.o ^
build/libs/imgui_tables.cpp.o ^
build/libs/imgui_widgets.cpp.o ^
-lglfw3 ^
-o main.exe
