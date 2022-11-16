add_executable(lab8.elf main_m2.c missile.c gameControl.c)
target_link_libraries(lab8.elf ${330_LIBS} touchscreen interrupts intervalTimer)
set_target_properties(lab8.elf PROPERTIES LINKER_LANGUAGE CXX)
