#CMakeLists

# ./

add_executable(main 
    main.cpp
    socket/HttpListener.cpp socket/HttpSession.cpp socket/HttpStarter.cpp
    logic/PostLogicImpl.cpp logic/SessionLogicMemory.cpp logic/UserInfoLogicImpl.cpp logic/UserLogicImpl.cpp logic/CommentLogicImpl.cpp
    storage/CommentStorageODBC.cpp storage/PostStorageODBC.cpp storage/UserStorageODBC.cpp 
    utils/DBConnection.cpp
)
# 
 
target_link_libraries(main 
    ${Boost_LIBRARIES} 
    ${ODBC_LIBRARY} 
    ${GTEST_BOTH_LIBRARIES})