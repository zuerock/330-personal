add_executable(lab9.elf main_m1.c words.c gameDisplay.c)
target_link_libraries(lab9.elf ${330_LIBS} touchscreen interrupts intervalTimer buttons_switches)
set_target_properties(lab9.elf PROPERTIES LINKER_LANGUAGE CXX)
