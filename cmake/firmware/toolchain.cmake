set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR ARM)

set(CMAKE_C_COMPILER arm-none-eabi-gcc)
set(CMAKE_CXX_COMPILER arm-none-eabi-g++)

# This avoids the need for the linker, but it still requires an archiving tool.
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)
# CMAKE_TRY_COMPILE_PLATFORM_VARIABLES

# These are visible to the project as the
# CMAKE_<LANG>_FLAGS, CMAKE_<LANG>_FLAGS_<CONFIG>, CMAKE_<TARGETTYPE>_LINKER_FLAGS and
# CMAKE_<TARGETTYPE>_LINKER_FLAGS_<CONFIG> variables, which were covered back in Section 14.3,
# “Compiler And Linker Variables”. 
# CMAKE_<LANG>_COMPILER_EXTERNAL_TOOLCHAIN variable which can be used to specify the directory in
# which these utilities are located
