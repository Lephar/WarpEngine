#include <unistd.h>
#include <pthread.h>
#include <uuid/uuid.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>

#include "network.h"

#define UUID_STRING_SIZE 37
#include <stdio.h>

int32_t server;
pthread_t listener, receiver, sender;

void establishConnection(void) {
	server = socket(AF_INET, SOCK_STREAM, 0);
	setsockopt(server, IPPROTO_TCP, TCP_NODELAY, &(int32_t) {1}, sizeof(int32_t));
	setsockopt(server, IPPROTO_IP, IP_TOS, &(uint8_t) {IPTOS_LOWDELAY}, sizeof(uint8_t));

	struct sockaddr_in serverAddress = {
			.sin_family = AF_INET,
			.sin_port = htons(7076)
	};
	inet_pton(AF_INET, "10.0.0.30", &serverAddress.sin_addr);

	(void) connect(server, (struct sockaddr*) &serverAddress, sizeof(struct sockaddr_in));

	uuid_t uuidBinary;
	recv(server, uuidBinary, sizeof(uuid_t), MSG_WAITALL);

	char uuidString[UUID_STRING_SIZE];
	uuid_unparse_upper(uuidBinary, uuidString);
	printf("%s\n", uuidString);

	close(server);
}

_Noreturn void* startReceiver(__attribute__((unused)) void* parameter) {
	while(1){
	}
}

_Noreturn void* startSender(__attribute__((unused)) void* parameter) {
	while(1){
	}
}

void startDaemon(void) {
	establishConnection();

	//pthread_create(&receiver, NULL, startReceiver, NULL);
	//pthread_create(&sender, NULL, startSender, NULL);
}

void stopDaemon(void) {
	//pthread_cancel(receiver);
	//pthread_cancel(sender);

	//pthread_join(receiver, NULL);
	//pthread_join(sender, NULL);

	//close(server);
}
