

#include <furi.h>
#include <furi_hal.h>

#include <furi_hal_gpio.h>
#include <furi_hal_resources.h>

#include "mcp960X.h"

uint8_t i2c_find_device() {
    uint8_t addr = 0;

    furi_hal_i2c_acquire(&furi_hal_i2c_handle_external);

    for(uint8_t try_addr = 0; try_addr != 0xff; try_addr++) {
        if(furi_hal_i2c_is_device_ready(&furi_hal_i2c_handle_external, try_addr, 5)) {
            addr = try_addr;
            break;
        }
    }

    furi_hal_i2c_release(&furi_hal_i2c_handle_external);
    return addr;
}

bool i2c_init_mcp9600(uint8_t addr) {
    bool result = false;
    uint8_t buffer[2];
  
    uint8_t thermocouple_type = 0;
    uint8_t thermocouple_filter = 0;
    addr = 0xC0;
    furi_hal_i2c_acquire(&furi_hal_i2c_handle_external);

    // read version number
    buffer[0] = MCP9600_DEVICEID;
    // if(furi_hal_i2c_trx(&furi_hal_i2c_handle_external, addr, buffer, 1, buffer, 1, 100)) {
    // (furi_hal_i2c_trx(&furi_hal_i2c_handle_external, addr, buffer, 1, buffer, 1, 100)); {   

    buffer[0] = 0;    
    furi_hal_i2c_tx(&furi_hal_i2c_handle_external, addr, buffer, 1, 100);
    furi_hal_i2c_rx(&furi_hal_i2c_handle_external, addr, buffer, 1, 100);
    FURI_LOG_I("thermocuple", " version->: %x", buffer[0]);

    
    if(furi_hal_i2c_tx(&furi_hal_i2c_handle_external, addr, buffer, 1, 100)) {    
        FURI_LOG_I("thermocuple", " version: %x", buffer[0]);
        buffer[0] = MCP9600_SENSORCONFIG; 
        buffer[1] = ((0x07 & thermocouple_type) << 4) | (0x07 & thermocouple_filter);
        if(furi_hal_i2c_tx(&furi_hal_i2c_handle_external, addr, buffer, 2, 100)) {
            FURI_LOG_I("thermocuple", " thremocouple config  - OK ");
            /* Device configuration register - MCP9600_DEVICECONFIG 
            Cold-Junction resolution - 0.0625,  ADC Resolution - 18 bits
            Burst Mode Temperature Samples 1,  Burst mode on */
            buffer[0] = MCP9600_DEVICECONFIG; 
            buffer[1] = 0x82;
            if(furi_hal_i2c_tx(&furi_hal_i2c_handle_external, addr, buffer, 2, 100)) {
                FURI_LOG_I("thermocuple", " config ADC resolution, burst mode  - OK ");
                result = true;
            }
        }
    // buffer[0] = 0;    
    // furi_hal_i2c_tx(&furi_hal_i2c_handle_external, addr, buffer, 1, 100);
    // furi_hal_i2c_rx(&furi_hal_i2c_handle_external, addr, buffer, 2, 100);
    // FURI_LOG_I("thermocuple", " hot junction->: %d",  (buffer[0]*256 + buffer[1]));


    }

    furi_hal_i2c_release(&furi_hal_i2c_handle_external);
    FURI_LOG_I("thermocuple", " unsuccessful initialization");
    return result;

}

uint16_t  read_hot_junction_mcp9600(uint8_t addr) {
    
     uint8_t buffer[2];
    furi_hal_i2c_acquire(&furi_hal_i2c_handle_external);

    buffer[0] = MCP9600_HOTJUNCTION; 
    furi_hal_i2c_tx(&furi_hal_i2c_handle_external, addr, buffer, 1, 100);
    furi_delay_ms(1000);
    furi_hal_i2c_rx(&furi_hal_i2c_handle_external, addr, buffer, 2, 100);

    uint16_t  hj = (buffer[0]*256 + buffer[1]);
    FURI_LOG_I("thermocuple", " hot junction->: %d", hj );

    furi_hal_i2c_release(&furi_hal_i2c_handle_external);
    return hj;

}

uint16_t  read_cold_junction_mcp9600(uint8_t addr) {
    
    uint8_t buffer[2];
    furi_hal_i2c_acquire(&furi_hal_i2c_handle_external);

    buffer[0] = MCP9600_COLDJUNCTION; 
    furi_hal_i2c_tx(&furi_hal_i2c_handle_external, addr, buffer, 1, 100);
    furi_delay_ms(1000);
    furi_hal_i2c_rx(&furi_hal_i2c_handle_external, addr, buffer, 2, 100);

    uint16_t  hj = (buffer[0]*256 + buffer[1]);
    FURI_LOG_I("thermocuple", " hot junction->: %d", hj );

    furi_hal_i2c_release(&furi_hal_i2c_handle_external);
    return hj;

}


