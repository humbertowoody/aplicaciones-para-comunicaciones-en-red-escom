/**
 * @file servidor.c
 * @author Roldán Morales Ana Karen / Ortega Alcocer Humberto Alejandro
 * @brief Práctica 3 - Servidor de Chat
 * @date 2022-11-27
 * @copyright Copyright (c) 2022
 */
// Librerías locales.
#include "constantes.h"    // Para constantes de operación.
#include "mensaje.h"       // Para operaciones con mensajes.
#include "usuario.h"       // Para operaciones con usuarios.
#include "cola_usuarios.h" // Para la cola de usuarios.

// Librerías de sistema.
#include <stdio.h>      // Operaciones de entrada y salida.
#include <stdlib.h>     // Funciones estándar de C.
#include <sys/socket.h> // Para funciones de sockets.
#include <sys/types.h>  // Estructuras y tipos de datos utilizados en varias operaciones de sockets y demás.
#include <arpa/inet.h>  // Funcionalidades para manipulación de información de red.
#include <netinet/in.h> // Funcionalidades para operaciones con direccionamiento de red
#include <unistd.h>     // Para operaciones con distintas llamadas al sistema operativoipo
#include <pthread.h>    // Para manipulación y operaciones con hilos.
#include <string.h>     // Para manipulación de cadenas de caracteres.
#include <netdb.h>      // Definición de funciones para traducción entre tipos de datos para redes.

// Prototipos de funciones.
void *hilo_cliente(void *); // Función para ejecución de un hilo para cada cliente.

// Estructura para representar el argumento del hilo que controlará los clientes.
typedef struct st_arg_hilo_cliente
{
  int socket;                          // El descriptor del socket del cliente conectado.
  cola_usuarios **usuarios_conectados; // Un apuntador al inicio de la cola de usuarios.
  pthread_mutex_t **mutex_cola;        // Un apuntador al mutex para sincronizar operaciones en la cola de usuarios.
} arg_hilo_cliente;                    // Definición de tiopo arg_hilo_cliente.

// Función principal.
int main(void)
{
  // Variables locales.
  struct addrinfo hints,                     // Estructura con la configuración que buscamos en las opciones de direccionamiento disponibles.
      *informacion_servidor,                 // Estructura con la lista ligada de direccionamientos disponibles
      *iterador;                             // Iterador de direccionamientos disponibles.
  struct sockaddr_in informacion_cliente;    // Estructura con la información de red del cliente que se ha conectado.
  int descriptor_socket_servidor,            // Descriptor del socket del servidor.
      descriptor_socket_cliente,             // Descriptor del socket del cliente.
      temporal_resultados,                   // Variable para almacenar el resultado de alguna operación temporal.
      temporal_reutilizable;                 // Variable para almacenar un valor temporal.
  socklen_t longitud_informacion_cliente;    // Variable para almacenar el tamaño de la estructura con los datos del cliente que se conectó.
  struct linger opciones_linger;             // Estructura para almacenar las opciones de Linger.
  cola_usuarios *usuarios_conectados = NULL; // Cola con usuarios.
  pthread_t descriptores_hilo_cliente[100];  // Descriptores de los hilos de los clientes conectados.
  arg_hilo_cliente argumentos_clientes[100]; // Arreglo dinámico con los argumentos generados para cada cliente.
  size_t num_clientes = 0;                   // El número de clientes conectados.
  pthread_mutex_t *mutex_cola;               // Mutex  para sincronizar las operaciones en la cola de usuarios.

  // Creamos la memoria para el mutex.
  mutex_cola = calloc(1, sizeof(pthread_mutex_t));

  // Inicializamos el mutex para la cola de usuarios.
  pthread_mutex_init(
      mutex_cola, // Un apuntador al mutex a inicializar.
      NULL        // Sin atributos.
  );

  // Mostramos mensaje de inicio de programa.
  printf("\tPráctica 3 - Servidor de Chat\n");
  printf("Realizado por:\n");
  printf("\t- Roldán Morales Ana Karen\n");
  printf("\t- Ortega Alcocer Humberto Alejandro\n");
  printf("Materia:\n");
  printf("\t- Aplicaciones para comunicaciones en Red.\n");
  printf("\t- 3CM13\n");
  printf("Escuela Superior de Cómputo del Instituto Politécnico Nacional\n\n");
  printf("Inicializando servidor...\n");

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
    return EJECUCION_ERROR;
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
    return EJECUCION_ERROR;
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
    return EJECUCION_ERROR;
  }

  // Fin de inicialización de programa.
  printf("¡Servidor iniciado correctamente!\n");
  printf("\t- IP del servidor: %s\n", IP_SERVIDOR);
  printf("\t- Puerto del servidor: %i\n", PUERTO_SERVIDOR);
  printf("- Esperando conexiones...\n");

  // Ajustamos el tamaño de nuestra estructura de información del cliente.
  longitud_informacion_cliente = sizeof(informacion_cliente);

  // Ciclo principal infinito de ejecución del servidor.
  while (1)
  {
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

      // Continuamos la ejecución del ciclo.
      continue;
    }

    // Imprimimos la información del cliente conectado.
    printf("- ✅ Conexión aceptada exitosamente.\n");
    printf("\t- IP: %s\n", inet_ntoa(informacion_cliente.sin_addr));
    printf("\t- Puerto: %i\n", ntohs(informacion_cliente.sin_port));

    // Aumentamos el número de clientes conectados.
    num_clientes++; // Usamos ++ porque la posición actual será la que usaremos para las asignaciones.

    // Creamos el argumento para el hilo a partir del mensaje recibido.
    argumentos_clientes[num_clientes].usuarios_conectados = &usuarios_conectados;
    argumentos_clientes[num_clientes].socket = descriptor_socket_cliente;
    argumentos_clientes[num_clientes].mutex_cola = &mutex_cola;

    // Informamos que estamos creando el hilo.
    printf("\t- Creando hilo para mensajes entrantes del cliente nuevo...\n");

    // Asignamos el descriptor del socket cliente a nuestro argumento del hilo.
    pthread_create(
        &descriptores_hilo_cliente[num_clientes],  // El descriptor del hilo a crear.
        NULL,                                      // Sin atributos para el hilo.
        &hilo_cliente,                             // La función a ejecutar.
        (void *)&argumentos_clientes[num_clientes] // El elemento en el arreglo de argumentos con la información para este cliente.
    );

    // Mensaje de confirmación
    printf("\t- ✅ Hilo iniciado correctamente para el cliente nuevo.\n");
  }

  // Fin de la ejecución, realmente jamás llegaremos aquí.
  return EJECUCION_EXITOSA;
}

// Implementación de funciones.

/**
 * @brief Función que representa un hilo que gestiona la conexión con cada
 *        cliente de chat.
 * @param argumento Una estructura de tipo st_arg_hilo_cliente con la información requerida para el hilo.
 * @return void
 */
void *hilo_cliente(void *argumento)
{
  // Variables locales.
  arg_hilo_cliente *argumento_hilo = (arg_hilo_cliente *)argumento; // Almacenará el argumento para este hilo.
  mensaje *mensaje_recibido;                                        // Mensaje que recibimos del cliente.
  char *buffer;                                                     // El buffer para almacenar los datos recibidos.
  int temporal;                                                     // Variable temporal.
  usuario usuario_actual;                                           // Para almacenar el usuario actual de este hilo.
  cola_usuarios *elemento_encontrado;                               // Un apuntador para las operaciones de búsqueda.

  // Asignamos tamaño del búffer.
  buffer = (char *)malloc(sizeof(mensaje));

  // Mensaje indicando
  printf("\t[%i] - Esperando inicio de sesión...\n", argumento_hilo->socket);

  // Limpiamos el buffer.
  memset(buffer, 0, sizeof(mensaje));

  // Esperamos el mensaje de conexión.
  temporal = recv(
      argumento_hilo->socket, // Descriptor del socket dónde recibiremos los datos.
      buffer,                 // Buffer dónde almacenaremos los datos
      sizeof(mensaje),        // El tamaño máximo del buffer.
      0                       // Sin banderas.
  );

  // Validamos que hayamos recibido algo.
  if (temporal <= 0)
  {
    // Mostramos mensaje de error.
    printf("\t[%i] - Error: No se recibió un mensaje de conexión válido. Cerrando conexión.\n", argumento_hilo->socket);

    // Cerramos el socket.
    close(argumento_hilo->socket);

    // Finalizamos la ejecución.
    return (void *)EJECUCION_ERROR;
  }

  // Deserializamos el mensaje.
  mensaje_recibido = (mensaje *)buffer;

  // Copiamos la información en el usuario.
  usuario_actual = mensaje_recibido->autor;

  // Mostramos mensaje de confirmación.
  printf("\t[%i] - ✅ Inicio de sesión exitoso.\n", argumento_hilo->socket);
  printf("\t\t[%i] - Nombre: %s\n", argumento_hilo->socket, usuario_actual.nombre);
  printf("\t\t[%i] - Edad: %i\n", argumento_hilo->socket, usuario_actual.edad);

  // Agregamos el usuario a la cola de usuarios.
  temporal = agregar_en_cola(
      argumento_hilo->usuarios_conectados, // Apuntador al inicio de la cola.
      usuario_actual,                      // Usuario recibido.
      argumento_hilo->socket,              // El socket del usuario.
      argumento_hilo->mutex_cola           // El mutex de la cola.
  );

  // Validamos que se haya agregado a la cola correctamente.
  if (temporal != EJECUCION_EXITOSA)
  {
    // Mostramos un mensaje de error.
    printf("\t[%i] Error: No se pudo añadir al usuario %s a la cola.\n", argumento_hilo->socket, usuario_actual.nombre);
  }
  else
  {
    printf("\t[%i] - ✅ Usuario (%s) agregado correctamente a la cola de usuarios conectados.\n", argumento_hilo->socket, usuario_actual.nombre);
  }

  // Imprimimos el mensaje.
  printf("\t[%i] - ", argumento_hilo->socket);
  imprimir_mensaje(*mensaje_recibido);

  // Retransmitimos el mensaje.
  transmitir_mensaje_a_cola_de_usuarios(
      *argumento_hilo->usuarios_conectados, // Apuntador al inicio de la cola de usuarios.
      *mensaje_recibido,                    // El mensaje a transmitir.
      argumento_hilo->mutex_cola            // El mutex de la cola de usuarios.
  );

  // Mensaje de confirmación
  printf("\t[%i] - ✅ Inicia escucha de mensajes para %s.\n", argumento_hilo->socket, usuario_actual.nombre);

  // Comenzamos ciclo infinito.
  do
  {
    // Limpiamos el buffer.
    memset(buffer, 0, sizeof(mensaje));

    // Esperamos un mensaje.
    temporal = recv(
        argumento_hilo->socket, // Descriptor del socket dónde recibiremos los datos.
        buffer,                 // El buffer dónde almacenaremos la información.
        sizeof(mensaje),        // El tamaño máximo del mensaje a leer.
        0                       // Sin banderas.
    );

    // Validamos que hayamos recibido algo.
    if (temporal <= 0)
    {
      // Imprimimos mensaje de error.
      printf("\t[%i] - Error: No se ha recibido un mensaje válido para cliente: %s. Cerrando conexión.\n", argumento_hilo->socket, usuario_actual.nombre);

      // Finalizamos la ejecución del cliente.
      break;
    }

    // Deserializamos el mensaje recibido.
    mensaje_recibido = (mensaje *)buffer;

    // Validamos si es un mensaje directo para tratar de completar la información.
    if (mensaje_recibido->tipo == MSJ_PRIVADO)
    {
      // Buscamos al usuario.
      elemento_encontrado = encontrar_usuario(*argumento_hilo->usuarios_conectados, mensaje_recibido->destinatario);

      // Si lo encontramos asignamos la edad correcta.
      if (elemento_encontrado != NULL)
      {
        // Asignamos el valor del servidor.
        mensaje_recibido->destinatario = elemento_encontrado->data_usuario;
      }
    }

    // Imprimimos mensaje.
    printf("\t[%i] - ", argumento_hilo->socket);
    imprimir_mensaje(*mensaje_recibido);

    // Retransmitimos el mensaje a los demás clientes.
    transmitir_mensaje_a_cola_de_usuarios(
        *argumento_hilo->usuarios_conectados, // El nodo inicial de la lista ligada.
        *mensaje_recibido,                    // El mensaje a transmitir.
        argumento_hilo->mutex_cola            // El mutex para proteger la lista mientras se hace la transmisión
    );
  } while (mensaje_recibido->tipo != DESCONEXION); // Mientras no recibamos un mensaje de desconexión.

  // Eliminamos al usuario de la cola de usuarios.
  temporal = eliminar_de_cola(
      argumento_hilo->usuarios_conectados, // El nodo inicial de la lista ligada de usuarios conectados.
      argumento_hilo->socket,              // El socket (ID del elemento) del usuario a eliminar.
      argumento_hilo->mutex_cola           // El mutex para prevenir race-conditions
  );

  // Validamos que se haya eliminado el usuario de la cola.
  if (temporal != EJECUCION_EXITOSA)
  {
    // Mostrar mensaje de error.
    printf("\t[%i] - Error: No se pudo eliminar de la cola el usuario: %s\n", argumento_hilo->socket, usuario_actual.nombre);
    printf("\t[%i] - Cola de usuarios: ", argumento_hilo->socket);
    imprimir_cola(*argumento_hilo->usuarios_conectados);
  }
  else
  {
    printf("\t[%i] - ✅ Se ha desconectado exitosamente al usuario: %s\n", argumento_hilo->socket, usuario_actual.nombre);
  }

  // Cerramos el socket.
  close(argumento_hilo->socket);

  // Liberamos la memoria del buffer.
  free(buffer);

  // Finalizamos ejecución de hilo.
  return (void *)EJECUCION_EXITOSA;
}
