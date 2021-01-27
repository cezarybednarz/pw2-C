cmake_minimum_required (VERSION 3.1)
project (CACTI)

enable_testing()

#set(CMAKE_C_STANDARD ...)
set(CMAKE_C_FLAGS "-g -Wall -Wextra -pthread")

# http://stackoverflow.com/questions/10555706/
macro (add_executable _name)
  # invoke built-in add_executable
  _add_executable(${ARGV})
  if (TARGET ${_name})
    target_link_libraries(${_name} cacti)
  endif()
endmacro()

add_library(cacti STATIC cacti.c actor_system.c actor.c  queue.c threadpool.c err.c)
add_executable(macierz macierz.c)
add_executable(silnia silnia.c)
add_executable(test_queue test_queue.c) # todo usunac
add_executable(test_actor test_actor.c) # todo usunac
add_subdirectory(test)

install(TARGETS cacti DESTINATION .)
