

#include "N76E003.h" //SPDR

#include "SFR_Macro.h" //clr_SPIF
#include "Function_define.h" //SET_BIT7
#include "XN297L.h" //W_REGISTER


uint8_t rcv_result;//Store the received results.
//All the hardware registers in XN297L are mapped into the RAM of MCU,
//thus,modify the mapping register first,then send the content to
//XN297L through SPI.
uint8_t CONFIG_mapping;
uint8_t SETUP_AW_mapping;
uint8_t RF_CH_mapping;
uint8_t RF_SETUP_mapping;
uint8_t STATUS_mapping;
uint8_t TX_addr[5]={1, 2, 3, 4, 5};
uint8_t RX_addr_P0[5]= {1, 2, 3, 4, 5};
uint8_t  BB_cal_data[]    = {0x12,0xED,0x67,0x9C,0x46};                               //1MÀŸ¬ ≈‰÷√
uint8_t  RF_cal_data[]    = {0xF6,0x3F,0x5D};
uint8_t  RF_cal2_data[]   = {0x45,0x21,0xef,0x2C,0x5A,0x40};
uint8_t  Dem_cal_data[]   = {0x01};  
uint8_t  Dem_cal2_data[]  = {0x0b,0xDF,0x02};
uint8_t  EN_AA_mapping;  
uint8_t  SETUP_RETR_mapping; 

//Which register would u like to check?
void read_single_byte_register(uint8_t addr)
{
//Inform the module that the MCU wants to read a specific register.
SS = 0;
    SPDR = R_REGISTER + addr;
    while (!(SPSR & SET_BIT7))
        ;
    clr_SPIF;

//Now,send an arbitrary data in exchange for the contents.
    SPDR = 0x55;
    while (!(SPSR & SET_BIT7))
        ;
    clr_SPIF;
  //Read in the contents.
  rcv_result=SPDR;

//Work is done,return to the idle state.
SS = 1;
}

//If a data package is sent to the TX FIFO,the transmitter will 
//transmits the data package.
void write_TX_payload(uint8_t payload)
{
	// int i;
//Send cmd through SPI ...
SS = 0;
    SPDR = W_TX_PAYLOAD;
    while (!(SPSR & SET_BIT7))
        ;
    clr_SPIF;

    //Send the payload
//	  for(i=0;i<32;i++)
//	  {
	 SPDR = payload;
    while (!(SPSR & SET_BIT7))
        ;
    clr_SPIF;
//	   }
//Work is done,return to the idle state.
SS = 1;
}

void flush_TX_FIFO()
{ 
//Send cmd through SPI ...
SS = 0;
    SPDR = FLUSH_TX;
    while (!(SPSR & SET_BIT7))
        ;
    clr_SPIF;
//Work is done,return to the idle state.
SS = 1;
}

void flush_RX_FIFO()
{
    //Send cmd through SPI ...
    SS = 0;
    SPDR = FLUSH_RX;
    while (!(SPSR & SET_BIT7))
        ;
    clr_SPIF;
    //Work is done,return to the idle state.
SS = 1;
}

//Switch XN297L from power down mode to TX mode.
void PWR_DN_mode_to_TX_mode()
{
//Template:
//CONFIG_mapping=EN_PM|MASK_RX_DR|MASK_TX_DS|MASK_MAX_RT|EN_CRC|CRCO|PWR_UP|PRIM_RX;
//CONFIG_mapping = EN_PM | MASK_TX_DS | PWR_UP;
CONFIG_mapping = EN_PM | MASK_RX_DR | MASK_MAX_RT | PWR_UP;
//CONFIG_mapping = EN_PM  | PWR_UP;
//Enable crystal oscillator
//Enable power management module
//The module is configured as transmitter.
//An active low interrupt will generate on IRQ pin after data sent.

//Send through SPI...
SS = 0;
//Send the command sequentially according to the command-format.
//Inform XN297L that the MCU wants to write the register 'CONFIG'
//Write to SFDR will start the transmission intantly.
SPDR = W_REGISTER + CONFIG;
while (!(SPSR & SET_BIT7))
    ;
clr_SPIF;

//Send the content
SPDR = CONFIG_mapping;
while (!(SPSR & SET_BIT7))
    ;
clr_SPIF;

//Work is done,return to the idle state.
SS = 1;
}

void setup_address_witdth(enum address_width_t addr_wid)
{ 
    switch (addr_wid)
    {
    case AW_3_BYTES:
        SETUP_AW_mapping = AW_3_BYTES;
        break;
    case AW_4_BYTES:
        SETUP_AW_mapping = AW_4_BYTES;
        break;
    case AW_5_BYTES:
        SETUP_AW_mapping = AW_5_BYTES;
        break;
    }

    //Send through SPI...
    SS = 0;
    //Send the command sequentially according to the command-format.
    //Inform XN297L that the MCU wants to write the register 'SETUP_AW'
    SPDR = W_REGISTER + SETUP_AW;
    while (!(SPSR & SET_BIT7))
        ;
    clr_SPIF;
    //Send the content
    SPDR = SETUP_AW_mapping;
    while (!(SPSR & SET_BIT7))
        ;
    clr_SPIF;

    //Work is done,return to the idle state.
    SS = 1;
}

//The freq is in MHZ
//Freq range:2400MHZ-2483MHZ
// The channels, which is integer times of 16MHz, such as 2400 MHz, 2416 MHz,
// 2432MHz, 2448 MHz, 2464 MHz, and 2480MHz is not recommended.
// Because of the receiver sensitivity is degraded about 2dB in these channels.
void set_frequency_channel(uint16_t freq)
{
    RF_CH_mapping = freq - 2400;

    //Send through SPI...
    SS = 0;
    //Send the command sequentially according to the command-format.
    //Inform XN297L that the MCU wants to write the register 'RF_CH'
    SPDR = W_REGISTER + RF_CH;
    while (!(SPSR & SET_BIT7))
        ;
    clr_SPIF;
    //Send the content
    SPDR = RF_CH_mapping;
    while (!(SPSR & SET_BIT7))
        ;
    clr_SPIF;

    //Work is done,return to the idle state.
    SS = 1;
}

void set_spi_data_rate(enum spi_data_rate_t data_rate)
{
    //Erase the previous data field stored in the variable
    RF_SETUP_mapping &= 0x3f; //RF_SETUP[7:6]=0;
    switch (data_rate)
    {
    case DR_2MBPS:
        RF_SETUP_mapping |= DR_2MBPS;
        break;
    case DR_1MBPS:
        RF_SETUP_mapping |= DR_1MBPS;
        break;
    case DR_250KBPS:
        RF_SETUP_mapping |= DR_250KBPS;
        break;
    }
    //Send through SPI...
    SS = 0;
    //Send the command sequentially according to the command-format.
    //Inform XN297L that the MCU wants to write the register 'RF_SETUP'
    SPDR = W_REGISTER + RF_SETUP;
    while (!(SPSR & SET_BIT7))
        ;
    clr_SPIF;
    //Send the content
    SPDR = RF_SETUP_mapping;
    while (!(SPSR & SET_BIT7))
        ;
    clr_SPIF;

    //Work is done,return to the idle state.
    SS = 1;
}

/*-------------------------------------------*/
void set_rf_output_power(enum rf_output_power_t output_power)
{
    //Erase the previous data field stored in the variable
    RF_SETUP_mapping &= 0xc0; //RF_SETUP[5:0]=0;

    switch (output_power)
    {
    case RF11DBM:
        RF_SETUP_mapping |= RF11DBM;
        break;
    case RF9DBM:
        RF_SETUP_mapping |= RF9DBM;
        break;
    case RF5DBM:
        RF_SETUP_mapping |= RF5DBM;
        break;
    case RF4DBM:
        RF_SETUP_mapping |= RF4DBM;
        break;
    case RF_1DBM:
        RF_SETUP_mapping |= RF_1DBM;
        break;
    case RF_10DBM:
        RF_SETUP_mapping |= RF_10DBM;
        break;
    case RF_23DBM:
        RF_SETUP_mapping |= RF_23DBM;
        break;
    }

    //Send through SPI...
    SS = 0;
    //Send the command sequentially according to the command-format.
    //Inform XN297L that the MCU wants to write the register 'RF_SETUP'
    SPDR = W_REGISTER + RF_SETUP;
    while (!(SPSR & SET_BIT7))
        ;
    clr_SPIF;
    //Send the content
    SPDR = RF_SETUP_mapping;
    while (!(SPSR & SET_BIT7))
        ;
    clr_SPIF;

    //Work is done,return to the idle state.
    SS = 1;
}

/*-------------------------------------*/
//The TX address width should be assigned before setting the address.
//ie. setup_address_witdth(AW_3_BYTES);
void set_TX_address(uint8_t *addr)
{
    int i;

    //Send through SPI...
    SS = 0;
    //Send the command sequentially according to the command-format.
    //Inform XN297L that the MCU wants to write the register 'TX_ADDR'
    SPDR = W_REGISTER + TX_ADDR;
    while (!(SPSR & SET_BIT7))
        ;
    clr_SPIF;

  //Send the content
    switch (SETUP_AW_mapping)
    {
    case AW_3_BYTES:
        for (i = 0; i < 3; i++)
        {
            SPDR = addr[i];
            while (!(SPSR & SET_BIT7))
                ;
            clr_SPIF;
        }
        break;
    case AW_4_BYTES:
        for (i = 0; i < 4; i++)
        {
            SPDR =  addr[i];
            while (!(SPSR & SET_BIT7))
                ;
            clr_SPIF;
        }
        break;
    case AW_5_BYTES:
        for (i = 0; i < 5; i++)
        {
            SPDR =  addr[i];
            while (!(SPSR & SET_BIT7))
                ;
            clr_SPIF;
        }
        break;
    }

     //Work is done,return to the idle state.
    SS = 1;
}

 //After the data sent interrupt is generated,the interrupt flag
//should be cleared in order to generate a new one.
void clr_data_sent_interrupt_flag()
{
STATUS_mapping |= TX_DS; //Write 1 to clear the interrupt.

    //Send through SPI...
    SS = 0;
    //Send the command sequentially according to the command-format.
    //Inform XN297L that the MCU wants to write the register 'STATUS'
    SPDR = W_REGISTER + STATUS;
    while (!(SPSR & SET_BIT7))
        ;
    clr_SPIF;
    //Send the content
    SPDR = STATUS_mapping;
    while (!(SPSR & SET_BIT7))
        ;
    clr_SPIF;

    //Work is done,return to the idle state.
    SS = 1;
}

void clr_MAX_RT_interrupt_flag()
{
STATUS_mapping |= MAX_RT; //Write 1 to clear the interrupt.

    //Send through SPI...
    SS = 0;
    //Send the command sequentially according to the command-format.
    //Inform XN297L that the MCU wants to write the register 'STATUS'
    SPDR = W_REGISTER + STATUS;
    while (!(SPSR & SET_BIT7))
        ;
    clr_SPIF;
    //Send the content
    SPDR = STATUS_mapping;
    while (!(SPSR & SET_BIT7))
        ;
    clr_SPIF;

    //Work is done,return to the idle state.
    SS = 1;
}

void config_BB_CAL()
{
    int i;
    //Send through SPI...
    SS = 0;
    //Send the command sequentially according to the command-format.
    //Inform XN297L that the MCU wants to write the register 'BB_CAL'
    SPDR = W_REGISTER + BB_CAL;
    while (!(SPSR & SET_BIT7))
        ;
    clr_SPIF;
  //Send the content
        for (i = 0; i < 5; i++)
        {
            SPDR = BB_cal_data[i];
            while (!(SPSR & SET_BIT7))
                ;
            clr_SPIF;
        }
     //Work is done,return to the idle state.
    SS = 1;
}

void config_RF_CAL()
{
    int i;
    //Send through SPI...
    SS = 0;
    //Send the command sequentially according to the command-format.
    //Inform XN297L that the MCU wants to write the register 'RF_CAL'
    SPDR = W_REGISTER + RF_CAL;
    while (!(SPSR & SET_BIT7))
        ;
    clr_SPIF;
  //Send the content
        for (i = 0; i < 3; i++)
        {
            SPDR = RF_cal_data[i];
            while (!(SPSR & SET_BIT7))
                ;
            clr_SPIF;
        }
     //Work is done,return to the idle state.
    SS = 1;
}

void config_RF_CAL2()
{
    int i;
    //Send through SPI...
    SS = 0;
    //Send the command sequentially according to the command-format.
    //Inform XN297L that the MCU wants to write the register 'RF_CAL2'
    SPDR = W_REGISTER + RF_CAL2;
    while (!(SPSR & SET_BIT7))
        ;
    clr_SPIF;
  //Send the content
        for (i = 0; i < 6; i++)
        {
            SPDR = RF_cal2_data[i];
            while (!(SPSR & SET_BIT7))
                ;
            clr_SPIF;
        }
     //Work is done,return to the idle state.
    SS = 1;
}

void config_DEM_CAL()
{
    //Send through SPI...
    SS = 0;
    //Send the command sequentially according to the command-format.
    //Inform XN297L that the MCU wants to write the register 'DEM_CAL'
    SPDR = W_REGISTER + DEM_CAL;
    while (!(SPSR & SET_BIT7))
        ;
    clr_SPIF;
  //Send the content
            SPDR = Dem_cal_data[0];
            while (!(SPSR & SET_BIT7))
                ;
            clr_SPIF;

     //Work is done,return to the idle state.
    SS = 1;
}

void config_DEM_CAL2()
{
    int i;
    //Send through SPI...
    SS = 0;
    //Send the command sequentially according to the command-format.
    //Inform XN297L that the MCU wants to write the register 'DEM_CAL2'
    SPDR = W_REGISTER + DEM_CAL2;
    while (!(SPSR & SET_BIT7))
        ;
    clr_SPIF; 
  //Send the content
        for (i = 0; i < 3; i++)
        {
            SPDR = Dem_cal2_data[i];
            while (!(SPSR & SET_BIT7))
                ;
            clr_SPIF;
        }
     //Work is done,return to the idle state.
    SS = 1;
}

void XN297L_software_reset()
{
    //Send cmd through SPI...
    SS = 0;
    SPDR = RST_FSPI;
    while (!(SPSR & SET_BIT7))
        ;
    clr_SPIF;
    //Send the content
    SPDR = 0x5a;
    while (!(SPSR & SET_BIT7))
        ;
    clr_SPIF;
    //Work is done,return to the idle state.
    SS = 1;
}

void XN297L_reset_release()
{
  //Send cmd through SPI...
    SS = 0;
    SPDR = RST_FSPI;
    while (!(SPSR & SET_BIT7))
        ;
    clr_SPIF;
    //Send the content
    SPDR = 0xa5;
    while (!(SPSR & SET_BIT7))
        ;
    clr_SPIF;
    //Work is done,return to the idle state.
    SS = 1;
}


void clear_STATUS_register()
{	STATUS_mapping=RX_DR|TX_DS|MAX_RT;
   //Send through SPI...
    SS = 0;
    //Send the command sequentially according to the command-format.
    //Inform XN297L that the MCU wants to write the register 'STATUS'
    SPDR = W_REGISTER + STATUS;
    while (!(SPSR & SET_BIT7))
        ;
    clr_SPIF;
    //Send the content
    SPDR = STATUS_mapping;
    while (!(SPSR & SET_BIT7))
        ;
    clr_SPIF;

    //Work is done,return to the idle state.
    SS = 1;
}

void disable_pipe0_auto_ack()
{
    //EN_AA_mapping=ENAA_P5|ENAA_P4|ENAA_P3|ENAA_P2|ENAA_P1|ENAA_P0;
    EN_AA_mapping=0;
  //Send through SPI...
    SS = 0;
    //Send the command sequentially according to the command-format.
    //Inform XN297L that the MCU wants to write the register 'EN_AA'
    SPDR = W_REGISTER + EN_AA;
    while (!(SPSR & SET_BIT7))
        ;
    clr_SPIF;
    //Send the content
    SPDR = EN_AA_mapping;
    while (!(SPSR & SET_BIT7))
        ;
    clr_SPIF;

    //Work is done,return to the idle state.
    SS = 1;
}


void disable_auto_retransmit()
{
    SETUP_RETR_mapping=ARD|ARC;
  //Send through SPI...
    SS = 0;
    //Send the command sequentially according to the command-format.
    //Inform XN297L that the MCU wants to write the register 'SETUP_RETR'
    SPDR = W_REGISTER + SETUP_RETR;
    while (!(SPSR & SET_BIT7))
        ;
    clr_SPIF;
    //Send the content
    SPDR = SETUP_RETR_mapping;
    while (!(SPSR & SET_BIT7))
        ;
    clr_SPIF;

    //Work is done,return to the idle state.
    SS = 1;
}






























