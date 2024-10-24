#CMakeLists

add_executable(user_storage 
    ./storage/UserStorageTest.cpp
    ../src/storage/CommentStorageODBC.cpp
    ../src/storage/PostStorageODBC.cpp
    ../src/storage/UserStorageODBC.cpp 
    #utils
    ../src/utils/DBConnection.cpp
)

add_executable(post_storage
    ./storage/PostStorageTest.cpp
    ../src/storage/CommentStorageODBC.cpp
    ../src/storage/PostStorageODBC.cpp
    ../src/storage/UserStorageODBC.cpp 
    #utils
    ../src/utils/DBConnection.cpp
)

add_executable(comment_storage
    ./storage/CommentStorageTest.cpp
    ../src/storage/CommentStorageODBC.cpp
    ../src/storage/PostStorageODBC.cpp
    ../src/storage/UserStorageODBC.cpp 
    #utils
    ../src/utils/DBConnection.cpp
)

target_link_libraries(comment_storage ${Boost_LIBRARIES} ${ODBC_LIBRARY} ${GTEST_BOTH_LIBRARIES})
target_link_libraries(user_storage ${Boost_LIBRARIES} ${ODBC_LIBRARY} ${GTEST_BOTH_LIBRARIES})
target_link_libraries(post_storage ${Boost_LIBRARIES} ${ODBC_LIBRARY} ${GTEST_BOTH_LIBRARIES})