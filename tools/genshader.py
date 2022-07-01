#!/usr/bin/env python

import sys
import os

shader = open(f"{os.getenv('SHADERDIR')}/{sys.argv[1]}", "rt")
src = open(f"{os.getenv('SRCDIR')}/sh_{sys.argv[1]}.c", "wt")
header = open(f"{os.getenv('INCDIR')}/sh_{sys.argv[1]}.h", "wt")

src.write(f"#include <sh_{sys.argv[1]}.h>\n\n")
src.write("#pragma GCC diagnostic ignored \"-Woverlength-strings\"\n")
src.write(f"const GLchar sh_{sys.argv[1].replace('.', '_')}[] = \"")

src.write(shader.read().replace("\n", "\\n\"\n\""))

src.write("\";\n")

header.write(f"#ifndef SH_{sys.argv[1].replace('.', '_').upper()}_H\n")
header.write(f"#define SH_{sys.argv[1].replace('.', '_').upper()}_H\n\n")
header.write("#include <glad/gl.h>\n\n")
header.write(f"extern const GLchar sh_{sys.argv[1].replace('.', '_')}[];\n\n")
header.write("#endif\n")

shader.close()
src.close()
header.close()
