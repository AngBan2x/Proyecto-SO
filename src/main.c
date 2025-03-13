#include "../include/estructura.h"
#include "../include/proceso.h"

int main() {
  srand(time(NULL));
  pthread_t hilos[NUM_EMISORES + NUM_RECEPTORES + NUM_SEMAFOROS + 3];
  int hiloIndex = 0;

  sem_init(&mapa.bloqueo, 0, 1);
  sem_init(&mutexMultas, 0, 1);

  for (int i = 0; i < NUM_RECEPTORES; i++) {
    sem_init(&receptores[i].mutex, 0, 1);
    sem_init(&receptores[i].vacio, 0, TAMANO_BUFFER);
    sem_init(&receptores[i].lleno, 0, 0);
    receptores[i].cabeza = 0;
    receptores[i].cola = 0;
  }

  for (int i = 0; i < NUM_EMISORES; i++) {
    sem_init(&emisores[i].bloqueo, 0, 1);
    sem_init(&conductores[i].bloqueo, 0, 1);
    emisores[i].senalesJustificadas = 0;
    emisores[i].siguienteIdSenal = 0;
    conductores[i].amonestaciones = 0;
  }

  pthread_create(&hilos[hiloIndex++], NULL, generarMapa, NULL);

  // Crear receptores
  for (int i = 0; i < NUM_RECEPTORES; i++) {
    int *id = malloc(sizeof(int));
    *id = i;
    pthread_create(&hilos[hiloIndex++], NULL, receptor, id);
  }

  // Crear emisores
  for (int i = 0; i < NUM_EMISORES; i++) {
    int *id = malloc(sizeof(int));
    *id = i;
    pthread_create(&hilos[hiloIndex++], NULL, emisor, id);
  }

  // Crear semáforos
  for (int i = 0; i < NUM_SEMAFOROS; i++) {
    int *id = malloc(sizeof(int));
    *id = i;
    pthread_create(&hilos[hiloIndex++], NULL, semaforo, id);
  }

  pthread_create(&hilos[hiloIndex++], NULL, gestionMultas, NULL);

  sleep(SEMANAS_SIMULACION_MES * 7);
  ejecucionActiva = false;

  for (int i = 0; i < hiloIndex; i++) {
    pthread_join(hilos[i], NULL);
  }

  printf("\n--- RESULTADOS FINALES ---\n");
  printf("a) Multas procesadas: %d\n", multasProcesadas);
  printf("b) Amonestaciones totales: %d\n", senalesInjustificadas);
  printf("c) Señales justificadas: %d | Injustificadas: %d\n",
         senalesJustificadas, senalesInjustificadas);
  printf("d) Multas por amonestaciones: %d\n", multasPorAmonestaciones);

  // Limpieza
  sem_destroy(&mapa.bloqueo);
  sem_destroy(&mutexMultas);
  for (int i = 0; i < NUM_RECEPTORES; i++) {
    sem_destroy(&receptores[i].mutex);
    sem_destroy(&receptores[i].vacio);
    sem_destroy(&receptores[i].lleno);
  }
  for (int i = 0; i < NUM_EMISORES; i++) {
    sem_destroy(&emisores[i].bloqueo);
    sem_destroy(&conductores[i].bloqueo);
  }

  return 0;
}
