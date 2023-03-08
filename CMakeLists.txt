cmake_minimum_required(VERSION 3.24)
project(untitled C)

set(CMAKE_C_STANDARD 17)
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
add_executable(run_program src/program/run_program.c src/program/opcodes.c src/program/condflags.h src/vm/memory.h src/vm/memory.c src/program/opcodes.h src/program/trap.c src/program/trap.h src/program/run_program.h)
add_executable(lc3_vm src/vm/os.c src/vm/os.h src/program/opcodes.c src/program/condflags.h src/vm/memory.h src/vm/memory.c src/program/opcodes.h src/program/trap.c src/program/trap.h src/vm/ui.c src/vm/ui.h src/vm/sysinfo.c src/vm/sysinfo.h src/vm/fsystem.c src/vm/fsystem.h)

target_link_libraries(lc3_vm ${Boost_LIBRARIES} rt Threads::Threads ${CMAKE_THREAD_LIBS_INIT} ${CMAKE_DL_LIBS} util)
target_link_libraries(run_program ${Boost_LIBRARIES} rt  Threads::Threads ${CMAKE_THREAD_LIBS_INIT} ${CMAKE_DL_LIBS} util)