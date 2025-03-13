#ifndef PROCESOS_H
#define PROCESOS_H

#include "estructura.h"
#include <stdio.h>

// Variables globales
extern int multasProcesadas;
extern int senalesJustificadas;
extern int senalesInjustificadas;
extern int multasPorAmonestaciones;

extern volatile bool ejecucionActiva;

// funcion auxiliar
double calcularDistancia(int x1, int y1, int x2, int y2);

// procesos
void *generarMapa(void *argumento);
void *emisor(void *argumento);
void *semaforo(void *argumento);
void *receptor(void *argumento);
void *gestionMultas(void *argumento);

#endif
