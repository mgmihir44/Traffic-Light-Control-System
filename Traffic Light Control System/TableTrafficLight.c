// ***** 0. Documentation Section *****
// TableTrafficLight.c for Lab 10
// Runs on LM4F120/TM4C123
// Index implementation of a Moore finite state machine to operate a traffic light.  
// Daniel Valvano, Jonathan Valvano
// January 15, 2016
//*********************************************************************************
//*********************************************************************************
/* 
 * Modified by: Mihir Garude
 * Date: 7th June, 2018
 */
//*********************************************************************************
//*********************************************************************************
// east/west red light connected to PB5
// east/west yellow light connected to PB4
// east/west green light connected to PB3
// north/south facing red light connected to PB2
// north/south facing yellow light connected to PB1
// north/south facing green light connected to PB0
// pedestrian detector connected to PE2 (1=pedestrian present)
// north/south car detector connected to PE1 (1=car present)
// east/west car detector connected to PE0 (1=car present)
// "walk" light connected to PF3 (built-in green LED)
// "don't walk" light connected to PF1 (built-in red LED)

// ***** 1. Pre-processor Directives Section *****
#include "TExaS.h"
#include "tm4c123gh6pm.h"

// ***** 2. Global Declarations Section *****
/* Structure to hold the different parameters for the FSM */
struct State{
		unsigned long out;				/* Output Pattern for Car Signal LEDs */
		unsigned long walk;				/* Output Pattern for Pedestrian Signal LEDs */
		unsigned long time;				/* Default 10ms delay */
  	unsigned long Next[9];		/* Next state based on value received from sensor(Switches) */
};
typedef const struct State SType;

/* Differemt States through which the FSM will cycle through */
#define GoW   0					/* Go West */
#define WW    1					/* Wait on West */
#define GoS   2					/* Go South */
#define WS    3					/* Wait on South */
#define GoP   4					/* Pedistrian Walk */
#define ROn   5					/* Start Flashing Huury UP LED */
#define ROff  6					/* Start Flashing Huury UP LED */
#define ROn1  7					/* Start Flashing Huury UP LED */
#define Roff1 8					/* Pedistrian do not walk */

/* Initialization of the FSM */
SType FSM[9] ={
	0x21, 0x02, 50, {GoW, WW, GoW, WW, WW, WW, WW, WW},
	0x22, 0x02, 35, {GoS, GoS, GoS, GoS, GoP, GoS, GoP, GoS},
	0x0C, 0x02, 50, {GoS, GoS, WS, WS, WS, WS, WS, WS},
	0x14, 0x02, 35, {GoW, GoW, GoW, GoW, GoP, GoP, GoW, GoP},
	0x24, 0x08, 50, {GoP, ROn, ROn, ROn, GoP, ROn, ROn, ROn},
	0x24, 0x02, 15, {ROff, ROff, ROff, ROff, ROff, ROff, ROff, ROff},
	0x24, 0x00, 15, {ROn1, ROn1, ROn1, ROn1, ROn1, ROn1, ROn1, ROn1},
	0x24, 0x00, 15, {GoW, GoS, GoW, GoW, GoP, GoS, GoW, GoW}
};

unsigned long S;					/* Index into the current state */
unsigned long input;			/* Input from Sensors */

// FUNCTION PROTOTYPES: Each subroutine defined
void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts
void Systick_Init(void);							/* Initialize the Systick Timer */
//void PLL_Init(void);								/* Initialize the PLL */
void PortB_Init(void);								/* Initialize Port B registers */
void PortE_Init(void);								/* Initialize Port E registers */
void PortF_Init(void);								/* Initialize Port F registers */
void SysTick_Wait(unsigned long delay);			/* SysTick for Delay */
void SysTick_wait10ms(unsigned long delay);	/* SysTick delay for 10ms */
void Init_All(void);									/* Initializer function */

// ***** 3. Subroutines Section *****

int main(void){ 
  TExaS_Init(SW_PIN_PE210, LED_PIN_PB543210,ScopeOff); // activate grader and set system clock to 80 MHz
 
  EnableInterrupts();
	Init_All();						/* Initialize all Ports, SysTick and PLL */
	S = GoW;							/* Initially the cars are going West */
  while(1){
		GPIO_PORTB_DATA_R = FSM[S].out;				/* Display Car Signals based on current state*/
		GPIO_PORTF_DATA_R = FSM[S].walk;			/* Display Pedestrian Signal based on cuurent state*/
		SysTick_wait10ms(FSM[S].time);				/* Wait for some time in current state */
		input = GPIO_PORTE_DATA_R;						/* Get input from sensors */
		S = FSM[S].Next[input];								/* Change state based on input */
  }
}

/*
 * This function call all other initialization functions.
 * Inputs: None
 * Outputs: None
 */
void Init_All(void){
	Systick_Init();
	//PLL_Init();
	PortB_Init();
	PortE_Init();
	PortF_Init();
}

/*
 * Initialize the PortB.
 * Inputs: None
 * Outputs: None
 */
void PortB_Init(void){
	volatile unsigned long delay;
	SYSCTL_RCGC2_R |= 0x02;							/* Enable clock for Port B */
	delay = SYSCTL_RCGC2_R;							/* Wait for the clock to become stable */
	GPIO_PORTB_LOCK_R |= 0x4C4F434B; 		/* Unlock Port B registers */
	GPIO_PORTB_CR_R |= 0x3F;						/* Use Port B 5 - 0 for LED */
	GPIO_PORTB_DIR_R |= 0x3F;						/* Output on Port B 5-0*/
	GPIO_PORTB_AFSEL_R &= ~(0x3F);			/* GPIO functionality of Port B 5-0*/
	GPIO_PORTB_DEN_R |= 0x3F;						/* Enable digital functions for Port B 5-0*/
	GPIO_PORTB_AMSEL_R &= ~(0x3F);			/* Disable Analog functionality for Port B 5-0*/
	GPIO_PORTB_PCTL_R &= ~(0x000FFFFF);	/* Basic GPIO functionality for Port B 5-0*/
}

/*
 * Initialize the PortE.
 * Inputs: None
 * Outputs: None
 */
void PortE_Init(void){
	volatile unsigned long delay;
	SYSCTL_RCGC2_R |= 0x10;							/* Enable clock for Port E */
	delay = SYSCTL_RCGC2_R;							/* Wait for the clock to become stable */
	GPIO_PORTE_LOCK_R |= 0x4C4F434B; 		/* Unlock Port E registers */
	GPIO_PORTE_CR_R |= 0x07;						/* Use Port E 2 - 0 for LED */
	GPIO_PORTE_DIR_R &= ~(0x07);				/* Use Port E 2-0 for inputs */
	GPIO_PORTE_AFSEL_R &= ~(0x07);			/* Disable alternate function for Port E 2-0 */
	GPIO_PORTE_DEN_R |= 0x07;						/* Enable digital functions for Port E 2-0 */
	GPIO_PORTE_AMSEL_R &= ~(0x07);			/* Disable analog function on Port E 2-0 */
	GPIO_PORTE_PCTL_R &= ~(0x00000FFF);	/* Basic GPIO functionality for Port E 2-0 */
}

/*
 * Initialize the PortE.
 * Inputs: None
 * Outputs: None
 */
void PortF_Init(void){
	volatile unsigned long delay;
	SYSCTL_RCGC2_R |= 0x00000020;				/* Enable clock for Port F */
	delay = SYSCTL_RCGC2_R;							/* Wait for the clock to become stable */
	GPIO_PORTF_LOCK_R |= 0x4C4F434B; 		/* Unlock Port F registers */
	GPIO_PORTF_CR_R |= 0x0A;						/* Use Port F 3 and Port F 1 for LED */
	GPIO_PORTF_DIR_R |= 0x0A;						/* Use Port F 3 and Port F 1 as output */
	GPIO_PORTF_AFSEL_R &= ~(0x0A);			/* Disable alternate functionality on PF3 and PF1 */
	GPIO_PORTF_DEN_R |= 0x0A;						/* Enable digital functions on PF3 and PF1 */
	GPIO_PORTF_AMSEL_R &= ~(0x0A); 			/* Disable analog functionality on PF3 and PF1 */
	GPIO_PORTE_PCTL_R &= ~(0x0000F0F0);	/* Basic GPIO functionality for PF3 and PF1 */
}

/*
 * Initialize the SysTick Timer.
 * Inputs: None
 * Outputs: None
 */
void Systick_Init(void){
	NVIC_ST_CTRL_R = 0;							/* Disable SysTick Timer during initialization */
	NVIC_ST_CTRL_R = 0x00000005;		/* Enable SysTick using System Clock */
}

/*
 * Delay function which will delay for 10ms as we
 * are using a contact value of 800000 when we call
 * this function.
 * Inputs: delay in ms
 * Outputs: None
 */
void SysTick_Wait(unsigned long delay){
	NVIC_ST_RELOAD_R = delay - 1; 						/* number of counts to wait */
	NVIC_ST_CURRENT_R = 0;										/* clear the current register, writing any value will clear it */
	while((NVIC_ST_CTRL_R&0x00010000) == 0){}	/* Wait till count bit becomes 1 */
}

/*
 * Delay function which will wait for delay ms.
 * Inputs: delay in ms
 * Outputs: None
 */
void SysTick_wait10ms(unsigned long delay){
	unsigned long i;
	for(i = 0; i < delay; i++){
		SysTick_Wait(800000);										/* Delay of 10ms */
	}
}

/*
 * Initialize the PLL using external oscillator.
 * Inputs: None
 * Outputs: None
 */

//void PLL_Init(void){
//	SYSCTL_RCC2_R |= 0x80000000;		/* Override the RCC field with RCC2, Page 260 of datasheet */
//	SYSCTL_RCC2_R |= 0x00000800;		/* Bypass PLL, i.e disable PLL during initialization*/
//	SYSCTL_RCC_R &= ~(0x000007C0);	/* Clear the XTAL bits 10 - 6, Page 254 of datasheet*/
//	SYSCTL_RCC_R  |= 0x0000540;			/* Initialize the for 16Mhz oscillator connected to launchpad*/
//	SYSCTL_RCC2_R &= ~(0x00000070);	/* Initialize for the main oscillator source */
//	SYSCTL_RCC2_R &= ~(0x00002000);	/* Clear Power Down so the PLL operates normally */
//	SYSCTL_RCC2_R |= 0x40000000;		/* Append SYSDIV2LSB bit to the SYSDIV2 filed to create a 
//																	 * 7 bit divisor using the 400Mhz PLL output, refer table 5-6 on page 224 
//																	 */
//	SYSCTL_RCC2_R &= ~(0x1FC00000);	/* Cler bits 28 - 22 which are system clock divider*/
//	SYSCTL_RCC2_R |= 0x01000000;		/* Initialize system for 80Mhz clock*/
//	SYSCTL_RCC2_R &= ~(0x00000800);	/* Enable PLL by clearing BYPASS*/
//}
