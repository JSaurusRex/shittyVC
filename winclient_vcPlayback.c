#include<stdio.h>
#include<winsock2.h>

#define MINIAUDIO_IMPLEMENTATION
#include "miniaudio.h"

#pragma comment(lib,"ws2_32.lib") //Winsock Library

int sockfd = 0;
int counter = 0;
INT16 bufferI[4410];
INT16 bufferO[4410];
int newBuffer = 0;
#define PORT 8080

void data_callbackC(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount)
{
    // MA_ASSERT(pDevice->capture.format == pDevice->playback.format);
    // MA_ASSERT(pDevice->capture.channels == pDevice->playback.channels);

    /* In this example the format and channel count are the same for both input and output which means we can just memcpy(). */
    // if(newBuffer < 2)
    // {
        memcpy(bufferI, pInput, 4410*2);
        //memcpy(pOutput, bufferO, 4410*2);
        newBuffer++;
    // }else
        //printf("missed buffer\n");

    //write(sockfd, pInput, 4410*2);
    //read(sockfd, pOutput, 4410*2);
    
    //MA_COPY_MEMORY(pOutput, pInput, frameCount * ma_get_bytes_per_frame(pDevice->capture.format, pDevice->capture.channels));
    printf("size: %i   counter: %i\n", frameCount, counter++);
}

void data_callbackP(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount)
{
    // MA_ASSERT(pDevice->capture.format == pDevice->playback.format);
    // MA_ASSERT(pDevice->capture.channels == pDevice->playback.channels);

    /* In this example the format and channel count are the same for both input and output which means we can just memcpy(). */
    // if(newBuffer < 2)
    // {
        //memcpy(bufferI, pInput, 4410*2);
        memcpy(pOutput, bufferO, 4410*2);
        newBuffer++;
    // }else
        //printf("missed buffer\n");

    //write(sockfd, pInput, 4410*2);
    //read(sockfd, pOutput, 4410*2);
    
    //MA_COPY_MEMORY(pOutput, pInput, frameCount * ma_get_bytes_per_frame(pDevice->capture.format, pDevice->capture.channels));
    printf("size: %i   counter: %i\n", frameCount, counter++);
}

int main(int argc , char *argv[])
{
	WSADATA wsa;
	SOCKET s;
	struct sockaddr_in server;
	int recv_size;
    if(argc < 2)
    {
        printf("give ip >:(\n");
        return 0;
    }

	printf("\nInitialising Winsock...");
	if (WSAStartup(MAKEWORD(2,2),&wsa) != 0)
	{
		printf("Failed. Error Code : %d",WSAGetLastError());
		return 1;
	}
	
	printf("Initialised.\n");
	
	//Create a socket
	if((s = socket(AF_INET , SOCK_STREAM , 0 )) == INVALID_SOCKET)
	{
		printf("Could not create socket : %d" , WSAGetLastError());
	}

	printf("Socket created.\n");
	
	
	server.sin_addr.s_addr = inet_addr(argv[1]);
	server.sin_family = AF_INET;
	server.sin_port = htons( PORT );

	//Connect to remote server
	if (connect(s , (struct sockaddr *)&server , sizeof(server)) < 0)
	{
		puts("connect error");
		return 1;
	}
	
	puts("Connected");

    ma_result result;
    ma_encoder_config encoderConfig;
    //ma_encoder encoder;
    ma_device_config deviceConfig;
    ma_device device;
    //encoderConfig = ma_encoder_config_init(ma_encoding_format_wav, ma_format_s16, 1, 44100);

    // if (ma_encoder_init_file("ugh.mp3", &encoderConfig, &encoder) != MA_SUCCESS) {
    //     printf("Failed to initialize output file.\n");
    //     return -1;
    // }

    deviceConfig = ma_device_config_init(ma_device_type_capture);
    deviceConfig.capture.format   = ma_format_s16;
    deviceConfig.playback.format   = ma_format_s16;
    deviceConfig.capture.channels = 1;
    deviceConfig.playback.channels = 1;
    deviceConfig.sampleRate       = 44100;
    deviceConfig.dataCallback     = data_callbackC;
    //deviceConfig.pUserData        = &encoder;
    deviceConfig.periodSizeInMilliseconds = 100;

    // result = ma_device_init(NULL, &deviceConfig, &device);
    // if (result != MA_SUCCESS) {
    //     printf("Failed to initialize capture device.\n");
    //     return -2;
    // }

    // result = ma_device_start(&device);
    // if (result != MA_SUCCESS) {
    //     ma_device_uninit(&device);
    //     printf("Failed to start device.\n");
    //     return -3;
    // }

    ma_device_config deviceConfigP;
    ma_device deviceP;

    deviceConfigP = ma_device_config_init(ma_device_type_playback);
    deviceConfigP.capture.format   = ma_format_s16;
    deviceConfigP.playback.format   = ma_format_s16;
    deviceConfigP.capture.channels = 1;
    deviceConfigP.playback.channels = 1;
    deviceConfigP.sampleRate       = 44100;
    deviceConfigP.dataCallback     = data_callbackP;
    //deviceConfigP.pUserData        = &encoder;
    deviceConfigP.periodSizeInMilliseconds = 100;

    result = ma_device_init(NULL, &deviceConfigP, &deviceP);
    if (result != MA_SUCCESS) {
        printf("Failed to initialize capture device.\n");
        return -2;
    }

    result = ma_device_start(&deviceP);
    if (result != MA_SUCCESS) {
        ma_device_uninit(&deviceP);
        printf("Failed to start device.\n");
        return -3;
    }

    int time = 0;
    while(1)
    {
        if(newBuffer !=0)
        {
            bufferI[0] = time;
            send(s, bufferI, 4410*2, 0);
            recv(s, bufferO, 4410*2, 0);
            time = bufferO[0];
            bufferO[0] = 0;
            newBuffer = 0;
        }
        Sleep(5);
    }
    //getchar();
	
	//Send some data
	// message = "GET / HTTP/1.1\r\n\r\n";
	// if( send(s , message , strlen(message) , 0) < 0)
	// {
	// 	puts("Send failed");
	// 	return 1;
	// }
	// puts("Data Send\n");
	
	// //Receive a reply from the server
	// if((recv_size = recv(s , server_reply , 2000 , 0)) == SOCKET_ERROR)
	// {
	// 	puts("recv failed");
	// }
	
	// puts("Reply received\n");

	// //Add a NULL terminating character to make it a proper string before printing
	// //server_reply[recv_size] =;
	// puts(server_reply);

	return 0;
}