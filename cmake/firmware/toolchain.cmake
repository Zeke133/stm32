
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR ARM)

set(CMAKE_C_COMPILER arm-none-eabi-gcc)
set(CMAKE_CXX_COMPILER arm-none-eabi-g++)

# This avoids the need for the linker, but it still requires an archiving tool.
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)
# CMAKE_TRY_COMPILE_PLATFORM_VARIABLES

# Stupid syntax let flags be defined only in one string
# SET(CMAKE_EXE_LINKER_FLAGS_INIT
#     "--specs=nano.specs"
#     "--specs=nosys.specs"
#     "-nostartfiles"
# )

# CMAKE_<LANG>_COMPILER_EXTERNAL_TOOLCHAIN variable which can be used to specify the directory in
# which these utilities are located

