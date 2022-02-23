/*
Demonstrates how to capture data from a microphone using the low-level API.

This example simply captures data from your default microphone until you press Enter. The output is saved to the file
specified on the command line.

Capturing works in a very similar way to playback. The only difference is the direction of data movement. Instead of
the application sending data to the device, the device will send data to the application. This example just writes the
data received by the microphone straight to a WAV file.
*/
#define MINIAUDIO_IMPLEMENTATION
#include "miniaudio.h"

#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>

#define MAX 80
#define PORT 8080
#define SA struct sockaddr
int sockfd = 0;
int counter = 0;
int8_t bufferI[2205];
int8_t bufferO[2205];
void data_callback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount)
{
    MA_ASSERT(pDevice->capture.format == pDevice->playback.format);
    MA_ASSERT(pDevice->capture.channels == pDevice->playback.channels);

    /* In this example the format and channel count are the same for both input and output which means we can just memcpy(). */
    memcpy(bufferI, pInput, frameCount);
    memcpy(pOutput, bufferO, frameCount);
    
    //MA_COPY_MEMORY(pOutput, pInput, frameCount * ma_get_bytes_per_frame(pDevice->capture.format, pDevice->capture.channels));
    printf("size: %i   counter: %i\n", frameCount, counter++);
}

int main(int argc, char** argv)
{
    ma_result result;
    ma_encoder_config encoderConfig;
    ma_encoder encoder;
    ma_device_config deviceConfig;
    ma_device device;

    if (argc < 2) {
        printf("No output file.\n");
        return -1;
    }

    encoderConfig = ma_encoder_config_init(ma_encoding_format_wav, ma_format_u8, 1, 44100);

    // if (ma_encoder_init_file(argv[1], &encoderConfig, &encoder) != MA_SUCCESS) {
    //     printf("Failed to initialize output file.\n");
    //     return -1;
    // }

    deviceConfig = ma_device_config_init(ma_device_type_duplex);
    deviceConfig.capture.format   = ma_format_u8;
    deviceConfig.playback.format   = ma_format_u8;
    deviceConfig.capture.channels = 1;
    deviceConfig.playback.channels = 1;
    deviceConfig.sampleRate       = 44100;
    deviceConfig.dataCallback     = data_callback;
    deviceConfig.pUserData        = &encoder;
    deviceConfig.periodSizeInMilliseconds = 50;

    int connfd;
	struct sockaddr_in servaddr, cli;

	// socket create and verification
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd == -1) {
		printf("socket creation failed...\n");
		exit(0);
	}
	else
		printf("Socket successfully created..\n");
	bzero(&servaddr, sizeof(servaddr));

	// assign IP, PORT
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = inet_addr(argv[1]);
	servaddr.sin_port = htons(PORT);

	// connect the client socket to server socket
	if (connect(sockfd, (SA*)&servaddr, sizeof(servaddr)) != 0) {
		printf("connection with the server failed...\n");
		exit(0);
	}
	else
		printf("connected to the server..\n");

    result = ma_device_init(NULL, &deviceConfig, &device);
    if (result != MA_SUCCESS) {
        printf("Failed to initialize capture device.\n");
        return -2;
    }

    result = ma_device_start(&device);
    if (result != MA_SUCCESS) {
        ma_device_uninit(&device);
        printf("Failed to start device.\n");
        return -3;
    }

    //printf("Press Enter to stop recording...\n");
    //getchar();
    while(1)
    {
        write(sockfd, bufferI, 2205);
        read(sockfd, bufferO, 2205);
    }
    
    ma_device_uninit(&device);
    ma_encoder_uninit(&encoder);
    close(sockfd);

    return 0;
}
