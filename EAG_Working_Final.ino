
#include <compat/deprecated.h>
#include <FlexiTimer2.h>
//http://www.arduino.cc/playground/Main/FlexiTimer2

// All definitions
#define NUMCHANNELS 6
#define HEADERLEN 4
#define PACKETLEN (NUMCHANNELS * 2 + HEADERLEN + 1)
#define SAMPFREQ 256                      // ADC sampling rate 256
#define TIMER2VAL (1024/(SAMPFREQ))       // Set 256Hz sampling frequency                    
#define LED1  13
#define CAL_SIG 9

// Global constants and variables
volatile unsigned char TXBuf[PACKETLEN];  //The transmission packet
volatile unsigned char TXIndex;           //Next byte to write in the transmission packet.
volatile unsigned char CurrentCh;         //Current channel being sampled.
volatile unsigned char counter = 0;   //Additional divider used to generate CAL_SIG
volatile unsigned int ADC_Value = 0;    //ADC current value
int pins[NUMCHANNELS]={A0,A1,A2,A3,A4,A5};//Change according to the pins you are using


//~~~~~~~~~~
// Functions
//~~~~~~~~~~

void Toggle_LED1(void){

if((digitalRead(LED1))==HIGH){ digitalWrite(LED1,LOW); }
else{ digitalWrite(LED1,HIGH); }

}


/****************************************************/
/*  Function name: toggle_GAL_SIG                   */
/*  Parameters                                      */
/*    Input   :  No                             */
/*    Output  :  No                                 */
/*    Action: Switches-over GAL_SIG.                */
/****************************************************/
void toggle_GAL_SIG(void){
  
if(digitalRead(CAL_SIG) == HIGH){ digitalWrite(CAL_SIG, LOW); }
else{ digitalWrite(CAL_SIG, HIGH); }

}


/****************************************************/
/*  Function name: setup                            */
/*  Parameters                                      */
/*    Input   :  No                             */
/*    Output  :  No                                 */
/*    Action: Initializes all peripherals           */
/****************************************************/
void setup() {

noInterrupts();  // Disable all interrupts before initialization

// LED1
pinMode(LED1, OUTPUT);  //Setup LED1 direction
digitalWrite(LED1,LOW); //Setup LED1 state
pinMode(CAL_SIG, OUTPUT);


//Write packet header and footer
TXBuf[0] = 0xa5;    //Sync 0
TXBuf[1] = 0x5a;    //Sync 1
TXBuf[2] = 2;       //Protocol version
TXBuf[3] = 0;       //Packet counter
TXBuf[4] = 0x02;    //CH1 High Byte
TXBuf[5] = 0x00;    //CH1 Low Byte
TXBuf[6] = 0x02;    //CH2 High Byte
TXBuf[7] = 0x00;    //CH2 Low Byte
TXBuf[8] = 0x02;    //CH3 High Byte
TXBuf[9] = 0x00;    //CH3 Low Byte
TXBuf[10] = 0x02;   //CH4 High Byte
TXBuf[11] = 0x00;   //CH4 Low Byte
TXBuf[12] = 0x02;   //CH5 High Byte
TXBuf[13] = 0x00;   //CH5 Low Byte
TXBuf[14] = 0x02;   //CH6 High Byte
TXBuf[15] = 0x00;   //CH6 Low Byte 
TXBuf[2 * NUMCHANNELS + HEADERLEN] =  0x01; // Switches state

// Timer2
// Timer2 is used to setup the analag channels sampling frequency and packet update.
// Whenever interrupt occures, the current read packet is sent to the PC
// In addition the CAL_SIG is generated as well, so Timer1 is not required in this case!
FlexiTimer2::set(TIMER2VAL, Timer2_Overflow_ISR);
FlexiTimer2::start();

// Serial Port
Serial.begin(57600);
//Set speed to 57600 bps

// MCU sleep mode = idle.
//outb(MCUCR,(inp(MCUCR) | (1<<SE)) & (~(1<<SM0) | ~(1<<SM1) | ~(1<<SM2)));

interrupts();  // Enable all interrupts after initialization has been completed
}

/****************************************************/
/*  Function name: Timer2_Overflow_ISR              */
/*  Parameters                                      */
/*    Input   :  No                             */
/*    Output  :  No                                 */
/*    Action: Determines ADC sampling frequency.    */
/****************************************************/
void Timer2_Overflow_ISR()
{
  // Toggle LED1 with ADC sampling frequency /2
  Toggle_LED1();
  
  //Read the 6 ADC inputs and store current values in Packet
  for(CurrentCh=0;CurrentCh<NUMCHANNELS;CurrentCh++){
    //ADC_Value = analogRead(pins[CurrentCh]);
     ADC_Value = analogRead(A5);
    //ADC_Value = 0x2323;
    TXBuf[((2*CurrentCh) + HEADERLEN)] = ((unsigned char)((ADC_Value & 0xFF00) >> 8));  // Write High Byte
    TXBuf[((2*CurrentCh) + HEADERLEN + 1)] = ((unsigned char)(ADC_Value & 0x00FF)); // Write Low Byte
  }
   
  // Send Packet
  for(TXIndex=0;TXIndex<17;TXIndex++){
    //Serial.write(TXBuf[TXIndex]);
    Serial.print(TXBuf[TXIndex]);
    Serial.print(" ");
  }
Serial.println();
  
  // Increment the packet counter
  TXBuf[3]++;     
  
  // Generate the CAL_SIGnal
  counter++;    // increment the devider counter
  if(counter == 12){  // 250/12/2 = 10.4Hz ->Toggle frequency
    counter = 0;
    toggle_GAL_SIG(); // Generate CAL signal with frequ ~10Hz
  }
}


/****************************************************/
/*  Function name: loop                             */
/*  Parameters                                      */
/*    Input   :  No                             */
/*    Output  :  No                                 */
/*    Action: Puts MCU into sleep mode.             */
/****************************************************/
void loop() {
  
__asm__ __volatile__ ("sleep");

}
