#CMakeLists

add_executable(test main.cpp)
target_link_libraries(test ${Boost_LIBRARIES} ${ODBC_LIBRARY} ${GTEST_BOTH_LIBRARIES})