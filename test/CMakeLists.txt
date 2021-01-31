include_directories(..)

add_executable(test_empty test_empty.c)
add_executable(actorSystem_test actorSystem_test.c)
add_executable(test_queue test_queue.c)
add_executable(test_actor test_actor.c)
add_executable(gen_matrix gen_matrix.c)
add_test(test_empty test_empty)

set_tests_properties(test_empty PROPERTIES TIMEOUT 1)
