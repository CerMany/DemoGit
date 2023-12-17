#include "MKL46Z4.h"

#define SW1_PIN (1u << 3)
#define SW3_PIN (1u << 12)
#define RED_LED_PIN  (1u << 29)
#define GREEN_LED_PIN (1u << 5)

volatile int32_t msTicks = 0; 
int sw1Pressed = 0;
int sw3Pressed = 0;

// led
void init_led() {
    SIM->SCGC5 |= SIM_SCGC5_PORTE_MASK;
    PORTE->PCR[29] = PORT_PCR_MUX(1u);
    PTE->PDDR |= RED_LED_PIN;
    PTE->PSOR |= RED_LED_PIN; 

    SIM->SCGC5 |= SIM_SCGC5_PORTD_MASK;
    PORTD->PCR[5] = PORT_PCR_MUX(1u);
    PTD->PDDR |= GREEN_LED_PIN;
    PTD->PSOR |= GREEN_LED_PIN; 
}
//switch
void InitPressSW(void) {
    SIM->SCGC5 |= SIM_SCGC5_PORTC_MASK;

    PORTC->PCR[3] = PORT_PCR_MUX(1) | PORT_PCR_PE_MASK | PORT_PCR_PS_MASK | PORT_PCR_IRQC(0xA);
    PTC->PDDR &= ~SW1_PIN;

    PORTC->PCR[12] = PORT_PCR_MUX(1) | PORT_PCR_PE_MASK | PORT_PCR_PS_MASK | PORT_PCR_IRQC(0xA);
    PTC->PDDR &= ~SW3_PIN;

    NVIC_EnableIRQ(PORTC_PORTD_IRQn);
}

// xu ly ngat tu nut nhan
void PORTC_PORTD_IRQHandler(void) {
    if ((PTC->PDIR & SW1_PIN)==0) {
				sw1Pressed++;
				sw1Pressed = sw1Pressed%2; 			
				msTicks =0;
    }
    if ((PTC->PDIR & SW3_PIN)==0) {
			
					sw3Pressed++;
				  sw3Pressed = sw3Pressed%2; 
					msTicks=0;
    }
		// dung ngat de chay chuong trinh
    PORTC->PCR[3] |= PORT_PCR_ISF_MASK; 
    PORTC->PCR[12] |= PORT_PCR_ISF_MASK; 
}

// SysTick
void init_systick_interrupt(){
	SysTick->LOAD = SystemCoreClock / 1000; //configured the SysTick to count in 1ms/* Select Core Clock & Enable SysTick & Enable Interrupt */
	SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk |SysTick_CTRL_TICKINT_Msk |SysTick_CTRL_ENABLE_Msk;
}

void SysTick_Handler (void) { // SysTick interrupt Handler
			msTicks++; // Increment counter
}

void Delay (uint32_t TICK) {
while (msTicks < TICK); // Wait 500ms
msTicks = 0; // Reset counter
}


int main(void) {
    init_led();
    InitPressSW();
    init_systick_interrupt();
	

    while (1) {
			
        if ((sw1Pressed == 1) && (sw3Pressed == 0) && (msTicks < 3000)) {
            // nhan sw1 nhan sw3 time < 2s Buzzer
					  PTD->PCOR |= GREEN_LED_PIN; //bat den xanh
            PTE->PSOR |= RED_LED_PIN; // chuyen trang thai led do
        } else if ((sw1Pressed == 1) && (sw3Pressed == 0) && (msTicks > 3000)) {
            // nhan sw1 ko sw3 time > 2s 
            PTE->PCOR |= RED_LED_PIN; // bat led do
					  PTD->PSOR |= GREEN_LED_PIN;// tat led xanh
        }  else if ((sw1Pressed == 1) && (sw3Pressed == 1)) {
            // nhan sw1 sau do nhan sw2
					  PTE->PSOR |= RED_LED_PIN; // tat led do
            PTD->PCOR |= GREEN_LED_PIN; // bat led xanh
        } else {
						// tat led
            PTE->PSOR |= RED_LED_PIN; 
            PTD->PSOR |= GREEN_LED_PIN; 
        }
    }
}