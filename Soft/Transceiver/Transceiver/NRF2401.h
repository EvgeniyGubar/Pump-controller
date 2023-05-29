
/* Memory Map */
#define CONFIG      0x00
#define EN_AA       0x01
#define EN_RXADDR   0x02
#define SETUP_AW    0x03
#define SETUP_RETR  0x04
#define RF_CH       0x05
#define RF_SETUP    0x06
#define STATUS      0x07
#define OBSERVE_TX  0x08
#define RPD         0x09
#define RX_ADDR_P0  0x0A
#define RX_ADDR_P1  0x0B
#define RX_ADDR_P2  0x0C
#define RX_ADDR_P3  0x0D
#define RX_ADDR_P4  0x0E
#define RX_ADDR_P5  0x0F
#define TX_ADDR     0x10
#define RX_PW_P0    0x11
#define RX_PW_P1    0x12
#define RX_PW_P2    0x13
#define RX_PW_P3    0x14
#define RX_PW_P4    0x15
#define RX_PW_P5    0x16
#define FIFO_STATUS 0x17
#define DYNPD       0x1C

/* Bit Mnemonics */

/* configuration register */
#define MASK_RX_DR  6
#define MASK_TX_DS  5
#define MASK_MAX_RT 4
#define EN_CRC      3
#define CRCO        2
#define PWR_UP      1
#define PRIM_RX     0

/* enable auto acknowledgment */
#define ENAA_P5     5
#define ENAA_P4     4
#define ENAA_P3     3
#define ENAA_P2     2
#define ENAA_P1     1
#define ENAA_P0     0

/* enable rx addresses */
#define ERX_P5      5
#define ERX_P4      4
#define ERX_P3      3
#define ERX_P2      2
#define ERX_P1      1
#define ERX_P0      0

/* setup of address width */
#define AW          0 /* 2 bits */

/* setup of auto re-transmission */
#define ARD         4 /* 4 bits */
#define ARC         0 /* 4 bits */

/* RF setup register */
#define CONT_WAVE   7
#define RF_DR_LOW   5
#define PLL_LOCK    4
#define RF_DR_HIGH  3
#define RF_PWR      1 /* 2 bits */   

/* general status register */
#define RX_DR       6
#define TX_DS       5
#define MAX_RT      4
#define RX_P_NO     1 /* 3 bits */
#define TX_FULL     0

/* transmit observe register */
#define PLOS_CNT    4 /* 4 bits */
#define ARC_CNT     0 /* 4 bits */

/* fifo status */
#define TX_REUSE    6
#define FIFO_FULL   5
#define TX_EMPTY    4
#define RX_FULL     1
#define RX_EMPTY    0

/* dynamic length */
#define DPL_P0      0
#define DPL_P1      1
#define DPL_P2      2
#define DPL_P3      3
#define DPL_P4      4
#define DPL_P5      5

/* Instruction Mnemonics */
#define R_REGISTER    0x00 /* last 4 bits will indicate reg. address */
#define W_REGISTER    0x20 /* last 4 bits will indicate reg. address */
#define REGISTER_MASK 0x1F
#define R_RX_PAYLOAD  0x61
#define W_TX_PAYLOAD  0xA0
#define FLUSH_TX      0xE1
#define FLUSH_RX      0xE2
#define REUSE_TX_PL   0xE3
#define ACTIVATE      0x50 
#define R_RX_PL_WID   0x60
#define NOP           0xFF

#define nrf24_ADDR_LEN 5
#define nrf24_SETTING ((1<<EN_CRC)|(0<<CRCO))

#define NRF24_TRANSMISSON_OK 0
#define NRF24_MESSAGE_LOST   1

/* adjustment functions */
void nrf24_init_config(uint8_t channel, uint8_t pay_length);
void nrf24_rx_address(uint8_t* adr);
void nrf24_tx_address(uint8_t* adr);

/* state check functions */
uint8_t nrf24_dataReady();
uint8_t nrf24_isSending();
uint8_t nrf24_getStatus();
uint8_t nrf24_rxFifoEmpty();

/* core TX / RX functions */
void    nrf24_send(uint8_t* value);
void    nrf24_getData(uint8_t* data);

/* use in dynamic length mode */
uint8_t nrf24_payloadLength();

/* post transmission analysis */
uint8_t nrf24_lastMessageStatus();
uint8_t nrf24_retransmissionCount();

/* power management */
void    nrf24_powerUpRx();
void    nrf24_powerUpTx();
void    nrf24_powerDown();
void    nrf24_transmitSync(uint8_t* dataout,uint8_t len);
void    nrf24_transferSync(uint8_t* dataout,uint8_t* datain,uint8_t len);
void    nrf24_write_reg(uint8_t reg, uint8_t value);
void    nrf24_readRegister(uint8_t reg, uint8_t* value, uint8_t len);
void    nrf24_writeRegister(uint8_t reg, uint8_t* value, uint8_t len);

#define nrf24l01_CSN_PORT PORTB //ножка СSN
#define nrf24l01_CSN_DDR DDRB
#define nrf24l01_CSN_PIN 4

#define nrf24l01_CE_PORT PORTB //ножка СE
#define nrf24l01_CE_DDR DDRB
#define nrf24l01_CE_PIN 3

// макрос уст "1" на ножке порта (x-порт,y-ножка)
#define CLEARBIT(x,y) x&=~(1<<y)
// макрос уст "0" на ножке порта (x-порт,y-ножка)
#define SETBIT(x,y)   x|=1<<y
// удобная замена для управл ножкой CE
#define nrf24_ce_low CLEARBIT(nrf24l01_CE_PORT,nrf24l01_CE_PIN)
#define nrf24_ce_high SETBIT(nrf24l01_CE_PORT,nrf24l01_CE_PIN)
// удобная замена для управл ножкой CSN
#define nrf24_csn_low CLEARBIT(nrf24l01_CSN_PORT,nrf24l01_CSN_PIN)
#define nrf24_csn_high SETBIT(nrf24l01_CSN_PORT,nrf24l01_CSN_PIN)


uint8_t payload_len;

/************************************************************/
/*              ИНИЦИАЛИЗАЦИЯ И КОНФИГУРАЦИЯ                */
/************************************************************/
void nrf24_init_config(uint8_t channel, uint8_t pay_length)
{
	SETBIT(nrf24l01_CSN_DDR,nrf24l01_CSN_PIN);   //устанавливаем ножку CSN на выход
	SETBIT(nrf24l01_CE_DDR,nrf24l01_CE_PIN);     //устанавливаем ножку CE на выход
	nrf24_csn_low;                               //ноль на ножке
	nrf24_ce_low;                                //ноль на ножке
	
	nrf24_write_reg(RF_CH,channel);         //Установка частоты передачи RF (2400+RF_CH) 2400-2525
	
	payload_len = pay_length;                    //Задаем размер статической полезной нагрузки

	nrf24_write_reg(RX_PW_P0, 0x00); // Pipe not used
	nrf24_write_reg(RX_PW_P1, payload_len); // определяем размер полезной нагрузки для этой трубы
	nrf24_write_reg(RX_PW_P2, 0x00); // Pipe not used
	nrf24_write_reg(RX_PW_P3, 0x00); // Pipe not used
	nrf24_write_reg(RX_PW_P4, 0x00); // Pipe not used
	nrf24_write_reg(RX_PW_P5, 0x00); // Pipe not used
	
	nrf24_write_reg(RF_SETUP,(0<<RF_DR_HIGH)|(1<<RF_DR_LOW)|(0x03<<RF_PWR));                              // 1 Mbps, TX gain: 0dbm
	nrf24_write_reg(CONFIG,nrf24_SETTING);                                                                // CRC enable, 1 byte CRC length
	nrf24_write_reg(EN_AA,(1<<ENAA_P0)|(1<<ENAA_P1)|(0<<ENAA_P2)|(0<<ENAA_P3)|(0<<ENAA_P4)|(0<<ENAA_P5)); // Auto Acknowledgment
	nrf24_write_reg(EN_RXADDR,(1<<ERX_P0)|(1<<ERX_P1)|(0<<ERX_P2)|(0<<ERX_P3)|(0<<ERX_P4)|(0<<ERX_P5));   // Enable RX addresses
	nrf24_write_reg(SETUP_RETR,(0x04<<ARD)|(0x0F<<ARC));	                                              // Auto retransmit delay: 1000 us and Up to 15 retransmit trials
	nrf24_write_reg(DYNPD,(0<<DPL_P0)|(0<<DPL_P1)|(0<<DPL_P2)|(0<<DPL_P3)|(0<<DPL_P4)|(0<<DPL_P5));       // Dynamic length configurations: No dynamic length

	nrf24_powerUpRx();	//Переходим в режим приемника
}

/************************************************************/
/*              ЗАПИСЬ АДРЕСА ПРИЕМНИКА                     */
/************************************************************/

void nrf24_rx_address(uint8_t * adr)
{
	nrf24_ce_low;
	nrf24_writeRegister(RX_ADDR_P1,adr,nrf24_ADDR_LEN);
	nrf24_ce_high;
}

/************************************************************/
/*            ЗАПИСЬ АДРЕСА ПЕРЕДАТЧИКА                     */
/************************************************************/
void nrf24_tx_address(uint8_t* adr)
{
	/* RX_ADDR_P0 must be set to the sending addr for auto ack to work. */
	nrf24_writeRegister(RX_ADDR_P0,adr,nrf24_ADDR_LEN);
	nrf24_writeRegister(TX_ADDR,adr,nrf24_ADDR_LEN);
}

/************************************************************/
/* ПРОВЕРЯЕТ НАЛИЧИЕ ПРИНЯТЫХ ДАННЫХ, ЕСЛИ ЕСТЬ ТО ВОЗВР. 1 */
/************************************************************/
uint8_t nrf24_dataReady()
{
	uint8_t status = nrf24_getStatus();
	// We can short circuit on RX_DR, but if it's not set, we still need
	// to check the FIFO for any pending packets
	if ( status & (1 << RX_DR) )
	{
		return 1;
	}
	return !nrf24_rxFifoEmpty();
}

/************************************************************/
/*       ПРОВЕРЯЕТ РЕГИСТРЫ FIFO RX ПУСТЫЕ ИЛИ НЕТ          */
/************************************************************/
uint8_t nrf24_rxFifoEmpty()
{
	uint8_t fifoStatus;
	nrf24_readRegister(FIFO_STATUS,&fifoStatus,1);
	return (fifoStatus & (1 << RX_EMPTY));
}

/************************************************************/
/*       ВОЗВРАЩАЕТ ДЛИННУ ДАННЫХ ОЖИДАЮЩИХ В RX fifo       */
/************************************************************/
uint8_t nrf24_payloadLength()
{
	uint8_t status;
	nrf24_csn_low;
	SPI_MasterTransmit(R_RX_PL_WID);
	status = SPI_MasterTransmit(NOP);
	nrf24_csn_high;
	return status;
}

/************************************************************/
/*            СЧИТЫВАЕТ ПРИНЯТЫЕ БАЙТЫ В МАССИВ DATA        */
/************************************************************/
void nrf24_getData(uint8_t* data)
{
	nrf24_csn_low;
	SPI_MasterTransmit( R_RX_PAYLOAD );
	nrf24_transferSync(data,data,payload_len);  //считываем столько байт, сколько прописали в инициализации
	nrf24_csn_high;
	nrf24_write_reg(STATUS,(1<<RX_DR));         //сбрасываем флаг
}

/************************************************************/
/*   ВОЗВРАЩАЕТ ЧИСЛО РЕТРАНСМИССИЙ В МИНУВШЕЙ ПОСЫЛКЕ      */
/************************************************************/
uint8_t nrf24_retransmissionCount()
{
	uint8_t rv;
	nrf24_readRegister(OBSERVE_TX,&rv,1);
	rv = rv & 0x0F;
	return rv;
}

/************************************************************/
/* ОТПРАВКА ПАКЕТА ПО АДРЕСУ. ПАКЕТЫ RX TX ДОЖНЫ БЫТЬ РАВНЫ */
/************************************************************/
// Sends a data package to the default address. Be sure to send the correct
// amount of bytes as configured as payload on the receiver.
void nrf24_send(uint8_t* value)
{
	nrf24_ce_low;                             //переходим в режим standby-1
	nrf24_powerUpTx();                        //устанавливаем режим передатчика
	nrf24_csn_low;
	SPI_MasterTransmit(FLUSH_TX);             //команда очистки TX FIFO
	nrf24_csn_high;
	nrf24_csn_low;
	SPI_MasterTransmit(W_TX_PAYLOAD);         //команда на запись данных в TX FIFO
	nrf24_transmitSync(value,payload_len);    //записываем данные
	nrf24_csn_high;
	nrf24_ce_high;                            //отправляем данные
}

/************************************************************/
/*           ПРОВЕРКА НА ОТПРАВКУ ПАКЕТА                    */
/************************************************************/
uint8_t nrf24_isSending()
{
	uint8_t status;
	status = nrf24_getStatus();
	if((status & ((1 << TX_DS)  | (1 << MAX_RT))))   //проверка флагов успешной передачи и неуспешной
	{
		return 0; /* false */
	}
	return 1; /* true */
}

/************************************************************/
/*            ПОЛУЧЕНИЕ СТАТУСНОГО РЕГИСТРА                 */
/************************************************************/
uint8_t nrf24_getStatus()
{
	uint8_t rv;
	nrf24_csn_low;
	rv = SPI_MasterTransmit(NOP);
	nrf24_csn_high;
	return rv;
}

/************************************************************/
/*             ПРОВЕРКА КАЧЕСТВА ПЕРЕДАЧИ                   */
/************************************************************/
uint8_t nrf24_lastMessageStatus()
{
	uint8_t rv;
	rv = nrf24_getStatus();
	if((rv & ((1 << TX_DS))))
	{
		return NRF24_TRANSMISSON_OK;
	}
	else if((rv & ((1 << MAX_RT))))
	{
		return NRF24_MESSAGE_LOST;
	}
	
	//else                           //вероятно еще посылает
	//{
	return 0xFF;
	//}
}

/************************************************************/
/*                   РЕЖИМ ПРИЕМНИКА                        */
/************************************************************/
void nrf24_powerUpRx()
{
	nrf24_csn_low;
	SPI_MasterTransmit(FLUSH_RX);
	nrf24_csn_high;
	nrf24_write_reg(STATUS,(1<<RX_DR)|(1<<TX_DS)|(1<<MAX_RT));
	nrf24_ce_low;
	nrf24_write_reg(CONFIG,nrf24_SETTING|((1<<PWR_UP)|(1<<PRIM_RX)));
	nrf24_ce_high;
}

/************************************************************/
/*                     РЕЖИМ ПЕРЕДАТЧИКА                    */
/************************************************************/
void nrf24_powerUpTx()
{
	nrf24_write_reg(STATUS,(1<<RX_DR)|(1<<TX_DS)|(1<<MAX_RT));
	nrf24_write_reg(CONFIG,nrf24_SETTING|((1<<PWR_UP)|(0<<PRIM_RX)));
}

/************************************************************/
/*                  POWER DOWN РЕЖИМ                        */
/************************************************************/
void nrf24_powerDown()
{
	nrf24_ce_low;
	nrf24_write_reg(CONFIG,nrf24_SETTING);
}

/************************************************************/
/*         ЧТЕНИЕ НЕСКОЛЬКИХ БАЙТ ИЗ RX FIFO                */
/************************************************************/
void nrf24_transferSync(uint8_t* dataout,uint8_t* datain,uint8_t len)
{
	uint8_t i;
	for(i=0;i<len;i++)
	{
		datain[i] = SPI_MasterTransmit(dataout[i]);
	}
}

/************************************************************/
/*             ЗАПИСЬ НЕСКОЛЬКИХ БАЙТ                       */
/************************************************************/
/* send multiple bytes over SPI */
void nrf24_transmitSync(uint8_t* dataout,uint8_t len)
{
	uint8_t i;
	for(i=0;i<len;i++)
	{
		SPI_MasterTransmit(dataout[i]);
	}
}

/************************************************************/
/*     ЗАПИСЬ ТОЛЬКО ОДНОГО БАЙТА В УКАЗАННЫЙ РЕГИСТР       */
/************************************************************/
void nrf24_write_reg(uint8_t reg, uint8_t value)
{
	nrf24_csn_low;
	SPI_MasterTransmit(W_REGISTER | (REGISTER_MASK & reg));
	SPI_MasterTransmit(value);
	nrf24_csn_high;
}

/************************************************************/
/*     ЧТЕНИЕ НЕСКОЛЬКИХ БАЙТОВ ИЗ ОДНОГО РЕГИСТРА          */
/************************************************************/
void nrf24_readRegister(uint8_t reg, uint8_t* value, uint8_t len)
{
	nrf24_csn_low;
	SPI_MasterTransmit(R_REGISTER | (REGISTER_MASK & reg));
	nrf24_transferSync(value,value,len);
	nrf24_csn_high;
}

/************************************************************/
/*         ЗАПИСЬ НЕСКОЛЬКИХ БАЙТ В ОДИН РЕГИСТР            */
/************************************************************/
/* Write to a single register of nrf24 */
void nrf24_writeRegister(uint8_t reg, uint8_t* value, uint8_t len)
{
	nrf24_csn_low;
	SPI_MasterTransmit(W_REGISTER | (REGISTER_MASK & reg));
	nrf24_transmitSync(value,len);
	nrf24_csn_high;
}
