cmake_minimum_required(VERSION 3.24)
project(untitled C)

set(CMAKE_C_STANDARD 17)
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
add_executable(run_program run_program.c opcodes.c condflags.h memory.h memory.c opcodes.h trap.c trap.h run_program.h)
add_executable(lc3_vm os.c os.h opcodes.c condflags.h memory.h memory.c opcodes.h trap.c trap.h ui.c ui.h sysinfo.c sysinfo.h fsystem.c fsystem.h)

target_link_libraries(lc3_vm ${Boost_LIBRARIES} rt Threads::Threads ${CMAKE_THREAD_LIBS_INIT} ${CMAKE_DL_LIBS} util)
target_link_libraries(run_program ${Boost_LIBRARIES} rt  Threads::Threads ${CMAKE_THREAD_LIBS_INIT} ${CMAKE_DL_LIBS} util)