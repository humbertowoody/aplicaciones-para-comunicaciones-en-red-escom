/**
 * @file http.c
 * @author Roldán Morales Ana Karen / Ortega Alcocer Humberto Alejandro
 * @brief Práctica 4 - Implementación de funciones de librería HTTP.
 * @date 2022-11-27
 * @copyright Copyright (c) 2022
 */
// Librerías personalizadas.
#include "http.h" // Definición de las estructuras y prototipos de funciones.

// Librerías estándar.
#include <string.h> // Para manipulación de cadenas de caracteres
#include <stdio.h>  // Para operaciones de entrada y salida estándar

/**
 * @brief Procesa una solicitud HTTP almacenada en un búffer de caracteres
 *        y llena los campos disponibles dentro de una estructura HTTP destino.
 *
 * @param buffer El búffer de caracteres con la solicitud HTTP.
 * @param solicitud_procesada Un apuntador a la esctructura con la información obtenida.
 * @return int Un entero indicando el resultado de la operación, 0 si fue exitosa, -1 en otro caso.
 */
int procesar_solicitud(char *buffer, solicitud_http *solicitud_procesada)
{
  // Variables locales.
  char *metodo,           // El método obtenido.
      *uri,               // EL URI obtenido de la petición.
      *version,           // La versión de HTTP obtenida del búffer.
      *nombre_encabezado, // Variable para almacenar temporalmente el nombre del encabezado.
      *valor_encabezado,  // Variable para almacenar temporalmente el valor del encabezado.
      *temporal;          // Variable temporal para copiar el búffer.
  int num_encabezados;    // Variable para almacenar el número de encabezados.

  // Imprimimos la solicitud para debuggear:
  // printf("Solicitud:\n%s\n", buffer);

  // Copiamos el buffer en temporal para no afectarlo por strtok.
  strcpy(temporal, buffer);

  // Extraemos el método, el URI y la versión del protocolo.
  metodo = strtok(temporal, " \t\r\n");
  uri = strtok(NULL, " \t");
  version = strtok(NULL, " \t\r\n");

  // Copiamos los datos obtenidos a la estructura.
  strncpy(solicitud_procesada->metodo, metodo, TAM_METODO);
  strncpy(solicitud_procesada->uri, uri, TAM_URI);
  strncpy(solicitud_procesada->version, version, TAM_VERSION);

  // Tokenizamos a partir de saltos de línea (en líneas).
  for (num_encabezados = 0; num_encabezados < TAM_MAX_ENCABEZADOS && strcmp(nombre_encabezado, "Content-Length") != 0; num_encabezados++)
  {
    // Extraemos el nombre del encabezado.
    nombre_encabezado = strtok(NULL, "\r\n: \t");

    // Si no se pudo obtener, no hay más encabezados.
    if (!nombre_encabezado)
    {
      // Rompemos el ciclo.
      break;
    }

    // Copiamos el nombre en la estructura en la posición actual.
    strncpy(solicitud_procesada->encabezados[num_encabezados].nombre, nombre_encabezado, TAM_NOMBRE_ENCABEZADO);

    // Extraemos el valor del encabezado.
    valor_encabezado = strtok(NULL, "\r\n");

    if (!valor_encabezado)
    {
      break;
    }

    // Copiamos el valor del encabezado en la estructura en la posición actual.
    strncpy(solicitud_procesada->encabezados[num_encabezados].valor, valor_encabezado, TAM_VALOR_ENCABEZADO);
  }

  // Guardamos el número de encabezados leídos en nuestra estructura.
  solicitud_procesada->num_encabezados = num_encabezados;

  // Ahora seguiría procesar el payload...

  // Si todo salió bien, regreasmos 0.
  return 0;
}

/**
 * @brief Imprime una solicitud HTTP formateada (para debug).
 *
 * @param solicitud La solicitud HTTP a imprimir.
 */
void imprimir_solicitud(solicitud_http *solicitud)
{
  printf("\t\t|----------------------------------------------------------|\n");
  printf("\t\t|                       Solicitud HTTP                     |\n");
  printf("\t\t|----------------------------------------------------------|\n");
  printf("\t\t| Método       -> %-40s |\n", solicitud->metodo);
  printf("\t\t| URI          -> %-40s |\n", solicitud->uri);
  printf("\t\t| Versión HTTP -> %-40s |\n", solicitud->version);
  printf("\t\t|----------------------------------------------------------|\n");
  printf("\t\t|           Encabezados HTTP:  nombre -> valor             |\n");
  printf("\t\t|----------------------------------------------------------|\n");
  for (int i = 0; i < solicitud->num_encabezados; i++)
  {
    printf("\t\t| %-15s -> %-37s |\n", solicitud->encabezados[i].nombre, solicitud->encabezados[i].valor);
  }
  printf("\t\t|----------------------------------------------------------|\n");
}

/**
 * @brief Imprime la respuesta en la salida estándar (para debug).
 *
 * @param respuesta Un apuntador a la respuesta HTTP preparada previamente.
 */
void imprimir_respuesta(respuesta_http *respuesta)
{
  printf("\t\t|---------------------------------------------------------------|\n");
  printf("\t\t|                          Respuesta HTTP                       |\n");
  printf("\t\t|---------------------------------------------------------------|\n");
  printf("\t\t| Versión del Protocolo -> HTTP/1.1                             |\n");
  printf("\t\t| Código de Respuesta -> %-3i                                    |\n", respuesta->codigo);
  printf("\t\t|---------------------------------------------------------------|\n");
  printf("\t\t|               Encabezados HTTP: nombre -> valor               |\n");
  printf("\t\t|---------------------------------------------------------------|\n");
  printf("\t\t| Server         -> Aplicaciones para Comunicaciones en Red/1.0 |\n");
  printf("\t\t| Content-Type   -> text/html                                   |\n");
  printf("\t\t| Content-Length -> %-3zu                                         |\n", strlen(respuesta->cuerpo));
  printf("\t\t|---------------------------------------------------------------|\n");
  printf("\t\t|                            Cuerpo                             |\n");
  printf("\t\t|---------------------------------------------------------------|\n");
  printf("\t\t| ");
  for (int i = 0; i < strlen(respuesta->cuerpo); i++)
  {
    if (i % 62 == 0 && i != 0)
    {
      printf("\n\t\t| %c", respuesta->cuerpo[i]);
    }
    else
    {
      printf("%c", respuesta->cuerpo[i]);
    }
  }
  printf("\n");
  // printf("\t\t| %s |\n", respuesta->cuerpo);
  printf("\t\t|---------------------------------------------------------------|\n");
}

/**
 * @brief Serializa una respuesta HTTP a una cadena de caracteres para enviarse.
 *
 * @param buffer El buffer destino dónde irá la información serializada.
 * @param respuesta La respuesta con la información a serializar.
 */
void serializar_respuesta(char *buffer, respuesta_http *respuesta)
{
  // Variables locales.
  char *descripcion; // Variable para almacenar la descripción del código HTTP.

  // Encontramos la descripción para el código suministrado.
  switch (respuesta->codigo)
  {
  case 200:
    descripcion = "OK";
    break;

  case 404:
    descripcion = "Not Found";
    break;

  case 500:
    descripcion = "Internal Server Error";
    break;

  default:
    respuesta->codigo = 501;
    descripcion = "Not implemented";
    break;
  }

  // Realizamos el serializado de forma cruda.
  sprintf(
      buffer,                                                                                                                          // El destino de la operación.
      "HTTP/1.1 %i %s\r\nServer: Aplicaciones para Comunicaciones en Red/1.0\nContent-Type: text/html\nContent-Length: %zu\r\n\r\n%s", // El formato.
      respuesta->codigo,                                                                                                               // Segundo el código HTTP.
      descripcion,                                                                                                                     // Tercero la descripción del código HTTP.
      strlen(respuesta->cuerpo),                                                                                                       // El tamaño del cuerpo (en bytes.)
      respuesta->cuerpo                                                                                                                // El "cuerpo", código en HTML.
  );
}
