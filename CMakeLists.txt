# LimeOS CMakeLists.txt
# Copyright(R) Ryzerth 2019

cmake_minimum_required(VERSION 3.9)
project(LimeOS)

enable_language(ASM)

# ================= CONFIG =================

set(ARCH "i686")

# ================= CONFIG =================

# Kernel source
file(GLOB KERN_SRC "src/*.cpp" "src/*.asm")
include_directories(LimeOS "src/common")
include_directories(LimeOS "src/kstd")
file(GLOB LIB_COMMON "src/common/*.cpp" "src/common/*.asm")
file(GLOB LIB_MISC "src/common/misc/*.cpp" "src/common/misc/*.asm")
file(GLOB LIB_KSTD "src/kstd/*.cpp" "src/kstd/*.asm")
file(GLOB LIB_VFS "src/common/vfs/*.cpp" "src/common/vfs/*.asm")
file(GLOB LIB_RAMFS "src/common/ramfs/*.cpp" "src/common/ramfs/*.asm")
file(GLOB LIB_KIO "src/common/kernio/*.cpp" "src/common/kernio/*.asm")
file(GLOB LIB_ELF "src/common/elf/*.cpp" "src/common/elf/*.asm")
file(GLOB LIB_KSC "src/common/kscript/*.cpp" "src/common/kscript/*.asm")
file(GLOB LIB_KMOD "src/common/kmod/*.cpp" "src/common/kmod/*.asm")

if(${ARCH} STREQUAL "i686")
    # Toolchain config
    set(CMAKE_ASM_COMPILER "i686-elf-as")
    set(CMAKE_C_COMPILER "i686-elf-gcc")
    set(CMAKE_CXX_COMPILER "i686-elf-g++")

    set(CMAKE_CXX_FLAGS "-DKERNEL_CODE -ffreestanding -O2 -fno-rtti -Wno-write-strings -Wno-multichar -Wno-unused-parameter -Wno-overflow -Wno-narrowing -fno-exceptions -Wno-trigraphs -Wno-pointer-arith -Wno-varargs ${CMAKE_CXX_FLAGS}")
    set(CMAKE_EXE_LINKER_FLAGS "-T ../src/arch/i686/linker.ld -ffreestanding -O2 -nostdlib -lgcc -Wwrite-strings ${CMAKE_EXE_LINKER_FLAGS}")

    # Library source
    file(GLOB LIB_INIT "src/arch/i686/init/*.cpp" "src/arch/i686/init/*.asm")
    file(GLOB LIB_GDT "src/arch/i686/gdt/*.cpp" "src/arch/i686/gdt/*.asm")
    file(GLOB LIB_IDT "src/arch/i686/interrupts/*.cpp" "src/arch/i686/interrupts/*.asm")
    file(GLOB LIB_PAGING "src/arch/i686/paging/*.cpp" "src/arch/i686/paging/*.asm")
    file(GLOB LIB_VGA "src/arch/i686/temp_vga/*.cpp" "src/arch/i686/temp_vga/*.asm")
    file(GLOB LIB_KAPI "src/arch/i686/kapi/*.cpp" "src/arch/i686/kapi/*.asm")
    file(GLOB LIB_SCHD "src/arch/i686/scheduler/*.cpp" "src/arch/i686/scheduler/*.asm")
    file(GLOB LIB_PIT "src/arch/i686/pit/*.cpp" "src/arch/i686/pit/*.asm")

    include_directories(LimeOS "src/arch/i686")

    set(SRCS ${KERN_SRC} ${LIB_COMMON} ${LIB_MISC} ${LIB_INIT} ${LIB_GDT} ${LIB_IDT} ${LIB_PAGING} ${LIB_VGA} ${LIB_KSTD} ${LIB_VFS} ${LIB_RAMFS} ${LIB_KIO} ${LIB_ELF} ${LIB_KSC} ${LIB_KMOD} ${LIB_KAPI} ${LIB_SCHD} ${LIB_PIT})
    
    # GRUB target
    add_custom_target(iso COMMAND sh ../tools/build_iso.sh)
    add_custom_target(ramfs COMMAND sh ../tools/build_ramfs.sh)

    # Configuring dependencies
    add_dependencies(iso LimeOS)
    add_dependencies(iso ramfs)
    

    # Output
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "../bin/i686/")

elseif(${ARCH} STREQUAL "arm")
    # Toolchain config
    set(CMAKE_ASM_COMPILER "arm-none-eabi-as")
    set(CMAKE_C_COMPILER "arm-none-eabi-gcc")
    set(CMAKE_CXX_COMPILER "arm-none-eabi-g++")

    set(CMAKE_CXX_FLAGS "-ffreestanding -O2 ${CMAKE_CXX_FLAGS}")
    set(CMAKE_EXE_LINKER_FLAGS "-ffreestanding -O2 -nostdlib -lgcc ${CMAKE_EXE_LINKER_FLAGS}")

    include_directories(LimeOS "src/arch/arm")
    include_directories(LimeOS "src/common")

    set(SRCS ${LIB_COMMON} ${KERN_SRC} ${LIB_MISC} ${LIB_KSTD} ${LIB_VFS} ${LIB_RAMFS})

    # Output
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "../bin/arm/")

endif()

add_executable(LimeOS ${SRCS})

set_target_properties(LimeOS PROPERTIES OUTPUT_NAME "LimeOS.bin")