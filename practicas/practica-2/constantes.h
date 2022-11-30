/**
 * @file constantes.h
 * @author Roldán Morales Ana Karen / Ortega Alcocer Humberto Alejandro
 * @brief Práctica 1 - Constantes de operación
 * @date 2022-11-27
 * @copyright Copyright (c) 2022
 */
#ifndef CONSTANTES_H_INCLUIDO
#define CONSTANTES_H_INCLUIDO

// El puerto dónde el servidor escuchará conexiones entrantes.
#define PUERTO_SERVIDOR 9090
#define PUERTO_SERVIDOR_STR "9090"

// La dirección IP del servidor.
#define IP_SERVIDOR "127.0.0.1"

// El tamaño del búffer.
#define TAM_BUFFER 15000 // 15KB

// Constantes para las operaciones disponibles en el servidor.
#define OPERACION_LISTAR_CATALOGO 1
#define OPERACION_DESCARGAR_CANCION 2

// Constantes de tamaño máximo de catálogo de canciones.
#define TAM_MAX_CATALOGO_CANCIONES 10

#endif
