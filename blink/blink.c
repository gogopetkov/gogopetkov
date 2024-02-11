#include <furi_hal.h>
#include <gui/gui.h>

int gpio_blink_main(void* p) {
    UNUSED(p);
    furi_hal_gpio_init_simple(&gpio_ext_pa7, GpioModeOutputPushPull);

    do {
        furi_hal_gpio_write(&gpio_ext_pa7, true); //LED on
        furi_delay_ms(100);
        furi_hal_gpio_write(&gpio_ext_pa7, false); //LED off
        furi_delay_ms(2000);
    } while(furi_hal_gpio_read(&gpio_button_back));

    furi_hal_gpio_init_simple(&gpio_ext_pa7, GpioModeAnalog);


    return 0;
}