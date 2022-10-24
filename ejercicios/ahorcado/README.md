# Juego de Ahorcado

Implementar el juego Ahorcado usando sockets de flujo bloqueantes.

## Consdieraciones

- El juego debe tener niveles de dificultad, es decir, el cliente podrá
  seleccionar entre distintas dificultades disponibles.
- El servidor validará cuando el usuario haya ganado o perdido.

## Compilación

Para compilar el cliente y/o servidor usaremos un `Makefile` con el siguiente
comando:

```bash
make
```

Para limpiar (eliminar) los archivos compilados (los archivos objeto + los
binarios), podemos usar el siguiente comando:

```bash
make clean
```

## Ejecución

Para ejecutar cualquiera de los programas, bastará con usar:

```bash
./cliente
```

Para ejecutar el cliente, o bien:

```bash
./servidor
```

Para ejecutar el servidor.

## Configuración del programa

Para modificar los parámetros de configuración del programa, se tendrá que 
modificar el archivo `constantes.h`, el cual contiene las siguientes opciones:

- Puerto del Servidor.
- IP del Servidor.
- Tamaño del búffer de lectura/escritura (para cliente y servidor).
- Cadena de fin de juego.
- Cadena de inicio de juego.
- ID's para cada dificultad (fácil, media y difícil).
- Longitud máxima de palabra para cada dificultad (fácil, media y difícil).

### Configuraciones específicas al servidor.

El servidor cuenta con constantes particulares a su ejecución que podrán
ser modificadas a voluntad, entre ellas están:

- Nombre del archivo de diccionario de palabras.
- Mínimo de palabras que deben existir en el archivo.
- Máximo de palabras que deben existir en el archivo.
- Longitud máxima para una palabra.
  - En esta variable se recomienda mantener el valor establecido pues se utilizó
    un valor propocionado por la RAE para la palabra más grande conocida en el
    español.

## Flujo de juego

El flujo de juego es el siguiente:

1. Inicia el servidor
  1.1. Se carga el archivo diccionario
  1.2. Se incializa la conectividad en red
  1.3. Comienza la escucha en el puerto especificado.
2. Inicia el cliente
  2.1. El cliente establece la conexión con el servidor
3. El cliente lee la dificultad y la envía al servidor
4. El servidor busca una palabra en la dificultad y la ofusca
5. Inicia el ciclo de juego principal
 5.1. El servidor envía la palabra ofuscada
 5.2. El cliente lee el intento del usuario y lo envía al servidor
 5.3. El servidor recibe el intento y verifica si es la palabra original
6. Si se terminan los intentos o el usuario le atina a la palabra, cierra la
   conexión con el cliente.

## Equipo de trabajo

- Roldán Morales Ana Karen
- Ortega Alcocer Humberto Alejandro

## Datos de la Materia

- Aplicaciones para comunicaciones en Red
- 3CM13
- Escuela Superior de Cómputo del Instituto Politécnico Nacional


