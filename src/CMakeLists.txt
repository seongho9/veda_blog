#CMakeLists

# ./

# add_subdirectory(storage)
# add_subdirectory(domain)
# add_subdirectory(socket)
add_executable(main 
    main.cpp
)
# 
 
target_link_libraries(main 
    ${Boost_LIBRARIES} 
    ${ODBC_LIBRARY} 
    ${GTEST_BOTH_LIBRARIES})