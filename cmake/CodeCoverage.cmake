# Code Coverage Configuration
add_library(ra_coverage INTERFACE)
add_library(ra::coverage ALIAS ra_coverage)

if(MC_BUILD_COVERAGE AND CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
  target_compile_options(ra_coverage INTERFACE -O0 -g --coverage)
  target_link_libraries(ra_coverage INTERFACE --coverage)
  target_link_options(ra_coverage INTERFACE --coverage)
endif()
