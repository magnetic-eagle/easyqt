add_executable(test-application application.cxx)
target_link_libraries(test-application easyqt::easyqt)
add_executable(test-utils utils.cxx)
target_link_libraries(test-utils easyqt::easyqt)

add_test(NAME Application COMMAND test-application)
add_test(NAME Utils COMMAND test-utils)
