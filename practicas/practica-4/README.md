# Práctica 4 - Servidor HTTP básico

En esta práctica se desarrolló un servidor básico de HTTP que permite atender
las instrucciones básicas: `GET`, `POST`, `DELETE`, `PUT` y `HEAD`.

## Compilación

Para compilar el programa usamos `make` el cual toma la configuración del `Makefile` en esta carpeta.

Para compilar el servidor, se usa el siguiente comando:

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
./servidor
```

Para ejecutar el servidor.

### Pruebas con Postman

Para facilitar las pruebas del servidor se incluye una colección de [Postman][url-postman] con la cual se podrán hacer pruebas de forma fácil y rápida.

La colección se llama `Práctica 4 - Servidor HTTP.postman_collection.json` y para
utilizarla deberás importarla en tu [Postman local][url-postman] y podrás utilizarla
para probar cada una de las rutas en el servidor.

## Configuración del programa.

Para modificar los parámetros de configuración de programa, se deben considerar
dos archivos principales: `servidor.c` y `http.h`. En el primero se modifican 
las opciones del _servidor_ en sí mismo y en el otro la operación y límites de HTTP.

### Opciones en `servidor.c`

En este archivo las opciones que podemos modificar son:

- `PUERTO_SERVIDOR`: una cadena con el puerto dónde se ejecutará nuestro servidor.
- `TAM_BUFFER`: un entero con el tamaño máximo del búffer.

### Opciones en `http.h`

En este archivo se definen las estructuras y operación de la abstracción sobre HTTP
que realizamos, por ello, las opciones disponibles son:

- `TAM_METODO`: el número de bytes para almacenar el método HTTP usado.
- `TAM_URI`: el número de bytes para almacenar la URI.
- `TAM_VERSION`: el número de bytes para almacenar la versión del protocolo HTTP.
- `TAM_DESCRIPCION`: el número de bytes para la descripción del código de respuesta HTTP.
- `TAM_NOMBRE_ENCABEZADO`: el número de bytes para el nombre de cada encabezado HTTP.
- `TAM_VALOR_ENCABEZADO`: el número de bytes para el valor de cada en cabezado HTTP.
- `TAM_MAX_ENCABEZADOS`: el número máximo de encabezados HTTP que puede tener una solicitud o respuesta.
- `TAM_CUERPO`: el número máximo de bytes para el cuerpo de la solicitud o respuesta HTTP.

## Equipo de trabajo

- Roldán Morales Ana Karen
- Ortega Alcocer Humberto Alejandro

## Datos de la Materia

- Aplicaciones para comunicaciones en red
- 3CM13
- Escuela Superior de Cómputo del Instituto Politécnico Nacional

[url-postman]: https://www.postman.com/downloads/
