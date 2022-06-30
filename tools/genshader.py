#!/usr/bin/env python

import sys
import os

shader = open(sys.argv[1], "rt")
src = open(f"{os.getenv('SRCDIR')}/{sys.argv[2]}.c", "wt")
header = open(f"{os.getenv('INCDIR')}/{sys.argv[2]}.h", "wt")

src.write(f"#include <{sys.argv[2]}.h>\n\n")
src.write(f"const GLchar* {sys.argv[2].replace('.', '_')} = \"")

src.write(shader.read().replace("\n", "\\n\"\n\""))

src.write("\";\n")

header.write("#include <glad/gl.h>\n\n")
header.write(f"extern const GLchar* {sys.argv[2].replace('.', '_')};\n")

shader.close()
src.close()
header.close()
