################################################################################
# File:     conf.mk
# Author:   GAndaLF
# Brief:    Makefile configuration script for hw_test_template target.
#
# This file contains configuration variables for hw_test_template target.
# After setting these variables, actual build script is called.
################################################################################

# Name for output files.
TARGET_NAME := template

# Directory containing compilation result.
OUT_DIR := out/

# Defines passed as compiler arguments
GLOBAL_DEFS := \
STM32F40_41xxx \

# Paths to header files (.h).
# In the code path relative to this path should be provided.
INC_DIRS := \
../../../src/code \
../../../src/hw \
../../../src/utils \
../../../src/external/stm32 \
../../../src/external/cmsis \

# Paths to source files (.c and .S).
# In these paths source files are searched recursively in the subfolders.
SRC_DIRS := \
../../../src/hw/core_init \
../../../src/hw/startup \
../../../src/hw/gpio_f4 \
../../../src/utils \

# Additional source files to compile.
C_SRC_FILES := \
./main.c \

# Paths to the precompiled libraries (.a).
LIB_DIRS := \

# Precompiled libraries used.
# Here we provide only the name of the library. File name also contains lib
# prefix and .a extension. For example for math library we provide here only
# the name "m", but actual file name is "libm.a".
LIBS := \
m

# Linker script provided as an argument during linking.
LD_SCRIPT := ../../../src/linker.ld

include ../build.mk