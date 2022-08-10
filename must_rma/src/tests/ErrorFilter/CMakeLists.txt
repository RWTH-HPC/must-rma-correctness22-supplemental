addTestExecutable(ErrorFilter_test1 single_error.c)
addTestExecutable(ErrorFilter_test2 errors_with_different_ids.c)
addTestExecutable(ErrorFilter_test3 errors_from_different_functions.c)
addTestExecutable(ErrorFilter_test4 errors_with_different_ids_2.c)

# these tests require to be built with debug info
foreach(target ErrorFilter_test1 ErrorFilter_test2 ErrorFilter_test3 ErrorFilter_test4)
    target_compile_options(${target} PRIVATE -g)
endforeach()
