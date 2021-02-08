#ifndef XN297L_h
#define XN297L_h

#include "Common.h"

//This file contains the SFRs and bits definitions of XN297L
/********************SPI  Commands  ********************/
#define R_REGISTER 0x00   //Read command and status registers. \
                        //AAAAA =5 bit Register Map Address.
#define W_REGISTER 0x20   //Write command and status registers. \
                        //AAAAA = 5 bit Register Map Address.  \
                        //Executable in power down or standby modes only. \
                        //This cmd is used to configure RF module.
#define R_RX_PAYLOAD 0x61 //Read RX Payload                                          \
                          //A read operation starts at byte 0. Payload is deleted \
                          //from RX FIFO after it is read. Used in RX mode.
#define W_TX_PAYLOAD 0xa0 //Write TX Payload \
                          //A write operation starts at byte 0. Used in TX payload.
#define FLUSH_TX 0xE1     //Flush TX FIFO,used in TX mode.
#define FLUSH_RX 0xE2     //Flush RX FIFO,used in RX mode.
#define RST_FSPI 0x53  	//Software reset & Reset release.
/******************CONTROL  REGISTER*******************/
#define CONFIG 0
#define SETUP_AW 0x03
#define RF_CH 0x05 //Sets the frequency channel \
                   //F0=2400+(RF_CH<6:0>
#define RF_SETUP 0x06
#define STATUS 0x07
#define RX_ADDR_P0 0x0A
#define TX_ADDR 0x10
#define RX_PW_P0 0x11
//These registers are used for activating carrier,
//and they should be assigned to some fixed values.
#define		DEM_CAL				0x19
#define     RF_CAL2				0x1A
#define     DEM_CAL2			0x1B
#define		RF_CAL				0x1E
#define		BB_CAL				0x1F

#define EN_RXADDR 0x02
#define		FIFO_STATUS			0x17
#define		EN_AA				0x01
#define		SETUP_RETR		0x04
/*******************BIT DEFINITIONS********************/
/* CONFIG */
#define EN_PM 0x80       //1:Enable power management module, STB1->STB3 \
                         //0:Disable power management module,STB3->STB1
#define MASK_RX_DR 0x40  //Mask interrupt caused by RX_DR(data reception) \
                         //1: Interrupt not reflected on the IRQ pin      \
                         //0: Reflect RX_DR as active low interrupt on the IRQ pin
#define MASK_TX_DS 0x20  //Mask interrupt caused by TX_DS(data transmission) \
                         //1: Interrupt not reflected on the IRQ pin         \
                         //0: Reflect TX_DS as active low interrupt on the IRQ pin
#define MASK_MAX_RT 0x10 //Mask interrupt caused by MAX_RT(max re-transmitting) \
                         //1: Interrupt not reflected on the IRQ pin            \
                         //0: Reflect MAX_RT as active low interrupt on the IRQ pin
#define EN_CRC 0x08
#define CRCO 0x04 //CRC encoding scheme.
#define PWR_UP 0x02
#define PRIM_RX 0x01 //RX/TX ctrl  1:PRX 0:PTX
/* STATUS */
#define RX_DR 0x40
#define TX_DS 0x20 //Data Sent TX FIFO interrupt. Asserted when
                   //packet transmitted on TX.
#define MAX_RT 0x10 //Maximum number of TX retransmits interrupt, 
                    //write 1 to clear bit
/* EN_AA */
#define ENAA_P5 0x20  // Enable auto acknowledgement data pipe 5
#define ENAA_P4 0x10  // Enable auto acknowledgement data pipe 4
#define ENAA_P3 0x08  // Enable auto acknowledgement data pipe 3
#define ENAA_P2 0x04  // Enable auto acknowledgement data pipe 2
#define ENAA_P1 0x02  // Enable auto acknowledgement data pipe 1
#define ENAA_P0	0x01  // Enable auto acknowledgement data pipe 0

/* SETUP_RETR */
#define     ARD 0    //Auto Retransmit Delay 0:wait 250us
#define     ARC 0    //Auto Retransmit Count  0 re-transmit-disabled

enum address_width_t
{
    AW_3_BYTES = 1,
    AW_4_BYTES = 2,
    AW_5_BYTES = 3,
};

enum spi_data_rate_t
{
    DR_2MBPS = 1 << 6,
    DR_1MBPS = 0 << 6,
    DR_250KBPS = 3 << 6,
};

enum rf_output_power_t
{
    RF11DBM = 0X27,
    RF9DBM = 0x15,
    RF5DBM = 0x2c,
    RF4DBM = 0x14,
    RF_1DBM = 0X2a,
    RF_10DBM = 0x19,
    RF_23DBM = 0x30,
};

void read_single_byte_register(uint8_t addr);
void read_RX_payload();
void write_TX_payload(uint8_t payload);
void flush_TX_FIFO();
void flush_RX_FIFO();
void PWR_DN_mode_to_TX_mode();
void setup_address_witdth(enum address_width_t addr_wid);
void set_frequency_channel(uint16_t freq);
void set_spi_data_rate(enum spi_data_rate_t data_rate);
void set_rf_output_power(enum rf_output_power_t output_power);
void clr_data_sent_interrupt_flag();
void clr_data_rcv_interrupt_flag();
void clr_MAX_RT_interrupt_flag();
void set_TX_address(uint8_t *addr);
void set_RX_address_P0(uint8_t *addr);
void set_RX_payload_width(uint8_t width);
void config_BB_CAL();
void config_RF_CAL();
void config_RF_CAL2();
void config_DEM_CAL();
void config_DEM_CAL2();
void XN297L_software_reset();
void XN297L_reset_release();
void clear_STATUS_register();
void disable_pipe0_auto_ack();
void disable_auto_retransmit();

#endif
