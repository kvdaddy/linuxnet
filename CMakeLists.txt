cmake_minimum_required(VERSION 2.8)
project(linux_network)


set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS}")


include_directories(${CMAKE_SOURCE_DIR})

add_executable(ex5_1 ex5_1.c)
add_executable(ex5_2 ex5_2.c)
add_executable(ex5_3 ex5_3.c)
add_executable(ex5_10 ex5_10.c)
add_executable(ex5_11 ex5_11.c)
add_executable(ex6_1 ex6_1.c)
add_executable(ex7_1 ex7_1.c)
add_executable(ex9_3 ex9_3.c)
add_executable(ex9_4 ex9_4.c)
target_link_libraries(ex9_4 pthread)
add_executable(ex9_5 ex9_5.c)
add_executable(ex9_7 ex9_7.c)
add_executable(ex11_3 list_timer.cc ex11_3.cc)

add_executable(ex14_1 ex14_1.c)
target_link_libraries(ex14_1 pthread)

add_executable(ex14_5 ex14_5.c)
target_link_libraries(ex14_5 pthread)
