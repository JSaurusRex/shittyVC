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
#include <pthread.h>
#include <sys/socket.h>
#include <time.h>
//#include <time.h>


#include <raylib.h>

#define MAX 4410
#define EXTRABYTES 0
#define TOTALSIZE (MAX +EXTRABYTES)
#define PORT 8080
#define SA struct sockaddr
int sockfd = 0;
int counter = 0;
int16_t bufferI[TOTALSIZE];
int16_t bufferO[TOTALSIZE];
pthread_mutex_t bufferLock;
int newBuffer = 0;
int serverTimer = 0, localTimer = 0;

int counterbuff = 0;
int lastCounter = 0;
void data_callback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount)
{
    // MA_ASSERT(pDevice->capture.format == pDevice->playback.format);
    // MA_ASSERT(pDevice->capture.channels == pDevice->playback.channels);
    // if(WindowShouldClose())
    // {
    //     exit(0);
    // }
    /* In this example the format and channel count are the same for both input and output which means we can just memcpy(). */
    // if(newBuffer == 0)
    // {
    //     memcpy(bufferI, pInput, MAX*2);
    //     memcpy(pOutput, bufferO, MAX*2);
        
    // }
    // newBuffer ++;
    pthread_mutex_lock(&bufferLock);
    memcpy(bufferI+EXTRABYTES, pInput, MAX*2);
    //bufferI[0] = serverTimer;
    
    localTimer++;
    if(lastCounter == counterbuff)
    {
        printf("missed buffer\n");
    }
    lastCounter = counterbuff;
    
    // serverTimer = ((int16_t*)bufferO)[0];
    // printf("latency %i\n", bufferO[1]);
    // while(serverTimer - localTimer > 2)
    // {
    //     printf("battling tcp traffic jam\n");
    //     read(sockfd, bufferO, TOTALSIZE*2);
    //     localTimer++;
    //     serverTimer = ((int16_t*)bufferO)[0];
    // }
    memcpy(pOutput, bufferO+EXTRABYTES, MAX*2);
    pthread_mutex_unlock(&bufferLock);
    
    //MA_COPY_MEMORY(pOutput, pInput, frameCount * ma_get_bytes_per_frame(pDevice->capture.format, pDevice->capture.channels));
    //printf("size: %i   counter: %i\n", frameCount, counter++);
}

void sendServer ()
{
    int16_t inbetweenBufferI[TOTALSIZE];
    int16_t inbetweenBufferO[TOTALSIZE];
    while(true)
    {
        pthread_mutex_lock(&bufferLock);
        int differences = 0;
        for(int i = 0; i < TOTALSIZE; i++)
        {
            if(bufferO[i] == inbetweenBufferO[i])
                differences++;
            bufferO[i] = inbetweenBufferO[i];
            inbetweenBufferI[i] = bufferI[i];
        }
        pthread_mutex_unlock(&bufferLock);
        printf("found %i differences\n", differences);

        write(sockfd, inbetweenBufferI, TOTALSIZE*2);
        read(sockfd, inbetweenBufferO, TOTALSIZE*2);
        counterbuff++;
        //usleep(80000);
    }
}


int main(int argc, char** argv)
{
    ma_result result;
    ma_encoder_config encoderConfig;
    ma_encoder encoder;
    ma_device_config deviceConfig;
    ma_device device;

    InitWindow(400, 400, "VoiceChat");
    SetTargetFPS(60);
    char ip [50] = {'\0'};
    int ipLength= 0;
    pthread_mutex_init(&bufferLock, NULL) ;
    while(!WindowShouldClose())
    {
        BeginDrawing();
            ClearBackground((Color){.r=0, .b=0, .g=bufferI[50]});
            int key = GetKeyPressed();
            while(key)
            {
                if(key == 65 || (key >= 46 && key <= 57))
                    ip[ipLength++] = key;
                if(key == 259)
                    ip[ipLength--] = '\0';
                if(ipLength < 0)
                    ipLength = 0;
                if(key == 257)
                    break;
                ip[ipLength] = '\0';
                if(IsKeyDown(341) && key == 86)
                {
                    strcat(ip, GetClipboardText());
                }
                if(IsKeyDown(341) && key == 259)
                {
                    ip[0] = '\0';
                    ipLength = 0;
                }
                printf("%i  %s\n", key, ip);
                key = GetKeyPressed();
            }
            if(key == 257)
                break;
            DrawText("ip", 200, 100, 40, WHITE);
            DrawText(ip, 40, 200, 40, WHITE);
        EndDrawing();
    }
    if(WindowShouldClose())
        exit(0);


    deviceConfig = ma_device_config_init(ma_device_type_duplex);
    deviceConfig.capture.format   = ma_format_s16;
    deviceConfig.playback.format   = ma_format_s16;
    deviceConfig.capture.channels = 1;
    deviceConfig.playback.channels = 1;
    deviceConfig.sampleRate       = 44100;
    deviceConfig.dataCallback     = data_callback;
    //deviceConfig.pUserData        = &encoder;
    deviceConfig.periodSizeInMilliseconds = 100;

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
	servaddr.sin_addr.s_addr = inet_addr(ip);
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

    pthread_t sendThread;
    pthread_create(&sendThread, NULL, sendServer, NULL);

    //printf("Press Enter to stop recording...\n");
    //getchar();
    int averageVolume = 0;
    while(!WindowShouldClose())
    {
        // if(newBuffer !=0)
        // {
        //     bufferI[0] = time;
        //     write(sockfd, bufferI, MAX*2);
        //     read(sockfd, bufferO, MAX*2);
        //     newBuffer = 0;
        //     //printf("write!  %i\n", bufferO[0]);
        //     time = bufferO[0];
        //     bufferO[0] = 0;
        // }
        BeginDrawing();
            //printf(bufferI[50]);
            int volume = 0;
            for(int i = 0; i < MAX; i++)
            {
                if(volume < bufferI[i])
                    volume = bufferI[i];
            }
            //printf("%u\n", volume);
            averageVolume = (averageVolume + volume) / 2;
            ClearBackground((Color){.r=0, .b=0, .g=averageVolume /20});
        EndDrawing();
    }
    //getchar();
    
    ma_device_uninit(&device);
    ma_encoder_uninit(&encoder);
    close(sockfd);

    return 0;
}
