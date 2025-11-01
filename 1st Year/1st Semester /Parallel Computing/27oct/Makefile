################################################################################
# Makefile for general code snippets
################################################################################
SHELL := /bin/sh

# Simple Makefile: build every top-level .c and .cpp into an executable with the same basename.
# Useful for running experiments from job.sh which expects executables in the working directory.

CC      := gcc
CXX     := g++
CPPFLAGS := -O3 -Wall -fopenmp

ifeq ($(DEBUG),yes)
CPPFLAGS += -ggdb3 -O0
endif

# Find top-level sources
C_SRCS := $(wildcard *.c)
CPP_SRCS := $(wildcard *.cpp)

C_EXES := $(patsubst %.c,% ,$(C_SRCS))
CPP_EXES := $(patsubst %.cpp,% ,$(CPP_SRCS))
EXES := $(C_EXES) $(CPP_EXES)

.PHONY: all clean
.DEFAULT_GOAL := all

all: $(EXES)

# Rules: compile C and C++ sources to executables with same basename
%: %.c
	$(CC) $(CPPFLAGS) -o $@ $<

%: %.cpp
	$(CXX) $(CPPFLAGS) -std=c++17 -o $@ $<

clean:
	rm -f $(EXES)

# Help
help:
	@echo "Usage: make [EXE]"
	@echo "Top-level C sources: $(C_SRCS)"
	@echo "Top-level C++ sources: $(CPP_SRCS)"
