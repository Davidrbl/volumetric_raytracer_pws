.PHONY: all clean run debug

.SECONDARY:

MKDIR := mkdir -p
DEBUGGER ?= lldb
TOOLSDIR ?= tools
SRCDIR ?= src
SHADERDIR ?= $(SRCDIR)/shaders
OBJDIR ?= obj
DEPDIR ?= dep
INCDIR ?= include
SHADERS := $(wildcard $(SHADERDIR)/*)
SRCS := $(subst $(SHADERDIR)/,$(SRCDIR)/sh_,$(SHADERS:=.c)) \
        $(filter-out sh_%,$(wildcard $(SRCDIR)/*.c))
OBJS := $(subst $(SRCDIR)/,$(OBJDIR)/,$(SRCS:.c=.o))
DEPS := $(subst $(SRCDIR)/,$(DEPDIR)/,$(SRCS:.c=.d))
BIN ?= main

ifeq ($(CC),cc)
CC := clang
endif

CPPFLAGS := -I$(INCDIR) $(shell pkg-config --cflags-only-I glfw3) $(CPPFLAGS)
CFLAGS := -Wall -Wextra -Wpedantic -pipe -std=c17 \
          $(shell pkg-config --cflags-only-other glfw3) $(CFLAGS)
LDFLAGS := -fuse-ld=lld $(shell pkg-config --libs glfw3) -lm $(LDFLAGS)

DEBUGFLAGS ?= -g -glldb
SANFLAGS ?= -fsanitize=undefined,address
OPTIFLAGS ?= -flto=thin -O2

ifeq ($(DEBUG),1)
CFLAGS += $(DEBUGFLAGS)
endif
ifeq ($(SANITIZE),1)
CFLAGS += $(SANFLAGS)
endif
ifeq ($(OPTI),1)
CFLAGS += $(OPTIFLAGS)
endif

all: $(BIN)

$(SRCDIR)/sh_%.c $(INCDIR)/sh_%.h &: $(SHADERDIR)/%
	@env SHADERDIR="$(SHADERDIR)" SRCDIR="$(SRCDIR)" INCDIR="$(INCDIR)" \
	    $(TOOLSDIR)/genshader.py "$(notdir $<)"

$(DEPDIR)/%.d: $(SRCDIR)/%.c
	@$(MKDIR) "$(DEPDIR)"
	@$(CC) $(CPPFLAGS) -M "$<" | sed 's,\($*\)\.o[ :]*,$(OBJDIR)/\1.o: ,g' > "$@"

include $(DEPS)

$(OBJDIR)/%.o: $(SRCDIR)/%.c
	@$(MKDIR) "$(OBJDIR)"
	$(CC) $(CPPFLAGS) $(CFLAGS) -c "$<" -o "$@"

$(BIN): $(OBJS)
	$(CC) $(CPPFLAGS) $(CFLAGS) $(LDFLAGS) $^ -o "$@"

clean:
	$(RM) $(OBJDIR)/* $(DEPDIR)/* $(SRCDIR)/sh_* $(INCDIR)/sh_* $(BIN)

run: $(BIN)
	@./$(BIN)

debug: $(BIN)
	@$(DEBUGGER) ./$(BIN)
