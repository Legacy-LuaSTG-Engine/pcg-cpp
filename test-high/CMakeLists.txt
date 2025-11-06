set(pcg_cpp_tests_link_test
    check-pcg8_once_insecure
    check-pcg8_oneseq_once_insecure
    check-pcg16_once_insecure
    check-pcg16_oneseq_once_insecure
    check-pcg32
    check-pcg32_fast
    check-pcg32_k1024
    check-pcg32_k1024_fast
    check-pcg32_k16384
    check-pcg32_k16384_fast
    check-pcg32_k2
    check-pcg32_k2_fast
    check-pcg32_k64
    check-pcg32_k64_fast
    check-pcg32_k64_oneseq
    check-pcg32_once_insecure
    check-pcg32_oneseq
    check-pcg32_oneseq_once_insecure
    check-pcg64
    check-pcg64_fast
    check-pcg64_k1024
    check-pcg64_k1024_fast
    check-pcg64_k32
    check-pcg64_k32_fast
    check-pcg64_k32_oneseq
    check-pcg64_once_insecure
    check-pcg64_oneseq
    check-pcg64_oneseq_once_insecure
    check-pcg128_once_insecure
    check-pcg128_oneseq_once_insecure
)

set(pcg_cpp_tests_link_test_noadvance
    check-pcg32_c1024
    check-pcg32_c1024_fast
    check-pcg32_c64
    check-pcg32_c64_fast
    check-pcg32_c64_oneseq
    check-pcg32_unique
    check-pcg64_c1024
    check-pcg64_c1024_fast
    check-pcg64_c32
    check-pcg64_c32_fast
    check-pcg64_c32_oneseq
    check-pcg64_unique
)

file(MAKE_DIRECTORY "${CMAKE_CURRENT_LIST_DIR}/actual")

foreach(test_name IN LISTS pcg_cpp_tests_link_test pcg_cpp_tests_link_test_noadvance)
    add_executable(${test_name})
    target_compile_features(${test_name} PRIVATE cxx_std_20)
    target_sources(${test_name} PRIVATE "${test_name}.cpp")
    target_link_libraries(${test_name} PRIVATE pcg_cpp)
    set_target_properties(${test_name} PROPERTIES FOLDER "pcg_tests")

    set(actual   "${CMAKE_CURRENT_LIST_DIR}/actual/${test_name}.out")
    set(expected "${CMAKE_CURRENT_LIST_DIR}/expected/${test_name}.out")
    add_test(
        NAME "run-${test_name}"
        COMMAND "$<TARGET_FILE:${test_name}>" -o "${actual}"
    )

    if (${test_name} STREQUAL "check-pcg32_unique" OR ${test_name} STREQUAL "check-pcg64_unique")
        continue()
    endif ()
    add_test(
        NAME "verify-${test_name}"
        COMMAND "${CMAKE_COMMAND}" -E compare_files --ignore-eol "${actual}" "${expected}"
    )
    set_tests_properties("verify-${test_name}" PROPERTIES DEPENDS "run-${test_name}")
endforeach()
