#include <setup_CPU.h> 

volatile uint8_t count=0;
volatile bool flag_WDT=false;

/////////////////////////////////////////////////////////////////////////////////
void delay_us(uint32_t us)// Грубо задержка в микросекундах   
{ 
    for (uint32_t i = 0; i < us * 8; i++) 
    {
        __asm__ volatile("nop");
    }
}
void delay_ms(uint32_t ms)
{
    for(uint32_t i = 0; i < ms * 2000; i++) 
    {
       __asm__ volatile("nop");
    }
}
//void LED_Blink(uint32_t duration_ms)// мигнуть
//{
   // PORTB|=1<<3;
    //delay_ms(duration_ms);
    //PORTB&=~(1<<3);
//}
///////////////////////////////////////////////////////////////////////////////////

void setup_sleep_ATtiny85(void)
{
   SREG&=~(1<<SREG_I);    
   ADCSRA&=~(1<<7);                                                                        // отк АЦП
   ACSR&=~(1<<7);                                                                          // отк компаратор
   __asm__ volatile ("wdr");                                                               // сброс
   PRR=0b00001111;
   DIDR0=0;                                                                         // отк таймеры
   WDTCR = (1<<4)|(1<<3);                                                                  // доступ
   WDTCR = (1<<6)|(1<<5)|(1<<0);                                                           // WDT 8c 
   MCUCR = (1<<4)|(1<<3);                                                                 // глубокий сон
   SREG|=1<<SREG_I;                                                                        // разрешение глобальных прерываний 
}

void initSPI(void)
{
   // USICR=0;
    //DDRB|=1<<2|1<<0;                                                                       // mosi(0),miso(1),sck(2),cs(3),reset(4) 
    //PORTB=0b00011111;                                                                      // pull up SPI (от помех)
    //USICR |= 1<<USIWM0;                                                                    // вкл SPI
    //USISR |= 1<<USIOIF;
}

uint8_t SPI1_Transfer(uint8_t tx_data)
 {
   uint8_t rx_data=0;
   for(uint8_t i=0;i<8;i++)
   {
      
      if(tx_data&0x80)MOSI_1; 
      else MOSI_0;
      tx_data = tx_data<<1;
      delay_us(10);
      SLK_1;
      delay_us(20);
      
      SLK_0;
      rx_data=rx_data<<1;//сдвигаем
      if(PINB&(1<<1))rx_data=rx_data|1;//единичку в бит 0
      delay_us(20);
      
   }
   return rx_data;//возвращаем принятое
}

uint8_t SX1278_ReadReg(uint8_t reg)                                                         // чтение регистра 
 {
   cs_0;
    uint8_t value;                                                                   
    SPI1_Transfer(reg & 0x7F);                                                              // адрес
    value = SPI1_Transfer(0x00);                                                            // чтение
    return value;
    cs_1;
}

void SX1278_WriteReg(uint8_t reg, uint8_t value)                                            //запись регистра
 {
   cs_0;
    SPI1_Transfer(reg | 0x80);                                                              // адрес
    SPI1_Transfer(value); 
    cs_1;                                                                  // запись
 }

 void SX1278_WriteFIFO(uint8_t *data, uint8_t len) 
 {  
    cs_0;
    SPI1_Transfer(REG_FIFO | 0x80);                                                         // Адрес FIFO с битом записи
    for (uint8_t i = 0; i < len; i++) {
        SPI1_Transfer(data[i]);                                                             // Отправляем данные
    }
    cs_1;
}

/////////////////////////////////SX1278 ФУНКЦИИ //////////////////////////////
uint8_t SX1278_Init(void) // инициализация
{ 
    uint8_t version; 
    RESET_0;//сброс 
    delay_ms(10);
    RESET_1;
    delay_ms(10);
    version = SX1278_ReadReg(0x42);// проверка версии
    if (version != 0x12){MOSI_1;return 0;}  // Ошибка
    SX1278_WriteReg(REG_OP_MODE, MODE_SLEEP);//переводим в сон
    delay_ms(10);
    SX1278_WriteReg(REG_OP_MODE, MODE_SLEEP);  //дубль
    delay_ms(10);
    SX1278_WriteReg(REG_FRF_MSB, 0x6C); // Частота: 433000000 / 61.035 = 7095000 = 0x6C 0x4F 0x38  (Fstep=61.035)
    SX1278_WriteReg(REG_FRF_MID, 0x4F);
    SX1278_WriteReg(REG_FRF_LSB, 0x38);
    SX1278_WriteReg(REG_PA_CONFIG, 0x80);  // мощность 0x8F = +17dBm, 0xFF = +20dBm max
    SX1278_WriteReg(REG_LNA, 0x23);  // LNA gain: max, boost on
    SX1278_WriteReg(REG_MODEM_CONFIG1, 0x72);  // BW=125kHz, CR=4/5, explicit header
    SX1278_WriteReg(REG_MODEM_CONFIG2, 0x74);  // SF7, CRC on
    //SX1278_WriteReg(0x26,0x00);
    //SX1278_WriteReg(0x20,0x00);
    //SX1278_WriteReg(0x21,0x08);
    SX1278_WriteReg(REG_FIFO_TX_BASE_ADDR, 0x80);// Установка базовых адресов FIFO
    SX1278_WriteReg(REG_FIFO_RX_BASE_ADDR, 0x00);
    SX1278_WriteReg(REG_OP_MODE, MODE_STDBY);// в режим ожидания
    return 1;  
}

uint8_t SX1278_SendPacket(uint8_t *data, uint8_t length) // отправка пакета
{ 
    SX1278_WriteReg(REG_OP_MODE, MODE_STDBY);// Переходим в режим ожидания
    SX1278_WriteReg(REG_FIFO_ADDR_PTR, 0x80);// Устанавливаем указатель FIFO на TX базу
    SX1278_WriteReg(REG_PAYLOAD_LENGTH, length);// Записываем длину пакета
    SX1278_WriteFIFO(data, length);// Записываем данные в FIFO
    SX1278_WriteReg(REG_IRQ_FLAGS, 0xFF);// Очищаем флаги прерываний
    SX1278_WriteReg(REG_OP_MODE, MODE_TX);// Переходим в режим передачи
    while (!(SX1278_ReadReg(REG_IRQ_FLAGS) & IRQ_TX_DONE_MASK)){delay_us(200);}
    SX1278_WriteReg(REG_IRQ_FLAGS, 0xFF); // Очищаем флаги
    SX1278_WriteReg(REG_OP_MODE, MODE_STDBY);// Возвращаемся в режим ожидания 
    return 1;  
}

ISR (WDT_vect)
{
   count++;
   if(count>=2)
   {
      flag_WDT=true;
      count=0;
   }
}
