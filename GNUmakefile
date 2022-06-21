.PHONY: all clean run

MKDIR := mkdir -p
SRCDIR ?= src
OBJDIR ?= obj
DEPDIR ?= dep
INCDIR ?= include
SRCS := $(wildcard $(SRCDIR)/*.c)
OBJS := $(subst $(SRCDIR)/,$(OBJDIR)/,$(SRCS:.c=.o))
DEPS := $(subst $(SRCDIR)/,$(DEPDIR)/,$(SRCS:.c=.d))
BIN ?= main

CC := clang
CPPFLAGS += -I$(INCDIR) $(shell pkg-config --cflags-only-I glfw3)
CFLAGS += -Wall -Wextra -Wpedantic -march=native -pipe -std=c17 -O3 -flto=thin \
	$(shell pkg-config --cflags-only-other glfw3)
LDFLAGS += -fuse-ld=lld $(shell pkg-config --libs glfw3)

all: $(BIN)

$(DEPDIR)/%.d: $(SRCDIR)/%.c
	@$(MKDIR) "$(DEPDIR)"
	@$(CC) $(CPPFLAGS) -M "$<" | sed 's,\($*\)\.o[ :]*,$(OBJDIR)/\1.o $@: ,g' > "$@"

include $(DEPS)

$(OBJDIR)/%.o: $(SRCDIR)/%.c
	@$(MKDIR) "$(OBJDIR)"
	$(CC) $(CPPFLAGS) $(CFLAGS) -c "$<" -o "$@"

$(BIN): $(OBJS)
	$(CC) $(CPPFLAGS) $(CFLAGS) $(LDFLAGS) $^ -o "$@"

clean:
	$(RM) $(OBJDIR)/* $(DEPDIR)/* $(BIN)

run: $(BIN)
	@./$(BIN)
