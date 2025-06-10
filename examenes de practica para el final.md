# Simulacros de Examen — Arquitectura de Computadoras y Sistemas Operativos

## Simulacro 1 · Teórico-conceptual

1. **TLB**  
   a) Explica qué es el Translation Look-aside Buffer y por qué reduce el tiempo de acceso a memoria en procesadores modernos.  
   b) ¿De qué depende su tasa de aciertos (“hit rate”) y cómo impacta eso en el CPI?

2. **CMR vs SMR**  
   Compara ambas tecnologías de grabación magnética indicando:  
   * Principio físico de cada una.  
   * Ventajas y desventajas concretas.  
   * Un caso de uso típico para cada tecnología.

3. **RAID 10**  
   Dispones de *seis discos de 1 TB*.  
   1. Diseña el arreglo para obtener la mayor capacidad útil posible manteniendo redundancia RAID 10.  
   2. ¿Cuánta capacidad neta obtienes?  
   3. ¿Se incrementa, disminuye o se mantiene la tasa de IOPS respecto a un solo disco? Justifica.

4. **Filesystem con journaling**  
   Explica qué es, cómo funciona el *journal* en el manejo de metadatos y qué ventaja aporta tras un apagón inesperado.

5. **Alta disponibilidad (HA) en VMs**  
   Define *High Availability* en entornos de virtualización y menciona dos mecanismos concretos que implementa un hipervisor típico para alcanzarla.

6. **UMA / NUMA**  
   a) Define ambos modelos.  
   b) En un procesador Intel Core i9 de escritorio, ¿cuál de los dos aplica y por qué?

7. **Swap-Out en Windows**  
   Describe el algoritmo que usa Windows para decidir qué página de RAM enviar a disco cuando hay presión de memoria. ¿Qué indicadores de hardware/software toma en cuenta?

8. **MBR vs UEFI**  
   ¿Cuáles son las principales diferencias de diseño entre el esquema de arranque MBR y UEFI? Nombra al menos dos implicancias de seguridad.

9. **Segmentación**  
   Explica el mecanismo de segmentación en x86 protegido, indicando qué campos del descriptor participan para: (i) verificar los privilegios de acceso y (ii) detectar violaciones de límite.

10. **Multiprocesamiento**  
    Señala dos condiciones necesarias para que aparezcan *race conditions* al acceder a memoria compartida y describe brevemente una técnica de sincronización que las evite.

---

## Simulacro 2 · Mixto teoría + cálculo

1. Calcula la latencia promedio de acceso a un disco HDD 7200 rpm (4 ms *seek* avg, 4 ms latencia rotacional avg, 0,5 ms transferencia), indicando todas las etapas.

2. Distingue entre **IaaS, PaaS y SaaS** y da un ejemplo real de servicio comercial para cada capa.

3. En un sistema con página de 4 KiB y TLB de 64 entradas:  
   a) ¿Cuánta memoria virtual puede mapearse sin fallos de TLB?  
   b) Si la tasa de fallos es 6 %, ¿cuál es el *overhead* aproximado asumiendo que un fallo de TLB cuesta 30 cycles y un acceso con *hit* cuesta 1 cycle?

4. Dibuja el *pipeline* de cinco etapas clásico (IF, ID, EX, MEM, WB) anotando en qué etapa se consulta la TLB y por qué.

5. **CMR/SMR**: decide qué tipo usarías para (i) un servidor de backup de vídeo y (ii) una base de datos OLTP. Razona.

6. Dados *cuatro discos de 2 TB* a 200 MB/s secuencial y 100 IOPS aleatorios:  
   a) ¿Capacidad y tolerancia a fallos de RAID 5?  
   b) Misma pregunta para RAID 6.  
   c) Compara *throughput* secuencial de ambos.

7. Un sistema Linux con ext4 usando journaling en modo “data-ordered” sufre un corte de energía:  
   a) Describe los pasos de recuperación al reiniciar.  
   b) ¿En qué casos se produciría journaling en “full data” y por qué ext4 no lo usa por defecto?

8. **NUMA awareness**: explica cómo un *scheduler* de SO optimiza la afinidad de hilos para minimizar latencia de memoria en sistemas NUMA.

9. En modo protegido x86-64 sin paginación, ¿qué verifica la CPU antes de cargar un descriptor de segmento de código a CS? Detalla los *checks* de privilegio.

10. **Boot flow con UEFI Secure Boot**: esquematiza la cadena de confianza desde el firmware hasta el kernel.

---

## Simulacro 3 · Práctico de segmentación

Se da el siguiente contexto en un 80386 (modo protegido, sin paginación):

`CS = 0x0403` 

`LDTR = 0x0006:0x8000` 

`Mem[0x8000] = 00 10 00 33 00 F0 00 00` 

`Mem[0x8008] = 00 10 00 00 00 F0 00 01` 


1. Interpreta la entrada de la LDT en `0x8000` como descriptor de segmento. Expón base, límite, DPL, tipo y *flags* (G, A, P).

2. ¿Qué ocurre al ejecutar `MOV DS, 0x000F`? Explica paso a paso (selector → índice, TI, RPL, chequeos, etc.).

3. A continuación se ejecuta `MOV [0x8000], RAX`.  
   a) ¿Se produce o no excepción?  
   b) Si se produce, ¿cuál (número y nombre)? Justifica.

4. Diseña dos ejemplos de *selectors* que provocarían GP #13 y SS #12 respectivamente al intentar cargarlos en DS.

5. Calcula la dirección lineal resultante de la instrucción `MOV EAX, [DS:0x1234]` si DS apunta al descriptor anterior y el desplazamiento es `0x1234`.

6. Menciona dos ventajas y dos desventajas de la segmentación frente a paginación para protección de memoria.

7. ¿Cómo coopera la segmentación con la paginación cuando ambas están habilitadas en x86? Describe la ruta de traducción.

8. El bit **AVL** en el descriptor, ¿para qué se suele usar en sistemas operativos modernos?

9. Dado un selector con RPL = 3 que intenta acceder a un descriptor con DPL = 0 y tipo *Data-RW*:  
   a) ¿Se permite el acceso?  
   b) ¿Y si el descriptor fuera *Code-Execute-Only, conforming*? Razona.

10. Propón un mecanismo simple de *sandboxing* aprovechando únicamente la segmentación (sin paginación) e indica sus limitaciones.

---

## Simulacro 4 · Preguntas cortas (≤ 4 líneas)

1. ¿Qué diferencia hay entre “interrupt” y “exception” en x86?  
2. Cita dos políticas de *scheduling* de Linux y para qué cargas se recomiendan.  
3. ¿Qué es un “dirty bit” en la caché y para qué lo usa el protocolo MESI?  
4. ¿Por qué RAID 0 no brinda tolerancia a fallos pero se emplea en servidores de bases de datos temporales?  
5. Define *copy-on-write* y da un ejemplo de uso en un SO.  
6. ¿Qué mejora ofrece el modo “journal checksum” de ext4?  
7. ¿Qué componente del CPU se encarga de resolver “false dependencies” (WAR y WAW)?  
8. Ventaja principal de UEFI *fast boot* respecto a POST tradicional.  
9. ¿Para qué sirve la instrucción `CLFLUSH` y en qué ataques *side-channel* se aprovecha?  
10. ¿Cuál es la diferencia entre “hard” y “soft” affinity al asignar hilos en un sistema NUMA?
