cmake_minimum_required(VERSION 2.8)
project(ankercfg)
add_executable(ankercfg ankercfg.c)
target_link_libraries(ankercfg usb-1.0)
