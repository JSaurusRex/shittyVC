
#include "raylib.h"

#include<stdio.h>
#include<winsock2.h>



#define MINIAUDIO_IMPLEMENTATION
#include "miniaudio.h"


#pragma comment(lib,"ws2_32.lib") //Winsock Library

int sockfd = 0;
int counter = 0;
#define BUFFSIZE 4410
#define EXTRABYTES 0
#define TOTALSIZE (BUFFSIZE +EXTRABYTES)
INT16 bufferI[BUFFSIZE+EXTRABYTES];
INT16 bufferO[BUFFSIZE+EXTRABYTES];
int newBuffer = 0;
#define PORT 8080
int serverTimer = 0, latency=0, localTimer = 0;

SOCKET s;

void data_callbackC(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount)
{
    // MA_ASSERT(pDevice->capture.format == pDevice->playback.format);
    // MA_ASSERT(pDevice->capture.channels == pDevice->playback.channels);

    /* In this example the format and channel count are the same for both input and output which means we can just memcpy(). */
    // if(newBuffer < 2)
    // {
    memcpy(bufferI+EXTRABYTES, pInput, BUFFSIZE*2);
    //memcpy(pOutput, bufferO, BUFFSIZE*2);
    //newBuffer++;
    // bufferI[0] = serverTimer;
    send(s, bufferI, TOTALSIZE*2, 0);
    int result = 1;
    result = recv(s, bufferO, TOTALSIZE*2, 0);
    // serverTimer = ((INT16*)bufferO)[0];
    // latency = ((INT16*)bufferO)[1];
    localTimer++;
    printf("latency %i\n", latency);
    // while(serverTimer - localTimer > 2)
    // {
    //     printf("battling tcp traffic jam\n");
    //     result = recv(s, bufferO, TOTALSIZE*2, 0);
    //     serverTimer = ((INT16*)bufferO)[0];
    //     latency = ((INT16*)bufferO)[1];
    //     localTimer++;
    // }
    
    // }else
        //printf("missed buffer\n");

    //write(sockfd, pInput, BUFFSIZE*2);
    //read(sockfd, pOutput, BUFFSIZE*2);
    
    //MA_COPY_MEMORY(pOutput, pInput, frameCount * ma_get_bytes_per_frame(pDevice->capture.format, pDevice->capture.channels));
    //printf("size: %i   counter: %i\n", frameCount, counter++);
}

void data_callbackP(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount)
{
    // MA_ASSERT(pDevice->capture.format == pDevice->playback.format);
    // MA_ASSERT(pDevice->capture.channels == pDevice->playback.channels);

    /* In this example the format and channel count are the same for both input and output which means we can just memcpy(). */
    // if(newBuffer < 2)
    // {
        //memcpy(bufferI, pInput, BUFFSIZE*2);
        //memcpy(pOutput, bufferO, BUFFSIZE*2);
    // memcpy(bufferI, pInput, BUFFSIZE*2);
    //memcpy(pOutput, bufferO, BUFFSIZE*2);
    //newBuffer++;
    // bufferI[0] = serverTimer;
    send(s, bufferI, TOTALSIZE*2, 0);
    recv(s, bufferO, TOTALSIZE*2, 0);
    // serverTimer = ((INT16*)bufferO)[0];
    // latency = ((INT16*)bufferO)[1];
    localTimer++;
    printf("latency %i\n", latency);
    // while(serverTimer - localTimer > 2)
    // {
    //     printf("battling tcp traffic jam\n");
    //     recv(s, bufferO, TOTALSIZE*2, 0);
    //     serverTimer = ((INT16*)bufferO)[0];
    //     latency = ((INT16*)bufferO)[1];
    //     localTimer++;
    // }

    memcpy(pOutput, bufferO+EXTRABYTES, BUFFSIZE*2);
    
    // ((INT16*)pOutput)[0] = ((INT16*)pOutput)[1];
    // }else
        //printf("missed buffer\n");

    //write(sockfd, pInput, BUFFSIZE*2);
    //read(sockfd, pOutput, BUFFSIZE*2);
    
    //MA_COPY_MEMORY(pOutput, pInput, frameCount * ma_get_bytes_per_frame(pDevice->capture.format, pDevice->capture.channels));
    //printf("size: %i   counter: %i\n", frameCount, counter++);
}

int main(int argc , char *argv[])
{
	WSADATA wsa;
	
	struct sockaddr_in server;
	int recv_size;
    // if(argc < 2)
    // {
    //     printf("give ip >:(\n");
    //     return 0;
    // }

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
	
	
	

    ma_result result;
    //ma_encoder encoder;
    
    //encoderConfig = ma_encoder_config_init(ma_encoding_format_wav, ma_format_s16, 1, 44100);

    // if (ma_encoder_init_file("ugh.mp3", &encoderConfig, &encoder) != MA_SUCCESS) {
    //     printf("Failed to initialize output file.\n");
    //     return -1;
    // }

   

    if(argc < 2)
    {

        ma_device_config deviceConfig;
        ma_device device;
        deviceConfig = ma_device_config_init(ma_device_type_capture);
        deviceConfig.capture.format   = ma_format_s16;
        deviceConfig.playback.format   = ma_format_s16;
        deviceConfig.capture.channels = 1;
        deviceConfig.playback.channels = 1;
        deviceConfig.sampleRate       = 44100;
        deviceConfig.dataCallback     = data_callbackC;
        //deviceConfig.pUserData        = &encoder;
        deviceConfig.periodSizeInMilliseconds = 100;
        result = ma_device_init(NULL, &deviceConfig, &device);


        //raylib
        InitWindow(400, 400, "VoiceChat");
        SetTargetFPS(60);
        char ip [50] = {'\0'};
        int ipLength= 0;
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
                //DrawText("ip", 200, 100, 40, WHITE);
                //DrawText(ip, 40, 200, 40, WHITE);
                SetWindowTitle(ip);
                //DrawTextPro(GetFontDefault(), "test\0", (Vector2){.x=40, .y=200}, (Vector2){.x=0, .y=0}, 0, 40, 0.4, WHITE);  
            EndDrawing();
        }
        if(WindowShouldClose())
            exit(0);

        server.sin_addr.s_addr = inet_addr(ip);
        server.sin_family = AF_INET;
        server.sin_port = htons( PORT );
        //Connect to remote server
        if (connect(s , (struct sockaddr *)&server , sizeof(server)) < 0)
        {
            puts("connect error");
            return 1;
        }
        puts("Connected");
        

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
        printf("MADE IT!\n");
        int averageVolume = 0;

        STARTUPINFO si;
        PROCESS_INFORMATION pi;

        ZeroMemory( &si, sizeof(si) );
        si.cb = sizeof(si);
        ZeroMemory( &pi, sizeof(pi) );
        char str [50] = "client_vc.exe ";
        strcat(str, ip);
        printf("%s\n", str);
        if( !CreateProcess( NULL,   // No module name (use command line)
        str,        // Command line
        NULL,           // Process handle not inheritable
        NULL,           // Thread handle not inheritable
        1,          // Set handle inheritance to FALSE
        0,              // No creation flags
        NULL,           // Use parent's environment block
        NULL,           // Use parent's starting directory 
        &si,            // Pointer to STARTUPINFO structure
        &pi )           // Pointer to PROCESS_INFORMATION structure
        )
            exit(0);
        // while(!WindowShouldClose())
        // {
        //     // if(newBuffer !=0)
        //     // {
        //     //     bufferI[0] = time;
        //     //     write(sockfd, bufferI, 4410*2);
        //     //     read(sockfd, bufferO, 4410*2);
        //     //     newBuffer = 0;
        //     //     //printf("write!  %i\n", bufferO[0]);
        //     //     time = bufferO[0];
        //     //     bufferO[0] = 0;
        //     // }
        //     BeginDrawing();
        //         //printf(bufferI[50]);
        //         int volume = 0;
        //         for(int i = 0; i < 4410; i++)
        //         {
        //             if(volume < bufferI[i])
        //                 volume = bufferI[i];
        //         }
        //         printf("%u\n", volume);
        //         averageVolume = (averageVolume + volume) / 2;
        //         //ClearBackground((Color){.r=0, .b=0, .g=averageVolume /20});
        //     EndDrawing();
        // }
        while(!WindowShouldClose())
        {
            //printf("window should close :thinking:?\n");
            BeginDrawing();
                char ping [3] = {'0'+latency%10, '0'+latency/10, '\0'};
                SetWindowTitle(ping);
            EndDrawing();
            Sleep(50);
        }
        printf("\nclose!!\n");
        printf("%i\n", TerminateProcess(pi.hProcess, 0));
        CloseHandle( pi.hProcess );
        CloseHandle( pi.hThread );

    }else{

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
        while(1)
        {
            Sleep(500);
        }
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