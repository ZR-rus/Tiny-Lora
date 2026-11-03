
/////////////////////////////////////LoRa///////////////////////////////////////////////
#include <setup_CPU.h>

int main() {
DDRB=0;  
delay_ms(1);
DDRB|=1<<4|1<<3|1<<2|1<<0;
PORTB|=1<<4;
PORTB|=1<<3;
delay_ms(100);
setup_sleep_ATtiny85();
count=0;
flag_WDT=0;
if (!SX1278_Init())while (1) {MOSI_1;}//{LED_Blink(100);delay_ms(100);}//бесконечно мигаем LED
else MOSI_0;  
SX1278_WriteReg(REG_OP_MODE, MODE_SLEEP);   
delay_ms(100);
while(1)
 { 
    if(flag_WDT)
    {
      flag_WDT=0;
      //DDRB|=1<<4|1<<3|1<<2|1<<0;
      //PORTB|=1<<4;
      //PORTB|=1<<3;
      if (SX1278_SendPacket((uint8_t*)helloMessage, messageLength))MOSI_0;        
      SX1278_WriteReg(REG_OP_MODE, MODE_SLEEP); 
    }
    //PORTB=0;
    //DDRB=0; 
    MCUCR |= 1<<5;
    __asm__ volatile("sleep"::);
    MCUCR &= ~(1<<5);
 }
}
*/

///////////////////////////Рабочая версия LoRa////////////////////////////////////
/*

int main() { 
setup_sleep_ATtiny85();
count=0;
flag_WDT=0;
while(1)
 { 
    if(flag_WDT)
    {
      flag_WDT=0;
      DDRB|=1<<1;
      PORTB|=1<<1;
      delay_ms(1000);
      PORTB&=~(1<<1);
    }
    //PORTB=255;
    DDRB=0b11111101;
    PORTB=0b00000010; 
    MCUCR |= 1<<5;
    __asm__ volatile("sleep"::);
    MCUCR &= ~(1<<5);
 }
}
 



/*
/////////////////////////////управление семисигментниками через регистры сдвига (595)//////////////////////////// 
#include <setup_CPU.h>
// PB0 - тактирование
// PB1 - данные
// PB2 - защелка

#define on PORTB|= 1<<1;PORTB|=1<<0;PORTB&=~(1<<0)
#define off PORTB&=~(1<<1);PORTB|=1<<0;PORTB&=~(1<<0)
void tire(){off;off;off;off;off;off;on;off;};
void tochka(){off;off;off;off;off;on;off;off;};
void Number_0(){on;on;on;on;on;off;off;on;};
void Number_1(){off;off;off;off;on;off;off;on;};
void Number_2(){on;off;on;on;off;off;on;on;};
void Number_3(){on;off;on;off;on;off;on;on;};
void Number_4(){off;on;off;off;on;off;on;on;};
void Number_5(){on;on;on;off;on;off;on;off;};
void Number_6(){on;on;on;on;on;off;on;off;};
void Number_7(){on;off;off;off;on;off;off;on;};
void Number_8(){on;on;on;on;on;off;on;on;};
void Number_9(){on;on;on;off;on;off;on;on;};
void Number_NULL(){off;off;off;off;off;off;off;off;}
void str_reset()
  { 
    for(int i=0;i<100;i++)
      {
        PORTB&=~(1<<1);
        PORTB^=1<<0;
      }
    PORTB&=~(1<<0);
 }

volatile bool flag_upload;
volatile unsigned int millisecond,second,minute=40,hour=02;

ISR (TIMER1_COMPA_vect) // Обработчик прерываний 
{
  millisecond=millisecond+1;
  //TCNT1H=0;TCNT1L=0;
  
  if (millisecond==1000)
   {
      millisecond=0;
      second=second+1;
      flag_upload=true;  
   }
   if(second==60)
   {
      minute=minute+1;
      second=0; 
   }
   if(minute==60)
   {
      hour=hour+1;
      minute=0;
   }
}
int main(){
  DDRB=255;
  PORTB&=~1<<2;
  TCCR1 = (1 << CTC1) | (1 << CS12)| (1 << CS11)| (1 << CS10);  // CTC режим + предделитель 64
  OCR1A=250;                        // устанавливаем число равное кол-ву тиков за 1 миллисек
  TIMSK|=1<<OCIE1A;                   // маска прерывания по сравнению Т1
  SREG|=1<<SREG_I;                    // разрешение глобальных прерываний 

  for(int i=0;i<6;i++)Number_0();
  PORTB|=1<<2;
  delay_ms(3000);
  str_reset();
  while (1)
   {
    if(flag_upload==true)
      {
        flag_upload=false;
        PORTB&=~1<<2;
        str_reset();
        //if(second%2==0)Number_NULL();
        //else tochka();
        
        switch (minute%10)
          {
             case 0:Number_0();break;
             case 1:Number_1();break;
             case 2:Number_2();break;
             case 3:Number_3();break;
             case 4:Number_4();break;
             case 5:Number_5();break;
             case 6:Number_6();break;
             case 7:Number_7();break;
             case 8:Number_8();break;
             case 9:Number_9();break;     
          }
        switch (minute/10)
          {
             case 0:Number_0();break;
             case 1:Number_1();break;
             case 2:Number_2();break;
             case 3:Number_3();break;
             case 4:Number_4();break;
             case 5:Number_5();break;
             case 6:Number_6();break;
             case 7:Number_7();break;
             case 8:Number_8();break;
             case 9:Number_9();break;     
          }
          tire();
          switch (hour%10)
          {
             case 0:Number_0();break;
             case 1:Number_1();break;
             case 2:Number_2();break;
             case 3:Number_3();break;
             case 4:Number_4();break;
             case 5:Number_5();break;
             case 6:Number_6();break;
             case 7:Number_7();break;
             case 8:Number_8();break;
             case 9:Number_9();break;     
          }
        switch (hour/10)
          {
             case 0:Number_0();break;
             case 1:Number_1();break;
             case 2:Number_2();break;
             case 3:Number_3();break;
             case 4:Number_4();break;
             case 5:Number_5();break;
             case 6:Number_6();break;
             case 7:Number_7();break;
             case 8:Number_8();break;
             case 9:Number_9();break;     
          }  

          Number_NULL();
          PORTB|=1<<2;
      }   
   }
}
*/
