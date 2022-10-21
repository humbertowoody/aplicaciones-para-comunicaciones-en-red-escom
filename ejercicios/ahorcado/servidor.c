/**
 * @file servidor.c
 * @author Roldán Morales Ana Karen / Ortega Alcocer Humberto Alejandro
 * @brief Tarea - Juego de Ahorcado - Servidor del juego.
 * @date 2022-10-18
 * @copyright Copyright (c) 2022
 */

// Bibliotecas requeridas.
#include <stdio.h>      // Entrada y salida estándar.
#include <stdlib.h>     // Funciones estándar de C
#include <time.h>       // Funciones de tiempo (para randomizar la semilla aleatoria).
#include <string.h>     // Funciones para usar cadenas de caracteres (strlen etc).
#include <sys/types.h>  // Definición de tipos y estructuras para uso de tarjetas de red.
#include <sys/socket.h> // Definición de tipos y funciones para uso de sockets.
#include <netdb.h>      // Definición de funciones para traducción entre tipos de datos para redes.
#include <arpa/inet.h>  // Funcionalidades para manipulación de información de redes (inet_ntoa etc)
#include <unistd.h>     // Para usar close() y cerrar conexiones.

// Incluímos constantes compartidas entre el cliente y el servidor.
#include "constantes.h"

// Constantes locales.
#define NOMBRE_DICCIONARIO_PALABRAS "palabras.txt" // El nombre del archivo dónde estarán las palabras de nuestro juego.
#define MIN_PALABRAS 5                             // Número mínimo de palabras en el archivo.
#define MAX_PALABRAS 512                           // Número máximo de palabras en el archivo.
#define LONGITUD_MAXIMA_PALABRA 25                 // Según la RAE (https://twitter.com/RAEinforma/status/1393536842708959234?ref_src=twsrc%5Etfw%7Ctwcamp%5Etweetembed%7Ctwterm%5E1393536842708959234%7Ctwgr%5E5ae51b75177505524208a8fe67e7484c5804c52d%7Ctwcon%5Es1_&ref_url=https%3A%2F%2Fwww.esquire.com%2Fes%2Factualidad%2Flibros%2Fa40359213%2Fpalabras-mas-largas-espanol-castellano%2F) la palabra más larga en español mide 23 caracteres.

// Prototipos de funciones.
int contar_lineas(FILE *);
int dificultad_palabra(char *);
char *ofuscar_palabra(char *);

// Función principal.
int main(void) // No requiero el uso de argumentos para este programa.
{
  // Variables locales.
  FILE *diccionario_palabras;              // Apuntador de tipo FILE al archivo de texto dónde se encuentran las palabras del juego.
  char palabras[MAX_PALABRAS][12],         //  Matriz con las palabras leídas desde el diccionario.
      buffer[TAM_BUFFER],                  // Buffer para guardar el mensaje recibido/enviado.
      *palabra_ofuscada;                   // Variable para guardar la palabra ofuscada.
  int contador_palabras_diccionario,       // Variable para almacenar el número de palabras presentes en el diccionario (saltos de línea).
      iterador,                            // Iterador para el diccionario.
      temporal_resultados,                 // Variable para usar temporalmente a lo largo del código.
      descriptor_socket,                   // Descriptor de nuestro socket.
      descriptor_socket_cliente,           // Descriptor de socket de cliente.
      intentos_juego,                      // Para almacenar el número de intentos realizados por el usuario.
      dificultad,                          // La dificultad seleccionada.
      temporal_reutilizable;               // Variable temporal genérica.
  struct sockaddr_in informacion_servidor, // Estructura con la información de red del servidor.
      informacion_cliente;                 // Estructura para almacenar la información de conexión del cliente.
  ssize_t bytes_enviados_recibidos;        // Para almacenar el número de bytes que leímos o enviamos.
  socklen_t longitud_informacion_cliente;  // Variable para almacenar el tamaño de la estructura con los datos del cliente que se conectó.

  // Mostramos mensaje inicial.
  printf("\tJuego de Ahorcado - Servidor\n\n");
  printf("Clásico juego de Ahorcado implementado en el modelo Cliente - Servidor usando sockets de flujo.\n");
  printf("Realizado por:\n");
  printf("\t- Roldán Morales Ana Karen\n");
  printf("\t- Ortega Alcocer Humberto Alejandro\n");
  printf("Materia:\n");
  printf("\t- Aplicaciones para comunicaciones en Red.\n");
  printf("\t- 3CM13\n");
  printf("Escuela Superior de Cómputo del Instituto Politécnico Nacional\n\n");
  printf("Inicializando servidor...\n");

  // Aleatorizamos la semilla de aleatoriedad usando el tiempo actual del sistema.
  srand(time(NULL));
  printf("\t- Semilla aleatoria inicializada correctamente.\n");

  // Abrimos el archivo.
  diccionario_palabras = fopen(NOMBRE_DICCIONARIO_PALABRAS, "r");

  // Verificamos que se haya podido abrir el archivo.
  if (diccionario_palabras == NULL)
  {
    // Mostramos mensaje y salimos.
    printf("Error: No se ha podido cargar el diccionario de palabras desde el archivo %s\n", NOMBRE_DICCIONARIO_PALABRAS);
    printf("Inténtalo de nuevo más tarde\n\n");
    return -1; // Para indicar al S.O. que la ejecución finalizó con errores.
  }

  // Contamos el número de palabras.
  contador_palabras_diccionario = contar_lineas(diccionario_palabras);

  // Verificamos que existan, cuando menos, MIN_PALABRAS en el diccionario.
  if (contador_palabras_diccionario < MIN_PALABRAS)
  {
    // Mostramos mensaje y salimos.
    printf("Error: El archivo %s debe contar con al menos %i palabras.\n", NOMBRE_DICCIONARIO_PALABRAS, MIN_PALABRAS);
    printf("Inténtalo de nuevo más tarde\n\n");
    return -1;
  }

  // Mostramos un aviso si el archivo cuenta más de las palabras máximas permitidas.
  if (contador_palabras_diccionario > MAX_PALABRAS)
  {
    // Mostramos aviso.
    printf("[Advertencia: El archivo cuenta con %i palabras en él, sin embargo el programa solo usará las primeras %i por seguridad.]\n", contador_palabras_diccionario, MAX_PALABRAS);
  }

  // Restablecemos la posición del cursor dentro del archivo.
  fseek(
      diccionario_palabras, // El apuntador al archivo (flujo).
      0,                    // El offset a preservar en el archivo.
      SEEK_SET              // Posición destino del cursor: Fin del archivo.
  );

  // Leemos las palabras.
  for (iterador = 0; iterador < MAX_PALABRAS && iterador < contador_palabras_diccionario; iterador++)
  {
    // Leemos la palabra.
    fscanf(diccionario_palabras, "%s", palabras[iterador]);

    // La siguiente línea sirve para que podamos ver las palabras que se van leyendo del archivo.
    // printf("Debug palabra leída: %s\n", palabras[iterador]);
  }

  // Informamos al usuario sobre la lectura.
  printf("\t- Diccionario cargado exitosamente: %i palabras cargadas.\n", iterador);

  // Pre-llenamos la estructura con ceros.
  memset(&informacion_servidor, 0, sizeof(informacion_servidor));

  // LLenamos la estructura con información del servidor.
  informacion_servidor.sin_family = AF_INET;                // Permitimos IPv4.
  informacion_servidor.sin_port = htons(PUERTO_SERVIDOR);   // Establecemos el puerto de escucha.
  informacion_servidor.sin_addr.s_addr = htonl(INADDR_ANY); // Escuchamos cualquier IP entrante.

  // Creamos el socket.
  descriptor_socket = socket(
      AF_INET,     // IPv4
      SOCK_STREAM, // Socket de flujo.
      0            // Cualquier protocolo.
  );

  // Validamos que se haya podido crear el socket.
  if (descriptor_socket == -1)
  {
    // Mostramos mensaje y salimos.
    printf("Error: No se ha podido crear el socket.\n");
    printf("Inténtalo de nuevo más tarde.\n\n");
    return -1;
  }

  // Usamos la variable temporal.
  temporal_reutilizable = 1;

  // Definimos las opciones del socket que queremos usar.
  temporal_resultados = setsockopt(
      descriptor_socket,            // El descriptor del socket a utilizar.
      SOL_SOCKET,                   // La configuración será a nivel socket.
      SO_REUSEADDR,                 // La opción en cuestión: Reutilizar la misma dirección de red.
      &temporal_reutilizable,       // Valor que queremos colocar en la opción: 1, equivalente a "true", es decir, activado [casteamos al tipo requerido].
      sizeof(temporal_reutilizable) // El tamaño del valor anterior, es un entero.
  );

  // Validamos que se hayan colocado las opciones correctamente.
  if (temporal_resultados < 0)
  {
    // Mostramos mensaje y salimos.
    printf("Error: No se han podido establecer las opciones del socket.\n");
    printf("Inténtalo de nuevo más tarde.\n\n");
    return -1;
  }

  // Realizamos el bind() de nuestro socket con las opciones proporcionadas.
  temporal_resultados = bind(
      descriptor_socket,                        // Descriptor del socket a utilizar.
      (struct sockaddr *)&informacion_servidor, // Usamos la información definida del servidor.
      sizeof(informacion_servidor)              // El tamaño de la estructura.
  );

  // Verificamos que se haya podido realizar el bind.
  if (temporal_resultados < 0)
  {
    // Mostramos mensaje y salimos.
    printf("Error: No se ha podido utilizar el puerto especificado (%i).\n", PUERTO_SERVIDOR);
    printf("Inténtalo de nuevo más tarde.\n");
    return -1;
  }

  // Comenzamos a escuchar en nuestro socket por información entrante (intentos de conexión entrantes, más específicamente).
  temporal_resultados = listen(
      descriptor_socket, // El descriptor del socket que queremos escuchar.
      3                  // Nuestro "backlog", o número de conexiones pendientes a encolar. Realmente aquí podríamos usar 1, pero por si las moscas.
  );

  // Validamos que se puedan escuchar conexiones entrantes.
  if (temporal_resultados < 0)
  {
    // Mostramos mensaje y salimos.
    printf("Error: No se ha podido iniciar a escuchar conexiones entrantes.\n");
    printf("Inténtalo de nuevo más tarde.\n\n");
    return -1;
  }

  // Mostramos mensaje de confirmación de inicialización de red.
  printf("\t- Conexión de red inicializada correctamente.\n");
  printf("\t- ¡Servidor inicializado correctamente!\n");
  printf("Esperando conexiones entrantes en: %s:%i\n", IP_SERVIDOR, PUERTO_SERVIDOR);

  // Ajustamos el tamaño de nuestra estructura de información del cliente.
  longitud_informacion_cliente = sizeof(informacion_cliente);

  // Ciclo infinito.
  while (1)
  {
    // Aceptamos la conexión entrante del cliente.
    descriptor_socket_cliente = accept(
        descriptor_socket,                       // Descriptor del socket del servidor dónde esperamos la conexión.
        (struct sockaddr *)&informacion_cliente, // Estructura dónde guardaremos la información del cliente que se ha conectado.
        &longitud_informacion_cliente            // El tamaño de la estructura dónde almacenaremos la información del cliente.
    );

    // Validamos que se haya aceptado correctamente la conexión.
    if (descriptor_socket_cliente < 0)
    {
      // Mostramos mensaje de error y esperamos la siguiente conexión.
      printf("[Advertencia: Un cliente trató de conectarse pero ocurrió un errror. Ignorando intento de conexión.]\n");
      break;
    }

    // Mostramos la información del cliente que se acaba de conectar.
    printf("¡Conexión aceptada exitosamente! Datos:\n");
    printf("\t- IP: %s\n", inet_ntoa(informacion_cliente.sin_addr));
    printf("\t- Puerto: %i\n", ntohs(informacion_cliente.sin_port));

    // Mostramos mensaje de inicio de juego.
    printf("¡Inicia Partida!\n");

    // Recibimos la dificultad.
    bytes_enviados_recibidos = recv(
        descriptor_socket_cliente, // El descriptor del socket dónde queremos leer datos.
        buffer,                    // El buffer dónde almacenar el mensaje entrante.
        sizeof(buffer),            // El tamaño máximo del buffer, calculado, (no de la constante) no vaya a ser...
        0                          // Ninguna bandera de configuración.
    );

    // Validamos que hayamos recibido algo.
    if (bytes_enviados_recibidos <= 0)
    {
      // Mostramos mensaje y usamos dificultad default.
      printf("[Advertencia: No se recibió una dificultad, usando fácil.]\n");
      dificultad = OPCION_DIFICULTAD_FACIL;
    }
    else
    {
      // Colocamos nuestro EOF en la i-ésima posición de nuestro buffer para que sea un string válido.
      buffer[bytes_enviados_recibidos] = '\0';

      // Colocamos la dificultad usando atoi().
      dificultad = atoi(buffer);

      // Validamos la dificultad o usamos default.
      if (dificultad != OPCION_DIFICULTAD_FACIL && dificultad != OPCION_DIFICULTAD_MEDIA && dificultad != OPCION_DIFICULTAD_DIFICIL)
      {
        // Mostramos una advertencia y usamos default.
        printf("[Advertencia: La dificultad recibida (%i) no es válida, usando fácil.]\n", dificultad);
        dificultad = OPCION_DIFICULTAD_FACIL;
      }
    }

    // Imprimimos la dificultad seleccionada.
    printf("\t- Dificultad seleccionada: %i.\n", dificultad);

    // Elegimos una palabra al azar en la dificultad seleccionada.
    do
    {
      temporal_reutilizable = rand() % contador_palabras_diccionario;
    } while (dificultad_palabra(palabras[temporal_reutilizable]) != dificultad);

    // Imprimimos la palabra encontrada.
    printf("\t- Palabra a usar: %s\n", palabras[temporal_reutilizable]);

    // Encontramos la palabra ofuscada correspondiente.
    palabra_ofuscada = ofuscar_palabra(palabras[temporal_reutilizable]);

    // Mostramos la palabra ofuscada correspondiente.
    printf("\t- Palabra ofuscada: %s (%s)\n", palabra_ofuscada, palabras[temporal_reutilizable]);

    // Mostramos el inicio de cada intento.
    printf("Intentos de jugador:\n");

    // Ciclo principal del juego.
    // Usamos 18 como el múltiplo común para determinar el número de intentos válidos.
    // 18/3 = 6 (Difícil)
    // 18/2 = 9 (Medio)
    // 18/1 = 18 (Fácil)
    for (intentos_juego = 0; intentos_juego < (18 / dificultad); intentos_juego++)
    {
      // Enviamos el estado actual.
      bytes_enviados_recibidos = send(
          descriptor_socket_cliente, // El descriptor del socket mediante el cual realizaremos el envío de los datos.
          palabra_ofuscada,          // El dato a enviar.
          strlen(palabra_ofuscada),  // La longitud del dato a enviar.
          0                          // No usamos banderas.
      );

      // Validamos el envío.
      if (bytes_enviados_recibidos <= 0)
      {
        // Mostramos mensaje de error y salimos.
        printf("Error: No se pudo enviar la palabra ofuscada.\n");
        printf("Inténtalo de nuevo más tarde.\n\n");
        return -1;
      }

      // Recibimos respuesta.
      bytes_enviados_recibidos = recv(
          descriptor_socket_cliente, // El descriptor del socket dónde queremos leer datos.
          buffer,                    // El buffer dónde almacenar el mensaje entrante.
          sizeof(buffer),            // El tamaño máximo del buffer, calculado, (no de la constante) no vaya a ser...
          0                          // Ninguna bandera de configuración.
      );

      // Validamos la recepción de datos.
      if (bytes_enviados_recibidos < 0)
      {
        // Mostramos mensaje de error y salimos.
        printf("Error: No se pudo recibir el intento del usuario.\n");
        printf("Inténtalo de nuevo más tarde.\n\n");
        return -1;
      }

      // Colocamos nuestro EOF al final del buffer.
      buffer[bytes_enviados_recibidos] = '\0';

      // Parseamos respuesta.
      printf("\t%i) %s - %s\n", intentos_juego, buffer, strcmp(buffer, palabras[temporal_reutilizable]) == 0 ? "Correcto" : "Incorrecto");

      // Si el jugador ganó, rompemos el ciclo.
      if (strcmp(buffer, palabras[temporal_reutilizable]) == 0)
      {
        // Mostramos un mensaje.
        printf("\t- Usuario ha ganado en intento #%i\n", intentos_juego);
        break;
      }
    }

    // Verificamos el estado final del juego.
    if (strcmp(buffer, palabras[temporal_reutilizable]) == 0)
    {
      // Enviamos mensaje de victoria.
      bytes_enviados_recibidos = send(
          descriptor_socket_cliente, // El descriptor del socket mediante el cual realizaremos el envío de los datos.
          CADENA_GANADOR,            // El dato a enviar.
          strlen(CADENA_GANADOR),    // La longitud del dato a enviar.
          0                          // No usamos banderas.
      );

      // Validamos el envío.
      if (bytes_enviados_recibidos <= 0)
      {
        // Mostramos mensaje de error y salimos.
        printf("Error: No se pudo enviar mensaje de victoria.\n");
        printf("Inténtalo de nuevo más tarde.\n\n");
        return -1;
      }
    }
    else
    {
      // Enviamos mensaje de fin de juego
      bytes_enviados_recibidos = send(
          descriptor_socket_cliente,   // El descriptor del socket mediante el cual realizaremos el envío de los datos.
          CADENA_FIN_DE_JUEGO,         // El dato a enviar.
          strlen(CADENA_FIN_DE_JUEGO), // La longitud del dato a enviar.
          0                            // No usamos banderas.
      );

      // Validamos el envío.
      if (bytes_enviados_recibidos <= 0)
      {
        // Mostramos mensaje de error y salimos.
        printf("Error: No se pudo enviar mensaje de intentos máximos superados.\n");
        printf("Inténtalo de nuevo más tarde.\n\n");
        return -1;
      }

      // Mostramos mensaje.
      printf("\t- Usuario ha perdido en intento #%i.", intentos_juego);
    }

    // Fin de partida.
    close(descriptor_socket_cliente);
  }

  // Cerramos el socket.
  close(descriptor_socket);

  // Fin de ejecución.
  return 0;
}

// Implementación de funciones.

/**
 * @brief Obtiene el número de líneas presentes en un archivo de texto.
 *
 * @param archivo El archivo a leer.
 * @return int El número de líneas presentes en el archivo.
 */
int contar_lineas(FILE *archivo)
{
  // Variables locales.
  char buffer[TAM_BUFFER]; // Búffer para leer el archivo en bloques de TAM_BUFFER.
  int contador_saltos = 0; // Contador del número de saltos de línea ('\n') encontrados.
  size_t bytes_leidos,     // Variable para almacenar el número de bytes leídos del archivo.
      iterador;            // Variable para iterar sobre nuestro búffer en búsqueda de saltos de línea.

  // Ciclo infinito para recorrer el archivo totalmente.
  // Se garantiza salida.
  for (;;)
  {
    // Leemos TAM_BUFFER bytes del archivo.
    bytes_leidos = fread(buffer, 1, TAM_BUFFER, archivo);

    // Validamos si ocurrió algún error.
    if (ferror(archivo))
    {
      // Mostramos mensaje y salimos.
      printf("Error: Ocurrió un problema al leer el archivo: %s\n", NOMBRE_DICCIONARIO_PALABRAS);
      printf("Inténtalo de nuevo más tarde\n\n");
      return -1; // Para indicarle al S.O. que la ejecución no fue satisfactoria.
    }

    // Ciclo para recorrer el buffer leído en búsqueda de saltos de línea.
    for (iterador = 0; iterador < bytes_leidos; iterador++)
    {
      // Si el caracter actual es un salto de línea, aumentamos el contador.
      if (buffer[iterador] == '\n')
        contador_saltos++;
    }

    // Si es el EOF (end of file, fin de archivo), salimos del ciclo.
    if (feof(archivo))
    {
      break;
    }
  }

  // Regresamos nuestro contador.
  return contador_saltos;
}

/**
 * @brief Función para calcular la dificultad de una palabra a partir de su longitud.
 *
 * @param palabra La palabra a validar.
 * @return int Una opción de longitud entre: OPCION_DIFICULTAD_FACIL, OPCION_DIFICULTAD_MEDIA y OPCION_DIFICULTAD_DIFICIL.
 */
int dificultad_palabra(char *palabra)
{
  // Variables locales.
  size_t longitud_palabra = strlen(palabra); // La longitud calculada por strlen().

  // Determinamos la dificultad.
  if (longitud_palabra <= LONGITUD_MAXIMA_FACIL)
  {
    return OPCION_DIFICULTAD_FACIL;
  }
  else if (longitud_palabra <= LONGITUD_MAXIMA_MEDIA)
  {
    return OPCION_DIFICULTAD_MEDIA;
  }
  else
  {
    return OPCION_DIFICULTAD_DIFICIL;
  }
}

/**
 * @brief Función que toma una palabra y la rellena con '_' para ofuscarla un poco.
 *
 * @return char* La palabra ofuscada.
 */
char *ofuscar_palabra(char *original)
{
  // Variables locales.
  char *resultado = NULL; // Variable con la palabra resultante ofuscada.
  int longitud,           // Varaible con la longitud de la palabra.
      iterador,           // Para iterar sobre la palabra.
      temporal,           // Para valores temporales.
      letras_a_ofuscar;   // Para guardar el número de letras que ofuscaremos.

  // Calculamos la longitud usando strlen.
  longitud = strlen(original);

  // Creamos la memoria para el nuevo string.
  resultado = (char *)malloc(longitud);

  // Copiamos los contenidos del original en el resultado.
  strcpy(resultado, original);

  // Calculamos el número de letras a ofuscar.
  letras_a_ofuscar = longitud - (longitud / 3); // 4 - (4/3) = 3 | 8 - (8/3) = 6 | 12 - (12/3) = 8.

  // Ofuscamos letras_a_ofuscar letras de la palabra.
  for (iterador = 0; iterador < letras_a_ofuscar; iterador++)
  {
    // Buscamos una posición aleatoria que no hayamos ofuscado previamente.
    do
    {
      temporal = rand() % longitud;
    } while (resultado[temporal] == '_');

    // Ofuscamos en la posición.
    resultado[temporal] = '_';
  }

  // Regresamos la palabra ofuscada.
  return (char *)resultado;
}
