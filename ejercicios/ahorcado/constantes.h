/**
 * @file constantes.h
 * @author Roldán Morales Ana Karen / Ortega Alcocer Humberto Alejandro
 * @brief Tarea - Juego de Ahorcado - constantes de operación del juego (para prevenir errores de conexión)
 * @date 2022-10-18
 * @copyright Copyright (c) 2022
 */

// El puerto dónde estará escuchando nuestro servidor.
#ifndef PUERTO_SERVIDOR
#define PUERTO_SERVIDOR 8080
#endif // PUERTO_SERVIDOR

// La dirección IP de nuestro servidor.
#ifndef IP_SERVIDOR
#define IP_SERVIDOR "127.0.0.1"
#endif // IP_SERVIDOR

// El tamaño de búffer que usará nuestro juego.
#ifndef TAM_BUFFER
#define TAM_BUFFER 512
#endif // TAM_BUFFER

// La cadena que indicará que el usuario perdió por límite de intentos (algo único pa' prevenir tramposos que le sepan a Wireshark >:c)
#ifndef CADENA_FIN_DE_JUEGO
#define CADENA_FIN_DE_JUEGO "uyseacaboeljuegolastimamargarito"
#endif // CADENA_FIN_DE_JUEGO

// La cadena que indicará que el usuario ganó (algo único pa' prevenir tramposos que le sepan a Wireshark >:c)
#ifndef CADENA_GANADOR
#define CADENA_GANADOR "wiiiiiifelicidadesyeeeiiiiwujuuuuu"
#endif // CADENA_GANADOR

// Las distintas opciones de dificultad.
#define OPCION_DIFICULTAD_FACIL 1
#define OPCION_DIFICULTAD_MEDIA 2
#define OPCION_DIFICULTAD_DIFICIL 3
#define LONGITUD_MAXIMA_FACIL 4
#define LONGITUD_MAXIMA_MEDIA 8
// -- No definimos longitud máxima para la dificultad "difícil" pues será infinita.
