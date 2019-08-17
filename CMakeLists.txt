project(ChairMega)
# Cross-compiling can only be done with v2.6 or later
cmake_minimum_required(VERSION 3.5)

# Set the target AVR device
#ATXMEGA 32A4-AU
set(AVR_TARGET_DEVICE atxmega32a4u)

include_directories("/usr/lib/avr/include")

# Set the GCC flags required
set(CMAKE_C_FLAGS " -Wall -std=gnu11 -O2 -flto -gdwarf-2 -funsigned-char -funsigned-bitfields -fpack-struct -fshort-enums -save-temps -mmcu=${AVR_TARGET_DEVICE}")
set(CMAKE_CXX_FLAGS " -Wall -std=c++11 -O2 -flto -gdwarf-2 -funsigned-char -funsigned-bitfields -fpack-struct -fshort-enums -save-temps -mmcu=${AVR_TARGET_DEVICE}")

set(CMAKE_VERBOSE_MAKEFILE ON)

set(CMAKE_C_OUTPUT_EXTENSION ".o")
add_definitions(-DF_CPU=32000000UL -D__AVR_ATxmega32A4U__ -Wa,-adhlns=$@.lst)
# Allow assembly files to be built
ENABLE_LANGUAGE(ASM)

file(GLOB SRC_FILES ${CMAKE_CURRENT_SOURCE_DIR}/src/*.c*

    )
file(GLOB HEADER_FILES ${CMAKE_CURRENT_SOURCE_DIR}/src/*.h*
)
# Make the executable
add_executable(chair_mega.elf ${SRC_FILES} ${HEADER_FILES})

#add_subdirectory(src/usb)

add_custom_target(program
    COMMAND  avrdude -c avrispmkII  -p ${AVR_TARGET_DEVICE} -e -U flash:w:chair_mega.elf
)

add_dependencies(program chair_mega.elf)
