/**
 * @file cliente.c
 * @author Roldán Morales Ana Karen / Ortega Alcocer Humberto Alejandro
 * @brief Práctica 3 - Cliente de Chat
 * @date 2022-11-27
 * @copyright Copyright (c) 2022
 */
// Librerías locales.
#include "constantes.h" // Para constantes de operación.
#include "mensaje.h"    // Para operaciones con mensajes.
#include "usuario.h"    // Para operaciones con usuarios.

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

// Prototipos de funciones.
void *hilo_recibir_mensajes(void *);    // Función para ejecución de un hilo que reciba e imprima los mensajes.
void limpiar_y_mostrar_prompt(usuario); // Función que limpia la consola y muestra el prompt.

// Estructura para representar el argumento del hilo que recibirá los mensajes.
typedef struct st_arg_hilo_recibir_mensajes
{
  int socket;                // El descriptor del socket por el cual escucharemos los mensajes entrantes.
  usuario usuario_actual;    // El usuario actual.
} arg_hilo_recibir_mensajes; // Definición del tipo arg_hilo_recibir_mensajes.

// Bandera global para finalizar la ejecución del hilo.
int BANDERA_SALIR = 0;

// Función principal.
int main(void)
{
  // Variables locales.
  usuario usuario_actual;                             // Estructura con la información del usuario actual.
  arg_hilo_recibir_mensajes argumento_hilo;           // Estructura con la información para el argumento del hilo a crear.
  int descriptor_socket,                              // Variable para almacenar el descriptor del socket.
      temporal;                                       // Para uso temporal en distintas partes del código.
  struct sockaddr_in informacion_servidor;            // Estructura con la información de conexión del servidor.
  mensaje mensaje_a_enviar;                           // Estructura con un mensaje a enviar de forma temporal.
  pthread_t descriptor_hilo_recibir_mensajes;         // Estructura con el descriptor del hilo que recibe mensajes.
  char contenido_temporal[LONGITUD_MAXIMA_CONTENIDO], // Para operaciones temporales con el contenido de un mensaje.
      *apuntador_temporal;                            // Para operaciones con strtok.
  pthread_mutex_t mutex_socket;                       // Mutex para sincronizar la lectura/escritura del socket.

  // Inicializamos el mutex para usar el socket..
  pthread_mutex_init(
      &mutex_socket, // Un apuntador al mutex a inicializar.
      NULL           // Sin atributos.
  );

  // Mostramos mensaje de inicio de programa.
  printf("\tPráctica 3 - Cliente de Chat\n");
  printf("Realizado por:\n");
  printf("\t- Roldán Morales Ana Karen\n");
  printf("\t- Ortega Alcocer Humberto Alejandro\n");
  printf("Materia:\n");
  printf("\t- Aplicaciones para comunicaciones en Red.\n");
  printf("\t- 3CM13\n");
  printf("Escuela Superior de Cómputo del Instituto Politécnico Nacional\n\n");

  // Solicitamos al usuario su nombre.
  printf("Introduce tu nombre de usuario: ");
  scanf("%s", usuario_actual.nombre);

  // Solicitamos al usuario su edad.
  printf("Introduce tu edad: ");
  scanf("%i", &usuario_actual.edad);

  // Validamos que el usuario sea mayor de edad.
  if (usuario_actual.edad < 18)
  {
    // Mostramos mensaje.
    printf("Error: Para usar cualquier tipo de chat deberás ser mayor a 18 años.\n");

    // Realmente este no es un error de programa, sino del usuario, así que 0.
    return EJECUCION_EXITOSA;
  }

  // Mensaje de confirmación para el usuario.
  printf("Tus datos registrados:\n");
  printf("\t- Usuario: %s\n", usuario_actual.nombre);
  printf("\t- Edad: %i\n", usuario_actual.edad);

  // Iniciamos conexión con el servidor.
  printf("Conectando con el servidor...\n");

  // Creamos el socket.
  descriptor_socket = socket(
      AF_INET,     // IPv4
      SOCK_STREAM, // Socket de flujo.
      0            // Cualquier protocolo.
  );

  // Validamos que se haya podido crear el socket.
  if (descriptor_socket <= 0)
  {
    // Mostramos mensaje de error.
    printf("Error: No se ha podido crear el socket.\n");
    printf("Inténtalo de nuevo más tarde.\n\n");

    // Finalizamos ejecución.
    return EJECUCION_ERROR;
  }

  // Mensaje de confirmación de creación de socket.
  printf("\t- ✅ Socket creado correctamente.\n");

  // Pre-limpiamos la estructura de información del servidor.
  memset((char *)&informacion_servidor, 0, sizeof(informacion_servidor));

  // Llenamos la información del servidor.
  informacion_servidor.sin_family = AF_INET;                     // IPv4
  informacion_servidor.sin_port = htons(PUERTO_SERVIDOR);        // Puerto del servidor.
  informacion_servidor.sin_addr.s_addr = inet_addr(IP_SERVIDOR); // IP del servidor.

  // Realizamos la conexión.
  temporal = connect(
      descriptor_socket,                        // El descriptor del socket.
      (struct sockaddr *)&informacion_servidor, // La información del servidor para la conexión.
      sizeof(informacion_servidor)              // El tamaño de la estructura de información del servidor.
  );

  // Validamos que se haya realizado la conexión.
  if (temporal < 0)
  {
    // Mostramos mensaje de error.
    printf("Error: No se ha podido realizar la conexión con el servidor.\n");
    printf("(¿Está el servidor corriendo en %s:%i?)\n", IP_SERVIDOR, PUERTO_SERVIDOR);
    printf("Inténtalo de nuevo más tarde.\n");

    // Cerramos el socket.
    close(descriptor_socket);

    // Finalizamos ejecución.
    return EJECUCION_ERROR;
  }

  // Mensaje de confirmación de conexión establecida.
  printf("\t- ✅ Conexión establecida correctamente.\n");

  // Limpiamos la estructura del mensaje a enviar.
  memset((char *)&mensaje_a_enviar, 0, sizeof(mensaje_a_enviar));

  // Preparamos el mensaje de conexión.
  mensaje_a_enviar.tipo = CONEXION;
  mensaje_a_enviar.autor = usuario_actual;

  // Enviamos el mensaje.
  temporal = send(
      descriptor_socket,               // El socket mediante el cual se envía el mensaje.
      (const char *)&mensaje_a_enviar, // El mensaje a enviar.
      sizeof(mensaje),                 // El tamaño de la estructura del mensaje a enviar.
      0                                // Sin banderas.
  );

  // Verificamos que se haya podido enviar el mensaje de conexión.
  if (temporal <= 0)
  {
    // Mostramos mensaje de error.
    printf("Error: No se ha podido enviar el mensaje de conexión al servidor.\n");
    printf("Inténtalo de nuevo más tarde.\n\n");

    // Cerramos el socket.
    close(descriptor_socket);

    // Finalizamos la ejecución.
    return EJECUCION_ERROR;
  }

  // Mensaje de confirmación de notificación de conexión a servidor.
  printf("\t- ✅ Sesión iniciada correctamente en el servidor.\n");

  // Creamos los datos para el hilo que recibirá los mensajes.
  argumento_hilo.socket = descriptor_socket;      // Asignamos el descriptor del socket en el que deberá escuchar.
  argumento_hilo.usuario_actual = usuario_actual; // Asignamos el usuario actual.

  // Mostramos instrucciones y mensaje de inicio de sala de chat.
  printf("Instrucciones:\n");
  printf("\t- Introduce 'salir' para finalizar la ejecución.\n");
  printf("\t- Para enviar un mensaje privado usa: /usuario bla bla bla...\n");
  printf("\t- ¡Diviértete 🎉!\n\n");
  printf("---- Sala de Chat ----\n");

  // Iniciamos el hilo que recibirá los mensajes entrantes.
  pthread_create(
      &descriptor_hilo_recibir_mensajes, // La estructura dónde se almacenará la información del hilo creado.
      NULL,                              // Sin opciones de hilo.
      &hilo_recibir_mensajes,            // La función que se ejecutará en el hilo.
      (void *)&argumento_hilo            // El argumento para el hilo que recibirá los mensajes.
  );

  // Iniciamos ciclo para enviar mensajes.
  do
  {
    // Limpiamos nuestra estructura mensaje.
    memset((char *)&mensaje_a_enviar, 0, sizeof(mensaje));

    // Limipiamos y mostramos prompt.
    limpiar_y_mostrar_prompt(usuario_actual);

    // Leemos la entrada del usuario.
    fgets(mensaje_a_enviar.contenido, LONGITUD_MAXIMA_CONTENIDO, stdin);

    // Quitamos el salto de línea.
    mensaje_a_enviar.contenido[strcspn(mensaje_a_enviar.contenido, "\n")] = 0;

    // Validamos que no sea una cadena vacía.
    if (strlen(mensaje_a_enviar.contenido) <= 1)
    {
      // Si lo es, saltamos el ciclo.
      continue;
    }

    // Asumimos que el tipo será "mensaje general".
    mensaje_a_enviar.tipo = MSJ_GENERAL;

    // Colocamos al autor del mensaje.
    mensaje_a_enviar.autor = usuario_actual;

    // Checamos si es un mensaje directo, si hubiera más comandos sería aquí abajo determinar el comando.
    if (mensaje_a_enviar.contenido[0] == '/')
    {
      // Colocamos el tipo de mensaje.
      mensaje_a_enviar.tipo = MSJ_PRIVADO; // Es un mensaje privado.

      // Copiamos el contenido para usar strtok.
      // Iniciamos en la posición inicial + 1 porque el primer caracter no lo necesitamos.
      strcpy(contenido_temporal, mensaje_a_enviar.contenido + 1);

      // Tokenizamos la cadena de caracteres en espacios.
      apuntador_temporal = strtok(contenido_temporal, " ");

      // Nuestro apuntador ahora tendrá nuestro nombre de usuario (el primer token).
      strcpy(mensaje_a_enviar.destinatario.nombre, apuntador_temporal);

      // Copiamos de vuelta el contenido a la variable temporal para operación.
      strcpy(contenido_temporal, mensaje_a_enviar.contenido);

      // Para limpiar el contenido (quitar el nombre del usuario y el '/'),
      // usaremos strchr, que devuelve un apuntador al caracter encontrado.
      apuntador_temporal = strchr(contenido_temporal, ' ');

      // Movemos el apuntador de la cadena al apuntador obtetnido previamente + 1,
      // que será el resto de la cadena obtenida.
      strcpy(mensaje_a_enviar.contenido, apuntador_temporal);
    }

    // Si no es el comando de salida, enviamos el mensaje.
    if (strcmp(mensaje_a_enviar.contenido, "salir") != 0 && strcmp(mensaje_a_enviar.contenido, "SALIR") != 0)
    {
      // Realizamos el envío.
      temporal = send(
          descriptor_socket,               // El descriptor del socket mediante el cual enviaremos la información.
          (const char *)&mensaje_a_enviar, // El mensaje a enviar serializado a const char*.
          sizeof(mensaje),                 // El tamaño del mensaje a enviar.
          0                                // Sin opciones
      );

      // Verificamos que se haya enviado correctamente.
      if (temporal <= 0)
      {
        // Mostramos mensaje de error.
        printf("Error: No se ha podido enviar mensaje.\n");
        printf("Inténtalo de nuevo más tarde.\n\n");
      }
    }
  } while (strcmp(mensaje_a_enviar.contenido, "salir") != 0 && strcmp(mensaje_a_enviar.contenido, "SALIR") != 0 && !BANDERA_SALIR); // Mientras el usuario no coloque la palabra "salir".

  // Mensaje final.
  printf("---- Fin de Sala de Chat ----\n");

  // Informamos al usuario que inicia el proceso de "cerrar sesión" (desconexión).
  printf("Desconectando del servidor...\n");

  // Colocamos la bandera en 1 para indicarle al hilo que finalice su ejecución.
  BANDERA_SALIR = 1;

  // Preparamos mensaje para envío de desconexión.
  memset((char *)&mensaje_a_enviar, 0, sizeof(mensaje_a_enviar));

  // Llenamos información del mensaje.
  mensaje_a_enviar.tipo = DESCONEXION;
  mensaje_a_enviar.autor = usuario_actual;

  // Enviamos mensaje de desconexión.
  temporal = send(
      descriptor_socket,               // El descriptor del socket para realizar el envío.
      (const char *)&mensaje_a_enviar, // El mensaje a enviar.
      sizeof(mensaje),                 // El tamaño del dato a enviar.
      0                                // Sin banderas.
  );

  // Verificamos que se haya enviado el mensaje de desconexión correctamente.
  if (temporal <= 0)
  {
    printf("Error: No se ha podido enviar el mensaje de desconexión.\n");
    printf("Inténtalo de nuevo más tarde.\n\n");

    // Cerramos el socket.
    close(descriptor_socket);

    // Salimos.
    return EJECUCION_ERROR;
  }

  // Esperamos que el hilo finalice, para esto deberá recibir un último mensaje.
  pthread_join(descriptor_hilo_recibir_mensajes, NULL);

  // Informamos al usuario que ya no escuchamos nuevos mensajes.
  printf("\t- ✅ Finalizado la escucha de mensajes nuevos.\n");

  // Cerramos el socket.
  close(descriptor_socket);

  // Informamos al usuario que hemos cerrado la conexión con el servidor.
  printf("\t- ✅ Conexión con el servidor finalizada correctamente.\n");
  printf("\n¡Hasta la próxima!\n");

  // Fin de programa.
  return EJECUCION_EXITOSA;
}

// Implementación de funciones.

/**
 * @brief Hilo que escucha los mensajes entrantes y los imprime.
 *
 * @param argumento Una estructura de tipo arg_hilo_recibir_mensajes con los datos del cliente.
 */
void *hilo_recibir_mensajes(void *argumento)
{
  // Variables locales.
  arg_hilo_recibir_mensajes *argumentos = (arg_hilo_recibir_mensajes *)argumento; // Los argumentos para el funcionamiento del hilo.
  int temporal;                                                                   // Variable temporal.
  mensaje *mensaje_a_recibir;                                                     // El mensaje a recibir.
  char *buffer;                                                                   // El búffer para las operaciones de recepción de mensajes

  // Asignamos el tamaño a nuestro buffer.
  buffer = (char *)malloc(sizeof(mensaje));

  // Ciclo mientras no se active la bandera para salir.
  while (!BANDERA_SALIR)
  {
    // Limpiamos el buffer.
    memset(buffer, 0, sizeof(mensaje));

    // Esperamos que nos llegue un mensaje.
    temporal = recv(
        argumentos->socket, // Descriptor del socket dónde esperaremos los mensajes.
        buffer,             // El buffer dónde almacenaremos los datos recibidos.
        sizeof(mensaje),    // El tamaño del búffer que leeremos.
        0                   // Sin opciones.
    );

    // Validamos que hayamos recibido información válida.
    if (temporal <= 0)
    {
      // Mostramos mensaje de error.
      printf("Error: No se ha recibido información válida desde el servidor.\n");
      printf("Inténtalo de nuevo más tarde.\n\n");

      // Colocamos la bandera en 1 para indicar al hilo principal que la ejecución terminó.
      BANDERA_SALIR = 1;

      // Rompemos el ciclo.
      break;
    }

    // Deserializamos la información recibida.
    mensaje_a_recibir = (mensaje *)buffer;

    // Verificamos si es un mensaje privado.
    if (mensaje_a_recibir->tipo == MSJ_PRIVADO)
    {
      // Verificamos si somos los destinatarios.
      if (strcmp(mensaje_a_recibir->destinatario.nombre, argumentos->usuario_actual.nombre) == 0)
      {
        // Salto de línea.
        printf("\n");

        // Imprimimos el mensaje privado.
        imprimir_mensaje(*mensaje_a_recibir);

        // Limpiamos y mostramos el prompt.
        limpiar_y_mostrar_prompt(argumentos->usuario_actual);
      }
    }
    // Ahora verificamos que no sea el texto que acabamos de enviar.
    else if (strcmp(mensaje_a_recibir->autor.nombre, argumentos->usuario_actual.nombre) != 0)
    {
      // Salto de línea.
      printf("\n");

      // Imprimimos el mensaje (conexión, desconexión, general).
      imprimir_mensaje(*mensaje_a_recibir);

      // Limpiamos y mostramos el prompt.
      limpiar_y_mostrar_prompt(argumentos->usuario_actual);
    }
  }

  // Liberamos la memoria del buffer.
  free(buffer);

  // Fin de ejecución del hilo.
  return EJECUCION_EXITOSA;
}

/**
 * @brief Función que limpia stdout y muestra el prompt de usuario para que
 *        introduzca otro mensaje.
 *
 * @param usuario_actual El usuario actual
 */
void limpiar_y_mostrar_prompt(usuario usuario_actual)
{
  // Saltamos la línea y mostramos el prompt.
  printf("(%s [%i])~> ", usuario_actual.nombre, usuario_actual.edad);

  // Hacemos un fflush a la salida estándar para eliminar cualquier texto que
  // haya estado presente. Medio incómodo para el usuario si recibe un mensaje
  // mientras está escribiendo pero... pues ni modo.
  fflush(stdout);
}
