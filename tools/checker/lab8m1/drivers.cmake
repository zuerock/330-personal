add_library(intervalTimer intervalTimer.c)
target_link_libraries(intervalTimer ${330_LIBS})

add_library(interrupts interrupts.c)
target_link_libraries(interrupts ${330_LIBS})
