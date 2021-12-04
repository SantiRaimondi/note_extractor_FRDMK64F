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
#define NOTE_SIZE 			520			/* Cantidad de muestras por nota detectada, mas 20 muestras de silencio necesarias por el funcionamiento del algoritmo */
#define STEP (float32_t) 	0.501466f 	/* Valor para simular la funcion linspace(0,(1+Ns/2),Ns). En nuestro caso, Ns = FFT_SIZE = 1024 */
#define FFT_SIZE (uint32_t) 1024
#define FUNDAMENTALS_SIZE 	50 			/* Cantidad de notas que se pueden detectar. Numero arbitrario, se puede modificar a lo que se crea conveniente */

#define SAMPLE_FRECUENCY_KHZ (uint32_t) 6		/* Frecuencia de muestreo para reconstruir los tonos */
#ifdef SAMPLE_FRECUENCY_KHZ
	#define SAMPLE_PERIOD_US (uint32_t) (1000U/SAMPLE_FRECUENCY_KHZ)	/* No es la forma mas precisa pero funciona */
#endif

#define SYSTICK_CLK_MHZ 		120		/* Frecuencia del Systick_CLK en MHZ */

#define DAC_BUFFER_INDEX 		0U		/* Como no se utiliza el buffer, siempre es 0 este valor */
#define DC_OFFSET (uint16_t) 32767		/* Offset de DC que trae la señal (mitad del rango dinamico si se considera un numero de 16bits). */

#define DURATION_MS 			500		/* Cantidad de tiempo en segundos que se va a reproducir cada nota en [ms] */

volatile bool systick_flag = false;		/* Indica interrupcion de Systick */

void SysTick_Handler(){

	systick_flag = true;

	// Add for ARM errata 838869, affects Cortex-M4, Cortex-M4F
	// Store immediate overlapping exception return operation might vector to incorrect interrupt.
    SDK_ISR_EXIT_BARRIER;
}

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
     * es de 1024 puntos.
     */
    float32_t f[FFT_SIZE];

    uint32_t i = 0;
    uint32_t j = 0;
	uint16_t end_note = 0;
	float32_t note[NOTE_SIZE];	/* Buffer donde se guardan las muestras de cada sonido que supero el threshold */
	float32_t note_padded[FFT_SIZE];
	float32_t Note[FFT_SIZE];	/* Se guarda el resultado de la FFT */
	float32_t result = 0;		/* Valor máximo de la FFT */
	uint8_t i_notes = 0;		/* Contador de notas detectadas */
	uint32_t index = 0;			/* Indice del valor maximo */
	float32_t fundamentals[FUNDAMENTALS_SIZE];	/* Frecuencias fundamentales de las notas detectadas, se prealoca lugar para hasta 50 notas */
	arm_fill_f32(0, fundamentals, FUNDAMENTALS_SIZE); /* Se borran los yuyos que tengan */

    /* Se llena la tabla */
    for(i = 0; i < FFT_SIZE; i++)
    {
    	f[i] = (STEP * i);
    }

    /* Se procesa el arreglo de muestras para obtener los tonos de las notas tocadas. */
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

    /******************************************************************
     * ESTA PARTE DE REGENERAR LA CANCION NO ESTÁ TERMINADA TODAVIA
     ******************************************************************
     * */

//    /* Se reconstruye el arreglo en base a los tonos procesados */
//    uint16_t j_limit = SAMPLE_FRECUENCY_KHZ*DURATION_MS; /* Limite del indice j para recorrer el arreglo */
//    uint32_t recreate_size = j_limit * i_notes;
////    uint32_t recreate_size = j_limit*1;	// Para DEBUG para generar un solo tono
//    /* Arreglo con la cancion reconstruida a partir del procesamiento de las notas en el dominio del tiempo
//     * El tamaño del arreglo simula que son muestras tomadas con una Fs de valor SAMPLE_FRECUENCY.
//     * En este caso SAMPLE_FRECUENCY = 2000 que es sobrado para nuestras notas musicales de valor maximo no superior a 400 [Hz]. */
//    float32_t recreate_song[recreate_size];

//    for(i = 0; i < i_notes; i++)
//    {
//    	for(j = 0; j < j_limit; j++)
//    	{
//    		index = i*j_limit+j;
//			recreate_song[index] = arm_sin_f32(2*PI*fundamentals[i]*2*counter_timer_sec);	/* El segundo x2 es porque la fft da la frec. en la mitad del espectro */
//			counter_timer_sec += (float32_t) j*(1/(SAMPLE_FRECUENCY_KHZ*1000.0f)); /* Se multiplica por 1000 para que quede en segundos */
//    	}
//    }
//
//    /* Para DEBUG, se genera un solo tono */
//	for(j = 0; j < recreate_size; j++)
//	{
//		recreate_song[j] = arm_sin_f32(2*PI*fundamentals[0]*2*counter_timer_sec);	/* El segundo x2 es porque la fft da la frec. en la mitad del espectro */
//		counter_timer_sec += (float32_t) j*(1/(SAMPLE_FRECUENCY_KHZ*1000.0f)); /* Se multiplica por 1000 para que quede en segundos */
//	}
//
    /* Tiene  un reloj de 120MHz.
     * Si se carga correctamente, devuelve 0UL.
     */
	if(SysTick_Config(SYSTICK_CLK_MHZ*SAMPLE_PERIOD_US) != 0UL)
    {
		PRINTF("ERROR!!");
    	while(1)
    	{}
    }

	index = 0; /* Reutilizamos la variable */
	i = 0;

	uint32_t counter_timer_us = 0;	/* Variable que simula el paso del tiempo para obtener los valores del seno */
	float32_t playing_note = fundamentals[i];	/* Nota que se va a reproducir durante DURATION_MS cantidad de tiempo */

    while(1)
    {
		if(systick_flag)
		{
			systick_flag = false;

			float32_t sin_value = 0;
			sin_value = arm_sin_f32(2*PI*playing_note*counter_timer_us/1000000);

//			PRINTF("%f \r\n", sin_value+1); /* Se agrega un offset para ver valores negativos en el SerialOscilloscope */

			/* Se convierte el float a int16_t que al ser entero permite operaciones bitwise necesaria para el manejo del DAC*/
			q15_t output_note = 0;
			output_note = (q15_t) (sin_value * DC_OFFSET);
			DAC_SetBufferValue(DAC0, DAC_BUFFER_INDEX, (output_note+DC_OFFSET)>>4);

			counter_timer_us += SAMPLE_PERIOD_US;
			index ++;

			if(index >= SAMPLE_FRECUENCY_KHZ*DURATION_MS)
			{
				index = 0;
				counter_timer_us = 0;
				i ++;
				if(i >= i_notes)	/* Se reproducen las notas de  la cancion de manera circular */
					i = 0;
				playing_note = fundamentals[i];
			}
		}
    }

    return 0 ;
}



