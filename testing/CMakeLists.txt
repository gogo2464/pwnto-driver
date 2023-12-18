find_package(Catch2 3 REQUIRED)
# These tests can use the Catch2-provided main
add_executable(tests testing.cpp)
target_link_libraries(tests PRIVATE Catch2::Catch2WithMain)

# These tests need their own main
add_executable(custom-main-tests testing.cpp test-main.cpp)
target_link_libraries(custom-main-tests PRIVATE Catch2::Catch2)