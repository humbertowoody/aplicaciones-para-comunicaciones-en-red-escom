/**
 * @file constantes.h
 * @author Roldán Morales Ana Karen / Ortega Alcocer Humberto Alejandro
 * @brief Práctica 1 - Constantes de operación
 * @date 2022-10-18
 * @copyright Copyright (c) 2022
 */
#ifndef CONSTANTES_H_INCLUIDO
#define CONSTANTES_H_INCLUIDO

// El puerto dónde el servidor escuchará conexiones entrantes.
#define PUERTO_SERVIDOR 8080

// La dirección IP del servidor.
#define IP_SERVIDOR "127.0.0.1"

// El tamaño del búffer.
#define TAM_BUFFER 512

// Opciones del socket.
#define ALGORITMO_NAGLE_ACTIVADO 1
#define TEMPORIZADOR_LECTURA_SEGUNDOS 10

// Constantes para las operaciones disponibles.
#define OPERACION_LISTAR_ARCHIVOS 1
#define OPERACION_SUBIR_ARCHIVOS 2
#define OPERACION_DESCARGAR_ARCHIVOS 3

// Nombre del archivo zip a enviar/recibir para intercambio de archivos/carpetas.
#define NOMBRE_ARCHIVO_ZIP_CLIENTE "comprimido_datos_transmision_c"
#define NOMBRE_ARCHIVO_ZIP_SERVIDOR "comprimido_datos_transmision_s"

#endif
