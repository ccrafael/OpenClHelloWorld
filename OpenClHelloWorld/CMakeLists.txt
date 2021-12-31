
configure_file(kernel.cl ${CMAKE_CURRENT_BINARY_DIR} COPYONLY)
configure_file(test.cl ${CMAKE_CURRENT_BINARY_DIR} COPYONLY)

INCLUDE_DIRECTORIES($ENV{OCL_ROOT}/include)

add_executable (OpenClHelloWorld 
	"OpenClHelloWorld.cpp" 
	"OpenClHelloWorld.h" 
	"main.cpp")

target_link_libraries(OpenClHelloWorld 
	$ENV{OCL_ROOT}/lib/x86_64/opencl.lib)

add_executable(
  hello_test

  "OpenClHelloWorld.cpp" 
  "OpenClHelloWorld.h" 
  test.cpp )

target_link_libraries(
  hello_test

  $ENV{OCL_ROOT}/lib/x86_64/opencl.lib
  gtest_main)

include(GoogleTest)

gtest_discover_tests(hello_test)