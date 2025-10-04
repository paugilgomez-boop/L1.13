#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>

int main(int argc, char *argv[]) {
	
	int sock_conn, sock_listen, ret;
	struct sockaddr_in serv_adr;
	char peticion[512];
	char respuesta[512];
		
		// INITIALITZATIONS
		// Obrim el socket:
		if ((sock_listen = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
			printf("Error creant socket\n");
	}
	
	// Fem el bind al port
	memset(&serv_adr, 0, sizeof(serv_adr)); // inicialitza a zero serv_adr
	serv_adr.sin_family = AF_INET;
	// associa el socket a una IP local y puerto
	serv_adr.sin_addr.s_addr = htonl(INADDR_ANY);   // todas las interfaces
	serv_adr.sin_port = htons(9400);                // puerto 9050
	
	if (bind(sock_listen, (struct sockaddr *)&serv_adr, sizeof(serv_adr)) < 0)
		printf("Error al bind\n");
	
	if (listen(sock_listen, 3) < 0)
		printf("Error en el listen\n");
	
	int i;
	// Atenderemos solo 5 peticiones
	for (;;)
	{
		printf("Escuchando\n");
		
		sock_conn = accept(sock_listen, NULL, NULL);
		printf("He recibido conexion\n");
		// sock_conn es el socket que usaremos para este cliente
		
		// Bucle de atenci�n al cliente
		int terminar = 0;
		while (terminar == 0){
		
			ret = read(sock_conn, peticion, sizeof(peticion));
			printf("Recibido\n");
			
			// Tenemos que añadir la marca de fin de string
			// para que no escriba lo que hay despues en el buffer
			peticion[ret] = '\0';
			
			printf("Peticion: %s\n", peticion);
			
			// vamos a ver qué quieren
			char *p = strtok(peticion, "/");
			int codigo = atoi(p);
			char nombre[20];
			
			if (codigo != 0){
				p = strtok(NULL, "/");
				strcpy(nombre, p);
				printf("Codigo: %d, Nombre: %s\n", codigo, nombre);
			}
			
			if (codigo == 0)
				terminar = 1;
			else if (codigo == 1) // piden la longitud del nombre
				sprintf(respuesta, "%d", (int)strlen(nombre));
			else if (codigo == 2)
			
				// quieren saber si el nombre es bonito
				if ((nombre[0] == 'M') || (nombre[0] == 'S'))
					strcpy(respuesta, "SI");
				else
					strcpy(respuesta, "NO");
			
			else if (codigo == 3)
			{
				p = strtok(NULL, "/");
				float altura = atof(p);
				if (altura > 1.70)
					sprintf(respuesta, "%s : Eres alto", nombre);
				else
					sprintf(respuesta, "%s : Eres bajo", nombre);
				
			}
			else if (codigo == 4){
				int i = 0, j = (int)strlen(nombre) - 1;
				while (i < j && nombre[i] == nombre[j]) { i++; j--; }
				
				if (i < j)
					strcpy(respuesta, "NO");
				else
					strcpy(respuesta, "SI");
			}
			else{
				for (int k = 0; nombre[k] != '\0'; ++k)
					nombre[k] = (char)toupper((unsigned char)nombre[k]);
				sprintf(respuesta, nombre);
			}
			if (codigo != 0){
				printf("Respuesta: %s\n", respuesta);
				// y lo enviamos
				write(sock_conn, respuesta, strlen(respuesta));
			}
		}
		close(sock_conn);
	}
}

