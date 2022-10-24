# Práctica 1

Realizar un programa que permita enviar/recibir archivos hacia/desde un servidor. El programa deberá permitir las siguientes funcionalidades:

- Listar los contenidos de la carpeta del servidor.
- Enviar archivos/carpetas.
- Descargar archivos/carpetas.

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

## Equipo de trabajo

- Roldán Morales Ana Karen
- Ortega Alcocer Humberto Alejandro

## Datos de la Materia

- Aplicaciones para comunicaciones en Red
- 3CM13
- Escuela Superior de Cómputo del Instituto Politécnico Nacional
