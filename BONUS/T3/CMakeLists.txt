# Server
set(DIR ${CMAKE_CURRENT_SOURCE_DIR}/server)

add_definitions(-DMAX_CONNECTIONS=${BONUS_TASK3_MAX_CONNECTIONS})

add_executable(bonus_task3_server

  ${DIR}/main.c
  ${DIR}/message_handler.c
  ${DIR}/connection_handler.c
)

find_package(Threads)
target_link_libraries(bonus_task3_server ${CMAKE_THREAD_LIBS_INIT} rt)

# Client
set(DIR ${CMAKE_CURRENT_SOURCE_DIR}/client)

add_executable(bonus_task3_client ${DIR}/main.c)
