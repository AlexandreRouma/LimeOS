cmake_minimum_required(VERSION 3.9)
project(vga_textmode)

enable_language(ASM)

set(CMAKE_ASM_COMPILER "i686-elf-as")
set(CMAKE_C_COMPILER "i686-elf-gcc")
set(CMAKE_CXX_COMPILER "i686-elf-g++")

get_filename_component(ABS_LIB_PATH "${CMAKE_CURRENT_LIST_DIR}/../../../bin/i686" ABSOLUTE)

set(CMAKE_CXX_FLAGS "-L${ABS_LIB_PATH} -l:libkernel.so -pie -fpic -ffreestanding -nostdlib -fno-rtti -Wno-write-strings -fno-exceptions -fdata-sections -ffunction-sections -Wl,--gc-sections ${CMAKE_CXX_FLAGS}")

file(GLOB MOD_SRC "src/*.cpp" "src/*.asm")

set(SRCS ${MOD_SRC})

include_directories(gfx_term "../../../src/common")
include_directories(gfx_term "../../../src/kstd")
include_directories(gfx_term "../../../src/arch/i686")
include_directories(gfx_term "src")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "../../../../bin/i686/modules")

add_executable(vga_textmode ${SRCS})

set_target_properties(vga_textmode PROPERTIES OUTPUT_NAME "vga_textmode.elf")