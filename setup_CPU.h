#ifndef setup_CPU_h
#define setup_CPU_h

#include <avr/io.h>
#include <avr/interrupt.h>

#define MOSI_0 		PORTB&=~(1<<0)//выход
#define MISO_0  	PORTB&=~(1<<1)//вход
#define SLK_0      	PORTB&=~(1<<2)//такт
#define cs_0 		PORTB&=~(1<<4)//Выбор чипа
#define RESET_0  	PORTB&=~(1<<3)//Сброс
#define MOSI_1      PORTB|=1<<0
#define MISO_1      PORTB|=1<<1
#define SLK_1       PORTB|=1<<2
#define cs_1 		PORTB|=1<<4
#define RESET_1   	PORTB|=1<<3 

// Регистры SX1278
#define REG_FIFO                  0x00
#define REG_OP_MODE               0x01
#define REG_FRF_MSB               0x06
#define REG_FRF_MID               0x07
#define REG_FRF_LSB               0x08
#define REG_PA_CONFIG             0x09
#define REG_LNA                   0x0C
#define REG_FIFO_ADDR_PTR         0x0D
#define REG_FIFO_TX_BASE_ADDR     0x0E
#define REG_FIFO_RX_BASE_ADDR     0x0F
#define REG_IRQ_FLAGS             0x12
#define REG_RX_NB_BYTES           0x13
#define REG_MODEM_CONFIG1         0x1D
#define REG_MODEM_CONFIG2         0x1E
#define REG_PAYLOAD_LENGTH        0x22
#define REG_DIO_MAPPING1          0x40
#define REG_VERSION               0x42

// Режимы SX1278
#define MODE_SLEEP                0x80
#define MODE_STDBY                0x81
#define MODE_TX                   0x83
#define MODE_RX_CONTINUOUS        0x85
#define MODE_RX_SINGLE            0x86

// Флаги прерываний
#define IRQ_TX_DONE_MASK          0x08
#define IRQ_RX_DONE_MASK          0x40

extern volatile uint8_t count;
extern volatile bool flag_WDT;
const uint8_t helloMessage[] = "Tiny-LoRa";
const uint8_t messageLength = 9;  

void delay_us(uint32_t us);
void delay_ms(uint32_t ms);
//void LED_Blink(uint32_t duration_ms);
void setup_sleep_ATtiny85(void);
void initSPI(void);
uint8_t SPI1_Transfer(uint8_t data);
uint8_t SX1278_ReadReg(uint8_t reg);
void SX1278_WriteReg(uint8_t reg, uint8_t value);
void SX1278_WriteFIFO(uint8_t *data, uint8_t len);
uint8_t SX1278_Init(void);
uint8_t SX1278_SendPacket(uint8_t *data, uint8_t length);

#endif