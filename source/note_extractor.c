
/**
 * @file    music_player.c
 * @brief   Application entry point.
 */
#include <stdio.h>
#include "board.h"
#include "peripherals.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "MK64F12.h"
#include "fsl_debug_console.h"

/* TODO: insert other include files here. */

//#include "Fur_elise_8bits_downsampled.h"
//#include "Fur_elise_8bits_downsampled_signed.h"

#include "Fur_elise_short_16bits_downsampled.h"
#include "Fur_elise_short_16bits_downsampled_signed.h"

/* TODO: insert other definitions and declarations here. */
#define SAMPLE_PERIOD_US 125 /* Equivale a Fs de 8[k/s] */
#define SYSTICK_CLK_MHZ 120	/* Frecuencia del Systick_CLK en MHZ */

#define WINDOW_SIZE (uint16_t) 5000	/* Tamaño de la ventana deslizante para calcular la media */
#define HALF_WINDOW_SIZE (uint16_t) (WINDOW_SIZE/2)
#define TENTH_WINDOW_SIZE (uint16_t) (WINDOW_SIZE/10)
#define THRESHOLD_GAIN 5
#define JUMP 		1400

#define SIXTEEN_BITS	/* Para 16bits */
//#define EIGHT_BITS		/* Para 8bits */

uint32_t index = 0;

#ifdef SIXTEEN_BITS
	uint16_t y_thresh[NUM_ELEMENTS];	/* Vector con los valores absolutos de los sonidos de las teclas presionadas */
	uint16_t median_buffer[WINDOW_SIZE];
#endif

#ifdef EIGHT_BITS
	uint8_t y_thresh[NUM_ELEMENTS];
	uint8_t median_buffer[WINDOW_SIZE];
#endif

void SysTick_Handler(){

//	uint16_t data_mask = (data[index] << 4) & 0x0FFF;		/* Para 8 bits, sonido real */
//	uint16_t data_mask = (y_thresh[index] << 4) & 0x0FFF;	/* Para 8 bits, sonido de tecla presionada */

//	uint16_t data_mask = (data[index] >> 4) & 0x0FFF;		/* Para 16 bits */
	uint16_t data_mask = (y_thresh[index] >> 4) & 0x0FFF;	/* Para 16 bits */

	DAC_SetBufferValue(DAC0, 0, data_mask);

	#ifdef SIXTEEN_BITS
		uint8_t valor = (uint8_t) (data_unsigned[index]>> 4);
		uint8_t valor_signed = (uint8_t) (y_thresh[index] >> 4);

		PRINTF("%d, ", valor);
		PRINTF("%d\r", valor_signed);
	#endif

	#ifdef EIGHT_BITS
		PRINTF("%d, ", data_unsigned[index]);
		PRINTF("%d\r", y_thresh[index]);
	#endif

	if(index < NUM_ELEMENTS)
		index ++;
	else
		index = 0;
//		while(1){}


	// Add for ARM errata 838869, affects Cortex-M4, Cortex-M4F
	// Store immediate overlapping exception retur n operation might vector to incorrect interrupt.
    SDK_ISR_EXIT_BARRIER;
}

/* Intercambia de lugar el valor de dos punteros. */
void swap(uint16_t *p,uint16_t *q) {
   int temp;

   temp=*p;
   *p=*q;
   *q=temp;
}

/*
 * @brief   Application entry point.
 */
int main(void) {

    /* Init board hardware. */
    BOARD_InitBootPins();
    BOARD_InitBootClocks();
    BOARD_InitBootPeripherals();
#ifndef BOARD_INIT_DEBUG_CONSOLE_PERIPHERAL
    /* Init FSL debug console. */
    BOARD_InitDebugConsole();
#endif

	uint32_t i = 1;

#ifdef SIXTEEN_BITS

	uint16_t threshold;

	while(i <= NUM_ELEMENTS)
	{
		/* Se calcula la mediana de la ventana deslizante usando valor absoluto.
		 * Equivale a: thresh = 5*median(abs(y_avg(max(1,i-5000):i)));
		 */
		if(i < WINDOW_SIZE)
		{
			/* Se crea una copia de la ventana. No se puede trabajar sobre data porque es const. */
			for(uint16_t j = 0; j < i; j++)
			{
				median_buffer[j] = (uint16_t) (abs(data[j]));
			}
			/* Se acomodan los valores en orden ascendente */
			for(uint16_t j = 0; j < i-1; j++)
			{
				for(uint16_t k = 0; k < i-j-1; k++)
				{
			         if(median_buffer[k] > median_buffer[k+1])
			            swap(&median_buffer[k],&median_buffer[k+1]);
				}
			}
			/* Se busca el valor del medio y se calcula el valor de umbral */
			uint32_t indice = (uint32_t) (i/2);
			uint16_t median = median_buffer[indice];
			threshold = (uint16_t) (THRESHOLD_GAIN * median);
		}
		else
		{  	uint16_t* median_buffer_ptr = median_buffer;
			for(uint16_t j = i-WINDOW_SIZE; j < i; j++)
			{
				*median_buffer_ptr = (uint16_t) (abs(data[j]));
				median_buffer_ptr ++;
			}
			/* Se acomodan los valores en orden ascendente */
			for(uint16_t j = 0; j < WINDOW_SIZE-1; j++)
			{
				for(uint16_t k = 0; k < WINDOW_SIZE-j-1; k++)
				{
			         if(median_buffer[k] > median_buffer[k+1])
			            swap(&median_buffer[k],&median_buffer[k+1]);
				}
			}
			uint32_t indice = (uint32_t) (WINDOW_SIZE/2);
			uint16_t median = median_buffer[indice];
			threshold = (uint16_t) (THRESHOLD_GAIN * median);
		}

		/* Si el dato actual (en valor absoluto) es mayor a 5 veces el valor de la mediana de la ventana,
		 * quiere decir que hay tendencia ascendente (se detecta que el pianista presionó una tecla).
		 * Se proceden a guardar la información de ese momento que sabemos que hay una
		 * tecla presionada para luego calcular su frecuencia.
		 */

		if((uint16_t)(abs(data[i])) > threshold)
		{
			for(uint16_t j = 0; j < TENTH_WINDOW_SIZE; j++)
			{
				if(j+i <= NUM_ELEMENTS)
				{
					y_thresh[i]= (uint16_t)(abs(data[i]));
					i++;
				}
			}
			i += JUMP;	/* Pega el salto de 1400 para garantizar saltear las muestras que ya guarde,
						   para que no entre nuevamente al if() con valores que son menores a los
						   guardados anteriormente */
		}

		i++;
	}
#endif

#ifdef EIGHT_BITS

	uint8_t threshold;

	while(i <= NUM_ELEMENTS)
	{
		/* Se calcula la mediana de la ventana deslizante usando valor absoluto.
		 * Equivale a: thresh = 5*median(abs(y_avg(max(1,i-5000):i)));
		 */
		if(i < WINDOW_SIZE)
		{
			uint32_t indice = (uint32_t) (i/2);
			threshold = (uint8_t) (THRESHOLD_GAIN * (uint8_t)(abs(data[indice])));
		}
		else
		{
			uint32_t indice = i - HALF_WINDOW_SIZE;
			threshold = (uint8_t) (THRESHOLD_GAIN * (uint8_t)(abs(data[indice])));
		}

		/* Si el dato actual (en valor absoluto) es mayor a 5 veces el valor de la mediana de la ventana,
		 * quiere decir que hay tendencia ascendente (se detecta que el pianista presionó una tecla).
		 * Se proceden a guardar la información de ese momento que sabemos que hay una
		 * tecla presionada para luego calcular su frecuencia.
		 */

		if(uint8_t)(abs(data[i])) > threshold)
		{
			for(uint16_t j = 0; j < TENTH_WINDOW_SIZE; j++)
			{
				if(j+i <= NUM_ELEMENTS)
				{
					y_thresh[i]= (uint8_t)(abs(data[i]));
					i++;
				}
			}

			i += JUMP;	/* Pega el salto de 1400 para garantizar saltear las muestras que ya guarde,
						   para que no entre nuevamente al if() con valores que son menores a los
						   guardados anteriormente */
		}

		i++;
	}
#endif

	/* Tiene  un reloj de 120MHz.
	 * Si se carga correctamente, devuelve 0UL.
	 * Funciona bien, lo corrobore con el cycle_delta entre dos interrupciones consecutivas */
 	if(SysTick_Config(SYSTICK_CLK_MHZ*SAMPLE_PERIOD_US) != 0UL)
    {
		PRINTF("ERROR!!");
		while(1)
		{}
    }

 	while(1)
 	{

 	}

 	return 0 ;
}
