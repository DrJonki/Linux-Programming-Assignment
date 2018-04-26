set(DIR ${CMAKE_CURRENT_SOURCE_DIR})

add_executable(part2_task5 ${DIR}/main.c)

find_package(Threads)
target_link_libraries(part2_task5 ${CMAKE_THREAD_LIBS_INIT})
