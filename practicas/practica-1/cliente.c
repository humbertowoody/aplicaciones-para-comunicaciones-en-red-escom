/**
 * @file cliente.c
 * @author Roldán Morales Ana Karen / Ortega Alcocer Humberto Alejandro
 * @brief Práctica 1 - Cliente de aplicación de envío/recepción de archivos.
 * @date 2022-10-18
 * @copyright Copyright (c) 2022
 */

// Bibliotecas requeridas.
#include <stdio.h>      // Para operaciones estándar de entrada y salida.
#include <stdlib.h>     // Funciones de librería estándar de C.
#include <sys/socket.h> // Para operaciones con sockets.
#include <netinet/in.h> // Para tipos de estructuras de infomración de conexión.
#include <string.h>     // Para operaciones con cadenas de caracteres (memset(), etc...)
#include <sys/types.h>  // Para los tipos de datos, estructuras y funciones para manejo de direcciones de red y puertos.
#include <arpa/inet.h>  // Para funciones de ayuda en la manipulación de información de red.
#include <netdb.h>      // Definición de funciones para traducción entre tipos de datos para redes.
#include <unistd.h>     // Para usar close() y cerrar conexiones.
#include <sys/stat.h>   // Para operaciones con archivos.

// Incluímos cabecera con constantes compartidas entre el cliente y servidor.
#include "constantes.h"

// Constantes locales.
#define OPCION_MENU_SALIR (OPERACION_DESCARGAR_ARCHIVOS + 1)

// Prototipos de funciones
void menu_principal(void);
void listar_archivos(void);
void subir_archivos(void);

// Función principal.
int main(void) // No se requieren argumentos para el programa principal.
{
  // Variables locales.
  int opcion_menu; // Para almacenar la opción del menú seleccionada por el usuario.

  // Mostramos inicialización de programa.
  printf("\tPráctica 1 - Envio/Recepción de archivos - Cliente\n\n");
  printf("Realizado por:\n");
  printf("\t- Roldán Morales Ana Karen\n");
  printf("\t- Ortega Alcocer Humberto Alejandro\n");
  printf("Materia:\n");
  printf("\t- Aplicaciones para comunicaciones en Red.\n");
  printf("\t- 3CM13\n");
  printf("Escuela Superior de Cómputo del Instituto Politécnico Nacional\n\n");
  printf("Configuración actual:\n");
  printf("\t- Tamaño del búffer: %i\n", TAM_BUFFER);
  printf("\t- Algoritmo de Nagle: %s\n", ALGORITMO_NAGLE_ACTIVADO == 1 ? "Activado" : "Desactivado");
  printf("\t- Temporizador de lectura: %is\n", TEMPORIZADOR_LECTURA_SEGUNDOS);
  printf("Si deseas modificar estas configuraciones, modifica el archivo \"constantes.h\" y recompila los programas.\n\n");

  // Ciclo principal.
  do
  {
    // Ciclo para mostrar menú y validar entrada de usuario.
    do
    {
      menu_principal();
      scanf("%i", &opcion_menu);
    } while (
        opcion_menu != OPERACION_LISTAR_ARCHIVOS &&
        opcion_menu != OPERACION_SUBIR_ARCHIVOS &&
        opcion_menu != OPERACION_DESCARGAR_ARCHIVOS &&
        opcion_menu != OPCION_MENU_SALIR);

    // Llamamos a la función adecuada dependiendo de la elección del usuario.
    switch (opcion_menu)
    {
    case OPERACION_LISTAR_ARCHIVOS:
      listar_archivos();
      break;

    case OPERACION_DESCARGAR_ARCHIVOS:
      printf("Descarga de archivos no implementada\n");
      break;

    case OPERACION_SUBIR_ARCHIVOS:
      subir_archivos();
      break;

    case OPCION_MENU_SALIR:
    default:
      printf("¡Gracias por utilizar este programa!\n");
      break;
    }
  } while (opcion_menu != OPCION_MENU_SALIR);

  // Fin de programa.
  return 0;
}

// Implementación de funciones.

/**
 * @brief Muestra el menú principal del cliente.
 *
 */
void menu_principal(void)
{
  // Mostramos el menú principal.

  printf("¿Qué operación deseas realizar?\n");
  printf("\t%i) Listar contenidos del servidor.\n", OPERACION_LISTAR_ARCHIVOS);
  printf("\t%i) Subir archivo/directorio desde el cliente.\n", OPERACION_SUBIR_ARCHIVOS);
  printf("\t%i) Descargar un archivo/directorio desde el servidor.\n", OPERACION_DESCARGAR_ARCHIVOS);
  printf("\t%i) Salir.\n\n", OPCION_MENU_SALIR);
  printf("Introduce tu elección: ");
}

/**
 * @brief Función que se conecta al servidor solicitando un listado de los archivos en su carpeta
 *        definida, recibe la información y la imprime en pantalla.
 */
void listar_archivos(void)
{
  // Variables locales.
  int descriptor_socket,                 // El descriptor del socket de conexión.
      resultado_operacion;               // Para almacenar el resultado de alguna operación (conexión, envío y/o recepción de datos, etc).
  struct sockaddr_in direccion_servidor; // Estructura con la información del servidor.
  char buffer[TAM_BUFFER];               // Buffer para operaciones de lectura/escritura.

  // Creamos el socket.
  descriptor_socket = socket(
      AF_INET,     // IPv4
      SOCK_STREAM, // Socket de flujo.
      0            // Todos los protocolos
  );

  // Validamos el socket.
  if (descriptor_socket < 0)
  {
    // Mostramos mensaje de error y volvemos al menú principal.
    printf("Error: No se ha podido crear el socket.\n");
    printf("Inténtalo de nuevo más tarde.\n\n");
    return;
  }

  // Pre-llenamos la estructura con ceros.
  memset(&direccion_servidor, 0, sizeof(direccion_servidor));

  // Llenamos la estructura con la información del servidor.
  direccion_servidor.sin_family = AF_INET;                     // IPv4
  direccion_servidor.sin_port = htons(PUERTO_SERVIDOR);        // Puerto a utilizar (en formato de Network Short).
  direccion_servidor.sin_addr.s_addr = inet_addr(IP_SERVIDOR); // La dirección IP del servidor (string) en formato in_addr_t.

  // Realizamos la conexión.
  resultado_operacion = connect(
      descriptor_socket,                      // El socket a utilizaqr.
      (struct sockaddr *)&direccion_servidor, // Cast de la estructura de dirección del servidor.
      sizeof(direccion_servidor)              // Tamaño de la estructura con la información del servidor.
  );

  // Validamos la conexión.
  if (resultado_operacion < 0)
  {
    // Mostramos mensaje de error.
    printf("Error: ¡No se ha podido realizar la conexión con el servidor!\n");
    printf("(¿Está el servidor corriendo en %s:%i?)\n", IP_SERVIDOR, PUERTO_SERVIDOR);
    printf("Inténtalo de nuevo más tarde.\n\n");

    // Cerramos el socket.
    close(descriptor_socket);

    // Volvemos al menú principal.
    return;
  }

  // Creamos la cadena de caracteres con el ID de operación.
  sprintf(buffer, "%i", OPERACION_LISTAR_ARCHIVOS);

  // Enviamos el ID de la operación.
  resultado_operacion = send(
      descriptor_socket, // El descriptor del socket que queremos usar.
      buffer,            // El dato a enviar.
      sizeof(buffer),    // El tamaño del dato que enviaremos.
      0                  // No usamos banderas.
  );

  // Validamos que se haya enviado correctamente la información.
  if (resultado_operacion < 0)
  {
    // Mostramos mensaje de error.
    printf("Error: No se ha podido enviar el ID de la operación al servidor.\n");
    printf("Inténtalo de nuevo más tarde.\n\n");

    // Cerramos el socket.
    close(descriptor_socket);

    // Volvemos al menú principal.
    return;
  }

  // Esperamos el listado de archivos desde el servidor.
  resultado_operacion = recv(
      descriptor_socket, // El descriptor del socket donde recibiremos los datos.
      buffer,            // El buffer dónde guardaremos la información recibida.
      sizeof(buffer),    // El tamaño máximo de la información a recibir.
      0                  // No usamos banderas
  );

  // Validamos que se haya recibido algo.
  if (resultado_operacion <= 0)
  {
    // Mostramos mensaje de error.
    printf("Error: No se ha podido recibir el listado de archivos desde el servidor.\n");
    printf("Inténtalo de nuevo más tarde.\n\n");

    // Cerramos el socket.
    close(descriptor_socket);

    // Volvemos al menú principal.
    return;
  }

  // Añadimos nuestro EOF al buffer para que sea una cadena válida.
  buffer[resultado_operacion] = '\0';

  // Mostramos el listado de archivos.
  printf("Listado de archivos recibido:\n");
  printf("%s\n", buffer);

  // Informamos al usuario del fin de la operación.
  printf("Fin de listado de archivos.\n\n");

  // Cerramos el socket.
  close(descriptor_socket);
}

/**
 * @brief Función que permite al usuario ingresar la ruta a un directorio/archivo local para subirlo
 * al servidor remoto en una ruta especificada por el usuario.
 */
void subir_archivos(void)
{
  // Variables locales.
  char ruta_origen[TAM_BUFFER],           // Ruta del archivo/carpeta a subir.
      ruta_destino[TAM_BUFFER],           // Ruta del archivo/carpeta destino en el servidor.
      buffer[TAM_BUFFER],                 // Buffer para operaciones de lectura/escritura.
      comando_zip[1024],                  // Comando usado para comprimir el archivo/directorio.
      nombre_archivo_con_extension[1024]; // Nombre dle archivo final con su extensión.
  struct stat informacion_stat;           // Estructura para almacenar la información de un archivo/carpeta.
  int descriptor_socket,                  // El descriptor del socket de conexión.
      resultado_operacion;                // Para almacenar el resultado de alguna operación (conexión, envío y/o recepción de datos, etc).
  long longitud_comprimido,               // La longitud final del archivo comprimido.
      progreso_envio,                     // El progreso del envío.
      datos_leidos;                       // Los datos leídos en una operación.
  struct sockaddr_in direccion_servidor;  // Estructura con la información del servidor.
  FILE *archivo;                          // Archivo comprimido a enviar.

  // Creamos el socket.
  descriptor_socket = socket(
      AF_INET,     // IPv4
      SOCK_STREAM, // Socket de flujo.
      0            // Todos los protocolos
  );

  // Validamos el socket.
  if (descriptor_socket < 0)
  {
    // Mostramos mensaje de error y volvemos al menú principal.
    printf("Error: No se ha podido crear el socket.\n");
    printf("Inténtalo de nuevo más tarde.\n\n");
    return;
  }

  // Pre-llenamos la estructura con ceros.
  memset(&direccion_servidor, 0, sizeof(direccion_servidor));

  // Llenamos la estructura con la información del servidor.
  direccion_servidor.sin_family = AF_INET;                     // IPv4
  direccion_servidor.sin_port = htons(PUERTO_SERVIDOR);        // Puerto a utilizar (en formato de Network Short).
  direccion_servidor.sin_addr.s_addr = inet_addr(IP_SERVIDOR); // La dirección IP del servidor (string) en formato in_addr_t.

  // Realizamos la conexión.
  resultado_operacion = connect(
      descriptor_socket,                      // El socket a utilizaqr.
      (struct sockaddr *)&direccion_servidor, // Cast de la estructura de dirección del servidor.
      sizeof(direccion_servidor)              // Tamaño de la estructura con la información del servidor.
  );

  // Validamos la conexión.
  if (resultado_operacion < 0)
  {
    // Mostramos mensaje de error.
    printf("Error: ¡No se ha podido realizar la conexión con el servidor!\n");
    printf("(¿Está el servidor corriendo en %s:%i?)\n", IP_SERVIDOR, PUERTO_SERVIDOR);
    printf("Inténtalo de nuevo más tarde.\n\n");

    // Cerramos el socket.
    close(descriptor_socket);

    // Volvemos al menú principal.
    return;
  }

  // Creamos la cadena de caracteres con el ID de operación.
  sprintf(buffer, "%i", OPERACION_SUBIR_ARCHIVOS);

  // Enviamos el ID de la operación.
  resultado_operacion = send(
      descriptor_socket, // El descriptor del socket que queremos usar.
      buffer,            // El dato a enviar.
      sizeof(buffer),    // El tamaño del dato que enviaremos.
      0                  // No usamos banderas.
  );

  // Validamos que se haya enviado correctamente la información.
  if (resultado_operacion < 0)
  {
    // Mostramos mensaje de error.
    printf("Error: No se ha podido enviar el ID de la operación al servidor.\n");
    printf("Inténtalo de nuevo más tarde.\n\n");

    // Cerramos el socket.
    close(descriptor_socket);

    // Volvemos al menú principal.
    return;
  }

  // Mostramos información al usuario.
  printf("Ha elegido subir archivo/carpeta al servidor.\n");

  // Obtenemos la ruta del archivo/carpeta a subir.
  printf("Introduce la ruta al archivo/carpeta local que quieras subir al servidor.\n> ");
  scanf("%s", ruta_origen);

  // Obtenemos la ruta del archivo/carpeta destino en el servidor.
  printf("Introduce la ruta al archivo/carpeta remoto dónde quieras guardar la información (si la carpeta no existe, se creará).\n> ");
  scanf("%s", ruta_destino);

  // Mostramos la información capturada.
  printf("\nOperación a realizar:\n");
  printf("- Subiremos el archivo/carpeta: %s\n", ruta_origen);
  printf("- Guardaremos la información remotamente en: %s\n", ruta_destino);

  // Verificamos que la ruta local exista.
  if (lstat(ruta_origen, &informacion_stat) != 0)
  {
    // Mostramos mensaje de error.
    printf("Error: La ruta origen especificada (%s) no existe o el programa no cuenta con persmiso para acceder a ella.\n", ruta_origen);
    printf("Inténtalo de nuevo más tarde.\n\n");

    // Cerramos el socket.
    close(descriptor_socket);

    // Volvemos al menú principal.
    return;
  }

  // Generamos el comando de zip para comprimir el archivo/carpeta previo a enviarlo.
  sprintf(comando_zip, "zip %s -r %s", NOMBRE_ARCHIVO_ZIP_CLIENTE, ruta_origen);

  // Debug para mostrar el comando a ser ejecutado.
  printf("Debug Comando zip: %s\n", comando_zip);

  // Generamos archivo zip con la ruta origen especificada
  resultado_operacion = system(comando_zip);

  // Validamos que se haya creado el zip correctamente.
  if (resultado_operacion != 0)
  {
    // Mostramos mensaje de error.
    printf("Error: Ha ocurrido un error creando el archivo comprimido con la información.\n");
    printf("Inténtalo de nuevo más tarde.\n\n");

    // Cerramos el socket.
    close(descriptor_socket);

    // Regresamos al menú principal.
    return;
  }

  // Generamos el nombre del archivo final con su extensión.
  sprintf(nombre_archivo_con_extension, "%s.zip", NOMBRE_ARCHIVO_ZIP_CLIENTE);

  // Abrimos el archivo comprimido en modo lectura.
  archivo = fopen(nombre_archivo_con_extension, "r");

  // Obtenemos el tamaño del archivo comprimido.
  fseek(archivo, 0L, SEEK_END);         // Posicionamos el cursor al final del archivo.
  longitud_comprimido = ftell(archivo); // Calculamos los bytes (posición actual).
  rewind(archivo);                      // Regresamos el cursor al inicio del archivo.

  // Imprimimos el tamaño del archivo.
  printf("\t- El archivo comprimido a enviar mide: %li bytes.\n", longitud_comprimido);

  // Enviamos el directorio destino al servidor.
  resultado_operacion = send(
      descriptor_socket,    // Descriptor del socket mediante el cual se enviará la información.
      ruta_destino,         // La ruta destino en el servidor.
      strlen(ruta_destino), // La longitud de la ruta destino a enviar.
      0                     // Sin opciones.
  );

  // Verificamos que se haya enviado correctamente.
  if (resultado_operacion <= 0)
  {
    // Mostramos error.
    printf("Error: No se ha podido enviar la ruta destino al servidor.\n");
    printf("Inténtalo de nuevo más tarde.\n\n");

    // Cerramos el socket.
    close(descriptor_socket);

    // Volvemos al menú principal.
    return;
  }

  // Colocamos la longitud del archivo en nuestro buffer de envío.
  sprintf(buffer, "%li", longitud_comprimido);

  // Enviamos la longitud del archivo.
  resultado_operacion = send(
      descriptor_socket, // El descriptor del socket mediatne el cual se enviará la información.
      buffer,            // El buffer que contiene la longitud del archivo comprimido.
      sizeof(buffer),    // El tamaño del buffer a enviar.
      0                  // Sin opciones.
  );

  // Verificamos que se haya enviado la longitud del archivo.
  if (resultado_operacion <= 0)
  {
    // Mostramos mensaje de error.
    printf("Error: No se ha podido enviar la longitud del archivo comprimido al servidor.\n");
    printf("Inténtalo de nuevo más tarde.\n\n");

    // Cerramos el socket.
    close(descriptor_socket);

    // Volvemos al menú principal.
    return;
  }

  // Imprimimos mensaje sobre envío.
  printf("Enviando información:\n");

  // Ciclo para enviar los datos.
  progreso_envio = 0;
  while (progreso_envio < longitud_comprimido)
  {
    // Limpiamos el buffer.
    memset(buffer, 0, sizeof(buffer));

    // Leemos TAM_BUFFER datos del archivo al buffer.
    datos_leidos = fread(
        buffer,         // Dónde guardar los datos.
        1,              // El tamaño en bytes de cada dato (1 byte)
        sizeof(buffer), // El número de datos a leer
        archivo         // El archivo de dónde se leerán los datos.
    );

    // Aumentamos nuestro contador.
    progreso_envio += datos_leidos;

    // Enviamos nuestro buffer al socket.
    resultado_operacion = send(
        descriptor_socket, // El socket mediante el cual enviar los datos.
        buffer,            // El buffer que contiene la información a enviar.
        datos_leidos,      // Enviamos los datos que leímos del archivo.
        0                  // Sin opciones.
    );

    if (resultado_operacion <= 0)
    {
      // Mostramos un mensaje de error.
      printf("Error: Ocurrió un problema enviando el buffer de datos.\n");
      printf("Inténtalo de nuevo más tarde.\n\n");

      // Cerramos el socket.
      close(descriptor_socket);

      // Cerramos el archivo.
      fclose(archivo);

      // Regresamos al menú principal.
      return;
    }

    // Imprimimos información sobre la transmisión.
    printf("\t- Enviados %li, Total %li, Porcentaje %i\%\n", progreso_envio, longitud_comprimido, (int)((progreso_envio * 100) / longitud_comprimido));
  }

  // Eliminamos el archivo zip
  sprintf(comando_zip, "rm -f %s", nombre_archivo_con_extension);
  system(comando_zip);

  // Mensaje final de ejcución.
  printf("Fin de envío de archivos/carpeta.\n\n");
}
