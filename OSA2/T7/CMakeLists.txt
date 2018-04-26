set(DIR ${CMAKE_CURRENT_SOURCE_DIR})

add_executable(part2_task7 ${DIR}/main.c)

target_link_libraries(part2_task7 rt)
