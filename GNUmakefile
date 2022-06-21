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

ifeq ($(CC),cc)
CC := clang
endif

CPPFLAGS := -I$(INCDIR) $(shell pkg-config --cflags-only-I glfw3) $(CPPFLAGS)
CFLAGS := -Wall -Wextra -Wpedantic -march=native -pipe -std=c17 \
          $(shell pkg-config --cflags-only-other glfw3) $(CFLAGS)
LDFLAGS := -fuse-ld=lld $(shell pkg-config --libs glfw3) $(LDFLAGS)

DEBUGFLAGS ?= -g -glldb -fsanitize=undefined,address
DEBUGOPTIFLAGS ?= $(DEBUGFLAGS) -flto=thin -O2
OPTIFLAGS ?= -flto=thin -O3

ifeq ($(MODE),debug)
CFLAGS += $(DEBUGFLAGS)
else ifeq ($(MODE),debugopti)
CFLAGS += $(DEBUGOPTIFLAGS)
else ifeq ($(MODE),opti)
CFLAGS += $(OPTIFLAGS)
endif

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
