/**
 * @file mensaje.h
 * @author Roldán Morales Ana Karen / Ortega Alcocer Humberto Alejandro
 * @brief Práctica 3 - Cabecera con la definición del tipo "Mensaje" y funciones relacionadas.
 * @date 2022-11-27
 * @copyright Copyright (c) 2022
 */
#ifndef MENSAJE_H_INCLUIDO
#define MENSAJE_H_INCLUIDO

// Librerías locales.
#include "usuario.h" // Para la estructura de usuario.

// Constantes.
#define LONGITUD_MAXIMA_CONTENIDO 1024 // El contenido máximo del mensaje.

// Enumeración para representar los tipos de mensaje.
typedef enum
{
  CONEXION = 1,    // Representa una conexión nueva de un usuario.
  DESCONEXION = 2, // Representa la desconexión de un usuario.
  MSJ_GENERAL = 3, // Representa un mensaje para todos
  MSJ_PRIVADO = 4  // Representa un mensaje privdo.
} tipo_mensaje;    // Definición de tipo tipo_mensaje.

// Estructura que representa un mensaje.
typedef struct st_mensaje
{
  tipo_mensaje tipo;                         // El tipo del mensaje.
  char contenido[LONGITUD_MAXIMA_CONTENIDO]; // El contenido del mensaje.
  usuario autor,                             // Información sobre el autor del mensaje.
      destinatario;                          // Información sobre el destinatario del mensaje.
} mensaje;                                   // Definición de tipo mensaje.

// Funciones.
void imprimir_mensaje(mensaje); // Función que imprime un mensaje.

#endif // MENSAJE_H_INCLUIDO
