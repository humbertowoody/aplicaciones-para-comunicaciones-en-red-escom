/**
 * @file http.h
 * @author Roldán Morales Ana Karen / Ortega Alcocer Humberto Alejandro
 * @brief Práctica 4 - Librería con estructuras y funciones para uso de HTTP
 * @date 2022-11-27
 * @copyright Copyright (c) 2022
 */
#ifndef HTTP_INCLUIDO
#define HTTP_INCLUIDO

// Constantes de operación.
#define TAM_METODO 8              // 8 bytes para el método.
#define TAM_URI 256               // 256 bytes para la URI.
#define TAM_VERSION 16            // 16 bytes para la versión HTTP.
#define TAM_DESCRIPCION 256       // 256 bytes para la descripción del código HTTP usado.
#define TAM_NOMBRE_ENCABEZADO 256 // 256 bytes para el nombre del encabezado
#define TAM_VALOR_ENCABEZADO 1024 // 1024 bytes para el valor del encabezado.
#define TAM_MAX_ENCABEZADOS 10    // Número máximo de encabezados en una solicitud/respuesta HTTP.
#define TAM_CUERPO 4096           // 4096 bytes para el cuerpo de la solicitud/respuesta.

/**
 * @brief Estructura que define un encabezado HTTP.
 *
 */
typedef struct st_encabezado_http
{
  char nombre[TAM_NOMBRE_ENCABEZADO], // El nombre del encabezado HTTP.
      valor[TAM_VALOR_ENCABEZADO];    // El valor del encabezado HTTP.
} encabezado_http;

/**
 * @brief Estructura que define una solicitud HTTP
 *
 */
typedef struct st_solicitud_http
{
  char metodo[TAM_METODO],                          // El método empleado en la solicitud.
      uri[TAM_URI],                                 // El URI de la petición.
      version[TAM_VERSION];                         // La versión de HTTP usada.
  encabezado_http encabezados[TAM_MAX_ENCABEZADOS]; // Los encabezados de la solicitud.
  int num_encabezados;                              // Para almacenar el número de encabezados.
} solicitud_http;

/**
 * @brief Estructura que define una respuesta HTTP.
 *
 */
typedef struct st_respuesta_http
{
  char cuerpo[TAM_CUERPO]; // El cuerpo de la respuesta (texto HTML).
  int codigo;              // El código HTTP de la respuesta.

} respuesta_http;

// Prototipos de funciones.
int procesar_solicitud(char *buffer, solicitud_http *solicitud_procesada); // Para procesar un búffer en una solicitud HTTP.
void imprimir_solicitud(solicitud_http *solicitud);                        // Imprime una solicitud HTTP.
void serializar_respuesta(char *buffer, respuesta_http *respuesta);        // Para serializar una respuesta HTTP.
void imprimir_respuesta(respuesta_http *respuesta);                        // Imprimir respuesta HTTP.

#endif // HTTP_INCLUIDO
