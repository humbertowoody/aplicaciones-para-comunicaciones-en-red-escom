# Práctica 2

En esta práctica se pretende realizar un carrito de compras de música dónde el
usuario pueda adquirir distintas canciones y las pueda descargar a su
local.

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
- Tamaño del búffer de lectura/escritura (para cliente y servidor).
- Tamaño máximo del catálogo de canciones.

## Equipo de trabajo

- Roldán Morales Ana Karen
- Ortega Alcocer Humberto Alejandro

## Datos de la Materia

- Aplicaciones para comunicaciones en red
- 3CM13
- Escuela Superior de Cómputo del Instituto Politécnico Nacional
