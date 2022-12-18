/**
 * @file servidor.c
 * @author Roldán Morales Ana Karen / Ortega Alcocer Humberto Alejandro
 * @brief Práctica 4 - Servidor HTTP
 * @date 2022-11-27
 * @copyright Copyright (c) 2022
 */
// Librerías personalizadas.
#include "http.h" // Definición de estructuras y funciones HTTP.

// Librerías estándar.
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h> // Para funciones de sockets.
#include <sys/types.h>  // Estructuras y tipos de datos utilizados en varias operaciones de sockets y demás.
#include <arpa/inet.h>  // Funcionalidades para manipulación de información de red.
#include <netinet/in.h> // Funcionalidades para operaciones con direccionamiento de red
#include <unistd.h>     // Para operaciones con distintas llamadas al sistema operativoipo
#include <pthread.h>    // Para manipulación y operaciones con hilos.
#include <string.h>     // Para manipulación de cadenas de caracteres.
#include <netdb.h>      // Definición de funciones para traducción entre tipos de datos para redes.

// Constantes locales.
#define PUERTO_SERVIDOR "8080" // El puerto dónde escuchará nuestro servidor.
#define TAM_BUFFER 65535       // El tamaño máximo del búffer para envío/recepción de datos.

/**
 * @brief Función principal del Servidor.
 *
 * @return int Estado de ejecución.
 */
int main()
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
  socklen_t longitud_informacion_cliente; // Variable para almacenar el tamaño de la estructura con los datos del cliente que se conectó.
  struct linger opciones_linger;          // Estructura para almacenar las opciones de Linger.
  char buffer[TAM_BUFFER];                // El buffer para almacenar los datos entrantes.
  solicitud_http solicitud;               // Estructura con la información de la solicitud http.
  respuesta_http respuesta;               // Estructura para la respuesta del servidor HTTP.

  // Mostramos mensaje de inicio de programa.
  printf("\tPráctica 4 - Servidor HTTP (básico)\n");
  printf("Realizado por:\n");
  printf("\t- Roldán Morales Ana Karen\n");
  printf("\t- Ortega Alcocer Humberto Alejandro\n");
  printf("Materia:\n");
  printf("\t- Aplicaciones para comunicaciones en Red.\n");
  printf("\t- 3CM13\n");
  printf("Escuela Superior de Cómputo del Instituto Politécnico Nacional\n\n");
  printf("Inicializando servidor...\n");

  // Pre-llenamos la estructura de opciones de configuración con ceros.
  memset(&hints, 0, sizeof(hints));

  // Llenamos la configuración que buscamos de direccionamiento.
  hints.ai_family = AF_INET6;      // IPv4 o IPv6.
  hints.ai_socktype = SOCK_STREAM; // Sockets de flujo.
  hints.ai_flags = AI_PASSIVE;     // Usar mi dirección IP.
  hints.ai_protocol = 0;           // Cualquier protocolo.
  hints.ai_canonname = NULL;       // Sin nombre canónico.
  hints.ai_addr = NULL;            // Será calculado en la lista ligada.
  hints.ai_next = NULL;            // Será calculado en la lista ligada.

  // Inicializamos nuestra lista ligada.
  temporal_resultados = getaddrinfo(NULL, PUERTO_SERVIDOR, &hints, &informacion_servidor);

  // Validamos que se haya podido inicializar.
  if (temporal_resultados != 0)
  {
    // Mostramos mensaje de error.
    printf("Error: No se ha podido prellenar la información de direccionamiento de red.\n");
    printf("Inténtalo de nuevo más tarde.\n\n");

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

    // Cerramos el socket.
    close(descriptor_socket_servidor);

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
    // Mostramos mensaje de error.
    printf("Error: No se ha podido iniciar a escuchar conexiones entrantes.\n");
    printf("Inténtalo de nuevo más tarde.\n\n");

    // Cerramos el socket.
    close(descriptor_socket_servidor);

    // Finalizamos la ejecución.
    return -1;
  }

  // Fin de inicialización de programa.
  printf("¡Servidor iniciado correctamente!\n");
  printf("\t- IP del servidor: 127.0.0.1\n");
  printf("\t- Puerto del servidor: %s\n", PUERTO_SERVIDOR);
  printf("- Esperando conexiones...\n");

  // Ajustamos el tamaño de nuestra estructura de información del cliente.
  longitud_informacion_cliente = sizeof(informacion_cliente);

  // Ciclo infinito principal de ejecución del servidor.
  while (1)
  {
    // Aceptamos la conexión entrante del cliente.
    descriptor_socket_cliente = accept(
        descriptor_socket_servidor,              // El socket dónde estamos escuchando.
        (struct sockaddr *)&informacion_cliente, // Dónde guardaremos la información del cliente.
        &longitud_informacion_cliente            // El tamaño de la información del cliente.
    );

    // Validamos que se haya aceptado correctamente la conexión.
    if (descriptor_socket_cliente < 0)
    {
      // Mostramos mensaje de error y esperamos la siguiente conexión.
      printf("[Advertencia: Un cliente trató de conectarse pero ocurrió un errror. Ignorando intento de conexión.]\n");

      // Continuamos la ejecución del ciclo.
      continue;
    }

    // Imprimimos la información del cliente conectado.
    printf("- ✅ Conexión aceptada exitosamente.\n");
    printf("\t- IP: %s\n", inet_ntoa(informacion_cliente.sin_addr));
    printf("\t- Puerto: %i\n", ntohs(informacion_cliente.sin_port));
    printf("\t- Esperando solicitud HTTP...\n");

    // Recibimos la solicitud HTTP en crudo.
    temporal_resultados = recv(
        descriptor_socket_cliente, // El socket dónde escucharemos la solicitud.
        buffer,                    // El búffer dónde almacenaremos el resultado.
        TAM_BUFFER,                // El tamaño máximo de bytes a leer.
        0                          // Sin opciones.
    );

    // Validamos que hayamos recibido algo.
    if (temporal_resultados > 0)
    {
      // Mostramos un mensaje de recepción exitosa.
      printf("\t- Solicitud HTTP recibida correctamente (%i bytes).\n", temporal_resultados);
      printf("\t- Procesando solicitud...\n");

      // Colocamos nuestro EOF al final del contenido.
      buffer[temporal_resultados] = '\0';

      // Convertimos nuestra solicitud en una estructura.
      temporal_resultados = procesar_solicitud(buffer, &solicitud);

      // Mostramos la solicitud recibida.
      imprimir_solicitud(&solicitud);

      // Mostramos mensaje de fin de procesamiento.
      printf("\t- Fin de procesamiento de solicitud.\n");

      // Mostramos mensaje de envío de respuesta.
      printf("\t- Enviando respuesta HTTP...\n");

      // Aquí definimos nuestra lógica para cada Método + URI.
      if (strcmp(solicitud.metodo, "GET") == 0)
      {
        // Para la URI: /saludo
        if (strcmp(solicitud.uri, "/saludo") == 0)
        {
          respuesta.codigo = 200;
          strncpy(respuesta.cuerpo, "<html><body><h1>¡Hola Profe!</h1><p>Esta es la ruta para saludar gente.</p></body></html>", TAM_CUERPO);
        }
        else
        {
          // La URI no se encuentra registrada.
          respuesta.codigo = 404;
          strncpy(respuesta.cuerpo, "<html><body><h1>Esta ruta no existe :(</h1></body></html>", TAM_CUERPO);
        }
      }
      else if (strcmp(solicitud.metodo, "POST") == 0)
      {
        // Para la URI: /usuarios
        if (strcmp(solicitud.uri, "/usuarios") == 0)
        {
          respuesta.codigo = 200;
          strncpy(respuesta.cuerpo, "<html><body><h1>Usuario creado exitosamente</h1><p>Si deseas agregar más usuarios, usa la misma ruta nuevamente.</p></body></html>", TAM_CUERPO);
        }
        else
        {
          // La URI no se encuentra registrada.
          respuesta.codigo = 404;
          strncpy(respuesta.cuerpo, "<html><body><h1>Intentaste hacer POST a una ruta que no conocemos</h1></body></html>", TAM_CUERPO);
        }
      }
      else if (strcmp(solicitud.metodo, "HEAD") == 0)
      {
        // 200 y sin payload.
        respuesta.codigo = 200;
        strncpy(respuesta.cuerpo, "", TAM_CUERPO);
      }
      else if (strcmp(solicitud.metodo, "PUT") == 0)
      {
        // Para la URI: /usuarios/1
        if (strcmp(solicitud.uri, "/usuarios/1") == 0)
        {
          respuesta.codigo = 200;
          strncpy(respuesta.cuerpo, "<html><body><h1>Se ha modificado correctamente al usuario #1</h1></body></html>", TAM_CUERPO);
        }
        else
        {
          // La URI no se encuentra registrada.
          respuesta.codigo = 404;
          strncpy(respuesta.cuerpo, "<html><body><h1>No se ha podido modificar nada pues la URI es inválida o no existe.</h1></body></html>", TAM_CUERPO);
        }
      }
      else if (strcmp(solicitud.metodo, "DELETE") == 0)
      {
        // Para la URI: /usuarios/1
        if (strcmp(solicitud.uri, "/usuarios/1") == 0)
        {
          respuesta.codigo = 200;
          strncpy(respuesta.cuerpo, "<html><body><h1>Se ha eliminado el usuario #1</h1></body></html>", TAM_CUERPO);
        }
        else
        {
          // La URI no se encuentra registrada.
          respuesta.codigo = 404;
          strncpy(respuesta.cuerpo, "<html><body><h1>No se puede eliminar nada pues la URI es inválida o no existe.</h1></body></html>", TAM_CUERPO);
        }
      }
      else
      {
        // El método no es soportado por nuestro servidor.
        respuesta.codigo = 501;
        strncpy(respuesta.cuerpo, "<html><body><h1>No se ha implementado el método que utilizaste.</h1></body></html>", TAM_CUERPO);
      }

      // Serializamos la respuesta.
      serializar_respuesta(buffer, &respuesta);

      printf("Buffer:\n%s\n", buffer);

      // Imprimimos la respuesta a enviar.
      printf("\t- Respuesta a enviar:\n");
      imprimir_respuesta(&respuesta);

      // Enviamos la respuesta.
      temporal_resultados = send(
          descriptor_socket_cliente, // El socket a dónde enviaremos la respuesta.
          buffer,                    // La respuesta a enviar
          strlen(buffer),            // El tamaño de la respuesta.
          0                          // Sin opciones
      );

      // Validamos que se haya podido enviar la respuesta.
      if (temporal_resultados <= 0)
      {
        // Mostramos mensaje de error:
        printf("Error: No se pudo enviar la respuesta HTTP usando el socket.\n");
        printf("Inténtalo de nuevo más tarde.\n\n");
      }
      else
      {
        // Mostramos mensaje de éxito.
        printf("\t- ✅ Respuesta HTTP enviada correctamente.\n");
      }
    }
    else
    {
      // Mostramos mensaje de solicitud inválida.
      printf("\t- No se recibió una solicitud HTTP válida.\n");
    }

    // Mostramos mensaje de cierre de socket.
    printf("\t- Cerrando socket...\n");

    // Cerramos el socket del cliente.
    close(descriptor_socket_cliente);

    // Mostramos mensaje de cierre de socket.
    printf("- ✅ Socket cerrado exitosamente.\n");
  }

  return 0;
}
