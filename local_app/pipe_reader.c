#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

#define PIPE_PATH "./tmp/tens_control"

int main(void)
{
        printf("Connecting to pipe... \n");

        int pipe_rd = open(PIPE_PATH, O_RDONLY | O_NONBLOCK);
        if (pipe_rd < 0)
        {
                printf("Failed to open open pipe \n");
                return -1;
        }

        printf("PIPE OPENED\n");

        // Main loop
        char buffer[2];
        while (1)
        {
                int bytes_read = read(pipe_rd, buffer, 1);
                if (bytes_read > 0) // Tab switch happens here, so data is sent
                {
                        printf("Received: %c\n", buffer[0]);
                        
                        // Toggle Arduino Operations here
                        if (buffer[0] == '1')
                        {
                                printf("Turning on LED\n");
                        }
                        else if (buffer[0] == '0')
                        {
                                printf("Turning off LED\n");
                        }
                        else
                        {
                                printf("Invalid command\n");
                        }
                }
                else // No tab switch happens here, so no data is sent
                {
                        // printf("No data\n");
                }
        }
}