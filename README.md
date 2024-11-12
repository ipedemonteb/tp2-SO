# TPE - Sistemas Operativos

El presente trabajo consiste en el desarrollo de múltiples funcionalidades de un sistema operativo, entre las cuales se encuentran:

- <b>Manejo de Memoria</b>: Se desarrollaron dos sistemas de manejo de memoria, el primero consistiendo de una lista de bloques libres y el segundo en un sistema buddy.

- <b>Manejo de Procesos</b>: Se dearrolló un algoritmo de scheduling de procesos para administrar la distribución del CPU.

- <b>Sincronización</b>: Se implementó un sistema de semáforos para evitar condiciones de carrera y deadlocks entre procesos.

- <b>Comunicación Entre Procesos</b>: Se desarrolló un sistema de pipes de lectura y escritura para comunicar procesos.

<details>
  <summary>Contenidos</summary>
  <ol>
    <li><a href="#instalación">Instalación</a></li>
    <li><a href="#instrucciones">Instrucciones</a></li>
    <li><a href="#integrantes">Integrantes</a></li>
  </ol>
</details>

## Instalación:

Clonar el repositorio:

- HTTPS:
  ```sh
  git clone https://github.com/ipedemonteb/tp2-SO.git
  ```
- SSH:
  ```sh
  git clone git@github.com:ipedemonteb/tp2-SO.git
  ```

Luego, se debe instalar la imagen de docker provista por la cátedra:
  ```sh
  docker run -d -v $PWD:/root --security-opt seccomp:unconfined -ti --name tp2-so agodio/itba-so-multi-platform:3.0
  ```

Una vez instalda, se debe inicializar la imagen para realizar la compilación del código mediante los siguientes comandos:
  ```sh
  docker start tp2-so
  ```
  ```sh
  docker exec -ti tp2-so bash
  ```
Una vez inicializado el contenedor, se debe realizar una instrucción ```cd``` y finalmente compilar según se desee:
- Free Block List System:
  ```sh
  make free_array
  ```
- Buddy System:
  ```sh
  make buddy
  ```
Al compilarse los archivos se debe salir del contenedor y ejecutar el siguiente comando:
  ```sh
  ./run.sh
  ```

Si se desea compilar el proyecto con el fin de realizar un debugging utilizando GDB, se debe acceder nuevamente al contenedor con los pasos provistos y ejecutar:
- Free Block List System:
  ```sh
  make debug_free_array
  ```
- Buddy System:
  ```sh
  make debug_buddy
  ```
Una vez compilados, se debe salir del contenedor y ejecutar:
  ```sh
  ./run.sh gdb
  ```

<p align="right">(<a href="#tpe---sistemas-operativos">Volver</a>)</p>

## Instrucciones:

Una vez ejecutado el código se accede a la terminal del proyecto, en la cual se pueden ejecutar una serie de comando para probar las distintas funcionalidades. En primer lugar, con el comando ```help``` se puede acceder a un listado de todos los disponibles. 

Para ejecutar procesos en el background, se puede utilizar ```Proceso &``` al igual que en Linux. Para crear un pipe anónimo desde la terminal entre dos procesos se debe ejecutar:
  ```sh
  $> Proceso1 | Proceso2
  ```

### Manejo de Memoria:

Para el manejo de memoria, se puede ejecutar el comando ```mem``` para conocer el estado de la misma, obteniendo en la pantalla su tamaño, el total disponible y el total utilizado. Además, se puede ejecutar el siguiente comando para ejecutar el testeo correspondiente:
  ```sh
  $> test_mem <tamaño-de-memoria-max>
  ```

### Procesos:

Los procesos proveen una serie de funcionalidades detalladas a continuación. En primer lugar, para conocer todos los detalles de los procesos de puede ejecutar el comando ```ps```. Si se quiere matar un proceso, se debe ejecutar:
  ```sh
  $> kill <pid>
  ```
Para bloquear un proceso, el comando es el siguiente:
  ```sh
  $> block <pid>
  ```
Si lo que se quiere es cambiar la prioridad del proceso, se debe ejecutar el siguiente comando con un valor de 1 a 5:
  ```sh
  $> nice <pid> <prioridad>
  ```
Finalmente, se puede ejecutar el comando ```loop``` para obtener un saludo junto con el PID del proceso. El testeo correspondiente a los procesos se ejecuta con:
  ```sh
  $> test_processes <max-cantidad-procesos-creados>
  ```
El testeo para las prioridades se ejecuta mediante:
  ```sh
  $> test_prio
  ```

### Sincronización:

Para evaluar la sincronización, se desarrolló una solución al problema de los filósofos comensales pero versión *Fórmula 1*. El mismo se puede ejecutar mediante el comando ```phylo```. La solución permite agregar un conductor o removerlo mediante las teclas ```a``` y ```r``` correspondientemente. 

El testeo correspondiente a la sincronización puede ser ejecutado mediante la siguiente línea, donde el flag indica si se utiliza el mecanismo de semáforos (1 en caso positivo, 0 en caso negativo):
```sh
  $> test_sync <max-cantidad-operaciones> <flag-semaforos>
  ```

### Comunicación Entre Procesos:

La comunicación entre procesos se puede realizar utilizando ```|``` como se mencionó anteriormente. Se implementaron algunos comandos para complementar el testeo de su funcionamiento, como por ejemplo ```cat```, que imprime la entrada estándar, ```filter``` que filtra las vocales de la misma y ```wc``` que cuenta la cantidad de líneas.

### Funcionalidades Adicionales:

Adicionalmente, se implementó la función ```clear``` para poder limpiar la pantalla de la terminal. Si se quiere desplazar la misma hacia arriba, se puede utilizar:
```sh
  $> move_up <lineas> 
  ```
donde lineas debe ir entre 0 y 9. 

Otra funcionalidad muy útil de la terminal consiste en un historial de comandos ejecutados, al que se puede acceder con la flecha superior del teclado. 


   <p align="right">(<a href="#tpe---sistemas-operativos">Volver</a>)</p>

## Integrantes:

Josefina Gonzalez Cornet (64550) - jgonzalezcornet@itba.edu.ar

Ignacio Pedemonte Berthoud (64908) - ipedemonteberthoud@itba.edu.ar

Agustín Tomás Romero (64268) - agromero@itba.edu.ar


<p align="right">(<a href="#tpe---sistemas-operativos">Volver</a>)</p>