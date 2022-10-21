# Arquitectura TCP/IP

Capas:

- Aplicación
- Transporte

> Todo viaja a través del protocolo IP, por lo que realmente es TCP o UDP el protocolo responsable de verificar que la información haya llegado correctamente.

- ICMP: sirve para saber si los paquetes están siendo rechazados, si la IP no existe, si la red no sirve (se cayó el router etc).



## Protocolo UDP

Es un protocolo que ofrece servicio de transporte 

### ¿Qué no ofrece UDP?

- Control de flujo
	- No proporciona control de flujo

### Usos de UDP

## Protocolo TCP

- Es un estándar del IETF, RFC 79

### Características TCP

- Orientado a conexión
	- Antes de poder transferir información, se debe establecer la conexión. Este proceso se conoce como: _Handshake_.
- Full Dúplex
- Fiable
