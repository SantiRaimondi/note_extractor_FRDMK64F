#include <stdio.h>
#include "board.h"
#include "peripherals.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "MK64F12.h"
#include "fsl_debug_console.h"

/* TODO: insert other include files here. */
#include "arm_math.h"
#include "y_thresh_avg.h"

/* TODO: insert other definitions and declarations here. */
#define NOTE_SIZE 520				/* Cantidad de muestras por nota detectada, mas 20 muestras de silencio necesarias por el funcionamiento del algoritmo */
#define STEP (float32_t) 0.501466f 	/* Valor para simular la funcion linspace(0,(1+Ns/2),Ns). En nuestro caso, Ns = FFT_SIZE = 1024 */
#define FFT_SIZE (uint32_t) 1024
#define FUNDAMENTALS_SIZE 50 		/* Cantidad de notas que se pueden detectar. Numero arbitrario, se puede modificar a lo que se crea conveniente */

#define SAMPLE_PERIOD_US 113 	/* Equivale a Fs de 8820[k/s] */
#define SYSTICK_CLK_MHZ 120		/* Frecuencia del Systick_CLK en MHZ */

int main(void){

    /* Init board hardware. */
    BOARD_InitBootPins();
    BOARD_InitBootClocks();
    BOARD_InitBootPeripherals();
#ifndef BOARD_INIT_DEBUG_CONSOLE_PERIPHERAL
    /* Init FSL debug console. */
    BOARD_InitDebugConsole();
#endif

    /* Inicializacion de la estructura de la FFT de 1024 puntos que se utilizara */
    arm_rfft_fast_instance_f32 fft_1024;

    if(arm_rfft_fast_init_f32(&fft_1024, FFT_SIZE) != ARM_MATH_SUCCESS)
    {
    	PRINTF("ERROR FFT INIT!");
    	while(1){}
    }

    /* Variables auxiliares */

    /* Tabla de frecuencias para detectar la frecuencia de la nota basado en el indice de la FFT.
     * Es de tamaño FFT_SIZE y no NOTE_SIZE porque se utiliza zero-padding y el resultado de la FFT
     * es de 1024 lugares.
     */
    float32_t f[FFT_SIZE];

    uint32_t i = 0;
    uint32_t j = 0;
	uint16_t end_note = 0;
    uint8_t i_notes = 0;	/* Cuenta la cantidad de notas que han sido detectadas */
	float32_t note[NOTE_SIZE];	/* Buffer donde se guardan las muestras de cada sonido que supero el threshold */
	float32_t note_padded[FFT_SIZE];
	float32_t Note[FFT_SIZE];	/* Se guarda el resultado de la FFT */
	float32_t result = 0;			/* Valor máximo de la FFT */
	uint32_t index = 0;				/* Indice del valor maximo */
	float32_t fundamentals[FUNDAMENTALS_SIZE];	/* Frecuencias fundamentales de las notas detectadas, se prealoca lugar para hasta 50 notas */

    /* Se llena la tabla */
    for(uint16_t i = 0; i < FFT_SIZE; i++)
    {
    	f[i] = (STEP * i);
    }

	while(i < DATA_SIZE)
    {
    	end_note = 0;
    	j = 0;

    	/* Mientras detecte sonido, o no hayan pasado mas de 20 muestras de silencio */
        while(((data_thresh[i] != 0) || (end_note > 0)) && (i < DATA_SIZE))
        {
        	note[j] = (float32_t)(data_thresh[i]);
        	i ++;
        	j ++;

        	if(data_thresh[i] != 0)
        		end_note = 20;
        	else
        		end_note --;

        	if(end_note == 0)
        	{
        		if(j > 25)
        		{
        			/* Se hace zero-padding previo a calculo de FFT */
        			for(uint16_t k = 0; k < NOTE_SIZE; k++)
        				note_padded[k] = note[k];

        			for(uint16_t k = NOTE_SIZE; k < FFT_SIZE; k++)
        				note_padded[k] = 0;

        			/* Se computa la FFT */
        			arm_rfft_fast_f32(&fft_1024, note_padded, Note, 0);

        			/* Se busca el indice del pico de mayor amplitud de la respuesta en frecuencia */
					arm_abs_f32(Note, Note, FFT_SIZE);
        			arm_max_f32(Note, FFT_SIZE, &result, &index);

        			/* Descarta las frecuencias menores a 20[Hz] que no son audibles */
        			if(f[index] > 20)
        			{
        				/* Se agrega una nueva frecuencia detectada */
        				fundamentals[i_notes] = f[index];
						i_notes ++;
        			}

        			i += 50;	/* Se saltean 50 silencios para evitar procesarlos al cuete. Dependiendo de la cancion, podria ser mas o menos */
        		}
        		break;
        	}
        }
        i++;
    }

    while(1)
    {}

    return 0 ;
}



