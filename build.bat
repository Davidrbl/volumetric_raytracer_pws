:: gcc src/main.c src/shader.c src/texture.c src/logging.c src/gl.c glfw3.dll -o main.exe -I include -std=c17 -Wall -Wextra -Wpedantic

gcc src/main.c -c -I include -I cimgui -std=c17 -Wall -Wextra -Wpedantic -o obj/main.o
gcc src/shader.c -c -I include -I cimgui -std=c17 -Wall -Wextra -Wpedantic -o obj/shader.o
gcc src/texture.c -c -I include -I cimgui -std=c17 -Wall -Wextra -Wpedantic -o obj/texture.o
gcc src/logging.c -c -I include -I cimgui -std=c17 -Wall -Wextra -Wpedantic -o obj/logging.o
gcc src/gl.c -c -I include -I cimgui -std=c17 -Wall -Wextra -Wpedantic -o obj/gl.o

g++ ^
obj/main.o ^
obj/shader.o ^
obj/texture.o ^
obj/logging.o ^
obj/gl.o ^
obj/cimgui.o ^
obj/imgui.o ^
obj/imgui_demo.o ^
obj/imgui_draw.o ^
obj/imgui_impl_glfw.o ^
obj/imgui_impl_opengl3.o ^
obj/imgui_tables.o ^
obj/imgui_widgets.o ^
-lglfw3 ^
-o main.exe
