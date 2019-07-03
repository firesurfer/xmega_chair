
# Example of a pre-build script to run - I'd like to integrate this into the 
# cmake scripts as much as possible (or at least the facility to run scripts)


# Haven't exactly automated much yet...
mkdir -p build
cd build
cmake -G "Unix Makefiles" -DCMAKE_TOOLCHAIN_FILE=./Toolchain-avr-gcc.cmake ..
make

# Did the makefile complete?
BUILD_COMPLETE=$?
if [ "$BUILD_COMPLETE" -ne 0 ] ; then
    echo "Build Failed ($BUILD_COMPLETE)"
    exit 1
fi

# I'd really like this to be integrated into the above :)
ImageName=`basename *.elf .elf`
#avr-objcopy -j .text -j .data -O ihex $ImageName.elf $ImageName.hex
#avr-objcopy -j .eeprom --change-section-lma .eeprom=0 -O ihex $ImageName.elf "$ImageName"_eeprom.hex

# Not required for the build process, but works well for an example of a post-build script that
# may need to be run
avr-size "$ImageName".elf
