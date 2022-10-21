/**
 * @file cliente.c
 * @author Roldán Morales Ana Karen / Ortega Alcocer Humberto Alejandro
 * @brief Tarea - Juego de Ahorcado - Cliente del juego.
 * @date 2022-10-18
 * @copyright Copyright (c) 2022
 */

// Bibliotecas requeridas.
#include <stdio.h>      // Entrada/Salida estándar.
#include <stdlib.h>     // Funcionalidades estándar.
#include <sys/socket.h> // Para usar sockets y funciones relacionadas (socket, send, recv, etc...)
#include <netinet/in.h> // Para la estructura de inforamción del servidor.
#include <sys/types.h>  // Para los tipos de datos, estructuras y demás requeridos para el manejo de sockets y conexiones.
#include <string.h>     // Para manipulación de cadenas de caracteres y uso de memset.
#include <arpa/inet.h>  // Para funciones de "parsing" de IP's y DNS's.

// Incluímos constantes.
#include "constantes.h"

// Prototipos de funciones.
void mostrar_menu_dificultad(void);

// Función principal.
int main(void) // No usamos argumentos en este programa.
{
  // Variables locales.
  int descriptor_socket,                 // El descriptor del socket.
      resultado_conexion,                // Variable para almacenar el resultado de la conexión.
      dificultad,                        // Variable para almacenar la dificultad seleccionada por el usuario.
      intentos;                          // Variable para llevar la cuenta del número de intentos realizados.
  ssize_t bytes_enviados_recibidos;      // Variable para almacenar el número de bytes enviados en un send() o recibidos en un recv().
  char buffer_mensaje[TAM_BUFFER];       // Buffer para el mensaje.
  struct sockaddr_in direccion_servidor; // Estructura con la información del servidor.

  // Mostramos mensaje inicial.
  printf("\tJuego de Ahorcado - Cliente\n\n");
  printf("Clásico juego de Ahorcado implementado en el modelo Cliente - Servidor usando sockets de flujo.\n");
  printf("Elige una dificultad y trata de adivinar la palabra en el menor número de intentos posibles.\n");
  printf("Realizado por:\n");
  printf("\t- Roldán Morales Ana Karen\n");
  printf("\t- Ortega Alcocer Humberto Alejandro\n");
  printf("Materia:\n");
  printf("\t- Aplicaciones para comunicaciones en Red.\n");
  printf("\t- 3CM13\n");
  printf("Escuela Superior de Cómputo del Instituto Politécnico Nacional\n\n");

  // Indicamos al usuario que estamos iniciando la conexión con el servidor.
  printf("Iniciando conexión...\n");

  // Creamos el socket.
  descriptor_socket = socket(
      AF_INET,     // IPv4
      SOCK_STREAM, // Socket de flujo
      0            // Sin especificar el protocolo (aceptar todos los protocolos)
  );

  // Validamos el socket.
  if (descriptor_socket < 0)
  {
    // Mostramos mensaje de error y salimos.
    printf("Error: ¡No se ha podido crear el socket!\n");
    printf("Inténtalo de nuevo más tarde.\n\n");
    return -1; // Usamos -1 para indicar al S.O. que algo salió mal :(
  }

  // Pre-llenamos la estructura con ceros.
  memset(&direccion_servidor, 0, sizeof(direccion_servidor));

  // Llenamos la estructura con la información del servidor.
  direccion_servidor.sin_family = AF_INET;                     // IPv4
  direccion_servidor.sin_port = htons(PUERTO_SERVIDOR);        // Puerto a utilizar (en formato de Network Short).
  direccion_servidor.sin_addr.s_addr = inet_addr(IP_SERVIDOR); // La dirección IP del servidor (string) en formato in_addr_t.

  // Realizamos la conexión.
  resultado_conexion = connect(
      descriptor_socket,                      // El socket a utilizar.
      (struct sockaddr *)&direccion_servidor, // Cast de la estructura de dirección del servidor.
      sizeof(direccion_servidor)              // Tamaño de la estructura con información del servidor.
  );

  // Validamos la conexión.
  if (resultado_conexion < 0)
  {
    // Mostramos mensaje de error y salimos.
    printf("Error: ¡No se ha podido realizar la conexión con el servidor!\n");
    printf("(¿Está el servidor corriendo en %s:%i?)\n", IP_SERVIDOR, PUERTO_SERVIDOR);
    printf("Inténtalo de nuevo más tarde.\n\n");
    return -1; // Usamos -1 para indicar al S.O. que algo salió mal.
  }

  // ---- Secuencia principal del juego.

  // Menú para elegir la dificultad.
  do
  {
    mostrar_menu_dificultad(); // Mostramos el menú de dificultades.
    scanf("%i", &dificultad);  // Leemos la dificultad seleccionada por el usuario.
  } while (dificultad < OPCION_DIFICULTAD_FACIL || dificultad > OPCION_DIFICULTAD_DIFICIL);

  // Convertimos la dificultad de int a char* usando snprintf() (sprintf() es inseguro).
  snprintf(
      buffer_mensaje,         // Cadena destino.
      sizeof(buffer_mensaje), // Tamaño máximo de la cadena destino
      "%i",                   // Formato a imprimir.
      dificultad              // Variable para el formato
  );

  // Enviamos la dificultad a nuestro servidor.
  bytes_enviados_recibidos = send(
      descriptor_socket,            // El socket mediante el cual realizaremos la transmisión.
      (const void *)buffer_mensaje, // El mensaje a enviar, cast-eado a const void*.
      strlen(buffer_mensaje),       // El tamaño/longitud del mensaje a enviar.
      0                             // No usamos banderas (MSG_OOB, MSG_DONTROUTE) porque no se necesitan.
  );

  // Validamos que se hayan enviado los bytes correspondientes.
  if (bytes_enviados_recibidos < 0)
  {
    // Mostramos mensaje y salimos.
    printf("Error: Ocurrió un problema al tratar de enviar la dificultad solicitada al servidor.\n");
    printf("Intente de nuevo más tarde.\n\n");
    return -1;
  }

  // Inicializamos los intentos realizados por el usuario.
  intentos = 0;

  // Aquí inicia el ciclo, el servidor enviará 2 tipos de mensajes:
  //   - La cadena "CADENA_FIN_DE_JUEGO" para indicar que se terminaron los intentos.
  //   - La cadena "CADENA_GANADOR" para indicar que el usuario ha ganado.
  //   - Otra cadena con el texto _tal cual_ debemos mostrar al usuario.
  // En cada caso, se recibirá el dato y se mostrará la interfaz con información de juego para el usuario.
  do
  {
    // Recibimos el dato del servidor.
    bytes_enviados_recibidos = recv(
        descriptor_socket,      // El socket del cual recibiremos los datos.
        buffer_mensaje,         // El buffer dónde almacenaremos el mensaje recibido.
        sizeof(buffer_mensaje), // El tamaño máximo del buffer, calculado, no de constante (no vaya a ser...)
        0                       // Banderas de configuración, ninguna.
    );

    // Validamos que hayamos recibido...algo.
    if (bytes_enviados_recibidos <= 0)
    {
      // Mostramos mensaje de error y salimos.
      printf("Error: Ocurrió un problema leyendo información del servidor.\n");
      printf("Inténtalo de nuevo más tarde.\n\n");
      return -1;
    }

    // Añadimos nuestro EOF (End Of File) en la i-ésima posición del arreglo de caracteres para que sea un string válido.
    buffer_mensaje[bytes_enviados_recibidos] = '\0';

    // Verificamos si el juego ha terminado.
    if ((strcmp(buffer_mensaje, CADENA_FIN_DE_JUEGO) == 0) || (strcmp(buffer_mensaje, CADENA_GANADOR) == 0))
    {
      break;
    }

    // Mostramos la interfaz principal.
    printf("Palabra: %s\n", buffer_mensaje);
    printf("Intentos realizados: %i\n\n", intentos);
    printf("¿Qué palabra crees que es? ");
    scanf("%s", buffer_mensaje);

    // Enviamos mensaje del usuario.
    bytes_enviados_recibidos = send(
        descriptor_socket,      // El descriptor del socket mediante el cual realizaremos el envío de los datos.
        buffer_mensaje,         // El dato a enviar.
        strlen(buffer_mensaje), // La longitud del dato a enviar.
        0                       // No usamos banderas.
    );

    // Validamos el envío.
    if (bytes_enviados_recibidos <= 0)
    {
      // Mostramos mensaje de error y salimos.
      printf("Error: No se pudo enviar intento de usuario.\n");
      printf("Inténtalo de nuevo más tarde.\n\n");
      return -1;
    }

    // Aumentamos los intentos realizados por el usuario.
    intentos++;
  } while ((strcmp(buffer_mensaje, CADENA_FIN_DE_JUEGO) != 0) && (strcmp(buffer_mensaje, CADENA_GANADOR) != 0));

  // Validamos estado final del juego.
  if (strcmp(buffer_mensaje, CADENA_GANADOR) == 0)
  {
    printf("+------------------------+\n");
    printf("| ¡GANASTE, FELICIDADES! |\n");
    printf("+------------------------+\n\n");
  }
  else
  {
    printf("+---------------------------------------+\n");
    printf("| PERDISTE, LÁSTIMA, AHÍ PA' LA PRÓXIMA |\n");
    printf("+---------------------------------------+\n\n");
  }

  // Mostramos información final.
  printf("¡Gracias por jugar ahorcado!\n");

  // Finalizamos la ejecución del juego.
  return 0;
}

// Implementación de funciones.

/**
 * @brief Función que muestra el menú para seleccionar la dificultad en consola.
 */
void mostrar_menu_dificultad()
{
  printf("Selecciona la dificultad con la que quieres jugar:\n");
  printf("\t %i) Fácil - Palabras simples de %i caracteres máximo.\n", OPCION_DIFICULTAD_FACIL, LONGITUD_MAXIMA_FACIL);
  printf("\t %i) Medio - Palabras cortas de %i caracteres máximo.\n", OPCION_DIFICULTAD_MEDIA, LONGITUD_MAXIMA_MEDIA);
  printf("\t %i) Difícil - Palabras largas de %i caracteres o más.\n\n", OPCION_DIFICULTAD_DIFICIL, LONGITUD_MAXIMA_MEDIA);
  printf("Introduce el número de tu elección [%i - %i]: ", OPCION_DIFICULTAD_FACIL, OPCION_DIFICULTAD_DIFICIL);
}
