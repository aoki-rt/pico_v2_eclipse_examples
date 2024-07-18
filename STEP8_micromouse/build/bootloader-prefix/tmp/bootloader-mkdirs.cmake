# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "/Users/aokimasatake/eclipse/esp-idf-v5.2.1/components/bootloader/subproject"
  "/Users/aokimasatake/eclipse-workspace/STEP8_micromouse/build/bootloader"
  "/Users/aokimasatake/eclipse-workspace/STEP8_micromouse/build/bootloader-prefix"
  "/Users/aokimasatake/eclipse-workspace/STEP8_micromouse/build/bootloader-prefix/tmp"
  "/Users/aokimasatake/eclipse-workspace/STEP8_micromouse/build/bootloader-prefix/src/bootloader-stamp"
  "/Users/aokimasatake/eclipse-workspace/STEP8_micromouse/build/bootloader-prefix/src"
  "/Users/aokimasatake/eclipse-workspace/STEP8_micromouse/build/bootloader-prefix/src/bootloader-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "/Users/aokimasatake/eclipse-workspace/STEP8_micromouse/build/bootloader-prefix/src/bootloader-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "/Users/aokimasatake/eclipse-workspace/STEP8_micromouse/build/bootloader-prefix/src/bootloader-stamp${cfgdir}") # cfgdir has leading slash
endif()
