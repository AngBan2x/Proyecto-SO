#include "../include/proceso.h"
#include <math.h>

// Variables globales

int multasProcesadas = 0;
int senalesJustificadas = 0;
int senalesInjustificadas = 0;
int multasPorAmonestaciones = 0;

volatile bool ejecucionActiva = true;

BufferReceptor receptores[NUM_RECEPTORES];
EstadoEmisor emisores[NUM_EMISORES];
EstadoConductor conductores[NUM_EMISORES];
Mapa mapa;
bool vehiculosEstacionados[NUM_RECEPTORES];

sem_t mutexMultas;

// Función auxiliar
double calcularDistancia(int x1, int y1, int x2, int y2) {
  return sqrt(pow(x2 - x1, 2) + pow(y2 - y1, 2));
}

void *generarMapa(void *argumento) {
  while (ejecucionActiva) {
    sem_wait(&mapa.bloqueo);

    for (int i = 0; i < NUM_SEMAFOROS; i++) {
      mapa.semaforos[i].x = rand() % RADIO_CIUDAD;
      mapa.semaforos[i].y = rand() % RADIO_CIUDAD;
      mapa.semaforos[i].luzVerde = true;
    }

    for (int i = 0; i < NUM_CENTROS_MULTAS; i++) {
      mapa.centros[i].x = rand() % RADIO_CIUDAD;
      mapa.centros[i].y = rand() % RADIO_CIUDAD;
    }

    sem_post(&mapa.bloqueo);
    sleep(1);
  }
  free(argumento);
  return NULL;
}

void *emisor(void *argumento) {
  int id = *((int *)argumento);
  free(argumento);

  while (ejecucionActiva) {
    SenalCorneta senal;
    senal.idEmisor = id;
    senal.validada = false;
    senal.procesada = false;
    senal.marcaTiempo = time(NULL);

    sem_wait(&emisores[id].bloqueo);
    senal.idSenal = emisores[id].siguienteIdSenal++;
    sem_post(&emisores[id].bloqueo);

    for (int i = 0; i < NUM_RECEPTORES; i++) {
      if (sem_trywait(&receptores[i].vacio) == 0) {
        sem_wait(&receptores[i].mutex);
        receptores[i].buffer[receptores[i].cola] = senal;
        receptores[i].cola = (receptores[i].cola + 1) % TAMANO_BUFFER;
        sem_post(&receptores[i].mutex);
        sem_post(&receptores[i].lleno);
        printf("[Emisor %d] Señal %d enviada a Receptor %d\n", id + 1,
               senal.idSenal, i + 1);
      }
    }
    usleep(50000);
  }
  return NULL;
}

void *semaforo(void *argumento) {
  int id = *((int *)argumento);
  free(argumento);

  while (ejecucionActiva) {
    for (int i = 0; i < NUM_RECEPTORES; i++) {
      if (sem_trywait(&receptores[i].lleno) == 0) {
        sem_wait(&receptores[i].mutex);
        int idx = receptores[i].cabeza;
        SenalCorneta *senal = &receptores[i].buffer[idx];

        if (!senal->validada) {
          senal->validada = true;
          bool justificada = (rand() % 5 == 0); // probabilidad de justificados

          if (justificada) {
            sem_wait(&emisores[senal->idEmisor].bloqueo);
            emisores[senal->idEmisor].senalesJustificadas++;
            sem_post(&emisores[senal->idEmisor].bloqueo);

            sem_wait(&mutexMultas);
            senalesJustificadas++;
            sem_post(&mutexMultas);
            printf("[Semaforo %d] Validada señal %d como JUSTIFICADA\n", id + 1,
                   senal->idSenal);
          } else {
            sem_wait(&conductores[senal->idEmisor].bloqueo);
            conductores[senal->idEmisor].amonestaciones++;
            sem_post(&conductores[senal->idEmisor].bloqueo);

            sem_wait(&mutexMultas);
            senalesInjustificadas++;
            sem_post(&mutexMultas);
            printf("[Semaforo %d] Validada señal %d como INJUSTIFICADA\n",
                   id + 1, senal->idSenal);
          }
        }

        receptores[i].cabeza = (idx + 1) % TAMANO_BUFFER;
        sem_post(&receptores[i].mutex);
        sem_post(&receptores[i].vacio);
      }
    }
    usleep(100000);
  }
  return NULL;
}

// Función del Receptor
void *receptor(void *argumento) {
  int id = *((int *)argumento);
  free(argumento);
  static int reproduccionesPorEmisor[NUM_EMISORES] = {0};

  while (ejecucionActiva) {
    sem_wait(&receptores[id].lleno);
    sem_wait(&receptores[id].mutex);

    int idx = receptores[id].cabeza;
    SenalCorneta *senal = &receptores[id].buffer[idx];

    if (senal->validada && !senal->procesada) {
      if (reproduccionesPorEmisor[senal->idEmisor] < 3) {
        printf("[Receptor %d] Reproduciendo señal %d del Emisor %d\n", id + 1,
               senal->idSenal, senal->idEmisor + 1);
        reproduccionesPorEmisor[senal->idEmisor]++;
      }
      senal->procesada = true;
    }

    receptores[id].cabeza = (idx + 1) % TAMANO_BUFFER;
    sem_post(&receptores[id].mutex);
    sem_post(&receptores[id].vacio);

    usleep(150000);
  }
  return NULL;
}

void *gestionMultas(void *argumento) {
  while (ejecucionActiva) {
    sleep(2);

    for (int i = 0; i < NUM_EMISORES; i++) {
      sem_wait(&emisores[i].bloqueo);
      int just = emisores[i].senalesJustificadas;
      sem_post(&emisores[i].bloqueo);

      sem_wait(&conductores[i].bloqueo);
      int amon = conductores[i].amonestaciones;
      sem_post(&conductores[i].bloqueo);

      if (just >= MAX_SENALES_JUSTIFICADAS_SEMANALES) {
        sem_wait(&mutexMultas);
        multasProcesadas++;
        emisores[i].senalesJustificadas = 0;
        sem_post(&mutexMultas);
        printf("\n¡MULTA! Emisor %d excedió señales justificadas\n", i + 1);
      }

      if (amon >= MAX_AMONESTACIONES_SEMANALES) {
        sem_wait(&mutexMultas);
        multasPorAmonestaciones++;
        conductores[i].amonestaciones = 0;
        sem_post(&mutexMultas);
        printf("\n¡MULTA! Emisor %d excedió amonestaciones\n", i + 1);
      }
    }
  }
  free(argumento);
  return NULL;
}
