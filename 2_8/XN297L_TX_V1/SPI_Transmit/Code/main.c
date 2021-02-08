
//Fcn description:
//Single Master,single slave architecture.
//Transmit data consecutively through MOSI pin, 
//SPIF will be set after completion of data transfer,
//clear SPIF and start a new transmission.

#include "N76E003.h"//Definitions for SFRs and bits in SFRs 
                    //which end their addresses as 0H or 8H. 
#include "SFR_Macro.h"//Setting and clring for each individual bit.
#include "Function_define.h"//Fcn definitions for the chip 
#include "Common.h"	//Fcn declarations for UART 
#include "Delay.h"//Fcn declarations for delay.
#include "XN297L.h"

extern uint8_t rcv_result;//Store the received results.
//All the hardware registers in XN297L are mapped into the RAM of MCU,
//thus,modify the mapping register first,then send the content to
//XN297L through SPI.
extern uint8_t CONFIG_mapping;
extern uint8_t SETUP_AW_mapping;
extern uint8_t RF_CH_mapping;
extern uint8_t RF_SETUP_mapping;
extern uint8_t STATUS_mapping;
extern uint8_t TX_addr[5];
extern uint8_t RX_addr_P0[5];
extern uint8_t EN_AA_mapping;
extern uint8_t SETUP_RETR_mapping;


bit indicator;
uint8_t rf_tx_send_buf;

 void main(void)
{
 Set_All_GPIO_Quasi_Mode;

	//The SPI operates as a Master:
	//Pin configuration.
//    P15_Quasi_Mode; // P15 (SS) Quasi mode
//    P10_Quasi_Mode; // P10(SPCLK) Quasi mode
//    P00_Quasi_Mode; // P00 (MOSI) Quasi mode
//    P01_Quasi_Mode; // P01 (MISO) Quasi mode
	
//Assign the default value to the RAM-mapped registers.
//These values are identical with the reset values of the reg. in XN297L.
CONFIG_mapping=EN_CRC;//Only CRC is enabled.
SETUP_AW_mapping=AW_5_BYTES;
RF_CH_mapping=0x4e;
RF_SETUP_mapping=0x3f;
STATUS_mapping=0x0e;	
EN_AA_mapping=1;
SETUP_RETR_mapping=0x03;

	// SPI in Master mode
	set_MSTR;
    
///*****************************************************************************************
//* For SPI INIT setting 
//*****************************************************************************************/
//#define		SPICLK_DIV2			clr_SPR0;clr_SPR1
//#define		SPICLK_DIV4			set_SPR0;clr_SPR1
//#define		SPICLK_DIV8			clr_SPR0;set_SPR1
//#define		SPICLK_DIV16		set_SPR0;set_SPR1	
	SPICLK_DIV16;//SPI clk rate @1Mbps
	
	//Configure SS pin 
	//SS input for mode fault detection.
	//clr_DISMODF;
	
	// SS  as General purpose I/O
    set_DISMODF;  
    clr_SSOE;
	
    //Automatic SS output.
	//set_DISMODF; 
    //set_SSOE;

   //Set the data format and the clk format of SPI
    //By default, SPI data is transferred MSB first
     clr_LSBFE; 
    //Set the idle state level of SPICLK 
     clr_CPOL; // The SPI clock is low in idle mode
	 // The data is sample on the first edge of SPI clock
     clr_CPHA; 

	clr_SPIF;
     //Enable_SPI_Interrupt; 
	// Enable SPI function
    set_SPIEN;            
	
//RF_TX Usage:
//After power on reset,it'll take 10MS until rf module switches into power-down mode,
//thus,at least 10MS delay is required before the initial procedure.
CE=0;//CE is only activated during transmission and receiving.
Timer3_Delay1ms(10);
 XN297L_software_reset();
 XN297L_reset_release();
 flush_TX_FIFO();
flush_RX_FIFO();
//Clear interrupt flag.
//clr_data_sent_interrupt_flag();
clear_STATUS_register();
//Open the receiving channel.
//Use the default channel0. 
//disable_pipe0_auto_ack();//For the receiver,
disable_auto_retransmit();
setup_address_witdth(AW_3_BYTES);//SETUP_AW
set_TX_address(TX_addr);//TX_ADDR
set_frequency_channel(2450);//RF_CH
set_spi_data_rate(DR_1MBPS);
set_rf_output_power(RF9DBM);//RF_SETUP
config_DEM_CAL();
config_RF_CAL2();
config_DEM_CAL2();
config_RF_CAL();
config_BB_CAL();

PWR_DN_mode_to_TX_mode();
Timer3_Delay1ms(20);//Wait for stable 


//Set P12 as falling edge triggered interrupt pin.
	Enable_INT_Port1;//Port1 is active as the 8-channel of pin interrupt.
    Enable_BIT2_FallEdge_Trig;
    PIF=0;   //Clr pin interrupt flag 
    //set_EPI; // Enable pin interrupt
             //Standby,wait for the external signal.

   EA=1;

CE = 1; ////The signal is active high and used to activate the chip in RX or TX mode
        //The crystal oscillator starts to output to other modules and
        //has strong drive capability.
//Transmission procedure:
Timer3_Delay1ms(1);//SPI Master device transmits data every 1m
	 InitialUART0_Timer1(9600);

   while(1)
   {



//read_single_byte_register(STATUS);
//read_single_byte_register(CONFIG);
// read_single_byte_register(RF_SETUP);
 //read_single_byte_register(FIFO_STATUS);
 //read_single_byte_register(EN_AA);	   
 
//Transmission procedure:
Timer3_Delay1ms(1000);//SPI Master device transmits data every 1ms 
 
 rf_tx_send_buf++;

write_TX_payload(rf_tx_send_buf);
Timer3_Delay1ms(3);
//read_single_byte_register(STATUS);
//read_single_byte_register(FIFO_STATUS);

 ////Check if the transmission is completed.
do
{
    read_single_byte_register(STATUS);
} 
   while (!(rcv_result & TX_DS)); //Sit in a tight loop,wait for the data transmitted.

flush_TX_FIFO();
clear_STATUS_register();
//CE = 0; //Disable crystal oscillator output for power saving.


 Send_Data_To_UART0(rf_tx_send_buf);




//How to check if the transmission process is implemented?
//An active low interrupt will generate on IRQ pin 
//after data sent.
// while (!(PIF&PIF2));//Check if an IRQ is generated.
//    PIF &=~PIF2;                             //clear interrupt flag
// //Clear the interrupt flag in XN297L
//  clr_data_sent_interrupt_flag();
   }



}//End of main()



//void SPI_ISR(void) interrupt 9 // Vecotr @  0x4B
//{
//    clr_SPIF;
//	SS=1;
//   //Retransmit the data.
//   //SPDR=0x55;
//   
//}






































	