#!/usr/bin/env python

import sys
import re

shader = open(sys.argv[1], "rt")

if sys.argv[2].endswith(".c"):
    out = open(sys.argv[2], "wt")
    shadername = re.sub(".*/(.*)\.c", "\\1", sys.argv[2])

    out.write(f"#include <{shadername}.h>\n\n")
    out.write(f"const GLchar* {shadername.replace('.', '_')} = \"")

    out.write(shader.read().replace("\n", "\\n\"\n\""))

    out.write("\";\n")

    out.close()
elif sys.argv[2].endswith(".h"):
    out = open(sys.argv[2], "wt")
    shadername = re.sub(".*/(.*)\.h", "\\1", sys.argv[2])

    out.write("#include <glad/gl.h>\n\n")
    out.write(f"extern const GLchar* {shadername.replace('.', '_')};\n")

    out.close()
else:
    shader.close()
    raise Exception("incorrect arguments")

shader.close()
