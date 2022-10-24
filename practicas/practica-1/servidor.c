/**
 * @file servidor.c
 * @author Roldán Morales Ana Karen / Ortega Alcocer Humberto Alejandro
 * @brief Práctica 1 - Servidor de aplicación de envío/recepción de archivos.
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
#include <sys/stat.h>   // Para funciones de validación de existencia de archivos y carpetas (mkdir(), stat(), etc...)
#include <dirent.h>     // Para operaciones con directorios de archivos y carpetas.

// Incluímos cabecera con constantes compartidas entre el cliente y servidor.
#include "constantes.h"

// Constantes locales.
#define CARPETA_SERVIDOR "./archivos-servidor"

// Prototipos de funciones.
void listado_archivos_recursivo(const char *, int, char *);

// Función principal.
int main(void) // No se requieren argumentos para el programa principal.
{
  // Variables locales.
  struct sockaddr_in informacion_servidor,      // Estructura con la información de red del servidor.
      informacion_cliente;                      // Estructura con la información de red del cliente que se ha conectado.
  int descriptor_socket_servidor,               // Descriptor del socket del servidor.
      descriptor_socket_cliente,                // Descriptor del socket del cliente.
      temporal_resultados,                      // Variable para almacenar el resultado de alguna operación temporal.
      temporal_reutilizable;                    // Variable para almacenar un valor temporal.
  long longitud_archivo,                        // Variable para almacenar la longitud del archivo final.
      datos_recibidos;                          // Variable para almacenar la cantidad de bytes ya recibidos por el programa.
  socklen_t longitud_informacion_cliente;       // Variable para almacenar el tamaño de la estructura con los datos del cliente que se conectó.
  char buffer[TAM_BUFFER],                      // Búffer para lectura/escritura de información.
      ruta_destino[TAM_BUFFER],                 // La ruta destino dónde colocaremos los archivos entrantes.
      nombre_archivo_con_extension[TAM_BUFFER], // El nombre del archivo con extensión adecuada.
      comando_final[TAM_BUFFER];                // El comando final a ser usado para descomprimir/comprimir la información.
  FILE *archivo;                                // Variable para nmanipular un archivo particular.
  struct stat informacion_stat;                 // Estructura para almacenar la información de un archivo/carpeta.

  // Información inicial del programa.
  printf("\tPráctica 1 - Envío/Recepción de archivos - Servidor\n\n");
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
  printf("Inicializando servidor...\n");

  // Comenzamos verificando que exista la carpeta del servidor, o la creamos en caso de que no exista.
  if (mkdir(CARPETA_SERVIDOR, 0777) == 0)
  {
    // Mostramos un aviso sobre la creación de la carpeta principal.
    printf("[Advertencia: La carpeta principal del servidor (%s) no existía pero ha sido creada correctamente.]\n", CARPETA_SERVIDOR);
  }

  // Pre-llenamos la estructura de información del servidor con ceros.
  memset(&informacion_servidor, 0, sizeof(informacion_servidor));

  // Llenamos la estructura con la información del servidor.
  informacion_servidor.sin_family = AF_INET;                // Permitimos IPv4.
  informacion_servidor.sin_port = htons(PUERTO_SERVIDOR);   // Establecemos el puerto de escucha.
  informacion_servidor.sin_addr.s_addr = htonl(INADDR_ANY); // Escuchamos cualquier IP entrante.

  // Creamos el socket.
  descriptor_socket_servidor = socket(
      AF_INET,     // IPv4
      SOCK_STREAM, // Socket de flujo
      0            // Cualquier protocolo.
  );

  // Validamos que se haya podido crear el socket.
  if (descriptor_socket_servidor == -1)
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
      descriptor_socket_servidor,   // El descriptor del socket a utilizar.
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
      descriptor_socket_servidor,               // Descriptor del socket a utilizar.
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
      descriptor_socket_servidor, // El descriptor del socket que queremos escuchar.
      3                           // Nuestro "backlog", o número de conexiones pendientes a encolar. Realmente aquí podríamos usar 1, pero por si las moscas.
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
    if (temporal_resultados <= 0)
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

    // Actuamos acorde a la operación indicada.
    switch (temporal_reutilizable)
    {
    case OPERACION_LISTAR_ARCHIVOS:
      // Mensaje de confirmación de operación en servidor.
      printf("Se ha recibido la instrucción para listar archivos.\n");

      // Limpiamos el buffer.
      buffer[0] = '\0';

      // Función para generar el listado.
      listado_archivos_recursivo(CARPETA_SERVIDOR, 0, buffer);

      // Enviamos el listado obtenido.
      temporal_resultados = send(
          descriptor_socket_cliente, // Descriptor del socket mediante el cual se enviará la información.
          buffer,                    // Cadena con el listado de archivos.
          strlen(buffer),            // Longitud de la cadena resultante.
          0                          // No usamos banderas.
      );

      // Verificamos que se haya enviado correctamente.
      if (temporal_resultados <= 0)
      {
        // Mostramos mensaje de error.
        printf("Error: No ha sido posible enviar el listado de archivos correctamente.\n");
        printf("Inténtalo de nuevo más tarde.\n\n");
      }
      else
      {
        printf("Listado de archivos enviado correctamente.\n");
      }

      // Fin de operación.
      break;

    case OPERACION_DESCARGAR_ARCHIVOS:
      printf("Se ha recibido la instrucción para descargar archivos.\n");

      // Recibimos la ruta destino del archivo.
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
        printf("Error: No se ha recibido una ruta destino desde el cliente.\n");
        printf("Inténtalo de nuevo más tarde.\n\n");

        // Salimos de la operación.
        break;
      }

      // Añadimos nuestro EOF a la cadena.
      buffer[temporal_resultados] = '\0';

      // Imprimimos la información.
      printf("\t- La ruta del archivo/carpeta a comprimir y desacargar es: %s\n", buffer);

      // Generamos la ruta usando la carpeta del servidor.
      sprintf(ruta_destino, "%s/%s", CARPETA_SERVIDOR, buffer);

      // Validamos que la ruta exista.
      if (lstat(ruta_destino, &informacion_stat) != 0)
      {
        // Mostramos mensaje de error.
        printf("Error: La ruta origen especificada (%s) no existe o el programa no cuenta con persmiso para acceder a ella.\n", ruta_destino);
        printf("Inténtalo de nuevo más tarde.\n\n");

        // Finalizamos la operación.
        break;
      }

      // Creamos el comando zip.
      sprintf(comando_final, "zip %s -r %s", NOMBRE_ARCHIVO_ZIP_SERVIDOR, ruta_destino);

      // Debug para mostrar el comando a ser ejecutado.
      printf("Debug comando zip: %s\n", comando_final);

      // Generamos el archivo zip con la ruta especificada.
      temporal_resultados = system(comando_final);

      // Validamos que se haya creado el archivo zip.
      if (temporal_resultados != 0)
      {
        // Mostramos mensaje de error.
        printf("Error: Ha ocurrido un problema generando el archivo comprimido con los datos a enviar al cliente.\n");
        printf("Inténtalo de nuevo más tarde.\n\n");

        // Salimos de la operación.
        break;
      }

      // Generamos el nombre del archivo con extensión.
      sprintf(nombre_archivo_con_extension, "%s.zip", NOMBRE_ARCHIVO_ZIP_SERVIDOR);

      // Abrimos el archivo .zip en modo de solo lectura.
      archivo = fopen(nombre_archivo_con_extension, "r");

      // Obtenemos el tamaño del archivo comprimido.
      fseek(archivo, 0L, SEEK_END);      // Posicionamos el cursor al final del archivo.
      longitud_archivo = ftell(archivo); // Calculamos los bytes (posición actual del cursor).
      rewind(archivo);                   // Regresamos el cursor al inicio del archivo.

      // Imprimimos el tamaño del archivo.
      printf("\t- El archivo comprimido a enviar mide: %li bytes.\n", longitud_archivo);

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
        printf("Error: No se ha podido enviar la longitud del archivo comprimido al cliente.\n");
        printf("Inténtalo de nuevo más tarde.\n\n");

        // Salimos de operación.
        break;
      }

      // Imprimimos mensaje sobre inicio de transmisión de inforación.
      printf("Enviando información:\n");

      // Ciclo para enviar los datos.
      datos_recibidos = 0; // En realidad son los datos enviados.
      while (datos_recibidos < longitud_archivo)
      {
        // Limpiamos el buffer.
        memset(buffer, 0, sizeof(buffer));

        // Leemos TAM_BUFFER datos del archivo al buffer.
        temporal_resultados = fread(
            buffer,         // Dónde guardar los datos leídos.
            1,              // Cuánto mide cada dato.
            sizeof(buffer), // El número de datos a leer (máximo)
            archivo         // El origen de los datos.
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

          // Fin de operación.
          break;
        }

        // Imprimimos información sobre la transmisión.
        printf("\t- Enviados %li, Total %li, Porcentaje %i%c\n", datos_recibidos, longitud_archivo, (int)((datos_recibidos * 100) / longitud_archivo), '%');
      }

      // Eliminamos el archivo zip
      sprintf(comando_final, "rm -f %s", nombre_archivo_con_extension);
      system(comando_final);

      // Mostramos información al usuario.
      printf("Ha finalizado el envío de información.\n");

      // Fin de operación.
      break;

    case OPERACION_SUBIR_ARCHIVOS:
      // Mensaje de inicio de operación.
      printf("Se ha recibido la instrucción para subir archivos.\n");

      // Recibimos la ruta destino del archivo.
      temporal_resultados = recv(
          descriptor_socket_cliente, // Descriptor del socket mediante el cual recibiremos la información
          ruta_destino,              // Guardaremos aquí la ruta destino.
          sizeof(ruta_destino),      // El tamaño de la variable para la ruta destino.
          0                          // Sin opciones.
      );

      // Validamos que hayamos recibido algo.
      if (temporal_resultados <= 0)
      {
        // Mensaje de error.
        printf("Error: No se ha recibido una ruta destino desde el cliente.\n");
        printf("Inténtalo de nuevo más tarde.\n\n");

        // Salimos de la operación.
        break;
      }

      // Añadimos nuestro EOF a la ruta obtenida.
      ruta_destino[temporal_resultados] = '\0';

      // Imprimimos la ruta dónde guardaremos el contenido del archivo comprimido a recibir.
      printf("\t- La ruta destino recibida es: %s.\n", ruta_destino);

      // Recibimos la longitud del archivo
      temporal_resultados = recv(
          descriptor_socket_cliente, // El descriptor del socket dónde recibiremos la información.
          buffer,                    // El búffer dónde almacenramos los datos recibidos.
          sizeof(buffer),            // La longitud máxima del buffer.
          0                          // Sin opciones
      );

      // Validamos que hayamos recibido algo.
      if (temporal_resultados <= 0)
      {
        // Mensaje de error.
        printf("Error: No se ha recibido una longitud de archivo válida desde el cliente.\n");
        printf("Inténtalo de nuevo más tarde.\n\n");

        // Salimos de la operación.
        break;
      }

      // Agregamos nuestro EOF a la cadena obtenida.
      buffer[temporal_resultados] = '\0';

      // Convertimos nuestra cadena en un entero.
      longitud_archivo = atoi(buffer);

      // Mostramos información.
      printf("\t- Se recibirán %li bytes desde el cliente.\n", longitud_archivo);

      // Generamos el nombre del archivo destino con su extensión.
      sprintf(nombre_archivo_con_extension, "%s.zip", NOMBRE_ARCHIVO_ZIP_SERVIDOR);

      // Creamos un archivo vacío en modo escritura.
      archivo = fopen(nombre_archivo_con_extension, "w");

      // Ciclo para recibir datos.
      datos_recibidos = 0;
      while (datos_recibidos < longitud_archivo)
      {
        // Recibir datos.
        temporal_resultados = recv(
            descriptor_socket_cliente, // El socket por el cual recibiremos los datos.
            buffer,                    // El buffer para almacenar los datos.
            sizeof(buffer),            // El tamaño máximo de datos a recibir
            0                          // Sin opciones.
        );

        // Validamos que se hayan recibido datos.
        if (temporal_resultados <= 0)
        {
          // Mensaje de error.
          printf("Error: No se ha recibido ningún dato del cliente en envío de archivo.\n");
          printf("Inténtalo de nuevo más tarde.\n\n");

          // Cerramos el archivo.
          fclose(archivo);

          // Salimos del ciclo.
          break;
        }

        // Escribimos los datos en el archivo.
        datos_recibidos += fwrite(
            buffer,              // De dónde leeremos los datos a escribir
            1,                   // Cuántos bytes mide cada ítem.
            temporal_resultados, // El número de ítems
            archivo              // El archivo dónde escribir los datos.
        );

        // Print de debug.
        printf("\t- Recibidos %li, Total %li, Porcentaje %i%c\n", datos_recibidos, longitud_archivo, (int)((datos_recibidos * 100) / longitud_archivo), '%');
      }

      // Guardamos el último proceso de escritura en el archivo.
      fflush(archivo);

      // Cerramos el archivo.
      fclose(archivo);

      // Print de información.
      printf("\t- Fin de recepción de datos.\n");

      // Verificamos si la ruta existe.

      // Generamos el comando final de unzip.
      sprintf(comando_final, "unzip %s -d %s/%s", nombre_archivo_con_extension, CARPETA_SERVIDOR, ruta_destino);

      // Impresión del comando final para debug.
      printf("Debug comando unzip: %s\n", comando_final);

      // Ejecutamos comando de descompresión unzip.
      temporal_resultados = system(comando_final);

      // Validamos que se haya descomprimido correctamente.
      if (temporal_resultados != 0)
      {
        // Mostramos mensaje de error.
        printf("Error: No se ha podido ejecutar el comando de descompresión correctamente.\n");
        printf("Inténtalo de nuevo más tarde.\n\n");
      }

      // Eliminamos el archivo.
      sprintf(comando_final, "rm -f %s", nombre_archivo_con_extension);
      system(comando_final);

      // Fin de operación.
      break;

    default:
      printf("Error: La operación recibida (%i) no es válida.\n", temporal_reutilizable);
      break;
    }

    // Informamos al usuario del cierre del socket.
    printf("Cerrando conexión para:\n");
    printf("\t- IP: %s\n", inet_ntoa(informacion_cliente.sin_addr));
    printf("\t- Puerto: %i\n", ntohs(informacion_cliente.sin_port));

    // Cerrramos el socket.
    close(descriptor_socket_cliente);
  }

  // Fin de ejecución.
  return 0;
}

// Implementación de funciones.

/**
 * @brief Función que recorre recursivamente la carpeta de archivos del servidor y guarda la estructura
 *        en una cadena resultante.
 *
 * @param nombre Nombre del directorio/ruta inicial.
 * @param nivel_indentacion  El nivel de indentación para impresión de información.
 * @param cadena_resultante Un apuntador a la cadena resultante dónde guardaremos la información final.
 */
void listado_archivos_recursivo(const char *nombre, int nivel_indentacion, char *cadena_resultante)
{
  // Variables locales.
  DIR *directorio;                // Variable para almacenar el directorio actual.
  struct dirent *elemento_actual; // Estructura con la información del elemento actual en el directorio.
  // char ruta[1024];                // Tamaño máximo de la ruta (nombre del directorio/archivo)

  // Validamos que el nombre sea un directorio.
  if (!(directorio = opendir(nombre)))
  {
    // Si no es un directorio, salimos, caso base.
    return;
  }

  // Iteramos recursivamente sobre cada elemento del directorio.
  while ((elemento_actual = readdir(directorio)) != NULL)
  {
    // Verificamos si el tipo de elemento actual es un directorio.
    if (elemento_actual->d_type == DT_DIR)
    {
      char ruta[1024]; // Variable para almacenar la ruta actual compuesta para cada elemento del directorio.

      // Verificamos si nos encontramos en los elementos de relación con el directorio actual (.) o el inmediato superior (..)
      if (strcmp(elemento_actual->d_name, ".") == 0 || strcmp(elemento_actual->d_name, "..") == 0)
      {
        // En caso de que sea, continuamos y así omitimos mostrarlos.
        continue;
      }
      // Imprimimos dentro de la variable ruta con el nombre completo.
      snprintf(ruta, sizeof(ruta), "%s/%s", nombre, elemento_actual->d_name);

      // Imprimimos dentro de nuestra variable final de información.
      sprintf(cadena_resultante + strlen(cadena_resultante), "%*s[%s]\n", nivel_indentacion, "", elemento_actual->d_name);

      // Paso recursivo.
      listado_archivos_recursivo(ruta, nivel_indentacion + 2, cadena_resultante);
    }
    else
    {
      // Imprimimos el elemento actual en nuestra cadena final.
      sprintf(cadena_resultante + strlen(cadena_resultante), "%*s- %s\n", nivel_indentacion, "", elemento_actual->d_name);
    }
  }

  // Cerramos el directorio actual.
  closedir(directorio);
}
