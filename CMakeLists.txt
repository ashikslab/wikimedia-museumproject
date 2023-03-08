cmake_minimum_required(VERSION 3.14)
project(testCurlPP LANGUAGES CXX)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FindPkgConfig)
pkg_check_modules(CURLPP REQUIRED curlpp)

add_executable(fetch_all
  fetcher.cc
)
add_executable(parse_all
  parser.cc
)

target_link_libraries(fetch_all
    ${CURLPP_LDFLAGS}
)
