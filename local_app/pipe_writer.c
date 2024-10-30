#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

// Workflow
// Chrome extension runs this file with boolean stdin (0 - off or 1 - on)
// This program writes to the pipe 0 or 1

// Code
// Receive boolean message from extension
// Write to pipe

#define PIPE_PATH "./tmp/tens_control"
#define MAXLINE 1000

void read_message();
void write_message();

char input_message[MAXLINE];
char output_message[MAXLINE];

int main(void)
{
        // Check if can write to pipe
        printf("Connecting to pipe... \n");
        int pipe_wr = open(PIPE_PATH, O_WRONLY | O_NONBLOCK);
        if (pipe_wr < 0)
        {
                printf("Failed to open open pipe \n");
                return -1;
        }
        else // Write the read value to pipe
        {
                read_message();
                usleep(10000);
                write_message();

                if (input_message[9])
                {
                        // write to pipe
                        int bytes_written = write(pipe_wr, &input_message[9], 1);
                        if (bytes_written > 0)
                        {
                                printf("Written %d to Pipe", input_message[9]);
                                
                        } else
                        {
                                printf("Failed to write to pipe as bytes written was 0 or less");
                        }
                }
                else // Input not loaded
                {
                        printf("Failed to write to pipe as toggle digit is not loaded");
                }
        }
        return 0;
}

void read_message()
{
        // read 32 bits from stdin
        uint32_t length = 0;
        fread(&length, 4, 1, stdin);

        if (length <= 0)
        {
                printf("Failed to read\n");
        }

        // read the rest of the bits from stdin
        fread(input_message, 1, length, stdin);
        input_message[length + 1] = '\0';
}

void write_message()
{
        uint32_t length = 11;

        // Write the 32 bit / 4 byte prefix
        fwrite(&length, 4, 1, stdout);
        
        // Write the actual message
        fwrite("{\"on\": ", 1, 7, stdout);
        fwrite("\"", 1, 1, stdout);
        fwrite(&input_message[9], 1, 1, stdout);
        fwrite("\"", 1, 1, stdout);
        fwrite("}", 1, 1, stdout);

        fflush(stdout);

        // Write the 32 bit / 4 byte prefix
        fwrite(&length, 4, 1, stderr);
}