.PHONY: all clean run debug

PROJECT := volumetric-ray-tracer
VERSION := 0.1.0

CC := clang
CXX := clang++
DEBUGGER := lldb
MKDIR := mkdir -p
SRCDIR := src
LIBDIR := libs
BUILDDIR := build
INCDIRS := include
SRCS := $(shell find $(SRCDIR) $(LIBDIR) -type f -name '*.c' -o -name '*.cpp')
OBJS := $(SRCS:%=$(BUILDDIR)/%.o)
DEPS := $(SRCS:%=$(BUILDDIR)/%.d)
BIN := $(BUILDDIR)/$(PROJECT)-$(VERSION)

CPPFLAGS := $(foreach INCDIR,$(INCDIRS),-I $(INCDIR)) $(CPPFLAGS)
CFLAGS := -pipe $(CFLAGS)
CXXFLAGS := -pipe $(CXXFLAGS)
LDFLAGS := -fuse-ld=mold -lm $(LDFLAGS)

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

OWNCFLAGS ?= -std=c17 -Wall -Wextra -Wpedantic
OWNCXXFLAGS ?= -std=c++20 -Wall -Wextra -Wpedantic
DEBUGFLAGS ?= -g
SANFLAGS ?= -fsanitize=undefined,address
OPTIFLAGS ?= -O2
LTOFLAGS ?= -flto
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
ifeq ($(strip $(LTO)),1)
CFLAGS += $(LTOFLAGS)
CXXFLAGS += $(LTOFLAGS)
endif
ifeq ($(strip $(STATIC)),1)
LDFLAGS += $(STATICFLAGS)
endif

all: $(BIN)

$(BUILDDIR)/%.c.d: %.c
	@$(MKDIR) $(@D)
	@$(CC) $(CPPFLAGS) -MM $< | sed 's,$(*F)\.o[: ]*,$(BUILDDIR)/$<.o $(BUILDDIR)/$<.d: ,g' > $@

$(BUILDDIR)/%.cpp.d: %.cpp
	@$(MKDIR) $(@D)
	@$(CXX) $(CPPFLAGS) -MM $< | sed 's,$(*F)\.o[: ]*,$(BUILDDIR)/$<.o $(BUILDDIR)/$<.d: ,g' > $@

include $(DEPS)

$(BUILDDIR)/$(SRCDIR)/%.c.o: $(SRCDIR)/%.c
	@$(MKDIR) $(@D)
	$(CC) $(CPPFLAGS) $(CFLAGS) $(OWNCFLAGS) -c $< -o $@

$(BUILDDIR)/$(SRCDIR)/%.cpp.o: $(SRCDIR)/%.cpp
	@$(MKDIR) $(@D)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $(OWNCXXFLAGS) -c $< -o $@

$(BUILDDIR)/$(LIBDIR)/%.c.o: $(LIBDIR)/%.c
	@$(MKDIR) $(@D)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

$(BUILDDIR)/$(LIBDIR)/%.cpp.o: $(LIBDIR)/%.cpp
	@$(MKDIR) $(@D)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@

$(BIN): $(OBJS)
	@$(MKDIR) $(@D)
	$(CXX) $(CPPFLAGS) $(CFLAGS) $(CXXFLAGS) $(OWNCFLAGS) $(OWNCXXFLAGS) $(LDFLAGS) $^ -o $@

clean:
	@$(RM) $(shell find $(BUILDDIR) -type f -name '*.o' -o -name '*.d') $(BIN)

run: $(BIN)
	./$(BIN) $(ARGS)

debug: $(BIN)
	@$(DEBUGGER) ./$(BIN)
