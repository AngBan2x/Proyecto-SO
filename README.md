# Proyecto-SO

### **Análisis del Sistema y Procesos:**
El código implementa un sistema distribuido de gestión de señales vehiculares con los siguientes procesos concurrentes:

1. **Emisores**:  
   Generan señales de corneta con ID único, enviándolas a todos los receptores mediante buffers compartidos. Usan:
   - *Buffer circular* para evitar desbordamientos (`TAMANO_BUFFER=10`).
   - *Semáforos contadores* (`vacio/lleno`) para control de capacidad.

2. **Semáforos**:  
   Actúan como validadores centralizados:
   - Deciden justificación de señales (50% probabilidad).
   - Actualizan contadores globales protegidos por semáforos (`mutexMultas`).

3. **Receptores**:  
   Procesan señales validadas con dos restricciones:
   - Máximo 3 reproducciones por emisor.
   - Marcan señales como procesadas para evitar duplicados.

4. **Gestión de Multas**:  
   Monitorea contadores semanales y aplica sanciones al superar umbrales.

---

### **Secciones y Recursos Críticos:**
| Recurso Crítico              | Sección Crítica                          | Mecanismo de Protección         |
|------------------------------|------------------------------------------|----------------------------------|
| Buffer de receptores          | Escritura/Lectura de señales             | Semáforos `mutex`, `vacio`, `lleno` |
| Contadores de señales         | Incremento de justificadas/injustificadas| Semáforo `mutexMultas`           |
| Mapa de semáforos/centros     | Actualización de posiciones              | Semáforo `mapa.bloqueo`          |
| ID de señales por emisor      | Generación de IDs secuenciales           | Semáforo `bloqueo` por emisor    |

---

### **Escenarios de Concurrencia:**
1. **Colisión en Buffer**:  
   Múltiples emisores intentando escribir en el mismo buffer. Solucionado con:
   ```c
   sem_trywait(&receptores[i].vacio); // Control de slots libres
   sem_wait(&receptores[i].mutex);    // Exclusión mutua para escritura
   ```

2. **Validación Simultánea**:  
   Varios semáforos accediendo a la misma señal. Prevenido por:
   ```c
   if (!senal->validada) { // Bandera atómica verificada antes de modificar
       senal->validada = true;
       // ... validación ...
   }
   ```

3. **Conteo de Multas**:  
   Acceso concurrente a contadores desde hilos de gestión y semáforos. Controlado con:
   ```c
   sem_wait(&mutexMultas);
   multasProcesadas++; // Operación atómica
   sem_post(&mutexMultas);
   ```

---

### **Asunciones Clave en el Diseño:**
1. **Modelado de Justificación**:  
   Se asume que el 50% de las señales son justificables (decisión aleatoria), aunque en un sistema real dependería de sensores o contexto geográfico.

2. **Ubicación Estática de Vehículos**:  
   Los receptores se marcan como `vehiculosEstacionados = true` permanentemente, simplificando la condición de validación.

3. **Mapa Idealizado**:  
   La generación aleatoria de posiciones cada 1 segundo simula un GPS perfecto sin errores de precisión.

4. **Equidad en Sincronización**:  
   Se usa `sem_trywait()` para evitar inanición, pero no se garantiza equidad estricta en el acceso a buffers.

---

### **Utilidad y Aprendizaje:**
1. **Patrones de Concurrencia**:  
   - Uso de *buffers acotados* para controlar el flujo de datos.
   - *Semáforos contadores* para gestión de recursos limitados.
   - *Exclusión mutua fina* (semáforos por recurso vs. globales).

2. **Prevención de Errores Comunes**:  
   - **Deadlocks**: Evitados con `sem_trywait()` y orden consistente en adquisición de semáforos.
   - **Race Conditions**: Todos los accesos a variables compartidas están protegidos.

3. **Diseño de Sistemas Distribuidos**:  
   - Simula arquitectura productor-consumidor multi-nivel (emisores → semáforos → receptores).
   - Muestra coordinación entre componentes heterogéneos (vehículos, semáforos, centros).

4. **Optimización de Recursos**:  
   - Buffer circular minimiza uso de memoria.
   - Semáforos POSIX son más eficientes que mutex para sincronización compleja.

---

Este código sirve como excelente laboratorio para estudiar desafíos de sistemas concurrentes del mundo real, como sistemas IoT vehiculares o redes de sensores urbanas.

---

## Requerimientos
- estar en un entorno linux (sea instalacion o WSL)
- tener gcc y make instalado
- tener git instalado tambien
