//The GPIOB6 pin has been used as PWM output

#define STM32F103xB
#include "stm32f1xx.h"

void System_Clock_Config(void);
void TIM4_PWM_OUTPUT_Config(void);//start PWM

int main()
{
  //Config
  System_Clock_Config();
  TIM4_PWM_OUTPUT_Config();
  
  while(2){;}
}

//Functions bodies

void System_Clock_Config(void)
{
  //Steps
  //1 LATENCY
  //2 Enable HSE and wait for
  //3 PLL multiplication factor
  //4 PLL entry clock source
  //5 Enable PLL and wait for
  //6 Set the prescalers
  //HPRE
  //PPRE1 is devided by 2
  //PPRE2
  //7 SW and wait for

  //1 LATENCY
  FLASH->ACR|=FLASH_ACR_LATENCY_1;
  
  //2 Enable HSE and wait for
  RCC->CR|=RCC_CR_HSEON;
  while(!(RCC->CR&RCC_CR_HSERDY));

  //3 PLL multiplication factor
  RCC->CFGR|=RCC_CFGR_PLLMULL9;
  
  //4 PLL entry clock source
  RCC->CFGR|=RCC_CFGR_PLLSRC;
  
  //5 Enable PLL and wait for
  RCC->CR|=RCC_CR_PLLON;
  while(!(RCC->CR&RCC_CR_PLLRDY));
  
  //6 Set the prescalers
  //HPRE
  //RCC->CFGR&=~RCC_CFGR_HPRE;//0xxx: SYSCLK not divided
  RCC->CFGR|=RCC_CFGR_HPRE;//1111: SYSCLK divided by 512  //APBx timer clock 0.140625 MHz
  
  //PPRE1 is devided by 16
  RCC->CFGR&=~RCC_CFGR_PPRE1;
  RCC->CFGR|=RCC_CFGR_PPRE1_Msk;
  
  //PPRE2
  RCC->CFGR&=~RCC_CFGR_PPRE2;
  
  //7 SW and wait for
  RCC->CFGR|=RCC_CFGR_SW_PLL;
  while(!(RCC->CFGR&RCC_CFGR_SWS_PLL));
}

void TIM4_PWM_OUTPUT_Config(void)
{
/*Steps*/
//Configure the PB6 as an output in alternate function output Push-pull
    //Clock
    RCC->APB2ENR|=RCC_APB2ENR_IOPBEN;
    //PB6 Reset
    GPIOB->BSRR|=GPIO_BSRR_BR6;

    //Mode
    GPIOB->CRL&=~GPIO_CRL_MODE6_Msk;
    GPIOB->CRL|=GPIO_CRL_MODE6_Msk;//0x3 11: Output mode, max speed 50 MHz.

    //Configurate
    GPIOB->CRL&=~GPIO_CRL_CNF6_Msk;
    GPIOB->CRL|=GPIO_CRL_CNF6_1;//0x2 In output mode 10: Alternate function output Push-pull

//Configure the TIM4
  //Clock
  RCC->APB1ENR|=RCC_APB1ENR_TIM4EN;
  
  //Counter as upcounter or downcounter
  TIM4->CR1|=TIM_CR1_DIR;//1: Counter used as downcounter.
  //TIM4->CR1&=~TIM_CR1_DIR;//0: Counter used as upcounter.
  //TIM4->CR1|=TIM_CR1_ARPE;//1: TIMx_ARR register is buffered.
  
  //Set the prescaler's value
  //The clock friquency on the timer 4 bus is 0.017578 MHz
  TIM4->PSC=17578-1;//Prescaler value. The counter clock frequency CK_CNT is equal to fCK_PSC/ (PSC[15:0] + 1).
  
  //Set the auto-reload register's value. Does the ARR must be 3 of higher?
  TIM4->ARR=10-1;     //ARR is the value to be loaded in the actual auto-reload register. The counter is blocked while
                      //the auto-reload value is null.

  //Set the output polarity for the PB6 pin
  //TIM4->CCMR1|=(0x6<<TIM_CCMR1_OC1M_Pos);//OC1M : Output Compare 1 mode 110: PWM mode 1 - In upcounting, channel 1 is active 
                                          //as long as TIMx_CNT<TIMx_CCR1 else inactive. 
                                          //In downcounting, channel 1 is inactive (OC1REF=‘0’) as long as TIMx_CNT>TIMx_CCR1 
//high level -> 1 0
//low level  -> 9 8 7 6 5 4 3 2
                                //else active (OC1REF=’1’)
  TIM4->CCMR1|=TIM_CCMR1_OC1M;//OC1M : Output Compare 1 mode 111: PWM mode 2 - In upcounting, channel 1 is inactive as long 
                                //as TIMx_CNT<TIMx_CCR1 else active. In downcounting, channel 1 is  active as long 
                                //as TIMx_CNT>TIMx_CCR1 else inactive.
//low level   -> 1 0 
//high level  -> 9 8 7 6 5 4 3 2
  
  //Set the capture/compare value. It must be less than TIM4->ARR value
  TIM4->CCR1=1;//Capture/Compare 1 value. If channel CC1 is configured as output: The active capture/compare register contains  
                //the value to be compared to the counter TIMx_CNT and signaled on OC1 output.
  TIM4->CCER|=TIM_CCER_CC1E;//Capture/Compare 1 output enable. 1: On - OC1 signal is output on  the corresponding output pin. 

  //Enable the timer's 4 counter
  TIM4->CR1|=TIM_CR1_CEN;//CEN: Counter enable. 1: Counter enabled
}