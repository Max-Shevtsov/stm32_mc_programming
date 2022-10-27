#include <stdint.h>
#include <stm32f10x.h>
#include <stdbool.h>
#define MIN(a,b) (((a)<(b))?(a):(b))

/*

int __attribute((noreturn)) main(void) {
	
	// Enable clock for GPIOC
	RCC->APB2ENR |= RCC_APB2ENR_IOPCEN;
	// Enable PC13 push-pull mode
	GPIOC->CRH &= ~GPIO_CRH_CNF13; //clear cnf bits
	GPIOC->CRH |= GPIO_CRH_MODE13_0; //Max speed = 10Mhz
	
	GPIOC->CRH = GPIOC->CRH & ~(GPIO_CRH_CNF14 | GPIO_CRH_MODE14) | GPIO_CRH_CNF14_1;	//PC14 = input
	GPIOC->ODR |= GPIO_ODR_ODR14;	// Enable PC14
	
	uint32_t i;
	uint8_t  button_state = 0xFF;

    while (1) {
		if(!(GPIOC->IDR & GPIO_IDR_IDR14) ){ // button is pressed
			button_state = ~button_state;
			for(i=0; i<300000; i++) { __NOP();};	//debouncing
			while(!(GPIOC->IDR & GPIO_IDR_IDR14)); // wait the button to be released
		}
		
		if (button_state) {
	    	GPIOC->ODR &= ~(GPIO_ODR_ODR13);
			for(i=0; i<300000; i++) { __NOP();};
	   		
			GPIOC->ODR |= (GPIO_ODR_ODR13); 
	    	for(i=0; i<300000; i++) { __NOP();};
		}
    }
}
*/
int delay_us(int us){
	for(int i = 0; i< us; i++);
}

int __attribute((noreturn)) main(void) {
	
	// Enable clock for GPIOC
	RCC->APB2ENR |= RCC_APB2ENR_IOPCEN;
	// Enable PC13 push-pull mode
	GPIOC->CRH &= ~GPIO_CRH_CNF13; //clear cnf bits
	GPIOC->CRH |= GPIO_CRH_MODE13_0; //Max speed = 10Mhz
	
	GPIOC->CRH = GPIOC->CRH & ~(GPIO_CRH_CNF14 | GPIO_CRH_MODE14) | GPIO_CRH_CNF14_1;	//PC14 = input
	GPIOC->ODR |= GPIO_ODR_ODR14;	// Enable PC14
	
	
	uint32_t btnPeriod = 10000,		ledPeriod = 1000000;
	uint32_t btnPhase = btnPeriod, 	ledPhase = ledPeriod;
	bool ledEnabled = true, 		buttonPrevState = GPIOC->IDR & (1<<14);
	
	while (1) {
		uint32_t tau = MIN(btnPhase, ledPhase);
		delay_us(tau);
		ledPhase -= tau; btnPhase -= tau;
		
		if(btnPhase == 0){//It's time to check button state
			btnPhase = btnPeriod;
			bool buttonNewState = GPIOC->IDR & (1<<14);
			if (!buttonNewState && buttonPrevState){//button line change level 1->0
				
				ledEnabled = !ledEnabled; //Logical NOT operation
				if(!ledEnabled){
					GPIOC->ODR |= (GPIO_ODR_ODR13);
				}
				
			}
			buttonPrevState = buttonNewState;
		}
		if (ledPhase == 0) { //It's time to change led state
			ledPhase = ledPeriod;
			if (ledEnabled) {
				//toggle with BSRR register
				uint32_t _gpios = GPIOC->ODR;
				GPIOC->BSRR = ((_gpios & (1<<13))<<16)|(~_gpios & (1<<13));
			}
		}
    }
}