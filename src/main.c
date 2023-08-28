/************************************************************************/
/* includes                                                             */
/************************************************************************/

#include "asf.h"

/************************************************************************/
/* defines                                                              */
/************************************************************************/

#define LED_PIO PIOC       // periferico que controla o LED
#define LED_PIO_ID ID_PIOC // ID do periférico PIOC (controla LED)
#define LED_PIO_IDX 8      // ID do LED no PIO
#define LED_PIO_IDX_MASK (1u << LED_PIO_IDX) // Mascara para CONTROLARMOS o LED

#define BUT_PIO PIOA
#define BUT_PIO_ID ID_PIOA
#define BUT_PIO_IDX 11
#define BUT_PIO_IDX_MASK (1u << BUT_PIO_IDX)

#define LED1_PIO PIOA
#define LED1_PIO_ID ID_PIOA
#define LED1_PIO_IDX 0
#define LED1_PIO_IDX_MASK (1u << LED1_PIO_IDX)

#define BUT1_PIO PIOD
#define BUT1_PIO_ID ID_PIOD
#define BUT1_PIO_IDX 28
#define BUT1_PIO_IDX_MASK (1u << BUT1_PIO_IDX)

#define LED2_PIO PIOC
#define LED2_PIO_ID ID_PIOC
#define LED2_PIO_IDX 30
#define LED2_PIO_IDX_MASK (1u << LED2_PIO_IDX)

#define BUT2_PIO PIOC
#define BUT2_PIO_ID ID_PIOC
#define BUT2_PIO_IDX 31
#define BUT2_PIO_IDX_MASK (1u << BUT2_PIO_IDX)

#define LED3_PIO PIOB
#define LED3_PIO_ID ID_PIOB
#define LED3_PIO_IDX 2
#define LED3_PIO_IDX_MASK (1u << LED3_PIO_IDX)

#define BUT3_PIO PIOA
#define BUT3_PIO_ID ID_PIOA
#define BUT3_PIO_IDX 19
#define BUT3_PIO_IDX_MASK (1u << BUT3_PIO_IDX)


#define _PIO_DEFAULT             (0u << 0)
#define _PIO_PULLUP              (1u << 0)
#define _PIO_DEBOUNCE            (1u << 3)

/************************************************************************/
/* constants                                                            */
/************************************************************************/

/************************************************************************/
/* variaveis globais                                                    */
/************************************************************************/

/************************************************************************/
/* prototypes                                                           */
/************************************************************************/



void init(void);

/************************************************************************/
/* interrupcoes                                                         */
/************************************************************************/

/************************************************************************/
/* funcoes                                                              */
/************************************************************************/
void _pio_set(Pio *p_pio, const uint32_t ul_mask){
	p_pio->PIO_SODR = ul_mask;
}

void _pio_clear(Pio *p_pio, const uint32_t ul_mask){
	p_pio->PIO_CODR = ul_mask;
}

void _pio_pull_up(Pio *p_pio, const uint32_t ul_mask, const uint32_t ul_pull_up_enable){
	if (ul_pull_up_enable == 1){
		p_pio->PIO_PUER = ul_mask;
	}
	else{
		p_pio-> PIO_PUDR = ul_mask;
	}	
}

void _pio_set_input(Pio *p_pio, const uint32_t ul_mask, const uint32_t ul_attribute){
	
		_pio_pull_up(p_pio,ul_mask, ul_attribute);
		
		if ( ul_attribute == _PIO_DEBOUNCE){
			p_pio-> PIO_IFSCER = ul_mask;
		}
		else{
			p_pio-> PIO_IFSCDR = ul_mask;
		}
}

void _pio_set_output(Pio *p_pio, const uint32_t ul_mask, const uint32_t ul_default_level, const uint32_t ul_multidrive_enable, const uint32_t ul_pull_up_enable)
{
		p_pio->PIO_PER = ul_mask; //enable
		p_pio->PIO_OER = ul_mask; //disable
		
		if (ul_default_level){
			_pio_set(p_pio,ul_mask);
		}
		else{
			_pio_clear(p_pio,ul_mask);
		}
		if (ul_multidrive_enable){
			p_pio->PIO_MDER = ul_mask; // Multi-driver Enable Register
		}
		else{
			p_pio->PIO_MDDR = ul_mask; //e Multi-driver Disable Register
		}
		if (ul_pull_up_enable){
			_pio_pull_up(p_pio,ul_mask,1);
		}
	

}

uint32_t _pio_get(Pio *p_pio, const pio_type_t ul_type, const uint32_t ul_mask){
	
		uint32_t modo;
		
		//se tipo entrada
		if (ul_type == PIO_INPUT) { 
			modo = p_pio->PIO_PDSR; //Pin Data Status Register
			} 
		else {
			modo = p_pio->PIO_ODSR; // Output Data Status Register
		}

		if ((modo & ul_mask) == 0) {
			return 0;
		}
		else {
			return 1;
		}
}


// Função de inicialização do uC
void init(void) {
  // Initialize the board clock
  sysclk_init();

  // Disativa WatchDog Timer
  WDT->WDT_MR = WDT_MR_WDDIS;

  pmc_enable_periph_clk(LED_PIO_ID);
  pmc_enable_periph_clk(LED1_PIO_ID);
  pmc_enable_periph_clk(LED2_PIO_ID);
  pmc_enable_periph_clk(LED3_PIO_ID);
  pmc_enable_periph_clk(BUT1_PIO_ID);
  pmc_enable_periph_clk(BUT2_PIO_ID);
  pmc_enable_periph_clk(BUT3_PIO_ID);

  // Inicializa PC8 como saída
  pio_set_output(LED_PIO, LED_PIO_IDX_MASK, 0, 0, 0);

  _pio_set_output(LED1_PIO, LED1_PIO_IDX_MASK, 0, 0, 0);
  _pio_set_output(LED2_PIO, LED2_PIO_IDX_MASK, 0, 0, 0);
  _pio_set_output(LED3_PIO, LED3_PIO_IDX_MASK, 0, 0, 0);
  
  _pio_set_input(BUT1_PIO, BUT1_PIO_IDX_MASK, _PIO_PULLUP | _PIO_DEBOUNCE);
  _pio_set_input(BUT2_PIO, BUT2_PIO_IDX_MASK, _PIO_PULLUP | _PIO_DEBOUNCE);
  _pio_set_input(BUT3_PIO, BUT3_PIO_IDX_MASK, _PIO_PULLUP | _PIO_DEBOUNCE);

  _pio_pull_up(BUT1_PIO, BUT1_PIO_IDX_MASK, 1);
  _pio_pull_up(BUT2_PIO, BUT2_PIO_IDX_MASK, 1);
  _pio_pull_up(BUT3_PIO, BUT3_PIO_IDX_MASK, 1);
}

/************************************************************************/
/* Main                                                                 */
/************************************************************************/

// Funcao principal chamada na inicalizacao do uC.
int main(void) {
  init();

  while (1) {
    if (!_pio_get(BUT1_PIO, PIO_INPUT,
                 BUT1_PIO_IDX_MASK)) { // Caso aperte Botao 1
      for (int i = 0; i < 5; i++) {
		_pio_set(LED1_PIO, LED1_PIO_IDX_MASK);
		delay_ms(200);
	    _pio_clear(LED1_PIO, LED1_PIO_IDX_MASK);
		delay_ms(200);

        
      }
      _pio_set(LED1_PIO, LED1_PIO_IDX_MASK);
    }
    if (!_pio_get(BUT2_PIO, PIO_INPUT,
                 BUT2_PIO_IDX_MASK)) { // Caso aperte Botao 2
      for (int i = 0; i < 5; i++) {
		 _pio_set(LED2_PIO, LED2_PIO_IDX_MASK);
		 delay_ms(200);
		_pio_clear(LED2_PIO, LED2_PIO_IDX_MASK);
		delay_ms(200);
        
        
      }
      _pio_set(LED2_PIO, LED2_PIO_IDX_MASK);
    }
    if (!_pio_get(BUT3_PIO, PIO_INPUT,
                 BUT3_PIO_IDX_MASK)) { // Caso aperte Botao 3
      for (int i = 0; i < 5; i++) {
		_pio_set(LED3_PIO, LED3_PIO_IDX_MASK);
		delay_ms(200);
		_pio_clear(LED3_PIO, LED3_PIO_IDX_MASK);
		delay_ms(200);
       
        
      }
      _pio_set(LED3_PIO, LED3_PIO_IDX_MASK);
    }
  }
  // nunca devemos chegar aqui!
  return 0;
}
