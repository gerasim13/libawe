LIB_NAME := libawe.a

CXX ?= g++

SRC_EXT = cpp
SRC_PATH = ./source

COMPILE_FLAGS = -std=c++11 -Wall -Wextra -g
RCOMPILE_FLAGS = -O3 -D NDEBUG
DCOMPILE_FLAGS = -O0 -D DEBUG

INCLUDES = -I $(SRC_PATH)/ -lportaudio -lsndfile -lsoxr

LINK_FLAGS = -lportaudio -lsndfile -lsoxr
RLINK_FLAGS = 
DLINK_FLAGS = 

DESTDIR = /

BUILD_PREFIX = build/unix
INSTALL_PREFIX = usr/local


# Shell used in this makefile
# bash is used for 'echo -en'
SHELL = /bin/bash

# Clear built-in rules
.SUFFIXES:

# Programs for installation
INSTALL = install
INSTALL_PROGRAM = $(INSTALL)
INSTALL_DATA = $(INSTALL) -m 644

# Verbose option, to output compile and link commands
export VERBOSE := false
export CMD_PREFIX := @
ifeq ($(VERBOSE),true)
	CMD_PREFIX := 
endif

# Combine compiler and linker flags
release: export CXXFLAGS := $(CXXFLAGS) $(COMPILE_FLAGS) $(RCOMPILE_FLAGS)
release: export LDFLAGS := $(LDFLAGS) $(LINK_FLAGS) $(RLINK_FLAGS)
debug: export CXXFLAGS := $(CXXFLAGS) $(COMPILE_FLAGS) $(DCOMPILE_FLAGS)
debug: export LDFLAGS := $(LDFLAGS) $(LINK_FLAGS) $(DLINK_FLAGS)

# Build and output paths
release: export BUILD_PATH := $(BUILD_PREFIX)/release
release: export LIB_PATH := bin/release
debug: export BUILD_PATH := $(BUILD_PREFIX)/debug
debug: export LIB_PATH := bin/debug
install: export LIB_PATH := bin/release

# Find all source files in the source directory, sorted by most
# recently modified
SOURCES = $(shell find $(SRC_PATH)/ -name '*.$(SRC_EXT)' -printf '%T@\t%p\n' \
					| sort -k 1nr | cut -f2-)
# fallback in case the above fails
rwildcard = $(foreach d, $(wildcard $1*), $(call rwildcard,$d/,$2) \
						$(filter $(subst *,%,$2), $d))
ifeq ($(SOURCES),)
	SOURCES := $(call rwildcard, $(SRC_PATH)/, *.$(SRC_EXT))
endif

# Set the object file names, with the source directory stripped
# from the path, and the build path prepended in its place
LIB_OBJECTS = $(SOURCES:$(SRC_PATH)/%.$(SRC_EXT)=$(BUILD_PATH)/%.o)
# Set the dependency files that will be used to add header dependencies
DEPS = $(LIB_OBJECTS:.o=.d)

# Standard, non-optimized release build
.PHONY: release
release: dirs
	@echo "Beginning release build"
	@$(MAKE) all --no-print-directory

# Debug build for gdb debugging
.PHONY: debug
debug: dirs
	@echo "Beginning debug build"
	@$(MAKE) all --no-print-directory

# Create the directories used in the build
.PHONY: dirs
dirs:
	@echo "Creating directories"
	@mkdir -p $(dir $(LIB_OBJECTS))
	@mkdir -p $(LIB_PATH)

# Installs to the set path
.PHONY: install
install:
	@echo "Installing to $(DESTDIR)$(INSTALL_PREFIX)/lib"
	@$(INSTALL_PROGRAM) $(LIB_PATH)/$(LIB_NAME) $(DESTDIR)$(INSTALL_PREFIX)/lib

# Uninstalls the library
.PHONY: uninstall
uninstall:
	@echo "Removing $(DESTDIR)$(INSTALL_PREFIX)/lib/$(LIB_NAME)"
	@$(RM) $(DESTDIR)$(INSTALL_PREFIX)/lib/$(LIB_NAME)

# Removes all build files
.PHONY: clean
clean:
	@echo "Deleting $(LIB_NAME) symlink"
	$(CMD_PREFIX)$(RM) $(LIB_NAME)
	@echo "Deleting directories"
	$(CMD_PREFIX)$(RM) -r $(BUILD_PREFIX)
	$(CMD_PREFIX)$(RM) -r bin

# Main rule, checks the executable and symlinks to the output
all: $(LIB_PATH)/$(LIB_NAME)
	@echo "Making symlink: $(LIB_NAME) -> $<"
	@$(RM) $(LIB_NAME)
	@ln -s $(LIB_PATH)/$(LIB_NAME) $(LIB_NAME)

# Link the library
$(LIB_PATH)/$(LIB_NAME): $(LIB_OBJECTS)
	@echo "Linking: $@"
	$(CMD_PREFIX)$(AR) rcs $@ $(LIB_OBJECTS)

# Add dependency files, if they exist
-include $(DEPS)

# Source file rules
# After the first compilation they will be joined with the rules from the
# dependency files to provide header dependencies
$(BUILD_PATH)/%.o: $(SRC_PATH)/%.$(SRC_EXT)
	@echo "Compiling: $< -> $@"
	$(CMD_PREFIX)$(CXX) $(CXXFLAGS) $(INCLUDES) -MP -MMD -c $< -o $@

