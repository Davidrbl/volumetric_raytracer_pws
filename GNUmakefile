.PHONY: all clean run debug

CC := clang
CXX := clang++
DEBUGGER := lldb
MKDIR := mkdir -p
SRCDIR := src
OBJDIR := obj
DEPDIR := dep
INCDIR := include
CSRCS := $(wildcard $(SRCDIR)/*.c)
CXXSRCS := $(wildcard $(SRCDIR)/*.cpp)
OBJS := $(subst $(SRCDIR)/,$(OBJDIR)/,$(CSRCS:.c=.o)) \
        $(subst $(SRCDIR)/,$(OBJDIR)/,$(CXXSRCS:.cpp=.o))
DEPS := $(subst $(SRCDIR)/,$(DEPDIR)/,$(CSRCS:.c=.d)) \
        $(subst $(SRCDIR)/,$(DEPDIR)/,$(CXXSRCS:.cpp=.d))
BIN ?= main

CPPFLAGS := -I$(INCDIR) $(CPPFLAGS)
CFLAGS := -Wall -Wextra -Wpedantic -pipe -std=c17 $(CFLAGS)
CXXFLAGS := -Wall -Wextra -Wpedantic -pipe -std=c++20 $(CXXFLAGS)
LDFLAGS := -fuse-ld=lld -lm $(LDFLAGS)

LIBS := glfw3

ifneq ($(strip $(LIBS)),)
ifeq ($(strip $(STATIC)),1)
CPPFLAGS += $(shell pkg-config --static --cflags-only-I $(LIBS))
CFLAGS += $(shell pkg-config --static --cflags-only-other $(LIBS))
CXXFLAGS += $(shell pkg-config --static --cflags-only-other $(LIBS))
LDFLAGS += $(shell pkg-config --static --libs $(LIBS))
else
CPPFLAGS += $(shell pkg-config --cflags-only-I $(LIBS))
CFLAGS += $(shell pkg-config --cflags-only-other $(LIBS))
CXXFLAGS += $(shell pkg-config --cflags-only-other $(LIBS))
LDFLAGS += $(shell pkg-config --libs $(LIBS))
endif
endif

DEBUGFLAGS ?= -g -glldb
SANFLAGS ?= -fsanitize=undefined,address
OPTIFLAGS ?= -flto=thin -O2
STATICFLAGS ?= -static

ifeq ($(strip $(DEBUG)),1)
CFLAGS += $(DEBUGFLAGS)
CXXFLAGS += $(DEBUGFLAGS)
endif
ifeq ($(strip $(SANITIZE)),1)
CFLAGS += $(SANFLAGS)
CXXFLAGS += $(SANFLAGS)
endif
ifeq ($(strip $(OPTI)),1)
CFLAGS += $(OPTIFLAGS)
CXXFLAGS += $(OPTIFLAGS)
endif
ifeq ($(strip $(STATIC)),1)
LDFLAGS += $(STATICFLAGS)
endif

all: $(BIN)

$(DEPDIR)/%.d: $(SRCDIR)/%.c
	@$(MKDIR) "$(DEPDIR)"
	@$(CC) $(CPPFLAGS) -MM "$<" | sed 's,\($*\)\.o[ :]*,$(OBJDIR)/\1.o: ,g' > "$@"

$(DEPDIR)/%.d: $(SRCDIR)/%.cpp
	@$(MKDIR) "$(DEPDIR)"
	@$(CXX) $(CPPFLAGS) -MM "$<" | sed 's,\($*\)\.o[ :]*,$(OBJDIR)/\1.o: ,g' > "$@"

include $(DEPS)

$(OBJDIR)/%.o: $(SRCDIR)/%.c
	@$(MKDIR) "$(OBJDIR)"
	$(CC) $(CPPFLAGS) $(CFLAGS) -c "$<" -o "$@"

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	@$(MKDIR) "$(OBJDIR)"
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c "$<" -o "$@"

$(BIN): $(OBJS)
	$(CXX) $(CPPFLAGS) $(CFLAGS) $(CXXFLAGS) $(LDFLAGS) $^ -o "$@"

clean:
	$(RM) $(OBJDIR)/*.o $(DEPDIR)/*.d $(BIN)

run: $(BIN)
	@./$(BIN) $(ARGS)

debug: $(BIN)
	@$(DEBUGGER) ./$(BIN)
