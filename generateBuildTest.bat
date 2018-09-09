ctest   --build-and-test . build ^
        --build-generator "Unix Makefiles" ^
        --build-options -DCMAKE_TOOLCHAIN_FILE=toolchain.cmake ^
        --test-command ctest -j 4