add_executable(lab8.elf main_m1.c missile.c)
target_link_libraries(lab8.elf ${330_LIBS} touchscreen interrupts intervalTimer)
set_target_properties(lab8.elf PROPERTIES LINKER_LANGUAGE CXX)
