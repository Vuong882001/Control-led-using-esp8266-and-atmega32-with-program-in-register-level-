#include <avr/io.h>
#include <avr/delay.h>
#include <avr/interrupt.h>

volatile unsigned char u_data;
//chuong trinh con phat du lieu
void uart_char_rx()
{
	while ( !(UCSRA & (1<<RXC)) ) ; //bit RXC trên UCSRA cho biet qua trinh nhan ket thuc hay chua
}

int main(void){
	//set baud, 4800 ung voi f=1Mhz, xem bang 70 trang 165, Atmega32 datasheet
	UBRRH=0;
	UBRRL=12;
	
	//set khung truyen va kich hoat bo nhan du lieu
	UCSRA=0x00;
	UCSRC=(1<<URSEL)|(1<<UCSZ1)|(1<<UCSZ0);// chon la thanh ghi UCSRC ,truyen 8bit
	UCSRB=(1<<TXEN)|(1<<RXEN)|(1<<RXCIE); // khoi dong bo phat va bo nhan data va cho phep ngat khi qua trinh nhan ket thuc
	sei();
	DDRA=0x03;
	DDRB=0xff;
	while(1){
		
	}
}

ISR(USART_RXC_vect)
{ //trinh phuc vu ngat USART hoan tat nhan
	_delay_ms(100);
	uart_char_rx(); // kiem tra RXC=1 chua
	u_data=UDR;
	if(u_data == 'a'){
		PORTA |= (1 << PA0); 
		} else if(u_data == 'b'){
		PORTA &=~(1 << PA0); 
		}else if(u_data == 'c'){
		PORTA |= (1 << PA1); 
		}else if(u_data == 'd'){
		PORTA &=~(1 << PA1); 
	}
}
