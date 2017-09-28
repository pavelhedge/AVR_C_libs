/*****************************************
Library I2C
*****************************************/

#include <avr/io.h>

#define TWI_BAUDRATE 100000
// Selects prescaler and bitrate register for given baudrate
#if (F_CPU / TWI_BAUDRATE - 16) / 2 <= 255
	#define TWI_PRESC	0
	#define TWI_TWBR	(char)((F_CPU * 1.0 / TWI_BAUDRATE - 16) / 2)

#elif ((F_CPU / TWI_BAUDRATE - 16) / 8 <= 255)
	#define TWI_PRESC	1
	#define TWI_TWBR	(char)((F_CPU * 1.0 / TWI_BAUDRATE - 16) / 8)

#elif ((F_CPU / TWI_BAUDRATE - 16) / 32 <= 255)
	#define TWI_PRESC	2
	#define TWI_TWBR	(char)((F_CPU * 1.0 / TWI_BAUDRATE - 16) / 32)
	
#else 
	#define TWI_PRESC	3
	#define TWI_TWBR	(char)((F_CPU * 1.0 / TWI_BAUDRATE - 16) / 128)
#endif


void i2c_master_init(void){
	DDRC = 0x00;
	PORTC = 1<<PORTC5 | 1<<PORTC4;					// Internal pull-up for SDA & SCL on
	
	TWBR = TWI_TWBR;                                  // Set bit rate register (Baud rate). Defined in header file.Driver presumes prescaler to be 00.
	TWSR = TWI_PRESC;							// Set prescaler
	TWDR = 0xFF;                                      // Default content = SDA released.
	TWCR =	(1<<TWEN)|                                 // Enable TWI-interface and release TWI pins.
			(0<<TWIE)|(0<<TWINT)|                      // Disable Interrupt.
			(0<<TWEA)|(0<<TWSTA)|(0<<TWSTO)|           // No Signal requests.
			(0<<TWWC);                                 //
}

void i2c_address_search(void){
	//UDR0 = 0;
	for(char i = 0; i < 254 ; i += 2){
		//while( ~TWCR & (1<<TWINT) );			// ∆дем, пока TWI закончит операцию

		TWCR =	(1<<TWEN)|                             // TWI Interface enabled.
		(0<<TWIE)|(1<<TWINT)|                  // clear the flag.
		(0<<TWEA)|(1<<TWSTA)|(0<<TWSTO)|       // Initiate a START condition.
		(0<<TWWC);                 
	
		while( ~TWCR & (1<<TWINT) );
		TWDR = i;
		TWCR =	(1<<TWEN)|                                 // TWI Interface enabled
		(0<<TWIE)|(1<<TWINT)|                      // Enable TWI Interrupt and clear the flag to send byte
		(0<<TWEA)|(0<<TWSTA)|(0<<TWSTO)|           //
		(0<<TWWC);     
	                            //
		while( ~TWCR & (1<<TWINT) );
		
		if (TWSR == 0x18 ||TWSR == 0x40) UDR0 = i;
	}
}
/*
void i2c_start(void){
	while( TWCR & (1<<TWIE) );
	TWCR =	(1<<TWEN)|                             // TWI Interface enabled.
			(1<<TWIE)|(1<<TWINT)|                  // Enable TWI Interrupt and clear the flag.
			(0<<TWEA)|(1<<TWSTA)|(0<<TWSTO)|       // Initiate a START condition.
			(0<<TWWC);                             //
}



void i2c_write(unsigned char msg){
	while( TWCR & (1<<TWIE) );
	TWDR = msg;
	TWCR =	(1<<TWEN)|                                 // TWI Interface enabled
			(1<<TWIE)|(1<<TWINT)|                      // Enable TWI Interrupt and clear the flag to send byte
			(0<<TWEA)|(0<<TWSTA)|(0<<TWSTO)|           //
			(0<<TWWC);                                 //
}

void i2c_stop(void){
	while( TWCR & (1<<TWIE) );
	TWCR =	(1<<TWEN)|                                 // TWI Interface enabled
			(0<<TWIE)|(1<<TWINT)|                      // Disable TWI Interrupt and clear the flag
			(0<<TWEA)|(0<<TWSTA)|(1<<TWSTO)|           // Initiate a STOP condition.
			(0<<TWWC);                                 //
}



*/