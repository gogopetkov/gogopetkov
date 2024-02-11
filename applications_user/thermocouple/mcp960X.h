#ifndef MCP9600_H
#define MCP9600_H

#define MCP9600_I2CADDR_DEFAULT 0xCE /// Default MCP9600 I2C address 0x67<<1
#define MCP9600_I2CADDR_67 0xCE /// Default MCP9600 I2C address 0x67<<1
#define MCP9600_I2CADDR_60 0xC0 /// Default MCP9600 I2C address 0x60<<1

#define MCP9600_HOTJUNCTION 0x00 ///< Hot junction temperature T_H
#define MCP9600_JUNCTIONDELTA 0x01 ///< Hot/Cold junction delta
#define MCP9600_COLDJUNCTION 0x02 ///< Cold junction temperature T_C
#define MCP9600_RAWDATAADC 0x03 ///< The 'raw' uV reading
#define MCP9600_STATUS 0x04 ///< Current device status
#define MCP9600_SENSORCONFIG 0x05 ///< Configuration for thermocouple type
#define MCP9600_DEVICECONFIG 0x06 ///< Device config like sleep mode
#define MCP9600_DEVICEID 0x20 ///< Device ID/Revision
#define MCP9600_ALERTCONFIG_1 0x08 ///< The first alert's config
#define MCP9600_ALERTHYST_1 0x0C ///< The first alert's hystersis
#define MCP9600_ALERTLIMIT_1 0x10 ///< the first alert's limitval

#define MCP960X_STATUS_ALERT1 0x01 ///< Bit flag for alert 1 status
#define MCP960X_STATUS_ALERT2 0x02 ///< Bit flag for alert 2 status
#define MCP960X_STATUS_ALERT3 0x04 ///< Bit flag for alert 3 status
#define MCP960X_STATUS_ALERT4 0x08 ///< Bit flag for alert 4 status
#define MCP960X_STATUS_INPUTRANGE 0x10 ///< Bit flag for input range
#define MCP960X_STATUS_THUPDATE 0x40 ///< Bit flag for TH update
#define MCP960X_STATUS_BURST 0x80 ///< Bit flag for burst complete

/*! The possible Thermocouple types */
typedef enum _themotype {
    MCP9600_TYPE_K,
    MCP9600_TYPE_J,
    MCP9600_TYPE_T,
    MCP9600_TYPE_N,
    MCP9600_TYPE_S,
    MCP9600_TYPE_E,
    MCP9600_TYPE_B,
    MCP9600_TYPE_R,
} MCP9600_ThemocoupleType;

/*! The possible ADC resolution settings */
typedef enum _resolution {
    MCP9600_ADCRESOLUTION_18,
    MCP9600_ADCRESOLUTION_16,
    MCP9600_ADCRESOLUTION_14,
    MCP9600_ADCRESOLUTION_12,
} MCP9600_ADCResolution;

typedef enum _low_power {
    MCP9600_LP_OFF,
    MCP9600_LP_ON,
    MCP9600_BURST,
} MCP9600_Low_Power_Mode;

typedef enum _burst_samples {
    MCP9600_BURST_1,
    MCP9600_BURST_2,
    MCP9600_BURST_4,
    MCP9600_BURST_8,
    MCP9600_BURST_16,
    MCP9600_BURST_32,
    MCP9600_BURST_64,
    MCP9600_BURST_128,
} MCP9600_Burst_Samples;

typedef enum _cold_resolution {
    MCP9600_0625,
    MCP9600_25,
} MCP9600_Cold_Junction_Resolution;

typedef enum _filter {
    MCP9600_FILTER_OFF,
    MCP9600_FILTER_MIN,
    MCP9600_FILTER_2,
    MCP9600_FILTER_3,
    MCP9600_FILTER_4,
    MCP9600_FILTER_5,
    MCP9600_FILTER_6,
    MCP9600_FILTER_MAX,
} MCP9600_Filter;

/*************************************************************************
  MCP9600 driver.
*************************************************************************/

uint8_t i2c_find_device();
bool i2c_init_mcp9600(uint8_t addr);
uint16_t  read_hot_junction_mcp9600(uint8_t addr) ;
uint16_t  read_cold_junction_mcp9600(uint8_t addr);

#endif // #define  MCP9600_H
