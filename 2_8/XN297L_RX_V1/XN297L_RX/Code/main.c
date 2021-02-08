
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


//bit capature_flag;
//Upload every 100ms.
//bit serial_port_upload_flag;
//uint16_t  serial_port_upload_count;

uint8_t rcv_result;//Store the received results.
uint8_t reg_state;//read in the SFRs.
bit rf_rcv_data_flag;

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
	
//RF_RX Usage:
//After power on reset,it'll take 10MS until rf module switches into power-down mode,
//thus,at least 10MS delay is required before the initial procedure.
CE=0;//CE is only activated during transmission and receiving.
Timer3_Delay1ms(10);
 XN297L_software_reset();
 XN297L_reset_release();

flush_RX_FIFO();
//Clear interrupt flag.
//clr_data_rcv_interrupt_flag();
clear_STATUS_register();
//Open the receiving channel.
//Use the default channel0.//EN_RXADDR 
disable_pipe0_auto_ack();//EN_AA
//disable_auto_retransmit();//SETUP_RETR

setup_address_witdth(AW_3_BYTES);//SETUP_AW
set_RX_address_P0(RX_addr_P0);

set_RX_payload_width(1);//RX_PW_P0

set_frequency_channel(2450);//RF_CH
set_spi_data_rate(DR_1MBPS);
set_rf_output_power(RF9DBM);//RF_SETUP


config_DEM_CAL();
config_RF_CAL2();
config_DEM_CAL2();
config_RF_CAL();
config_BB_CAL();

PWR_DN_mode_to_RX_mode();
Timer3_Delay1ms(20);//Wait for stable 
CE = 1; ////The signal is active high and used to activate the chip in RX or TX mode
        //The crystal oscillator starts to output to other modules and
        //has strong drive capability.
Timer3_Delay1ms(10);//Wait for stable 


	// LED=1;
 InitialUART0_Timer1(9600);


 //TIMER2 generates interrupt every 1MS.
// 	    TIMER2_DIV_4;
//		TIMER2_Auto_Reload_Delay_Mode;
//
//		RCMP2L = TIMER_DIV4_VALUE_1ms;
//		RCMP2H = TIMER_DIV4_VALUE_1ms>>8;
//		TL2 = 0;
//		TH2 = 0;	
//        set_ET2;                                    // Enable Timer2 interrupt 
//        set_TR2;  

//Set P12 as falling edge triggered interrupt pin.
	Enable_INT_Port1;//Port1 is active as the 8-channel of pin interrupt.
    Enable_BIT2_FallEdge_Trig;
    PIF=0;   //Clr pin interrupt flag 
    set_EPI; // Enable pin interrupt
             //Standby,wait for the external signal.


     EA=1;

   while(1)
   {


//read_single_byte_register(CONFIG);
//read_single_byte_register(STATUS);
//read_single_byte_register(FIFO_STATUS);
 //read_single_byte_register(EN_RXADDR);//ERX_P0,enable data pipe0
//read_single_byte_register(EN_AA);
//             //ENAA_P0,enable auto-acknowledgement data pipe0.	   
//read_single_byte_register(SETUP_AW);//Address width
//read_single_byte_register(RX_PW_P0);//receiving payload width	     
//read_single_byte_register(RF_CH);//Freq
//read_single_byte_register(RF_SETUP);//SPI data rate & output power.
  
 
 //Receiving procedure:
  //Standby,wait for the new data...


  if(rf_rcv_data_flag)
    {  rf_rcv_data_flag=0;

	  rcv_result=read_RX_payload();
	   flush_RX_FIFO();
	   clear_STATUS_register();	
	   Send_Data_To_UART0(rcv_result);
	} 



//Polling the data received interrupt flag in STATUS register.		 
//reg_state=read_single_byte_register(STATUS);
//			if(reg_state&RX_DR)
//			   {  
//			   rcv_result=read_RX_payload();
//
//			   flush_RX_FIFO();
//			   clear_STATUS_register();			   
//			   }

//Serial port upload the data periodically.
//		  if(serial_port_upload_flag)
//		  {serial_port_upload_flag=0;
//		  Send_Data_To_UART0(rcv_result);
//		   }

//	   if(capature_flag)
//	   {LED^=1;//Toggle LED
//		Timer3_Delay1ms(200);
//	   }
   }



}//End of main()


/************************************************************************************************************
*    Timer2 interrupt subroutine
************************************************************************************************************/
//void Timer2_ISR (void) interrupt 5
//{
//	  clr_TF2; 
//	                                 //Clear Timer2 Interrupt Flag
//  
//  serial_port_upload_count++;
//if(serial_port_upload_count==100)
//{serial_port_upload_count=0;
// serial_port_upload_flag=1;
//}	
//}


void PinInterrupt_ISR (void) interrupt 7
{
	if(PIF&PIF2)
	{
    PIF &=~PIF2;                             //clear interrupt flag
//When new data arrives RX_FIFO,an active low interrupt will generate on IRQ pin.
   //Inform the main processor to receive the incoming data.
     rf_rcv_data_flag=1;	
	}
}


//void SPI_ISR(void) interrupt 9 // Vecotr @  0x4B
//{
//    clr_SPIF;
//	SS=1;
//   //Retransmit the data.
//   //SPDR=0x55;
//   
//}






































	