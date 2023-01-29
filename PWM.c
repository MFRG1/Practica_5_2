#include "lib/include.h"

/*Usando el modulo 0 de PWM con una frecuencia de reloj del sistema de 20,000,000 Hz
  junto con el generador 0,1,2  habilitar alguno de los pwm's asociados y obtener un PWM
  cuya frecuencia sea de 50Hz

  Pag. 1672
  generador 0 --> PWM1 - PF1
  generador 1 --> PWM2 - PF2
  generador 2 --> PWM4 - PG0
*/

extern void Configura_Reg_PWM0(uint16_t freq)
{
    // SE HABILITA  PWM0 pag 398 
    SYSCTL->RCGCPWM |= (1<<0); 
    
    //SE HABILITA GPIO G y F POR QUE SE UTILIZA PG0, PF1 y PF2 
    ///pag. 383 INDICA LOS PUERTOS Y PINES DE CADA PWM Y GENERADOR
    SYSCTL->RCGCGPIO |= (1<<6) | (1<<5);
    
    //SE HABILITA LA FUNCION ALTERNATIVA pag. 770
    GPIOG_AHB->AFSEL |= (1<<0); //PG0 
    GPIOF_AHB->AFSEL |= (1<<1) | (1<<2); //PF1 y PF2
    
    //SE HABILITAN LOS PINES PWM   1 SALIDA Y  0 ENTRADA 
    GPIOG_AHB->DIR = (1<<0);
    GPIOF_AHB->DIR = (1<<1) | (1<<2);
    
    //PARA INDICAR QUE ES UNA FUNCION ALTERNATIVA TABLA Pag 1808 REGISTRO PCTL 
    //LE DIGOQ UE ES UNA SEÑAL PWM Pag 788, SE USA EL PIN 6  
    GPIOG_AHB->PCTL |= (GPIOG_AHB->PCTL&0xFFFFFFF0) | 0x00000006; //PG0
    GPIOF_AHB->PCTL |= (GPIOF_AHB->PCTL&0xFFFFF00F) | 0x00000660; //PF1 y PF2
    
    // 1 PARA DIGITAL O  0 ANALOGICO 
    GPIOG_AHB->DEN |= (1<<0); //PG0
    GPIOF_AHB->DEN |= (1<<1) | (1<<2); //PF1 PF2
    
    //HABILITA O DESHABILITA EL DIVISOR Y LE DA VALOR  pag 1747
    //1 EN EL BIT 8 PARA ACTIVAR DIVISOR EN EL BIT 0,1,2 PONEMOS UN "2" BINARIO PARA DIVIDIR ENTRE 8
                                           //  frec, tarjeta / frec PWM = cuentas
    PWM0->CC = (1<<8) | (0<<2) | (1<<1) | (0<<0);//SE HABILITA PORQUE 50 MIL CUENTAS NO CABE EN 16 BITS DEL CONTADOR PWM 
                                                      //20 millinoes/ 50 = 400 mil cuentas
    
    // SE DESACTIVA EL GENERADOR SEGUN LA TABLA 1672  
    //PARA SABER EL GENERADOR SE USA LA TABLA pag. 1708 Y SABER A QUE BIT SE LE PONE VALOR 
    //    GENERADOR
    PWM0->_0_CTL = (0<<0);//0 a 0 ESTE BIT INDICA SI HABILITADO O NO pag. 1708
    PWM0->_1_CTL = (0<<0);
    PWM0->_2_CTL = (0<<0);
    
   // MODO CONFIGURACION - REGRESIVO pag. 1727, pag. 1678 INDICA EL VALOR, 
   //1687 INDICA EL COMPARADOR DEL PWM 
   //(0X2<<2) PONE PWMB EN BAJO SI COUNTER=LOAD 
   //(0X3<<10) PONE PWMB EN ALTO CUANDO CONTADOR == COMPARADOR B, 
   //(0x0<<0) NO HACER NADA CUANDO COUNTER==0 
   //(0X3<<6) ENVIA PWMA A ALTO CUANDO CONTADOR == A COMPARADOR A 
   PWM0->_0_GENB |= (0X2<<2)|(0X3<<10)|(0x0<<0);   
   PWM0->_1_GENA |= (0X2<<2)|(0X3<<6)|(0x0<<0);  
   PWM0->_2_GENA |= (0X2<<2)|(0X3<<6)|(0x0<<0); 

                           // frec tiva  /divisor = resultado /frec PWM = cuentas
    // CONFIGURAR CARGAS: CUENTAS=fclk/fpwm  
    //20MHz,  cuentas = (20,000,000 / 8 = 2500000   /    50   = 50000) 
    PWM0->_0_LOAD = 50000;
    PWM0->_1_LOAD = 50000;
    PWM0->_2_LOAD = 50000;
    
    // EVALOR DE LOS CICLOS DE TRABAJO
    PWM0->_0_CMPB = 5000;//Cuentas para 2 ms
    PWM0->_1_CMPA = 5000;           
    PWM0->_2_CMPA = 5000;

    //PARA HABILITAR LOS GENERADORES 
    PWM0->_0_CTL = (1<<0);
    PWM0->_1_CTL = (1<<0);
    PWM0->_2_CTL = (1<<0);
    
    //SE HABILITAN LOS PWM QUE SE USAN PWM1, PWM2 y PWM4 pag. 1247 y 1233
    PWM0->ENABLE = (1<<1) | (1<<2) | (1<<4);          
}