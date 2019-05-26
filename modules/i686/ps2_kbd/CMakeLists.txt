cmake_minimum_required(VERSION 3.9)
project(vga_textmode)

enable_language(ASM)

set(CMAKE_ASM_COMPILER "i686-elf-as")
set(CMAKE_C_COMPILER "i686-elf-gcc")
set(CMAKE_CXX_COMPILER "i686-elf-g++")

set(CMAKE_CXX_FLAGS "-pie -fpic -ffreestanding -nostdlib -fno-rtti -Wno-write-strings -fno-exceptions -fdata-sections -ffunction-sections -Wl,--gc-sections ${CMAKE_CXX_FLAGS}")

file(GLOB MOD_SRC "src/*.cpp" "src/*.asm")
file(GLOB API_SRC "../../../src/arch/i686/kapi/api/*.cpp" "../../../src/arch/i686/kapi/api/*.asm")

set(SRCS ${MOD_SRC} ${API_SRC})

include_directories(vga_textmode "../../../src/arch/i686/kapi/api")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "../../../../bin/i686/modules")

add_executable(vga_textmode ${SRCS})

set_target_properties(vga_textmode PROPERTIES OUTPUT_NAME "vga_textmode.elf")