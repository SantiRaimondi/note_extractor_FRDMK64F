/*
 * Autor: Santiago Raimondi.
 * Fecha: 17/11/2021.
 *
 * Trabajo final de la materia DSP.
 *
 * @brief:
 *
 *
 */


/**
 * @file    note_extractor.c
 * @brief   Application entry point.
 */
#include <stdio.h>
#include "board.h"
#include "peripherals.h"
#include "pin_mux.h"
#include "clock_config.h"
//#include "MK64F12.h"
#include "fsl_debug_console.h"
#include "CancionBien.h"
/* TODO: insert other include files here. */

/* TODO: insert other definitions and declarations here. */
#define WINDOW_SIZE (uint16_t) 5000	/* Tamaño de la ventana deslizante para calcular la media */
#define HALF_WINDOW_SIZE (uint16_t) (WINDOW_SIZE/2)
#define TENTH_WINDOW_SIZE (uint16_t) (WINDOW_SIZE/10)

uint16_t y_tresh[NUM_ELEMENTS];	/* Declararlo global hace que inice con todos 0's: https://stackoverflow.com/questions/2589749/how-to-initialize-array-to-0-in-c
 	 	 	 	 	 	 		Tambien puedo usar arm_fill_f16();
 	 	 	 	 	 	 	*/

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

    while(1) {
	/*
			y_thresh = zeros(1,p);
			i = 1;
			while (i <= p)
				thresh = 5*median(abs(y_avg(max(1,i-5000):i)));
				if (abs(y_avg(i)) > thresh)
					for j = 0:500
						if (i + j <= p)
							y_thresh(i) = y_avg(i);
							i = i + 1;
						end
					end
					i = i + 1400;
				end
				i = i + 1;
			end
	*/

		uint16_t i = 1;
		uint16_t* window_ptr = data;	/* Se presupone que y_avg son valores positivos */
		uint16_t threshold;
		while(i <= NUM_ELEMENTS)
		{
			/* Se calcula la mediana de la ventana deslizante. Equivale a: thresh = 5*median(y_avg(max(1,i-5000):i)); */
			/* mediana = *(fin_ventana_ptr - inicio_ventana_ptr) */
			if(i < WINDOW_SIZE)
				threshold = 5* (*window_ptr);	/*Esta linea esta mal, deberia ser: threshold = 5* (*window_ptr-(uint16_t)(i/2)); */
			else
				threshold = 5* *(window_ptr - HALF_WINDOW_SIZE); /* Obtengo la mediana al restarle al puntero de la ventana la mitad del ancho de la ventana y ver el valor que hay en esa posicion de memoria */

			/* Si el dato actual es mayor a 5 veces el valor de la mediana de la ventana,
			 * quiere decir que hay tendencia ascendente (se detecta que el pianista
			 * presionó una tecla).
			 * Se proceden a guardar la información de ese momento que sabemos que hay una
			 * tecla presionada para luego calcular su frecuencia.
			 */
			if(data[i] > threshold)
			{
				for(uint16_t j = 0; j < TENTH_WINDOW_SIZE; j++)
				{
					if(j+i <= NUM_ELEMENTS)
					{
						y_thresh[i]= data[i];
						i++;
					}
				}
				i += 1400;	/* Hago un salto para ahorrar iteraciones, suponiendo un espacio entre tecla y tecla que se presiona 	*/
				window_ptr += 1400;
			}

			i++;
			window_ptr ++;
		}
    }

    return 0 ;
}

