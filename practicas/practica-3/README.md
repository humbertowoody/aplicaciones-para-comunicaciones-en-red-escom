# Práctica 3

En esta práctica realizamos una aplicación Cliente-Servidor de Chat. Para esto, utilizamos técnicas de
hilos y sincronización para usar apropiadamente las estructuras y poder comunicar eficientemente las
operaciones.

## Compilación

Para compilar el programa usamos `make` el cual toma la configuración del `Makefile` en esta carpeta.

Para compilar el cliente y/o servidor, se usa el siguiente comando:

```sh
make
```

Para limpiar (eliminar) los archivos compilados (archivos objeto + binarios), podemos usar el siguiente comando:

```sh
make clean
```

## Ejecución

Para ejecutar cualquiera de los programas, bastará con usar:

```sh
./cliente
```

Para ejecutar el cliente, o bien:

```sh
./servidor
```

Para ejecutar el servidor.

## Configuración del programa

Para modificar los parámetros de configuración del programa, se tendrá que
modificar el archivo `constantes.h`, el cual contiene las siguientes opciones:

- Puerto del Servidor.
- IP del Servidor.

A su vez, se puede modificar la longitud máxima del mensaje en `mensaje.h` y la longitud
máxima del nombre de usuario en `usuario.h`.


## Equipo de trabajo

- Roldán Morales Ana Karen
- Ortega Alcocer Humberto Alejandro

## Datos de la Materia

- Aplicaciones para comunicaciones en red
- 3CM13
- Escuela Superior de Cómputo del Instituto Politécnico Nacional
