:: gcc src/main.c src/shader.c src/texture.c src/logging.c src/gl.c glfw3.dll -o main.exe -I include -std=c17 -Wall -Wextra -Wpedantic

gcc src/main.c -c -I include -I cimgui -std=c17 -Wall -Wextra -Wpedantic -o object_files/main.o
gcc src/shader.c -c -I include -I cimgui -std=c17 -Wall -Wextra -Wpedantic -o object_files/shader.o
gcc src/texture.c -c -I include -I cimgui -std=c17 -Wall -Wextra -Wpedantic -o object_files/texture.o
gcc src/logging.c -c -I include -I cimgui -std=c17 -Wall -Wextra -Wpedantic -o object_files/logging.o
gcc src/gl.c -c -I include -I cimgui -std=c17 -Wall -Wextra -Wpedantic -o object_files/gl.o

g++ ^
object_files/main.o ^
object_files/shader.o ^
object_files/texture.o ^
object_files/logging.o ^
object_files/gl.o ^
cimgui/cimgui.o ^
cimgui/imgui.o ^
cimgui/imgui_demo.o ^
cimgui/imgui_draw.o ^
cimgui/imgui_impl_glfw.o ^
cimgui/imgui_impl_opengl3.o ^
cimgui/imgui_tables.o ^
cimgui/imgui_widgets.o ^
-lglfw3 ^
-o main.exe
