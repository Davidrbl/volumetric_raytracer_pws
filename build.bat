@set SRCDIR=src
@set SHADERDIR=%SRCDIR%/shaders
@set INCDIR=include

@python tools/genshader.py main.vert
@python tools/genshader.py main.geom
@python tools/genshader.py main.frag

gcc src/main.c src/shader.c src/texture.c src/gl.c src/sh_main.vert.c src/sh_main.geom.c src/sh_main.frag.c glfw3.dll -o main.exe -I include
