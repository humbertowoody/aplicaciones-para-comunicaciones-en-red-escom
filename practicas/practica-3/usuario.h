/**
 * @file usuario.h
 * @author Roldán Morales Ana Karen / Ortega Alcocer Humberto Alejandro
 * @brief Práctica 3 - Cabecera con la definición del tipo "Usario" y funciones relacionadas.
 * @date 2022-11-27
 * @copyright Copyright (c) 2022
 */
#ifndef USUARIO_H_INCLUIDO
#define USUARIO_H_INCLUIDO

// Constantes.
#define LONGITUD_MAXIMA_USUARIO_NOMBRE 64 // Considerando nombres larguísimos.

// Estructura que representa un usuario.
typedef struct st_usuario
{
  char nombre[LONGITUD_MAXIMA_USUARIO_NOMBRE]; // El nombre del usuario.
  int edad;                                    // La edad del usuario.
} usuario;

#endif // USUARIO_H_INCLUIDO
