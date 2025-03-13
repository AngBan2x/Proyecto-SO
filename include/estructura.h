#ifndef ESTRUCTURAS_H
#define ESTRUCTURAS_H

#include <pthread.h>
#include <semaphore.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#define MAX_SENALES_JUSTIFICADAS_SEMANALES 200
#define MAX_AMONESTACIONES_SEMANALES 150
#define TAMANO_BUFFER 3
#define NUM_EMISORES 3000
#define NUM_RECEPTORES 3000
#define NUM_SEMAFOROS 10
#define NUM_CENTROS_MULTAS 500
#define SEMANAS_SIMULACION_MES 1
#define RADIO_CIUDAD 12000

// Estructuras
typedef struct {
  int idEmisor;
  int idSenal;
  bool validada;
  bool procesada;
  time_t marcaTiempo;
} SenalCorneta;

typedef struct {
  SenalCorneta buffer[TAMANO_BUFFER];
  int cabeza;
  int cola;
  sem_t mutex;
  sem_t vacio;
  sem_t lleno;
} BufferReceptor;

typedef struct {
  int senalesJustificadas;
  int siguienteIdSenal;
  sem_t bloqueo;
} EstadoEmisor;

typedef struct {
  int amonestaciones;
  sem_t bloqueo;
} EstadoConductor;

typedef struct {
  int x;
  int y;
  bool luzVerde;
} Semaforo;

typedef struct {
  int x;
  int y;
} CentroMulta;

typedef struct {
  Semaforo semaforos[NUM_SEMAFOROS];
  CentroMulta centros[NUM_CENTROS_MULTAS];
  sem_t bloqueo;
} Mapa;

extern BufferReceptor receptores[NUM_RECEPTORES];
extern EstadoEmisor emisores[NUM_EMISORES];
extern EstadoConductor conductores[NUM_EMISORES];
extern Mapa mapa;
extern bool vehiculosEstacionados[NUM_RECEPTORES];

extern sem_t mutexMultas;

#endif // ESTRUCTURAS_H
