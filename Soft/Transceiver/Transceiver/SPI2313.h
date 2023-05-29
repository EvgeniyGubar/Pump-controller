
//ножки интерфейса SPI
#define MOSI  6
#define MISO  5
#define SCK   7

//порт интерфейса  SPI
#define SPI_DDR DDRB
#define SPI_PORT PORTB

/* Функция инициализации мастера шины SPI */
void SPI_MasterInit(void)
{
// Установка выводов SPI на вывод	 
SPI_DDR|=(1<<MOSI)|(1<<SCK);
}

/* Функция передачи байта данных outData. Ожидает окончания
передачи и возвращает принятый по ножке MISO байт */
unsigned char SPI_MasterTransmit(char outData)
{
USIDR = outData;	// Начало передачи
USISR|=(1<<USIOIF);	//опускаем флаг переполнения счетчика
while(!(USISR & (1<<USIOIF)))	//проверяем на переполнение
	{
		USICR|=(1<<USIWM0)|(1<<USICS1)|(1<<USICLK)|(1<<USITC);	//тактирование и установка параметров передачи
	}
return USIDR; //возвращаем принятый байт
}
