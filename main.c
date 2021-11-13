#include <avr/io.h>
#include <avr/interrupt.h>

#define F_CPU 128000L
//#define F_CPU 1000000L

#include <util/delay.h>
#include <avr/sleep.h>
#include <avr/power.h>
#include <avr/wdt.h>




void prevent_wdt_reset() {
  if(MCUSR & _BV(WDRF)){                    // If a reset was caused by the Watchdog Timer...
    MCUSR &= ~_BV(WDRF);                    // Clear the WDT reset flag
    WDTCR |= (_BV(WDCE) | _BV(WDE));        // Enable the WD Change Bit
    WDTCR = 0x00;                           // Disable the WDT
  }
}

/**
	 *	Setting the watchdog pre-scaler value with VCC = 5.0V and 16mHZ
	 *	WDP3 WDP2 WDP1 WDP0 | Number of WDT | Typical Time-out at Oscillator Cycles
	 *	0    0    0    0    |   2K cycles   | 16 ms
	 *	0    0    0    1    |   4K cycles   | 32 ms
	 *	0    0    1    0    |   8K cycles   | 64 ms
	 *	0    0    1    1    |  16K cycles   | 0.125 s
	 *	0    1    0    0    |  32K cycles   | 0.25 s
	 *	0    1    0    1    |  64K cycles   | 0.5 s
	 *	0    1    1    0    |  128K cycles  | 1.0 s
	 *	0    1    1    1    |  256K cycles  | 2.0 s
	 *	1    0    0    0    |  512K cycles  | 4.0 s
	 *	1    0    0    1    | 1024K cycles  | 8.0 s
	*/
    
void setup_wdt() {
  cli();                                    // with global interrupts disabled,
                                            // Set up Watch Dog Timer for Inactivity
  WDTCR |= (_BV(WDCE) | _BV(WDE));          // Enable the WD Change Bit
  WDTCR =   _BV(WDIE) |                     // Enable WDT Interrupt
  _BV(WDP1) ;                               // Set Timeout to ~64ms 
  sei();                                    // now re-enable global interrupts
}

volatile uint8_t FLAGS = 0x00;

ISR(WDT_vect)                               // ISR for watchdog timer just toggles a bit for now
{
  FLAGS = 1;
}

uint8_t SetAllPorts()                       // All outputs
{
    DDRB |= (1 << PB0);
    DDRB |= (1 << PB1);
    DDRB |= (1 << PB2);
    DDRB |= (1 << PB3);
    DDRB |= (1 << PB4);
    return 1;
}

uint8_t SetTimersPWM()
{
    TCCR0B |= (1 << CS00) ;     // Set Timer 0 prescaler to clock. At 1 MHz this is 125 kHz.    
    TCCR0A = (1 << WGM01) | (1 << WGM00) | (1 << COM0B1) | (1 << COM0A1) | (1 << COM0B0) | (1 << COM0A0);  // Set to 'Fast PWM' mode, Set OC0B and OC0A output on compare match, upwards counting.
    
    /*TCCR0A = 2 << COM0A0 | 2 << COM0B0 | 3 << WGM00;
    TCCR0B = 0 << WGM02  | 1 << CS00;
    TCCR1  = 0 << PWM1A  | 0 << COM1A0 | 1 << CS10;
    GTCCR  = 1 << PWM1B  | 2 << COM1B0;*/
    return 1;
}

uint8_t Conso()
{
    ADCSRA &= ~(1 << ADEN);       // disable the ADC
    PRR|=(1<<PRADC);              // Turn ADC off.
    PRR|=PRUSI;                   // Turn USI off.
    ACSR|=(1<<ACD);               // Turn off Analog comparator.    
    return 1;
}

int main(void)
{    
    prevent_wdt_reset();
    setup_wdt();
    SetAllPorts();
    SetTimersPWM();
    
    //Conso();
    uint8_t indexYe = 0;
    uint8_t indexOr = 99;
    
    //const uint8_t list_pwm[20] = {20, 210, 150, 70, 180, 250, 110, 50, 130, 220, 200, 210, 190, 220, 150, 100, 70, 90, 35, 90};
    const uint8_t list_pwm[100] = {223,236,205,255,237,236,214,229,242,222,213,222,
                                   217,209,238,254,230,220,218,233,220,213,233,211,
                                   222,240,245,220,208,214,231,206,229,221,234,229,
                                   228,251,216,226,220,236,222,251,210,250,242,226,
                                   214,218,213,203,212,233,201,219,212,241,234,238,
                                   201,250,224,255,243,249,253,253,210,234,250,239,
                                   211,228,230,235,224,226,207,236,249,221,207,253,
                                   224,250,211,214,203,211,239,251,211,248,249,230,
                                   211,244,235,239};
                                   
    const uint8_t toilet_limit[10] = {4, 2, 6, 2, 10, 4, 20, 6, 16, 6};

    uint8_t toilet_idx = 0;
    uint8_t toilet_wdt_cnt = 0;
    uint8_t toilet_cycle_cnt = 0;
    
    uint8_t ajustement = 0;
    

    //OCR0A = 0x88;
    while (1)
    {
        /*PORTB &= ~(1 << PB4);
        PORTB |= (1 << PB3);
        _delay_ms(1000);
        PORTB |= (1 << PB4);
        PORTB &= ~(1 << PB3);
        _delay_ms(1000);
        */
        /*
        _delay_ms(5000);
        power_all_disable();
        _delay_ms(5000);
        set_sleep_mode(SLEEP_MODE_PWR_DOWN);
        sleep_mode();
        */
        // turn off LED
        //PORTB &= ~(1 << PB4);
        //PORTB |= (1 << PB3);
        
        if(FLAGS)
        {
            FLAGS = 0;
            toilet_wdt_cnt++;
            
            if(!indexOr)
            {
                indexYe = 0;
                indexOr = 99;
            }
            else 
            {
                indexYe++;
                indexOr--; 
            }
            
            //ajustement = 255U - list_pwm[indexYe];
            //ajustement /= 2U;
            
            OCR0A = list_pwm[indexYe];
            OCR0B = list_pwm[indexOr];            
        }   

        if(toilet_wdt_cnt == 0xFF)
        {
            toilet_cycle_cnt++;
            toilet_wdt_cnt = 0;
        }
        
        if(toilet_cycle_cnt == toilet_limit[toilet_idx])
        {
            toilet_cycle_cnt = 0;
            PORTB ^= (1 << PB2);
            
            if(toilet_idx == 9)
            {                
                toilet_idx = 0;
            }
            else
            {
                toilet_idx++;
            }
        }               
        
        // now set it to state of led flag
        //PORTB |= ((FLAGS >> FLED) & 1) << PB4;
        //PORTB |= (~(FLAGS >> FLED) & 1) << PB3;
        _delay_ms(10);
        
    }

    
}