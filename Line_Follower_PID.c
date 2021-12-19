//System Clock
#define SYSCTL_RCGCGPIO_R			 (*((volatile unsigned long*) 0x400FE608))

// Timer 1 base address 
#define TM1_BASE                      0x40031000 

// General purpose timer register definitions for Timer 1
#define GPTM_CONFIG_R                *( volatile long *) ( TM1_BASE + 0x000 ) 
#define GPTM_TA_MODE_R               *( volatile long *) ( TM1_BASE + 0x004 )
#define GPTM_TB_MODE_R               *( volatile long *) ( TM1_BASE + 0x008 )
#define GPTM_CONTROL_R               *( volatile long *) ( TM1_BASE + 0x00C ) 
#define GPTM_INT_MASK_R              *( volatile long *) ( TM1_BASE + 0x018 ) 
#define GPTM_INT_CLEAR_R             *( volatile long *) ( TM1_BASE + 0x024 ) 
#define GPTM_TA_IL_R                 *( volatile long *) ( TM1_BASE + 0x028 ) 
#define GPTM_TA_MATCH_R              *( volatile long *) ( TM1_BASE + 0x030 )
#define GPTM_TB_IL_R                 *( volatile long *) ( TM1_BASE + 0x02C )
#define GPTM_TB_MATCH_R              *( volatile long *) ( TM1_BASE + 0x034 )

// Peripheral clock enabling for timer and GPIO
#define RCGC_TIMER_R                 *( volatile unsigned long *)0x400FE604 
#define RCGC2_GPIO_R                 *( volatile unsigned long *)0x400FE108 
#define CLOCK_GPIOF_GPIOB              0x00000022 // Port F & port B clock control 
#define SYS_CLOCK_FREQUENCY            16000000 

// GPIO PF2 alternate function configuration 
#define GPIO_PORTF_DATA_R            (*((volatile unsigned long*)0x400253FC))
#define GPIO_PORTF_AFSEL_R           *(( volatile unsigned long *)0x40025420 ) 
#define GPIO_PORTF_PCTL_R            *(( volatile unsigned long *)0x4002552C ) 
#define GPIO_PORTF_DEN_R             *(( volatile unsigned long *)0x4002551C )

//PORT B Initialization
#define GPIO_PORTB_DATA_R			  (*((volatile unsigned long*) 0x400053FC))
#define GPIO_PORTB_DIR_R			  (*((volatile unsigned long*) 0x40005400))
#define GPIO_PORTB_DEN_R			  (*((volatile unsigned long*) 0x4000551C))
#define GPIO_PORTB_PDR_R			  (*((volatile unsigned long*) 0x40005514))


// Timer config and mode bit field definitions 
#define TIM_16_BIT_CONFIG          0x00000004            // 16- bit timer 
#define TIM_PERIODIC_MODE          0x00000002            // Periodic timer mode 
#define TIM_ENABLE                 0x00000101            // Timer enable control

#define TIM_PWM_MODE               0x0000000A            // Timer in PWM mode 
#define TIM_CAPTURE_MODE           0x00000004            // Timer capture mode 

// Timer1 A reload value for 1 kHz PWM frequency
#define TIM_INTERVAL               16000    // Timer reload value for 1 kHz PWM frequency at clock frequency of 16 MHz 16 ,000 ,000/16000= 1 kHz 
#define TIM_A_MATCH                15500    // Timer match value PF2 Left Motor
#define TIM_B_MATCH                14000    // Timer match value PF3 Right Motor
#define Base_Speed_M1              11500
#define Base_Speed_M2              10000


// Timer and GPIO intialization and configuration 
void Timer1A_Init ( void ) 
{        
	RCGC2_GPIO_R |= CLOCK_GPIOF_GPIOB ;                 // Enable the clock for port F and Timer1 
    RCGC_TIMER_R |= 0x02 ; 
	
	// Enable clock for Timer1 
	GPIO_PORTB_DEN_R	|= 0xFF;
	GPIO_PORTB_DIR_R	&= ~(0xFF);
	
	// Configure PortF pin 2 & 3 as Timer1 A  and B output    
	GPIO_PORTF_AFSEL_R |= 0x0000000C ;    
	GPIO_PORTF_PCTL_R  |= 0x00007700 ;            // Configure as timer CCP pin    
	GPIO_PORTF_DEN_R   |= 0x0000000C ;            // Enable the clock for Timer 1    
	GPTM_CONTROL_R     &= ~ ( TIM_ENABLE );       // disable timer 1 A    
					
	GPTM_CONFIG_R   |= TIM_16_BIT_CONFIG ;        // Timer1 A configured as 16- bit timer    
	GPTM_TA_MODE_R  |= TIM_PWM_MODE ;             // Timer1 A in periodic timer , edge count and PWM mode
	GPTM_TA_MODE_R  &= ~( TIM_CAPTURE_MODE );        
	GPTM_TA_IL_R     = TIM_INTERVAL ;             // Make PWM frequency 1 kHz using reload value of 16000    
	GPTM_TA_MATCH_R  = TIM_A_MATCH ;              // Configure PWM duty cycle value ( should be less than 16000)     
	GPTM_TB_MODE_R  |= TIM_PWM_MODE ;             // Timer1 A in periodic timer , edge count and PWM mode
	GPTM_TB_MODE_R  &= ~( TIM_CAPTURE_MODE );        
	GPTM_TB_IL_R     = TIM_INTERVAL ;             // Make PWM frequency 1 kHz using reload value of 16000    
	GPTM_TB_MATCH_R  = TIM_B_MATCH ;              // Configure PWM duty cycle value ( should be less than 16000)     
	GPTM_CONTROL_R  |= TIM_ENABLE ;               // Enable timer1 A
	     
}
int main ( void )
{
	signed int Set_Point = 3;
	signed int Speed = 0;
	
	signed int Current_Value  = 0;
	signed int Previous_Error = 0;
	
	signed int Error   = 0;
	signed int D_Error = 0;
	signed int I_Error = 0;
	
	int KP = 1200;
	int KI = 0;
	int KD = 2050;
	
       // Initialize the timer	
    Timer1A_Init ();                          
	
	while (1) 
	{
		
		if (GPIO_PORTB_DATA_R == 0x1E)           //11110 = 0x1E
		{
				Current_Value = 0;
		}
		if (GPIO_PORTB_DATA_R == 0x1C)           //11100 = 0x1C
		{
				Current_Value = 1;
		}
		if (GPIO_PORTB_DATA_R == 0x18)           //11000 = 0x18
		{
				Current_Value = 2;
		}
		if (GPIO_PORTB_DATA_R == 0x11)           //10001 = 0x11
		{
				Current_Value = 3;
		}
		if (GPIO_PORTB_DATA_R == 0x2)            //00011 = 0x2
		{
				Current_Value = 4;
		}
		if (GPIO_PORTB_DATA_R == 0x07)           //00111 = 0x07
		{
				Current_Value = 5;
		}
		if (GPIO_PORTB_DATA_R == 0x0F)           //01111 = 0x0F
		{
				Current_Value = 6;
		}
		
		Error = Set_Point - Current_Value;
		D_Error = Error - Previous_Error;
		I_Error = I_Error + Error;
		
		Speed = KP*Error + KD*D_Error + KI*I_Error ;
		GPTM_TA_MATCH_R  =  Base_Speed_M1 + Speed;                  
		GPTM_TB_MATCH_R  =  Base_Speed_M2 - Speed;
	
		if(GPTM_TA_MATCH_R >= 15999 )
		{
			GPTM_TA_MATCH_R = 15500;
		}
		if(GPTM_TB_MATCH_R >= 15999 )
		{
			GPTM_TB_MATCH_R = 15500;
		}
		if(GPTM_TA_MATCH_R <= 0 )
		{
			GPTM_TA_MATCH_R =3000;
		}
		if(GPTM_TB_MATCH_R <= 0 )
		{
			GPTM_TB_MATCH_R = 3000;
		}		
		Previous_Error = Error;		
	}
}
