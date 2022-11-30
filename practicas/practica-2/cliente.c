/**
 * @file cliente.c
 * @author Roldán Morales Ana Karen / Ortega Alcocer Humberto Alejandro
 * @brief Práctica 3 - Cliente
 * @date 2022-11-27
 * @copyright Copyright (c) 2022
 */
// Bibliotecas requeridas.
#include <stdio.h>      // Operaciones de entrada y salida estándar.
#include <stdlib.h>     // Funciones estándar de C.
#include <string.h>     // Funciones y estructuras para manipulación de cadenas de caracteres.
#include <sys/socket.h> // Para operaciones con sockets.
#include <sys/types.h>  // Estructuras y tipos de datos utilizados en varias operaciones de sockets y demás.
#include <arpa/inet.h>  // Funcionalidades para manipulación de información de red.
#include <netinet/in.h> // Funcionalidades para operaciones con direccionamiento de red
#include <unistd.h>     // Para operaciones con distintas llamadas al sistema operativo

// Bibliotecas desarrolladas.
#include "cancion.h"    // Estructura que representa una canción.
#include "constantes.h" // Constantes compartidas entre cliente y servidor.

// Constantes locales.
#define OPCION_MENU_AGREGAR_CARRITO 1
#define OPCION_MENU_VISUALIZAR_CARRITO 2
#define OPCION_MENU_ELIMINAR_CARRITO 3
#define OPCION_MENU_COMPRAR_Y_SALIR 4
#define OPCION_MENU_SALIR 5
#define CARPETA_CLIENTE "./biblioteca_cliente"

// Prototipos de funciones.
void mostrar_menu(void);                  // Función para mostrar el menú principal.
void agregar_carrito(cancion *, int *);   // Muestra el catálogo y permite al usuario agregar una canción a su carrito de compras.
void eliminar_carrito(cancion *, int *);  // Elimina una canción del carrito de compras.
int comprar_carrito(cancion *, int);      // Finaliza la compra y descarga las canciones seleccionadas.
float visualizar_carrito(cancion *, int); // Función que muestra el carrito de compras actual.

// Función principal.
int main(void)
{
  // Variables locales.
  cancion carrito_compra[TAM_MAX_CATALOGO_CANCIONES]; // Arreglo con las canciones en el carrito.
  int opcion_menu,                                    // Variable para almacenar la opción del menú seleccionada.
      num_canciones_carrito = 0;                      // Número de elementos en el carrito.

  // Mostramos mensaje de inicio de programa.
  printf("\tPráctica 2 - Carrito de Compras\n");
  printf("Realizado por:\n");
  printf("\t- Roldán Morales Ana Karen\n");
  printf("\t- Ortega Alcocer Humberto Alejandro\n");
  printf("Materia:\n");
  printf("\t- Aplicaciones para comunicaciones en Red.\n");
  printf("\t- 3CM13\n");
  printf("Escuela Superior de Cómputo del Instituto Politécnico Nacional\n\n");

  // Ciclo principal.
  do
  {
    // Mostramos el menú principal.
    mostrar_menu();

    // Leemos la opción del usuario.
    scanf("%i", &opcion_menu);

    // Llamamos a la función correspondiente.
    switch (opcion_menu)
    {
    case OPCION_MENU_AGREGAR_CARRITO:
      agregar_carrito(carrito_compra, &num_canciones_carrito);
      break;

    case OPCION_MENU_VISUALIZAR_CARRITO:
      visualizar_carrito(carrito_compra, num_canciones_carrito);
      break;

    case OPCION_MENU_ELIMINAR_CARRITO:
      eliminar_carrito(carrito_compra, &num_canciones_carrito);
      break;

    case OPCION_MENU_COMPRAR_Y_SALIR:
      comprar_carrito(carrito_compra, num_canciones_carrito);
      break;

    case OPCION_MENU_SALIR:
      printf("¡Hasta la próxima!\n");
      break;

    default:
      printf("\t¡La opción seleccionada no es válida!\n");
      break;
    }

  } while (opcion_menu != OPCION_MENU_COMPRAR_Y_SALIR && opcion_menu != OPCION_MENU_SALIR);

  // Fin de programa.
  return 0;
}

// Implementación de funciones.

/**
 * @brief Función que muestra el menú principal de la aplicación.
 */
void mostrar_menu()
{
  printf("\t\tMenú Principal\n");
  printf("Selecciona una opción:\n");
  printf("\t%i) Agregar al carrito de compras.\n", OPCION_MENU_AGREGAR_CARRITO);
  printf("\t%i) Visualizar carrito de compras.\n", OPCION_MENU_VISUALIZAR_CARRITO);
  printf("\t%i) Eliminar del carrito de compras.\n", OPCION_MENU_ELIMINAR_CARRITO);
  printf("\t%i) Finalizar compra.\n", OPCION_MENU_COMPRAR_Y_SALIR);
  printf("\t%i) Salir.\n", OPCION_MENU_SALIR);
  printf("\nOpción deseada: ");
}

/**
 * @brief Función que muestra el catálogo de canciones disponible desde el servidor y permite
 * al usuario añadirlo a su carrito de compras.
 *
 * @param carrito El carrito de compras actual del usuario.
 * @param num_canciones El número de elementos en el carrito de compras del usuario.
 */
void agregar_carrito(cancion *carrito, int *num_canciones)
{
  // Variables locales.
  int descriptor_socket,                   // Para el descriptor del socket.
      temporal;                            // Para operaciones temporales.
  struct sockaddr_in informacion_servidor; // Para almacenar la información del servidor.
  char buffer[TAM_BUFFER];                 // Búffer para almacenar información.
  cancion *catalogo;                       // Catálogo de canciones.

  // Creamos el socket.
  descriptor_socket = socket(
      AF_INET,     // IPv4
      SOCK_STREAM, // Socket de flujo
      0            // Cualquier protocolo
  );

  // Validamos que se haya podido crear el socket.
  if (descriptor_socket <= 0)
  {
    // Mostramos mensaje de error.
    printf("Error: No se ha podido crear el socket.\n");
    printf("Inténtalo de nuevo más tarde.\n\n");

    // Finalizamos ejecución.
    return;
  }

  // Limpiamos la estructura de información del servidor.
  memset((char *)&informacion_servidor, 0, sizeof(informacion_servidor));

  // Llenamos la información del servidor.
  informacion_servidor.sin_family = AF_INET;                     // IPv4
  informacion_servidor.sin_port = htons(PUERTO_SERVIDOR);        // Puerto del servidor.
  informacion_servidor.sin_addr.s_addr = inet_addr(IP_SERVIDOR); // IP del servidor.

  // Realizamos la conexión.
  temporal = connect(
      descriptor_socket,                        // El socket a utilizaqr.
      (struct sockaddr *)&informacion_servidor, // Cast de la estructura de dirección del servidor.
      sizeof(informacion_servidor)              // Tamaño de la estructura con la información del servidor.
  );

  // Validamos la conexión.
  if (temporal < 0)
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

  // Mensaje de envío de operación al servidor.
  printf("\nSolicitando catálogo de canciones al servidor...\n");

  // Cargamos el ID de operación en el búffer.
  sprintf(buffer, "%i", OPERACION_LISTAR_CATALOGO);

  // Enviamos el ID de operación al servidor.
  temporal = send(
      descriptor_socket, // El socket a utilizar para enviar información.
      buffer,            // Buffer para almacenar la información.
      strlen(buffer),    // Longitud del buffer.
      0                  // Sin banderas.
  );

  // Validamos que se hayan enviado datos correctamente.
  if (temporal <= 0)
  {
    // Mostramos mensaje de error.
    printf("Error: No se ha podido enviar el ID de operación al servidor.\n");
    printf("Inténtalo de nuevo más tarde.\n\n");

    // Cerramos el socket.
    close(descriptor_socket);

    // Fin de ejecución.
    return;
  }

  // Recibimos los datos del servidor.
  temporal = recv(
      descriptor_socket, // El socket a utilizar para enviar información.
      buffer,            // Búffer dónde guardamos datos
      TAM_BUFFER,        // Tamaño máximo de los datos del búffer
      0                  // Sin banderas
  );

  // Validamos que se hayan recibido datos correctamente.
  if (temporal <= 0)
  {
    // Mostramos mensaje de error.
    printf("Error: No se ha podido recibir el catálogo de canciones.\n");
    printf("Inténtalo de nuevo más tarde.\n\n");

    // Cerramos el socket.
    close(descriptor_socket);

    // Fin de ejecución.
    return;
  }

  // Convertimos el buffer en el arreglo de canciones.
  catalogo = (cancion *)buffer;

  // Mostramos el encabezado.
  printf("¡Catálogo recibido correctamente!\nCatálogo de canciones:\n");
  printf("\n|-------------------------------------------------------------------|\n");
  printf("|  ID  |  Nombre  |  Artista  |  Album  |  Año  |  Precio Unitario  |\n");
  printf("|-------------------------------------------------------------------|\n");

  // Mostramos el catálogo.
  for (int cancion_actual = 0; cancion_actual < TAM_MAX_CATALOGO_CANCIONES; cancion_actual++)
  {
    // Mostramos el dato.
    printf("| %i ; %s ; %s ; %s ; %i ; %.2f |\n", cancion_actual + 1, catalogo[cancion_actual].nombre, catalogo[cancion_actual].artista, catalogo[cancion_actual].album, catalogo[cancion_actual].anio_album, catalogo[cancion_actual].precio);
    printf("|-------------------------------------------------------------------|\n");
  }

  // Preguntamos al usuario por la canción a añadir al carrito.
  printf("\nIntroduce el ID de la canción que deseas agregar al carrito: ");
  scanf("%i", &temporal);

  // Validamos si es un ID de canción válido.
  if (temporal <= 0 || temporal > TAM_MAX_CATALOGO_CANCIONES)
  {
    // Mostramos mensaje de error.
    printf("\t¡El ID ingresado no corresponde a ningún elemento del catálogo!\n");

    // Cerramos el socket.
    close(descriptor_socket);

    // Salimos.
    return;
  }

  // Agregamos al carrito y finalizamos.
  carrito[(*num_canciones)] = catalogo[temporal - 1];

  // Aumentamos el número de canciones en el carrito.
  (*num_canciones)++;

  // Cerramos el socket.
  close(descriptor_socket);
}

/**
 * @brief Función que permite eliminar una canción del carrito de compras del usuario.
 *
 * @param carrito El carrito de compras actual del usuario.
 * @param num_canciones El número de elementos en el carrito de compras del usuario.
 */
void eliminar_carrito(cancion *carrito, int *num_canciones)
{
  // Variables locales.
  int id_a_eliminar;

  // Validamos si el carrito de compras está vacío.
  if ((*num_canciones) <= 0)
  {
    // Mostramos un mensaje y finalizamos la ejecución.
    printf("\n\t¡El carrito de compras se encuentra vacío!\n\n");
  }
  else
  {
    // Mostramos el encabezado de la tabla.
    printf("\nCarrito de compras:\n");
    printf("|-------------------------------------------------------------------|\n");
    printf("|  ID  |  Nombre  |  Artista  |  Album  |  Año  |  Precio Unitario  |\n");
    printf("|-------------------------------------------------------------------|\n");

    // Iteramos sobre cada elemento presente en el carrito de compras del usuario.
    for (int elemento_actual = 0; elemento_actual < (*num_canciones); elemento_actual++)
    {
      // Mostramos el dato.
      printf("| %i ; %s ; %s ; %s ; %i ; %.2f |\n", elemento_actual + 1, carrito[elemento_actual].nombre, carrito[elemento_actual].artista, carrito[elemento_actual].album, carrito[elemento_actual].anio_album, carrito[elemento_actual].precio);
      printf("|-------------------------------------------------------------------|\n");
    }

    // Preguntamos el usuario por el ID a eliminar.
    printf("\n\nIntroduce el ID de la canción a eliminar del carrito: ");
    scanf("%i", &id_a_eliminar);

    // Validamos que sea un ID válido.
    if (id_a_eliminar > 0 && id_a_eliminar <= (*num_canciones))
    {
      // Iteramos para recorrer los elementos siguientes en el arreglo a una posición anterior.
      for (int elemento_actual = (id_a_eliminar - 1); elemento_actual < ((*num_canciones) - 1); elemento_actual++)
      {
        carrito[elemento_actual] = carrito[elemento_actual + 1];
      }

      // Decrementamos el numero de canciones en el carrito.
      (*num_canciones)--;

      // Mostramos mensaje de confirmación.
      printf("\n\t¡Se ha eliminado correctamente el ID #%i del carrito!\n\n", id_a_eliminar);
    }
    else
    {
      printf("\n\t¡El ID ingresado no existe en el carrito de compras!\n\n");
    }
  }
}

/**
 * @brief Función que permite realizar la "compra" de los elementos en el carrito de compras del
 * usuario y descarga los archivos MP3 desde el servidor correspondientes.
 *
 * @param carrito El carrito de compras actual del usuario.
 * @param num_canciones El número de elementos en el carrito de compras del usuario.
 * @return 1 si la compra fue exitosa, -1 si no lo fue.
 */
int comprar_carrito(cancion *carrito, int num_canciones)
{
  // Variables locales.
  float total;                             // Para almacenar el total calculado de precio del carrito.
  int descriptor_socket,                   // Para el descriptor del socket.
      temporal,                            // Para operaciones temporales.
      longitud_archivo,                    // Longitud del archivo a descargar.
      datos_leidos;                        // Para llevar al cuenta de los datos recibidos.
  struct sockaddr_in informacion_servidor; // Para almacenar la información del servidor.
  char buffer[TAM_BUFFER],                 // Búffer para almacenar información.
      nombre_archivo[255];                 // El nombre del archivo final.
  FILE *archivo;                           // El archivo que escribiremos en nuestro local.

  // Mostramos mensaje de información de operación.
  printf("\nRealizando compra de canciones.\n");
  // Mostramos el carrito (y calculamos el total).
  total = visualizar_carrito(carrito, num_canciones);

  // Validamos si el carrito contiene elementos.
  if (total >= 0)
  {
    // Mensaje de inicio de proceso.
    printf("¡Compra exitosa!\n");
    printf("Inicia descarga de catálogo de canciones...\n");

    // Iteramos sobre cada una de las canciones compradas.
    for (int cancion_actual = 0; cancion_actual < num_canciones; cancion_actual++)
    {
      // Mensaje de inicio de conexión.
      printf("\t- Estableciendo conexión con el servidor para descarga...\n");

      // Creamos el socket.
      descriptor_socket = socket(
          AF_INET,     // IPv4
          SOCK_STREAM, // Socket de flujo
          0            // Cualquier protocolo.
      );

      // Validamos que se haya podido crear el socket.
      if (descriptor_socket <= 0)
      {
        // Mostramos mensaje de error.
        printf("Error: No se ha podido crear el socket.\n");
        printf("Inténtalo de nuevo más tarde.\n\n");

        // Finalizamos ejecución.
        return -1;
      }

      // Limpiamos la estructura de información del servidor.
      memset((char *)&informacion_servidor, 0, sizeof(informacion_servidor));

      // Llenamos la información del servidor.
      informacion_servidor.sin_family = AF_INET;                     // IPv4
      informacion_servidor.sin_port = htons(PUERTO_SERVIDOR);        // Puerto del servidor.
      informacion_servidor.sin_addr.s_addr = inet_addr(IP_SERVIDOR); // IP del servidor.

      // Realizamos la conexión.
      temporal = connect(
          descriptor_socket,                        // El socket a utilizaqr.
          (struct sockaddr *)&informacion_servidor, // Cast de la estructura de dirección del servidor.
          sizeof(informacion_servidor)              // Tamaño de la estructura con la información del servidor.
      );

      // Validamos la conexión.
      if (temporal < 0)
      {
        // Mostramos mensaje de error.
        printf("Error: ¡No se ha podido realizar la conexión con el servidor!\n");
        printf("(¿Está el servidor corriendo en %s:%i?)\n", IP_SERVIDOR, PUERTO_SERVIDOR);
        printf("Inténtalo de nuevo más tarde.\n\n");

        // Cerramos el socket.
        close(descriptor_socket);

        // Volvemos al menú principal.
        return -1;
      }

      // Mensaje de confirmación de conexión.
      printf("\t- Conexión establecida correctamente.\n");

      // Mostramos mensaje por canción.
      printf("\t- Iniciando descarga de: %s\n", carrito[cancion_actual].nombre);

      // Cargamos el ID de operación en el búffer.
      sprintf(buffer, "%i", OPERACION_DESCARGAR_CANCION);

      // Enviamos el ID de operación al servidor.
      temporal = send(
          descriptor_socket, // El socket a utilizar para enviar información.
          buffer,            // Buffer con la información a enviar.
          sizeof(buffer),    // Longitud del buffer.
          0                  // Sin banderas.
      );

      // Validamos que se hayan enviado datos correctamente.
      if (temporal < 0)
      {
        // Mostramos mensaje de error.
        printf("Error: No se ha podido enviar el ID de operación al servidor.\n");
        printf("Inténtalo de nuevo más tarde.\n\n");

        // Cerramos el socket.
        close(descriptor_socket);

        // Fin de ejecución.
        return -1;
      }

      // Enviamos la canción a descargar al servidor.
      temporal = send(
          descriptor_socket,                      // El socket a utilizar para enviar información.
          (const char *)&carrito[cancion_actual], // Elemento a enviar
          sizeof(carrito[cancion_actual]),        // Tamaño del elemento a enviar
          0                                       // Sin banderas
      );

      // Validamos que se haya enviado correctamente.
      if (temporal <= 0)
      {
        // Mostramos mensaje de error.
        printf("Error: No se ha podido enviar la canción solicitada para descargarla.\n");
        printf("Inténtalo de nuevo más tarde.\n\n");

        // Cerramos el socket.
        close(descriptor_socket);

        // Fin de ejecución.
        return -1;
      }

      // Recibimos la longitud de la canción a descargar desde el servidor.
      temporal = recv(
          descriptor_socket, // El socket a utilizar para enviar información.
          buffer,            // Buffer para recibir datos
          sizeof(buffer),    // El tamaño máximo del buffer a recibir
          0                  // Sin banderas
      );

      // Validamos que se haya recibido algo.
      if (temporal <= 0)
      {
        // Mostramos mensaje de error.
        printf("Error: No se ha podido recibir la longitud de la canción a descargar.\n");
        printf("Inténtalo de nuevo más tarde.\n\n");

        // Cerramos el socket.
        close(descriptor_socket);

        // Fin de ejecución.
        return -1;
      }

      // Añadimos nuestro EOF a la cadena recibida.
      buffer[temporal] = '\0';

      // Convertimos nuestra cadena en un entero.
      longitud_archivo = atoi(buffer);

      // Formamos el nombre del archivo con ruta.
      sprintf(nombre_archivo, "%s/%s", CARPETA_CLIENTE, carrito[cancion_actual].ruta_mp3);

      // Mostramos información.
      printf("\t\t- Se recibirán %i bytes desde el servidor.\n", longitud_archivo);
      printf("\t\t- Se almacenarán en: %s\n", nombre_archivo);

      // Abrimos el archivo destino.
      archivo = fopen(nombre_archivo, "w");

      // Validamos que se haya podido abrir el archivo.
      if (archivo == NULL)
      {
        // Mostramos mensaje de error.
        printf("Error: No se ha podido abrir %s para escritura.\n", nombre_archivo);
        printf("Inténtalo de nuevo más tarde.\n");

        // Rompemos la ejecución del ciclo.
        break;
      }

      // Mostramos mensajes de progreso.
      printf("\t\t- Progreso:\n");

      // Ciclo para recibir datos.
      datos_leidos = 0;
      while (datos_leidos < longitud_archivo)
      {
        // Recibir datos.
        temporal = recv(
            descriptor_socket, // El socket a utilizar para enviar información.
            buffer,            // El buffer dónde almacenar la informaación
            sizeof(buffer),    // Tamaño del buffer
            0                  // Sin banderas.
        );

        // Validamos que se haya recibido algo.
        if (temporal <= 0)
        {
          // Mostramos mensaje de error.
          printf("Error: No se ha podido recibir un paquete.\n");
          printf("Inténtalo de nuevo más tarde.\n\n");

          // Cerramos el socket.
          close(descriptor_socket);

          // Cerramos el archivo.
          fclose(archivo);

          // Fin de ejecución.
          return -1;
        }

        // Escribimos los datos en el archivo.
        datos_leidos += fwrite(
            buffer,   // Dónde leeremos los datos.
            1,        // Cuántos bytes mide cada elemento.
            temporal, // El número de elementos.
            archivo   // El archivo dónde escribiremos los datos.
        );

        // Imprimimos progreso.
        printf("\t\t\t- Recibidos %i, Total %i, Porcentaje %i%c\n", datos_leidos, longitud_archivo, (int)((datos_leidos * 100) / longitud_archivo), '%');
      }

      // Escribimos al archivo.
      fflush(archivo);

      // Cerramos el archivo.
      fclose(archivo);

      // Print de información.
      printf("\t\t- Recibido correctamente: %s\n", nombre_archivo);
      printf("\t- Cerrando conexión con el servidor.\n");

      // Cerramos el socket.
      close(descriptor_socket);
    }

    // Mensaje de confirmación de compra.
    printf("\t¡Compra realizada correctamente!\n");

    return 0;
  }

  // El carrito está vacío.
  printf("Inicie de nuevo el programa e ingrese elementos al carrito antes de finalizar su compra. Gracias.\n\n");
  return -1;
}

/**
 * @brief Función que permite visualizar el carrito de compras actual del usuario.
 *
 * @param carrito El carrito de compras actual del usuario.
 * @param num_canciones El número de elementos en el carrito de compras del usuario.
 * @return El precio total del carrito de compras o -1 si está vacío.
 */
float visualizar_carrito(cancion *carrito, int num_canciones)
{
  // Variables locales.
  float total = 0; // Para almacenar el costo total del carrito de compras.

  // Validamos si el carrito de compras está vacío.
  if (num_canciones <= 0)
  {
    // Mostramos un mensaje y finalizamos la ejecución.
    printf("\n\t¡El carrito de compras se encuentra vacío!\n\n");

    // Regresamos un -1 para indicar que no hay elementos en el carrito.
    return -1;
  }
  else
  {
    // Mostramos el encabezado de la tabla.
    printf("\nCarrito de compras:\n");
    printf("\n\n|------------------------------------------------------------|\n");
    printf("|  Nombre  |  Artista  |  Album  |  Año  |  Precio Unitario  |\n");
    printf("|------------------------------------------------------------|\n");

    // Iteramos sobre cada elemento presente en el carrito de compras del usuario.
    for (int elemento_actual = 0; elemento_actual < num_canciones; elemento_actual++)
    {
      // Mostramos el dato.
      printf("| %s ; %s ; %s ; %i ; %.2f |\n", carrito[elemento_actual].nombre, carrito[elemento_actual].artista, carrito[elemento_actual].album, carrito[elemento_actual].anio_album, carrito[elemento_actual].precio);
      printf("|------------------------------------------------------------|\n");

      // Realizamos la suma del total.
      total += carrito[elemento_actual].precio;
    }

    // Mostramos el total.
    printf("\t\t TOTAL: %.2f\n\n\n", total);

    // Regresamos el total.
    return total;
  }
}
