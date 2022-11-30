/**
 * @file servidor.c
 * @author Roldán Morales Ana Karen / Ortega Alcocer Humberto Alejandro
 * @brief Práctica 1 - Servidor de aplicación de carrito de compras
 * @date 2022-11-27
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
#include <sys/stat.h>   // Para funciones de validación de existencia de archivos y carpetas (mkdir(), stat(), etc...)
#include <dirent.h>     // Para operaciones con directorios de archivos y carpetas.

// Bibliotecas desaarrolladas
#include "constantes.h" // Constantes de operación compartidas entre el cliente y el servidor.
#include "cancion.h"    // Definición de la estructura para representar una canción

// Constantes locales.
#define CARPETA_SERVIDOR "./biblioteca_servidor" // La carpeta dónde se encuentran (o deberían de encontrar) los archivos a descargar.

// Función principal.
int main(void) // No se requieren argumentos para el programa principal.
{
  // Variables locales.
  struct addrinfo hints,                  // Estructura con la configuración que buscamos en las opciones de direccionamiento disponibles.
      *informacion_servidor,              // Estructura con la lista ligada de direccionamientos disponibles
      *iterador;                          // Iterador de direccionamientos disponibles.
  struct sockaddr_in informacion_cliente; // Estructura con la información de red del cliente que se ha conectado.
  int descriptor_socket_servidor,         // Descriptor del socket del servidor.
      descriptor_socket_cliente,          // Descriptor del socket del cliente.
      temporal_resultados,                // Variable para almacenar el resultado de alguna operación temporal.
      temporal_reutilizable;              // Variable para almacenar un valor temporal.
  long longitud_archivo,                  // Variable para almacenar la longitud del archivo final.
      datos_recibidos;                    // Variable para almacenar la cantidad de bytes ya recibidos por el programa.
  socklen_t longitud_informacion_cliente; // Variable para almacenar el tamaño de la estructura con los datos del cliente que se conectó.
  char buffer[TAM_BUFFER],                // Búffer para lectura/escritura de información.
      ruta_destino[TAM_BUFFER];           // La ruta destino dónde colocaremos los archivos entrantes.
  FILE *archivo;                          // Variable para nmanipular un archivo particular.
  struct stat informacion_stat;           // Estructura para almacenar la información de un archivo/carpeta.
  struct linger opciones_linger;          // Estructura para almacenar las opciones de Linger.

  // Arreglo con las canciones disponibles en el servidor.
  cancion catalogo_canciones[TAM_MAX_CATALOGO_CANCIONES] = {
      {.nombre = "DIVE!",
       .artista = "Setsuna Yuki",
       .album = "DIVE!",
       .ruta_mp3 = "DIVE!.mp3",
       .anio_album = 2020,
       .precio = 49.50},
      {.nombre = "Error",
       .artista = "Tokoyami Towa",
       .album = "Cover",
       .ruta_mp3 = "ERROR.mp3",
       .anio_album = 2019,
       .precio = 12.50},
      {.nombre = "Gallows Bell",
       .artista = "nano",
       .album = "nanoir",
       .ruta_mp3 = "GALLOWS_BELL.mp3",
       .anio_album = 2012,
       .precio = 12.50},
      {.nombre = "Glow",
       .artista = "nano",
       .album = "nanoir",
       .ruta_mp3 = "Glow.mp3",
       .anio_album = 2012,
       .precio = 12.50},
      {.nombre = "Histery",
       .artista = "nano",
       .album = "nanoir",
       .ruta_mp3 = "Histery.mp3",
       .anio_album = 2012,
       .precio = 12.50},
      {.nombre = "Hontou no Oto",
       .artista = "Hakos Baelz",
       .album = "Cover",
       .ruta_mp3 = "Hontou_no_Oto.mp3",
       .anio_album = 2022,
       .precio = 12.50},
      {.nombre = "King",
       .artista = "Gawr Gura x Calliope Mori",
       .album = "Cover",
       .ruta_mp3 = "KING.mp3",
       .anio_album = 2022,
       .precio = 12.50},
      {.nombre = "Pianoforte Monologue",
       .artista = "Sakurauchi Riko",
       .album = "Pianoforte Monologue",
       .ruta_mp3 = "Pianoforte_Monologue.mp3",
       .anio_album = 2020,
       .precio = 12.50},
      {.nombre = "Snow Halation",
       .artista = "μ's",
       .album = "Snow Halation",
       .ruta_mp3 = "Snow_halation.mp3",
       .anio_album = 2010,
       .precio = 12.50},
      {.nombre = "Strawberry Trapper",
       .artista = "Guilty Kiss",
       .album = "Strawberry Trapper",
       .ruta_mp3 = "Strawberry_Trapper.mp3",
       .anio_album = 2016,
       .precio = 12.50}};
  cancion *cancion_temporal; // Canción temporal para operaciones

  // Información inicial del programa.
  printf("\tPráctica 2 - Carrito de Compras\n");
  printf("Realizado por:\n");
  printf("\t- Roldán Morales Ana Karen\n");
  printf("\t- Ortega Alcocer Humberto Alejandro\n");
  printf("Materia:\n");
  printf("\t- Aplicaciones para comunicaciones en Red.\n");
  printf("\t- 3CM13\n");
  printf("Escuela Superior de Cómputo del Instituto Politécnico Nacional\n\n");
  printf("Inicializando servidor...\n");

  // Comenzamos verificando que exista la carpeta del servidor, o la creamos en caso de que no exista.
  if (mkdir(CARPETA_SERVIDOR, 0777) == 0)
  {
    // Mostramos un aviso sobre la creación de la carpeta principal.
    printf("[Advertencia: La carpeta principal del servidor (%s) no existía pero ha sido creada correctamente.]\n", CARPETA_SERVIDOR);
  }

  // Pre-llenamos la estructura de información del servidor con ceros.
  memset(&hints, 0, sizeof(hints));

  // Llenamos los "hints" de información que buscamos para la conexión.
  hints.ai_family = AF_INET6;      // IPv4 o IPv6
  hints.ai_socktype = SOCK_STREAM; // Socket de flujo.
  hints.ai_flags = AI_PASSIVE;     // Usar mi dirección IP.
  hints.ai_protocol = 0;           // Cualquier protocolo.
  hints.ai_canonname = NULL;       // Sin nombre canónico.
  hints.ai_addr = NULL;            // La dirección será calculada luego.
  hints.ai_next = NULL;            // Iniciamos la lista ligada vacía.

  // Prellenamos la información de nuestra dirección.
  temporal_resultados = getaddrinfo(NULL, PUERTO_SERVIDOR_STR, &hints, &informacion_servidor);

  // Validamos que se haya podido pre-llenar.
  if (temporal_resultados != 0)
  {
    // Mostramos mensaje de error.
    printf("Error: No se ha podido prellenar la información de direccionamiento de red.\n");
    printf("Inténtalo de nuevo más tarde.\n");

    // Finalizamos la ejecución.
    return -1;
  }

  // Colocamos la información de Linger.
  opciones_linger.l_onoff = 1;  // Activamos Linger.
  opciones_linger.l_linger = 5; // Usamos un valor de 5 para el Linger.

  // Iteramos sobre todos los resultados de la lista ligada.
  for (iterador = informacion_servidor; iterador != NULL; iterador = iterador->ai_next)
  {
    printf("\t- Probando dirección...\n");
    // Tratamos de crear el socket.
    descriptor_socket_servidor = socket(
        iterador->ai_family,   // IPv4 o IPv6
        iterador->ai_socktype, // Socket de flujo
        iterador->ai_protocol  // Cualquier protocolo.
    );

    // Validamos que se haya podido crear el socket.
    if (descriptor_socket_servidor == -1)
    {
      // Mostramos mensaje y salimos.
      printf("Info: No se pudo crear socket en esta iteración.\n");

      // Continuamos la ejecución.
      continue;
    }
    else
    {
      printf("\t\t- ✅ Socket creado correctamente.\n");
    }

    // Tratamos de colocar las opciones del socket.

    // Usamos la variable temporal.
    temporal_reutilizable = 1;

    // Activamos la opción SO_REUSEADDR.
    temporal_resultados = setsockopt(
        descriptor_socket_servidor,   // El descriptor del socket a utilizar.
        SOL_SOCKET,                   // La configuración será a nivel socket.
        SO_REUSEADDR,                 // La opción en cuestión: Reutilizar la misma dirección de red.
        &temporal_reutilizable,       // Valor que queremos colocar en la opción: 1, equivalente a "true", es decir, activado [casteamos al tipo requerido].
        sizeof(temporal_reutilizable) // El tamaño del valor anterior, es un entero.
    );

    // Validamos que se hayan colocado las opciones correctamente.
    if (temporal_resultados < 0)
    {
      // Mostramos mensaje.
      printf("Info: No se pudo colocar la opción del socket REUSEADDR en esta iteración.\n");

      // Cerramos el socket.
      close(descriptor_socket_servidor);

      // Continuamos la siguiente iteración.
      continue;
    }
    else
    {
      printf("\t\t- ✅ Opción de socket SO_REUSEADDR configurada correctamente.\n");
    }

    // Activamos la opción SO_KEEPALIVE.
    temporal_resultados = setsockopt(
        descriptor_socket_servidor,   // El descriptor del socket a utilizar.
        SOL_SOCKET,                   // La configuración será a nivel socket.
        SO_KEEPALIVE,                 // La opción en cuestión: Reutilizar la misma dirección de red.
        &temporal_reutilizable,       // Valor que queremos colocar en la opción: 1, equivalente a "true", es decir, activado [casteamos al tipo requerido].
        sizeof(temporal_reutilizable) // El tamaño del valor anterior, es un entero.
    );

    // Validamos que se hayan colocado las opciones correctamente.
    if (temporal_resultados < 0)
    {
      // Mostramos mensaje.
      printf("Info: No se pudo colocar la opción del socket KEEPALIVE en esta iteración.\n");

      // Cerramos el socket.
      close(descriptor_socket_servidor);

      // Continuamos la siguiente iteración.
      continue;
    }
    else
    {
      printf("\t\t- ✅ Opción de socket SO_KEEPALIVE configurada correctamente.\n");
    }

    // Activamos la opción SO_LINGER.
    temporal_resultados = setsockopt(
        descriptor_socket_servidor,     // El descriptor del socket a utilizar.
        SOL_SOCKET,                     // La configuración será a nivel socket.
        SO_LINGER,                      // La opción en cuestión: Reutilizar la misma dirección de red.
        (const char *)&opciones_linger, // Valor que queremos colocar en la opción: 1, equivalente a "true", es decir, activado [casteamos al tipo requerido].
        sizeof(opciones_linger)         // El tamaño del valor anterior, es un entero.
    );

    // Validamos que se hayan colocado las opciones correctamente.
    if (temporal_resultados < 0)
    {
      // Mostramos mensaje.
      printf("Info: No se pudo colocar la opción del socket SO_LINGER en esta iteración.\n");

      // Cerramos el socket.
      close(descriptor_socket_servidor);

      // Continuamos la siguiente iteración.
      continue;
    }
    else
    {
      printf("\t\t- ✅ Opción de socket SO_LINGER configurada correctamente.\n");
    }

    // Realizamos el bind() de nuestro socket con las opciones proporcionadas.
    temporal_resultados = bind(
        descriptor_socket_servidor, // Descriptor del socket a utilizar.
        iterador->ai_addr,          // Usamos la información definida del servidor.
        iterador->ai_addrlen        // El tamaño de la estructura.
    );

    // Verificamos que se haya podido realizar el bind.
    if (temporal_resultados < 0)
    {
      // Mostramos mensaje y salimos.
      printf("Info: No se pudo hacer bind con el socket en esta iteración.\n");

      // Cerramos el socket.
      close(descriptor_socket_servidor);

      // Continuamos la iteración.
      continue;
    }
    else
    {
      printf("\t\t- ✅ Bind realizado correctamente.\n");
    }

    // Si llegamos a este punto, se encontró una dirección válida, así que rompemos el ciclo.
    break;
  }

  // Liberamos la memoria de nuestra información del servidor.
  freeaddrinfo(informacion_servidor);

  // Validamos que hayamos encontrado una dirección de red válida.
  if (iterador == NULL)
  {
    // Mostramos mensaje de error.
    printf("Error: No se encontró ninguna dirección de red válida.\n");
    printf("Inténtalo de nuevo más tarde.\n");

    // Finalizamos la ejecución.
    return -1;
  }

  // Comenzamos a escuchar en nuestro socket por información entrante (intentos de conexión entrantes, más específicamente).
  temporal_resultados = listen(
      descriptor_socket_servidor, // El descriptor del socket que queremos escuchar.
      10                          // Nuestro "backlog", o número de conexiones pendientes a encolar. Realmente aquí podríamos usar 1, pero por si las moscas.
  );

  // Validamos que se puedan escuchar conexiones entrantes.
  if (temporal_resultados < 0)
  {
    // Mostramos mensaje y salimos.
    printf("Error: No se ha podido iniciar a escuchar conexiones entrantes.\n");
    printf("Inténtalo de nuevo más tarde.\n\n");
    return -1;
  }

  // Fin de inicialización de programa.
  printf("¡Servidor iniciado correctamente!\n");
  printf("\t- IP del servidor: %s\n", IP_SERVIDOR);
  printf("\t- Puerto del servidor: %i\n", PUERTO_SERVIDOR);

  // Ajustamos el tamaño de nuestra estructura de información del cliente.
  longitud_informacion_cliente = sizeof(informacion_cliente);

  // Ciclo principal de ejecución del servidor.
  while (1)
  {
    // Información de espera.
    printf("Esperando conexiones...\n");

    // Aceptamos la conexión entrante del cliente.
    descriptor_socket_cliente = accept(
        descriptor_socket_servidor,              // Descriptor del socket del servidor dónde esperamos la conexión.
        (struct sockaddr *)&informacion_cliente, // Estructura dónde guardaremos la información del cliente que se ha conectado.
        &longitud_informacion_cliente            // El tamaño de la estructura dónde almacenaremos la información del cliente.
    );

    // Validamos que se haya aceptado correctamente la conexión.
    if (descriptor_socket_cliente < 0)
    {
      // Mostramos mensaje de error y esperamos la siguiente conexión.
      printf("[Advertencia: Un cliente trató de conectarse pero ocurrió un errror. Ignorando intento de conexión.]\n");
      continue;
    }

    // Mostramos la información del cliente que se acaba de conectar.
    printf("¡Conexión aceptada exitosamente! Datos:\n");
    printf("\t- IP: %s\n", inet_ntoa(informacion_cliente.sin_addr));
    printf("\t- Puerto: %i\n", ntohs(informacion_cliente.sin_port));

    // Mostramos mensaje de espera de ID de operación.
    printf("Esperando ID de operación...\n");

    // Leer la operación a realizar.
    temporal_resultados = recv(
        descriptor_socket_cliente, // El descriptor del socket desde dónde queremos leer información.
        buffer,                    // El buffer dónde almacenar el mensaje entrante.
        sizeof(buffer),            // El tamaño máximo del buffer a leer.
        0                          // Ninguna bandera de configuración.
    );

    // Validamos que hayamos recibido algo.
    if (temporal_resultados < 0)
    {
      // Mostramos mensaje de error.
      printf("Error: No se recibió un ID válido de operación.\n");

      // Cerramos el socket.
      close(descriptor_socket_cliente);

      // Saltamos el resto del ciclo y volvemos a esperar una conexión entrante.
      continue;
    }

    // Añadimos nuestro EOF al buffer para que sea una cadena válida.
    buffer[temporal_resultados] = '\0';

    // Convertimos la cadena a int.
    temporal_reutilizable = atoi(buffer);

    // Imprimimos mensaje de información de paquete recibido.
    printf("Se recibió la operación #%i.\n", temporal_reutilizable);

    // Actuamos acorde a la operación indicada.
    switch (temporal_reutilizable)
    {
    case OPERACION_LISTAR_CATALOGO:
      // Mensaje de confirmación de operación en servidor.
      printf("\t- Se ha recibido la instrucción para listar catálogo.\n");

      // Mensaje de confirmación para enviar el catálogo de canciones.
      printf("\t- Enviando catálogo de canciones (%lu bytes)...\n", sizeof(catalogo_canciones));

      // Enviamos el catálogo de canciones.
      temporal_resultados = send(
          descriptor_socket_cliente,        // Descriptor del socket mediante el cual se enviará la información.
          (const char *)catalogo_canciones, // Cadena con el listado de archivos.
          sizeof(catalogo_canciones),       // Longitud de la cadena resultante.
          0                                 // No usamos banderas.
      );

      // Verificamos que se haya enviado correctamente.
      if (temporal_resultados <= 0)
      {
        // Mostramos mensaje de error.
        printf("Error: No ha sido posible enviar el catálogo de canciones correctamente.\n");
        printf("Inténtalo de nuevo más tarde.\n\n");
      }
      else
      {
        printf("\t- Catálogo de canciones enviado correctamente.\n");
      }

      // Fin de operación.
      break;

    case OPERACION_DESCARGAR_CANCION:
      printf("\t- Se ha recibido la instrucción para descargar canción.\n");
      printf("\t- Esperando estructura de canción a descargar...\n");

      // Recibimos la estructura con la canción a descargar.
      temporal_resultados = recv(
          descriptor_socket_cliente, // Descriptor del socket mediante el cual recibiremos la información
          buffer,                    // Guardaremos aquí la ruta destino.
          sizeof(buffer),            // El tamaño de la variable para la ruta destino.
          0                          // Sin opciones.
      );

      // Validamos que hayamos recibido algo.
      if (temporal_resultados <= 0)
      {
        // Mensaje de error.
        printf("Error: No se ha recibido la canción solicitada por el cliente correctamente.\n");
        printf("Inténtalo de nuevo más tarde.\n\n");

        // Salimos de la operación.
        break;
      }

      // Cargamos el búffer en la estructura de la canción.
      cancion_temporal = (cancion *)buffer;

      // Imprimimos la información.
      printf("\t- Canción recibida:\n");
      printf("\t\t- Nombre: %s\n", cancion_temporal->nombre);
      printf("\t\t- Artista: %s\n", cancion_temporal->artista);
      printf("\t\t- Álbum: %s\n", cancion_temporal->album);
      printf("\t\t- Ruta MP3: %s\n", cancion_temporal->ruta_mp3);
      printf("\t\t- Año Álbum: %i\n", cancion_temporal->anio_album);
      printf("\t\t- Precio: %.2f\n", cancion_temporal->precio);

      // Generamos la ruta usando la carpeta del servidor.
      sprintf(ruta_destino, "%s/%s", CARPETA_SERVIDOR, cancion_temporal->ruta_mp3);

      // Mostramos la ruta final.
      printf("\t- Ruta final MP3: %s\n", ruta_destino);

      // Validamos que la ruta exista.
      if (lstat(ruta_destino, &informacion_stat) != 0)
      {
        // Mostramos mensaje de error.
        printf("Error: La ruta origen especificada (%s) no existe o el programa no cuenta con persmiso para acceder a ella.\n", ruta_destino);
        printf("Inténtalo de nuevo más tarde.\n\n");

        // Finalizamos la operación.
        break;
      }

      // Abrimos el archivo .zip en modo de solo lectura.
      archivo = fopen(ruta_destino, "r");

      // Validamos que se haya abierto el archivo.
      if (archivo == NULL)
      {
        // Mostramos mensaje de error.
        printf("Error: El archivo %s no pude ser abierto por el programa.\n", ruta_destino);
        printf("Inténtalo de nuevo más tarde.\n");

        // Finalizamos la operación.
        break;
      }

      // Obtenemos el tamaño del archivo comprimido.
      fseek(archivo, 0L, SEEK_END);      // Posicionamos el cursor al final del archivo.
      longitud_archivo = ftell(archivo); // Calculamos los bytes (posición actual del cursor).
      rewind(archivo);                   // Regresamos el cursor al inicio del archivo.

      // Imprimimos el tamaño del archivo.
      printf("\t- El archivo MP3 a enviar mide: %li bytes.\n", longitud_archivo);
      printf("\t- Enviando longitud de archivo...\n");

      // Colocamos en el buffer la longitud del archivo.
      sprintf(buffer, "%li", longitud_archivo);

      // Enviamos la longitud del archivo.
      temporal_resultados = send(
          descriptor_socket_cliente, // El descriptor del socket mediatne el cual se enviará la información.
          buffer,                    // El buffer que contiene la longitud del archivo comprimido.
          sizeof(buffer),            // El tamaño del buffer a enviar.
          0                          // Sin opciones.
      );

      // Verificamos que se haya enviado la longitud del archivo.
      if (temporal_resultados <= 0)
      {
        // Mostramos mensaje de error.
        printf("Error: No se ha podido enviar la longitud del archivo MP3 al cliente.\n");
        printf("Inténtalo de nuevo más tarde.\n\n");

        // Cerramos el archivo.
        fclose(archivo);

        // Salimos de operación.
        break;
      }

      // Imprimimos mensaje sobre inicio de transmisión de inforación.
      printf("\t- Longitud de archivo enviada correctamente.\n");
      printf("\t- Enviando información, progreso:\n");

      // Ciclo para enviar los datos.
      datos_recibidos = 0; // En realidad son los datos enviados.
      while (datos_recibidos < longitud_archivo)
      {
        // Limpiamos el buffer.
        memset(buffer, 0, sizeof(buffer));

        // Leemos TAM_BUFFER datos del archivo al buffer.
        temporal_resultados = fread(
            buffer,     // Dónde guardar los datos leídos.
            1,          // Cuánto mide cada dato.
            TAM_BUFFER, // El número de datos a leer (máximo)
            archivo     // El origen de los datos.
        );

        // Aumentamos nuestro contador.
        datos_recibidos += temporal_resultados;

        // Enviamos nuestro buffer al socket.
        temporal_resultados = send(
            descriptor_socket_cliente, // El socket a través del cual enviaremos los datos.
            buffer,                    // El buffer con los datos.
            temporal_resultados,       // El tamaño de datos a enviar
            0                          // Sin opciones.
        );

        // Validamos que hayamos enviado los datos correctamente.
        if (temporal_resultados <= 0)
        {
          // Mostramos un mensaje de error.
          printf("Error: Ocurrió un problema enviando el buffer de datos.\n");
          printf("Inténtalo de nuevo más tarde.\n\n");

          // Cerramos el archivo.
          fclose(archivo);

          // Fin de operación.
          break;
        }

        // Imprimimos información sobre la transmisión.
        printf("\t\t- Enviados %li, Total %li, Porcentaje %i%c\n", datos_recibidos, longitud_archivo, (int)((datos_recibidos * 100) / longitud_archivo), '%');
      }

      // Mostramos información al usuario.
      printf("\t- Ha finalizado el envío de información.\n");

      // Fin de operación.
      break;

    default:
      printf("Error: La operación recibida (%i) no es válida.\n", temporal_reutilizable);
      break;
    }

    // Informamos al usuario del cierre del socket.
    printf("Cerrando conexión para: %s:%i\n", inet_ntoa(informacion_cliente.sin_addr), ntohs(informacion_cliente.sin_port));

    // Cerramos el socket cliente.
    close(descriptor_socket_cliente);
  }

  // Este mensaje _nunca_ debería mostrarse.
  printf("El servidor se cerró.\n");

  // Fin de ejecución.
  return 0;
}
