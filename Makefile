

windows_client:
	x86_64-w64-mingw32-gcc winclient_vc.c -L./ -static -static-libgcc  -lm -lpthread -lws2_32 -o client_vc.exe -lglfw3 libraylib.a -std=c99 -Wl,-allow-multiple-definition -Wl,--subsystem,windows -Wl,-Bdynamic -lopengl32 -lgdi32 -Wl,-Bstatic -lwinmm
#x86_64-w64-mingw32-gcc winclient_vcPlayback.c -lm -lpthread -lws2_32 -o client_vcPlayback.exe -static -static-libgcc

windows_server:
	x86_64-w64-mingw32-gcc winserver_vc.c -lm -lpthread -lws2_32 -o server_vc.exe -static -static-libgcc

linux_server:
	gcc server_vc.c -lpthread -o server_vc

linux_client:
	gcc client_vc.c -ldl -lm -lpthread -o client_vc `pkg-config --libs --cflags gtk+-3.0` -lraylib -lGL -g