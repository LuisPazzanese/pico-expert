add_executable(pico_emb
    main.c
    tft_lcd_ili9341/ili9341/ili9341.c
    tft_lcd_ili9341/gfx/gfx_ili9341.c
    tft_lcd_ili9341/touch_resistive/touch_resistive.c
)

set_target_properties(pico_emb PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

target_include_directories(pico_emb PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}
    ${CMAKE_CURRENT_LIST_DIR}/tft_lcd_ili9341/gfx
    ${CMAKE_CURRENT_LIST_DIR}/tft_lcd_ili9341/ili9341
    ${CMAKE_CURRENT_LIST_DIR}/tft_lcd_ili9341/touch_resistive
)

target_link_libraries(pico_emb
    pico_stdlib
    hardware_spi
    hardware_adc
    hardware_gpio
)

pico_enable_stdio_uart(pico_emb 1)
pico_enable_stdio_usb(pico_emb 0)
pico_add_extra_outputs(pico_emb)
