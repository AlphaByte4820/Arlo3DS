#include <3ds.h>
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#define ARLO_IP "192.168.1.243"
#define ARLO_PORT 5000

static u32 *socBuffer = NULL;

int main(int argc, char **argv)
{
    gfxInitDefault();
    consoleInit(GFX_TOP, NULL);

    printf("=== ARLO 3DS v0.1 ===\n\n");
    printf("Starting network...\n");

    socBuffer = (u32*)memalign(0x1000, 0x100000);

    if (socBuffer == NULL)
    {
        printf("Memory allocation failed!\n");
    }
    else
    {
        Result rc = socInit(socBuffer, 0x100000);

        if (R_FAILED(rc))
        {
            printf("Network initialization failed.\n");
            printf("Error: %08lX\n", (unsigned long)rc);
        }
        else
        {
            printf("Network initialized!\n");
            printf("Connecting to Arlo...\n\n");

            int sock = socket(AF_INET, SOCK_STREAM, 0);

            if (sock < 0)
            {
                printf("Socket creation failed.\n");
            }
            else
            {
                struct sockaddr_in server;
                memset(&server, 0, sizeof(server));

                server.sin_family = AF_INET;
                server.sin_port = htons(ARLO_PORT);
                inet_pton(AF_INET, ARLO_IP, &server.sin_addr);

                if (connect(
                    sock,
                    (struct sockaddr *)&server,
                    sizeof(server)) < 0)
                {
                    printf("Connection FAILED.\n");
                    printf("%s:%d\n", ARLO_IP, ARLO_PORT);
                }
                else
                {
                    printf("CONNECTED TO ARLO!\n");
                    printf("--------------------\n");

                    char buffer[1024];
                    memset(buffer, 0, sizeof(buffer));

                    int received =
                        recv(sock, buffer, sizeof(buffer) - 1, 0);

                    if (received > 0)
                    {
                        buffer[received] = '\0';
                        printf("%s\n", buffer);
                    }

                    const char *hello =
                        "Hello Arlo from Nintendo 3DS!\n";

                    send(sock, hello, strlen(hello), 0);

                    memset(buffer, 0, sizeof(buffer));

                    received =
                        recv(sock, buffer, sizeof(buffer) - 1, 0);

                    if (received > 0)
                    {
                        buffer[received] = '\0';
                        printf("%s\n", buffer);
                    }
                }

                close(sock);
            }

            socExit();
        }

        free(socBuffer);
    }

    printf("\n--------------------\n");
    printf("Press START to exit.\n");

    while (aptMainLoop())
    {
        hidScanInput();

        if (hidKeysDown() & KEY_START)
            break;

        gfxFlushBuffers();
        gfxSwapBuffers();
        gspWaitForVBlank();
    }

    gfxExit();
    return 0;
}
