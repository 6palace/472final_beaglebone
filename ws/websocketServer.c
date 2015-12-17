#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include "./libwebsockets/lib/libwebsockets.h"


static int callback_http(struct libwebsocket_context * this,
								 struct libwebsocket *wsi,
								 enum libwebsocket_callback_reasons reason, void *user,
								 void *in, size_t len);

static int callback_robot_cmd(struct libwebsocket_context * this,
						 struct libwebsocket *wsi,
						 enum libwebsocket_callback_reasons reason, void *user,
						 void *in, size_t len);

static char storedTemp[512];

static struct libwebsocket_protocols protocols[] = {
	/* first protocol must always be HTTP handler */
	{
		"http-only",   // name
		callback_http, // callback
		0              // per_session_data_size
	},
	{
		"robot-cmd-protocol",   // protocol name
		callback_robot_cmd,     // callback
		0                       // per_session_data_size

	},
	{
		NULL, NULL, 0   /* End of list */
	}
};

int main(void) {
	int port = 9000;
	const char *interface = NULL;
	struct libwebsocket_context *context;
	const char *cert_path = NULL;
	const char *key_path = NULL;
	int opts = 0;
	int fd;

	struct lws_context_creation_info info;



   	system("rm /tmp/toWebsocket");
   mknod("/tmp/toWebsocket", S_IFIFO, 0);

	memset(&info, 0, sizeof info);
	info.port = port;
	info.iface = interface;
	info.protocols = protocols;
	info.ssl_cert_filepath = NULL;
	info.ssl_private_key_filepath = NULL;
	info.gid = -1;
	info.uid = -1;
	info.options = opts;

	context = libwebsocket_create_context(&info);

	if (context == NULL) {
		fprintf(stderr, "libwebsocket init failed\n");
		return -1;
	}
   
	printf("Server Started.\n");


   fd = open("/tmp/toWebsocket", O_RDONLY | O_NONBLOCK);
   fcntl(fd, F_SETFL, O_NONBLOCK);

   int mask = fcntl(fd, F_GETFL);

	while (1) {
		//libwebsocket_service(context, 50);
		char databuf[1024] = "";
		libwebsocket_service(context, 50); //set to 0ms on single threaded app according to https://libwebsockets.org/libwebsockets-api-doc.html
		printf("anded: %d\n", mask & O_NONBLOCK == O_NONBLOCK);
		//do more single threaded stuff here
		ssize_t r = read(fd, databuf, 100);
		if (r == -1 && errno == EAGAIN){
		    // no data yet
		    printf("errno is EAGAIN\n");
		} else if (r > 0){
		    // received data
		    // take databuf and process it
		    printf("recieved temp info\n");
		    strncpy(storedTemp, databuf, strlen(databuf));
		} else{
			//closed pipe
			printf("closed pipe\n");
		}


	}

	libwebsocket_context_destroy(context);
	return 0;
}

static int callback_http(struct libwebsocket_context * this,
						 struct libwebsocket *wsi,
						 enum libwebsocket_callback_reasons reason, void *user,
						 void *in, size_t len)
{
	return 0;
}

static int callback_robot_cmd(struct libwebsocket_context * this,
						 struct libwebsocket *wsi,
						 enum libwebsocket_callback_reasons reason, void *user,
						 void *in, size_t len)
{
	switch (reason) {
		case LWS_CALLBACK_ESTABLISHED:
			printf("Client Connected\n");
			break;
		case LWS_CALLBACK_RECEIVE:
			//From documentation: data has appeared for the server, it can be found at *in and is len bytes long 
			printf("Data Recieved: %s\n", (char*)in);

			if(!strncmp((char*) in, "ROBOTEMP", 8)){
			//send something back
				libwebsocket_callback_on_writable(this, wsi); //call this to get a LWS_CALLBACK_SERVER_WRITEABLE
			} else{
				FILE* fwsOut = fopen("/tmp/fromWebsocket", "w");
				fprintf(fwsOut, "%s\n", in);
				fflush(fwsOut);
				fclose(fwsOut);
			}

			break;
		case LWS_CALLBACK_SERVER_WRITEABLE:
			printf("Sending Data\n");
			int storedTempLen = strlen(storedTemp)+1;

			int bufSize = LWS_SEND_BUFFER_PRE_PADDING + storedTempLen + LWS_SEND_BUFFER_POST_PADDING;
			char* buf = (char*) malloc(bufSize);
			strncpy(buf+LWS_SEND_BUFFER_PRE_PADDING, storedTemp, storedTempLen);
			int howmuchwrote = lws_write(wsi, buf+LWS_SEND_BUFFER_PRE_PADDING, storedTempLen, LWS_WRITE_TEXT);

			free(buf);
			printf("Sent Data : %d bytes %d %s\n", howmuchwrote, strlen(storedTemp)+1, buf);
			break;
	}
	
	return 0;
}
