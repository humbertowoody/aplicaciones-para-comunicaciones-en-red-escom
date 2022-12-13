/**
 * @file mensaje.c
 * @author Roldán Morales Ana Karen / Ortega Alcocer Humberto Alejandro
 * @brief Práctica 3 - Implementación de funciones en cabecera de "Mensaje"
 * @date 2022-11-27
 * @copyright Copyright (c) 2022
 */
// Librerías locales.
#include "mensaje.h" // Definición de tipo "mensaje" y prototipos de funciones.

// Librerías de sistema.
#include <stdio.h> // Funciones de entrada y salida.

// Implementación de funciones.

/**
 * @brief Función que imprime un mensaje en la salida estándar.
 *
 * @param chat El mensaje a imprimir.
 */
void imprimir_mensaje(mensaje chat)
{
  // Imprimimos el mensaje acorde al tipo.
  switch (chat.tipo)
  {
  // Un mensaje de conexión de un usuario.
  case CONEXION:
    printf("[%s (%i)] se ha unido a la sala.\n", chat.autor.nombre, chat.autor.edad);
    break;

  // Un mensaje de desconexión de un usuario.
  case DESCONEXION:
    printf("[%s (%i)] ha abandonado el chat.\n", chat.autor.nombre, chat.autor.edad);
    break;

  // Un mensaje privado.
  case MSJ_PRIVADO:
    printf("[%s (%i) -> %s (%i)]: %s\n", chat.autor.nombre, chat.autor.edad, chat.destinatario.nombre, chat.destinatario.edad, chat.contenido);
    break;

  // Un mensaje general.
  case MSJ_GENERAL:
    printf("[%s (%i)]: %s\n", chat.autor.nombre, chat.autor.edad, chat.contenido);
    break;

  // En cualquier otro caso, mostraremos un error.
  default:
    printf("Error: Se recibió un mensaje con tipo desconocido.\n");
    break;
  }
}
