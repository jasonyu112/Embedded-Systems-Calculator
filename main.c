#include "tm4c123gh6pm.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>


//void initTimer();
//void Timer0Handler();
//void GPIOC_Handler();
//void GPIOB_Handler();

void initA();
void initE();
void initC();
void initB();
void PortC_keypad_init();
void delay(int n);
void initLCD(void);
void passData(unsigned char data);
void LCD_command(unsigned char command);
void LCD_data(unsigned char data);
void clear();
void display_product();
void print_str(char* str, int len);
void cleartop();
void start(int *a,int *b, char a_string[], int *a_len, char b_string[], int *b_len);
void convert(long long number, char *str);
int is_digit(char digit);
void UART_Tx (char data);
void initUartT();

//output pin
#define GPIODATA_A (*((volatile unsigned long *) 0x40004010))    //base address of gpio port A offset with pin 2 masking bit
#define GPIODIR_A (*((volatile unsigned long *) 0x40004400))   //address of pin direction Port A
#define GPIODEN_A (*((volatile unsigned long *) 0x4000451C))   //address of digital enable Port A
#define GPIOA_AFSEL (*((volatile unsigned long *) 0x40004420)) //address of afsel for PORT A
#define GPIOA_PCTL (*((volatile unsigned long *) 0x4000452C))
#define RCGCGPIO (*((volatile unsigned long *) 0x400FE608)) //Clocks set bit 0 and 1 for port A

//Port C
#define GPIOBASE_C (*((volatile unsigned long *) 0x40006000))    //base address of gpio port C
#define GPIODATA_C6_R (*((volatile unsigned long *) 0x40006100)) //port C pin 6 read
#define GPIODIR_C (*((volatile unsigned long *) 0x40006400))   //address of pin direction Port C
#define GPIODEN_C (*((volatile unsigned long *) 0x4000651C))   //address of digital enable Port C
#define GPIODEN_PORTC_PIN6 0x040 // mask for PC6

//Port B
#define GPIO_PORTB_0_7       (*((volatile unsigned long *)0x400053FC))

#define UART0 (*((volatile unsigned long *) 0x4000C000))
#define UARTFR (*((volatile unsigned long *) 0x4000C018)) //address for containing receive or transmit bit
#define UARTDR (*((volatile unsigned long *) 0x4000C000)) //
#define RCGCUART (*((volatile unsigned long *) 0x400FE618)) //address for uart clock
#define UART0_IBRD (*((volatile unsigned long *) 0x4000C024)) //address for uart0 ibrd
#define UART0_FBRD (*((volatile unsigned long *) 0x4000C028)) //address for UART0 fbrd
#define UARTLCRH (*((volatile unsigned long *) 0x4000C02C)) //address for UART0 Line Control
#define UART0CC (*((volatile unsigned long *) 0x4000CFC8)) //address for UART0 clock config
#define UARTCTL (*((volatile unsigned long *) 0x4000C030)) //address for enabling UART0


//keypad
#define GPIOBASE_C (*((volatile unsigned long *) 0x40006000))    //base address of gpio port C
#define GPIODATA_C6_R (*((volatile unsigned long *) 0x40006100)) //port C pin 6 read
#define GPIODIR_C (*((volatile unsigned long *) 0x40006400))   //address of pin direction Port C
#define GPIODEN_C (*((volatile unsigned long *) 0x4000651C))   //address of digital enable Port C
#define GPIODEN_PORTC_PIN6 0x040 // mask for PC6

#define GPIO_PORTB_0_7       (*((volatile unsigned long *)0x400053FC))
#define GPIO_PORTA_DATA (*((volatile unsigned long *) 0x40004010))
void initialState(int *a,int *b, char a_string[], int *a_len, char b_string[], int *b_len);

void initLCD(void){
     delay(100);
     LCD_command(0x38); //function set / 8 bit/ 2 lines
     //LCD_command(0x0C); //display on/ cursor on/ blinking on
     LCD_command(0x06); //entry mode/ increment/ display shift off
     LCD_command(0x0F); //display on/ cursor on/ blinking on
     LCD_command(0x01); //clear display
     delay(10000);
}
void LCD_data(unsigned char data){
    //print data
    passData(data);
    GPIO_PORTA_DATA_R &= (~(1<<3)); //Read/Write to 0
    GPIO_PORTA_DATA_R |= (1<<2); //RS to 1
    GPIO_PORTA_DATA_R |= (1<<4); //turn on Enable
    delay(10000);
    GPIO_PORTA_DATA_R &= (~(1<<4)); //disable Enable
}

void LCD_command(unsigned char command){
    //print command
    passData(command);
    GPIO_PORTA_DATA_R &= (~(1<<3)); //Read/Write to 0
    GPIO_PORTA_DATA_R &= (~(1<<2)); //RS to 0
    GPIO_PORTA_DATA_R |= (1<<4); //turn on Enable
    delay(10000);
    GPIO_PORTA_DATA_R &= (~(1<<4)); //disable Enable
}

void passData(unsigned char data){
    if((data&0x01)==0x01){  //d0
        GPIO_PORTE_DATA_R |= (1<<1);
    }
    else{
        GPIO_PORTE_DATA_R &= (~(1<<1));
    }
    if((data&0x02)==0x02){  //d1
        GPIO_PORTE_DATA_R |= (1<<2);
    }
    else{
        GPIO_PORTE_DATA_R &= (~(1<<2));
    }
    if((data&0x04)==0x04){  //d2
        GPIO_PORTE_DATA_R |= (1<<3);
    }
    else{
        GPIO_PORTE_DATA_R &= (~(1<<3));
    }
    if((data&0x08)==0x08){  //d3
        GPIO_PORTE_DATA_R |= (1<<4);
    }
    else{
        GPIO_PORTE_DATA_R &= (~(1<<4));
    }
    if((data&0x10)==0x10){  //d4
        GPIO_PORTE_DATA_R |= (1<<5);
    }
    else{
        GPIO_PORTE_DATA_R &= (~(1<<5));
    }
    if((data&0x20)==0x20){    //d5
        GPIO_PORTA_DATA_R |= (1<<5);
    }
    else{
        GPIO_PORTA_DATA_R &= (~(1<<5));
    }
    if((data&0x40)==0x40){    //d6
        GPIO_PORTA_DATA_R |= (1<<6);
    }
    else{
        GPIO_PORTA_DATA_R &= (~(1<<6));
    }
    if((data&0x80)==0x80){    //d7
        GPIO_PORTA_DATA_R |= (1<<7);
    }
    else{
        GPIO_PORTA_DATA_R &= (~(1<<7));
    }

}

void initE(){
    GPIO_PORTE_DEN_R |= (1<<0) | (1<<1) | (1<<2)| (1<<3) | (1<<4) | (1<<5) | (1<<6)| (1<<7);
    GPIO_PORTE_DIR_R |= (1<<0) | (1<<1) | (1<<2)| (1<<3) | (1<<4) | (1<<5) | (1<<6)| (1<<7);
}
void initA(){
    GPIO_PORTA_DEN_R |= (1<<0) | (1<<1) | (1<<2)| (1<<3) | (1<<4) | (1<<5) | (1<<6)| (1<<7);
    GPIO_PORTA_DIR_R |= (1<<0) | (1<<1) | (1<<2)| (1<<3) | (1<<4) | (1<<5) | (1<<6)| (1<<7);

}
void delay(int n)
{
    int i;
    for(i=0; i<n; i++){
    }
}

void clear(){
    //clear display instruction set
    LCD_command(0x01);
    delay(6000);

    //reset cursor
    LCD_command(0x02);
    delay(6000);
}

void display_product(int *a,int *b, char a_string[], int *a_len, char b_string[], int *b_len){ //displays product of A and B, clear top row, reset cursor, move to Start/A state
    //display product to bottom row
    LCD_command(0xC0); //address of 2nd row first column
    long long product = ((long long)(*a)) * ((long long)(*b));
    char product_str[16];
    convert(product,product_str);

    print_str(product_str, strlen(product_str));

    //reset move to start state
    start(a,b,a_string,a_len,b_string, b_len);
}
void convert(long long number, char *str)
{
    int i = 0;
    // Convert each digit to a character
    do {
        str[i++] = number % 10 + '0';
        number /= 10;
    } while (number != 0);

    // Null-terminate the string
    str[i] = '\0';

    // Reverse the string
    int length = i;
    int j = 0;
    for (j; j < length / 2; j++) {
        char temp = str[j];
        str[j] = str[length - j - 1];
        str[length - j - 1] = temp;
    }
}
void start(int *a,int *b, char a_string[], int *a_len, char b_string[], int *b_len){
    *a = -1;
    *b = -1;
    int i =0;
    for(i; i<8;i++){
        a_string[i] = ' ';
        b_string[i] = ' ';
    }
    *a_len = 0;
    *b_len = 0;
    cleartop();
}
void initialState(int *a,int *b, char a_string[], int *a_len, char b_string[], int *b_len){
    start(a,b,a_string,a_len,b_string,b_len);
    clear();

}
void print_str(char* str, int len){
    int i;
    for(i =0; i<len; i++){
        LCD_data(str[i]);
    }
}
int is_digit(char digit){
    if( (int)digit >= 48 && (int)digit <= 57){
        return 1;
    }
    return 0;
}

void cleartop(){
    LCD_command(0x80); //address of 1st row first column
    print_str("                ", 16);
    LCD_command(0x02); //reset cursor
    delay(10000);
}


/**
 * main.c
 */
/*
void initTimer(){
    SYSCTL_RCGCTIMER_R |= 0x01;  //enable timer0
    TIMER0_CTL_R &= (~(1<<0)); //disable timer
    TIMER0_CFG_R = 0x4; //16 bit timer
    TIMER0_TAMR_R = 0x2; //setting as periodic timer and counts down
    TIMER0_TAILR_R = 64000;  //count down from this number
    TIMER0_ICR_R |= 0x01; //clear interrupt status flags
    TIMER0_TAPR_R = 0xFF;
    TIMER0_CTL_R |= 0x01;
    TIMER0_IMR_R |= 0x01;
    //enable bit 19 in nvic
    NVIC_EN0_R |= (1<<19); //200000 NVIC_EN0_INT_M

}
void Timer0Handler(){
    GPIODATA_A = 0x7;
    volatile uint32_t ui32Loop;
    for(ui32Loop = 0;ui32Loop<10000;ui32Loop++){

    }
    TIMER0_ICR_R = 0x01;

}

*/

void init_A2(){
    GPIODEN_A = 0x04;
    GPIODIR_A = 0x04;
}

void initUartT(){
    GPIODIR_A |= 0x03;
    GPIODEN_A |= 0x03;
    GPIOA_AFSEL = 0x03;
    GPIOA_PCTL = 0x11;
}
void initC(){
    GPIODEN_C = GPIODEN_PORTC_PIN6; //digital enable PC6
    //GPIODIR_C &= ~GPIODEN_PORTC_PIN6; // clear direction bit for PC6 for output with bitwise AND, inverted mask
    GPIODIR_C = 0x0;
    GPIO_PORTC_PDR_R |= (0xF0); //Pull-down resistors


}
void initB(){
    GPIO_PORTB_DEN_R |= 0x7F;
    GPIO_PORTB_DIR_R |= 0x7F;
    //GPIO_PORTB_0_7 = 0x1E;

    /*
    GPIO_PORTB_DEN_R |= (1<<6); //digital enable PC6
    GPIO_PORTB_DIR_R &= ~(1<<6);

    //configure GPIOB6 interrupt sense
    GPIO_PORTB_IS_R &= ~(1<<6);
    // clear IBE register so not both edges are detected
    GPIO_PORTB_IBE_R &= ~(1<<6);
    // set IEV for rising edge trigger
    GPIO_PORTB_IEV_R |= (1<<6);
    //clear prior interrupt
    GPIO_PORTB_ICR_R |= (1<<6);
    //unmask interrupt for PB6
    GPIO_PORTB_IM_R |= (1<<6);

    //NVIC config
    NVIC_EN0_R |= (1<<1); // Port B
    //interrupt priority (maybe?)
    */
}
/*
void GPIOB_Handler(){
    //UART transmit '$' on rising edge
    if((GPIO_PORTB_MIS_R & (1<<6)) == (1<<6)){ //check if interrupt from pin 6
        UART_Tx('$');
    }
    volatile uint32_t ui32Loop;
    for(ui32Loop = 0;ui32Loop<200000;ui32Loop++){}
    //clear interrupt flag
    GPIO_PORTB_ICR_R |= (1<<6);

    //may need to insert some delay

}
*/

void UART_Tx (char data){
    while(UARTFR & 0x20); //busy waiting while full FIFO bit set
    UARTDR = data;
}

char UART_Rx(char c){
    char data;
    while(UARTFR & 0x10);
    data = UARTDR;
    return ((unsigned char) data);

}
void PortC_keypad_init(){
    GPIO_PORTC_CR_R  |= 0xF0; //Allow settings for all pins of PORTC
    GPIO_PORTC_DEN_R |= 0xF0; //Digital enable all portC pins
}
int main(void)
{
    volatile uint32_t ui32Loop;
    char symbol[4][4] = {
      {'*','1','4','7'},
      {'0','2','5','8'},
      {'d','a','b','c'},
      {'#','3','6','9'}
    };
    //init clocks
    RCGCGPIO = (1<<0)|(1<<1)|(1<<2)|(1<<4); //enable clock for port A and port C B
    RCGCUART = 0x01; //start UART clock
    for(ui32Loop = 0;ui32Loop<2000;ui32Loop++){

    }
    //init Port A for output
    init_A2();

    //init port C for input
    initC();
    PortC_keypad_init();
    //init port b for output
    initB();
    //initialize UART
    initUartT();
    //disable uarten in UARTCTL
    UARTCTL = 0x0;
    //set ibrd
    UART0_IBRD = 104; //9600 baud
    //set FBRD
    UART0_FBRD = 11; //9600 baud
    //set line control
    UARTLCRH = 0x60;
    UART0CC = 0x0;
    UARTCTL = 0x301;
    //initTimer();
    //
    //display init
    initE();
    initA();
    delay(10000);
    initLCD();

    int a =-1;
    int b = -1;
    char a_string[8];
    int a_len = 0;
    char b_string[8];
    int b_len = 0;

    initialState(&a,&b, a_string,&a_len, b_string,&b_len);

    while(1){
        GPIODATA_A = 0x3;
        volatile uint32_t i;
        volatile uint32_t j;
        for(i = 0; i < 4; i++)    //Scan columns loop
        {
            GPIO_PORTB_0_7 = (1 << i+1);
            for(ui32Loop = 0;ui32Loop<5;ui32Loop++){

            }
            for(j = 0; j < 4; j++)  //Scan rows
            {
                if((GPIO_PORTC_DATA_R & 0xF0) & (1 << j+4)){
                    if(((char)symbol[j][i]) == 'c'){
                        initialState(&a,&b, a_string,&a_len, b_string,&b_len);
                    }
                    else if(((char)symbol[j][i]) == '*' && a == -1){
                        a = atoi(a_string);
                        cleartop();
                    }
                    else if(((char)symbol[j][i]) == '#'){
                        clear();
                        if (a ==-1){
                            a = atoi(a_string);
                        }
                        if (b == -1){
                            b = atoi(b_string);
                        }
                        display_product(&a,&b, a_string,&a_len, b_string,&b_len);
                    }
                    else{
                        if(a == -1){
                            //A_state
                            if(is_digit((char)symbol[j][i])){
                                a_string[a_len] = ((char)symbol[j][i]);
                                a_len++;
                                LCD_data(symbol[j][i]);
                            }
                            if(a_len == 8){
                                a = atoi(a_string);
                                cleartop();
                                }

                        }else{
                            //b_state
                            if(is_digit((char)symbol[j][i])){
                                b_string[b_len] = ((char)symbol[j][i]);
                                b_len++;
                                LCD_data(symbol[j][i]);
                            }
                            if(b_len == 8){
                                b = atoi(b_string);
                                clear();
                                display_product(&a,&b, a_string,&a_len, b_string,&b_len);
                            }
                        }
                    }

                }
                for(ui32Loop = 0;ui32Loop<18000;ui32Loop++){

                }
            }
        }
    }
}
