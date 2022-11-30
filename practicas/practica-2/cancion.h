/**
 * @file cancion.h
 * @author Roldán Morales Ana Karen / Ortega Alcocer Humberto Alejandro
 * @brief Práctica 3 - Biblioteca de "Canción"
 * @date 2022-11-27
 * @copyright Copyright (c) 2022
 */
#ifndef CANCION_H_INCLUIDO
#define CANCION_H_INCLUIDO

// Constantes para canciones.
#define LONGITUD_NOMBRE_CANCION 255
#define LONGITUD_ARTISTA 255
#define LONGITUD_ALBUM 255
#define LONGITUD_RUTA_MP3 255

// Estructura que representa una canción.
typedef struct
{
  char nombre[LONGITUD_NOMBRE_CANCION], // El nombre de la canción
      artista[LONGITUD_ARTISTA],        // El nombre del artista
      album[LONGITUD_ALBUM],            // El nombre del álbum
      ruta_mp3[LONGITUD_RUTA_MP3];      // La dirección del archivo (en el servidor, el cliente ignora este campo totalmente)
  int anio_album;                       // Año de publicación del álbum
  float precio;                         // Precio de la canción.
} cancion;

#endif // CANCION_H_INCLUIDO
