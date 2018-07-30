
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR ARM)

set(CMAKE_ASM_COMPILER arm-none-eabi-as)
set(CMAKE_C_COMPILER arm-none-eabi-gcc)
set(CMAKE_CXX_COMPILER arm-none-eabi-g++)

set(CMAKE_SIZE arm-none-eabi-size)

# This avoids the need for the linker, but it still requires an archiving tool.
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)
# CMAKE_TRY_COMPILE_PLATFORM_VARIABLES
