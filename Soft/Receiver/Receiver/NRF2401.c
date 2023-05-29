/*
 * NRF2401.c
 *
 * Created: 03.06.2022 15:58:56
 *  Author: User
 */ 
#include "NRF2401.h"

// ������ ��� "1" �� ����� ����� (x-����,y-�����)
#define CLEARBIT(x,y) x&=~(1<<y)
// ������ ��� "0" �� ����� ����� (x-����,y-�����)
#define SETBIT(x,y)   x|=1<<y
// ������� ������ ��� ������ ������ CE
#define nrf24_ce_low CLEARBIT(nrf24l01_CE_PORT,nrf24l01_CE_PIN)
#define nrf24_ce_high SETBIT(nrf24l01_CE_PORT,nrf24l01_CE_PIN)
// ������� ������ ��� ������ ������ CSN
#define nrf24_csn_low CLEARBIT(nrf24l01_CSN_PORT,nrf24l01_CSN_PIN)
#define nrf24_csn_high SETBIT(nrf24l01_CSN_PORT,nrf24l01_CSN_PIN)

/************************************************************/
/*              ������������� � ������������                */
/************************************************************/
void nrf24_init_config(uint8_t channel, uint8_t pay_length)
{
	SETBIT(nrf24l01_CSN_DDR,nrf24l01_CSN_PIN);   //������������� ����� CSN �� �����
	SETBIT(nrf24l01_CE_DDR,nrf24l01_CE_PIN);     //������������� ����� CE �� �����
	nrf24_csn_low;                               //���� �� �����
	nrf24_ce_low;                                //���� �� �����
	
	nrf24_write_reg(RF_CH,channel);         //��������� ������� �������� RF (2400+RF_CH) 2400-2525
	
	payload_len = pay_length;                    //������ ������ ����������� �������� ��������

	nrf24_write_reg(RX_PW_P0, 0x00); // Pipe not used
	nrf24_write_reg(RX_PW_P1, payload_len); // ���������� ������ �������� �������� ��� ���� �����
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

	nrf24_powerUpRx();	//��������� � ����� ���������
}

/************************************************************/
/*              ������ ������ ���������                     */
/************************************************************/
void nrf24_rx_address(uint8_t * adr)
{
	nrf24_ce_low;
	nrf24_writeRegister(RX_ADDR_P1,adr,nrf24_ADDR_LEN);
	nrf24_ce_high;
}

/************************************************************/
/*            ������ ������ �����������                     */
/************************************************************/
void nrf24_tx_address(uint8_t* adr)
{
	/* RX_ADDR_P0 must be set to the sending addr for auto ack to work. */
	nrf24_writeRegister(RX_ADDR_P0,adr,nrf24_ADDR_LEN);
	nrf24_writeRegister(TX_ADDR,adr,nrf24_ADDR_LEN);
}

/************************************************************/
/* ��������� ������� �������� ������, ���� ���� �� �����. 1 */
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
/*       ��������� �������� FIFO RX ������ ��� ���          */
/************************************************************/
uint8_t nrf24_rxFifoEmpty()
{
	uint8_t fifoStatus;
	nrf24_readRegister(FIFO_STATUS,&fifoStatus,1);
	return (fifoStatus & (1 << RX_EMPTY));
}

/************************************************************/
/*       ���������� ������ ������ ��������� � RX fifo       */
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
/*            ��������� �������� ����� � ������ DATA        */
/************************************************************/
void nrf24_getData(uint8_t* data)
{
	nrf24_csn_low;
	SPI_MasterTransmit( R_RX_PAYLOAD );
	nrf24_transferSync(data,data,payload_len);  //��������� ������� ����, ������� ��������� � �������������
	nrf24_csn_high;
	nrf24_write_reg(STATUS,(1<<RX_DR));         //���������� ����
}

/************************************************************/
/*   ���������� ����� ������������� � �������� �������      */
/************************************************************/
uint8_t nrf24_retransmissionCount()
{
	uint8_t rv;
	nrf24_readRegister(OBSERVE_TX,&rv,1);
	rv = rv & 0x0F;
	return rv;
}

/************************************************************/
/* �������� ������ �� ������. ������ RX TX ����� ���� ����� */
/************************************************************/
// Sends a data package to the default address. Be sure to send the correct
// amount of bytes as configured as payload on the receiver.
void nrf24_send(uint8_t* value)
{
	nrf24_ce_low;                             //��������� � ����� standby-1
	nrf24_powerUpTx();                        //������������� ����� �����������
	nrf24_csn_low;
	SPI_MasterTransmit(FLUSH_TX);             //������� ������� TX FIFO
	nrf24_csn_high;
	nrf24_csn_low;
	SPI_MasterTransmit(W_TX_PAYLOAD);         //������� �� ������ ������ � TX FIFO
	nrf24_transmitSync(value,payload_len);    //���������� ������
	nrf24_csn_high;
	nrf24_ce_high;                            //���������� ������
}

/************************************************************/
/*           �������� �� �������� ������                    */
/************************************************************/
uint8_t nrf24_isSending()
{
	uint8_t status;
	status = nrf24_getStatus();
	if((status & ((1 << TX_DS)  | (1 << MAX_RT))))   //�������� ������ �������� �������� � ����������
	{
		return 0; /* false */
	}
	return 1; /* true */
}

/************************************************************/
/*            ��������� ���������� ��������                 */
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
/*             �������� �������� ��������                   */
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
	
	//else                           //�������� ��� ��������
	//{
	//return 0xFF;
	//}
}

/************************************************************/
/*                   ����� ���������                        */
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
/*                     ����� �����������                    */
/************************************************************/
void nrf24_powerUpTx()
{
	nrf24_write_reg(STATUS,(1<<RX_DR)|(1<<TX_DS)|(1<<MAX_RT));
	nrf24_write_reg(CONFIG,nrf24_SETTING|((1<<PWR_UP)|(0<<PRIM_RX)));
}

/************************************************************/
/*                  POWER DOWN �����                        */
/************************************************************/
void nrf24_powerDown()
{
	nrf24_ce_low;
	nrf24_write_reg(CONFIG,nrf24_SETTING);
}

/************************************************************/
/*         ������ ���������� ���� �� RX FIFO                */
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
/*             ������ ���������� ����                       */
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
/*     ������ ������ ������ ����� � ��������� �������       */
/************************************************************/
void nrf24_write_reg(uint8_t reg, uint8_t value)
{
	nrf24_csn_low;
	SPI_MasterTransmit(W_REGISTER | (REGISTER_MASK & reg));
	SPI_MasterTransmit(value);
	nrf24_csn_high;
}

/************************************************************/
/*     ������ ���������� ������ �� ������ ��������          */
/************************************************************/
void nrf24_readRegister(uint8_t reg, uint8_t* value, uint8_t len)
{
	nrf24_csn_low;
	SPI_MasterTransmit(R_REGISTER | (REGISTER_MASK & reg));
	nrf24_transferSync(value,value,len);
	nrf24_csn_high;
}

/************************************************************/
/*         ������ ���������� ���� � ���� �������            */
/************************************************************/
/* Write to a single register of nrf24 */
void nrf24_writeRegister(uint8_t reg, uint8_t* value, uint8_t len)
{
	nrf24_csn_low;
	SPI_MasterTransmit(W_REGISTER | (REGISTER_MASK & reg));
	nrf24_transmitSync(value,len);
	nrf24_csn_high;
}
