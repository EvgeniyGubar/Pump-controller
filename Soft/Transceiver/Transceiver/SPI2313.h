
//����� ���������� SPI
#define MOSI  6
#define MISO  5
#define SCK   7

//���� ����������  SPI
#define SPI_DDR DDRB
#define SPI_PORT PORTB

/* ������� ������������� ������� ���� SPI */
void SPI_MasterInit(void)
{
// ��������� ������� SPI �� �����	 
SPI_DDR|=(1<<MOSI)|(1<<SCK);
}

/* ������� �������� ����� ������ outData. ������� ���������
�������� � ���������� �������� �� ����� MISO ���� */
unsigned char SPI_MasterTransmit(char outData)
{
USIDR = outData;	// ������ ��������
USISR|=(1<<USIOIF);	//�������� ���� ������������ ��������
while(!(USISR & (1<<USIOIF)))	//��������� �� ������������
	{
		USICR|=(1<<USIWM0)|(1<<USICS1)|(1<<USICLK)|(1<<USITC);	//������������ � ��������� ���������� ��������
	}
return USIDR; //���������� �������� ����
}
