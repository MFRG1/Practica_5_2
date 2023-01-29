#include "lib/include.h"

/* Para controlar 3 potenciometros se habilitan los 3 canales en el modulo 0  
   pag. 1056 para saber que pines corresponden a cada canal a una velocidad de
   conversion de 250ksps dandole la mayor con evento de procesador 

   Pag. 1808
   Canal 1 -> PE2 - secuenciador 0 
   Canal 2 -> PE1 - secuenciador 1
   Canal 8 -> PE5 - secuenciador 2 
    */

extern void Configura_Reg_ADC0(void)
{
    //Pag 396 INICIALIZAR MODULO DEL RELOJ RCGCADC MODULO  0
    SYSCTL->RCGCADC = (1<<0);

    //Pag 382 (RGCGPIO) PUERTO  BASE   HABILITACION RELOJ
    //                     F     E      D       C      B     A
    SYSCTL->RCGCGPIO |= (1<<5)|(1<<4)|(0<<3)|(0<<2)|(0<<1)|(1<<0);

    //Pag 760 (GPIODIR) HABILITA PINES I/O 0 Y 1
    GPIOE_AHB->DIR = (0<<1) | (0<<2) | (0<<5); //PE1, PE2 y PE5

    //(GPIOAFSEL) pag.770 Enable alternate función para que el modulo analógico tenga control de esos pines
    GPIOE_AHB->AFSEL =  (1<<1) | (1<<2) | (1<<5); //PE1, PE2 y PE5

    //(GPIODEN) pag.781 DESHABILITA MODULO DIGITAL
    GPIOE_AHB->DEN = (0<<1) | (0<<2) | (0<<5); //PE1, PE2 y PE5

    //Pag 787 GPIOPCTL registro combinado con el GPIOAFSEL y la tabla pag 1808
    GPIOE_AHB->PCTL = GPIOE_AHB->PCTL & (0xFF0FF00F); //0 EN LOS BITS 1, 2, 5

    //(GPIOAMSEL) pag.786 habilitar analogico
    GPIOE_AHB->AMSEL = (1<<1) | (1<<2) | (1<<5); //PE1, PE2 y PE5

    //Pag 1159 EL REGISTRO (ADCPC) VELOCIDAD DE CONVERSION POR SEGUNDO
    ADC0->PC = (0<<2)|(0<<1)|(1<<0);//250ksps: 1 BINARIO INDICA  2 MILLONES SE DIVIDE EN /8 = 250ksps

    //Pag 1099 (ADCSSPRI) CONFG PRIORIDAD DE SECUENCUADORES
    ADC0->SSPRI = 0x3210;//MAYORNPRIORIDAD SEC. 0

    //Pag 1077 (ADCACTSS) CONTROL ACTIVIDAD SECUENCIADORES
    ADC0->ACTSS  =   (0<<3) | (0<<2) | (0<<1) | (0<<0);

    //Pag 1091 (ADCEMUX) SELECCIONA EL EVENTO QUE ACTIVA EL  (trigger)
    ADC0->EMUX  = (0x0000);//CONFG PROCESADOR 

    //Pag 1129 (ADCSSMUX2) DEF ENTRADAS ANALOGICAS CON CANAL Y SECUENCIADOR SELECC
    ADC0->SSMUX0 = (1<<0); //SEC 0 CHANEL 1
    ADC0->SSMUX1 = (2<<0); //SEC 1 CHANEL 2
    ADC0->SSMUX2 = (8<<0); //SEC 2 CHANEL 8

    //pag 1130 (ADCSSCTL2), CONF BIT CONTROL MUESTREO E INTERRUPCION 
    ADC0->SSCTL0 = (1<<2) | (1<<1);
    ADC0->SSCTL1 = (1<<2) | (1<<1);
    ADC0->SSCTL2 = (1<<2) | (1<<1);
    
    /* Enable ADC Interrupt */
    ADC0->IM |= (1<<0) | (1<<1) | (1<<2); /* Unmask ADC0 sequence 0, 1 y 2 interrupt pag 1082*/
    //NVIC_PRI4_R = (NVIC_PRI4_R & 0xFFFFFF00) | 0x00000020;
    //NVIC_EN0_R = 0x00010000;
    
    //Pag 1077 (ADCACTSS) ACTIVACION DE LOS SECUENCIADORES
    ADC0->ACTSS = (0<<3) | (1<<2) | (1<<1) | (1<<0);
    
    ADC0->PSSI |= (1<<0) | (1<<1) | (1<<2);  //CONF PROCESADOR
}

extern void ADC0_InSeq2(uint16_t *Result,uint16_t *duty)
{
    //HABILITA MODO DE CONFIGURACION
    ADC0->PSSI |= (1<<0) | (1<<1) | (1<<2);  //PROCESADIR
      
    // CONVERTIDOR
    while((ADC0->RIS&0x04)==0){};  
       
    //SEC 0 - CHANEL 1 - PE2
    Result[0] = ADC0->SSFIFO0&0xFFF; // FIFO0 ALMACENA MUESTRAS SEC 0 pag 860 
    duty[0] = (Result[0]*50000)/4096; //CUENTAS 20ms 
    
    //SEC  1 - CHANEL 2 - PE1
    Result[1] = ADC0->SSFIFO1&0xFFF; // FIFO0 ALMACENA MUESTRAS SEC 1
    duty[1] = (Result[1]*50000)/4096; //CUENTAS 20ms
     
    //SEC 2 - CHANEL 8 - PE5
    Result[2] = ADC0->SSFIFO2&0xFFF;// FIFO0 ALMACENA MUESTRAS SEC 2
    duty[2] = (Result[2]*50000)/4096;//CUENTAS 20ms
    
    ADC0->ISC = (1<<0) | (1<<1) | (1<<2);  //FINALIZAR CONFG

}


