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
        fprintf(stderr, "Connecting to the pipe...\n");
        // Check if can write to pipe
        int pipe_wr = open(PIPE_PATH, O_WRONLY | O_NONBLOCK);
        if (pipe_wr < 0)
        {
                fprintf(stderr, "Failed to open open pipe \n");
                return -1;
        }
        else 
        {
                // Since browser runs this file with stdin to communicate, 
                // read and write operations don't have delays
                
                // Read value from browser
                read_message();

                // write value back to browser
                write_message();

                // Write value to pipe
                if (input_message[9]) 
                {
                        int bytes_written = write(pipe_wr, &input_message[9], 1);
                        if (bytes_written > 0)
                        {
                                fprintf(stderr, "Written %d to Pipe", input_message[9]);
                                
                        } else
                        {
                                fprintf(stderr, "Failed to write to pipe as bytes written was 0 or less");
                        }
                }
                else // Input not loaded
                {
                        fprintf(stderr, "Failed to write to pipe as toggle digit is not loaded");
                }
        }
        return 0;
}

void read_message()
{
        // read 32 bits from stdin
        uint32_t length;
        fread(&length, 4, 1, stdin);

        if (length <= 0)
        {
                fprintf(stderr, "Failed to read\n");
        }

        // read the rest of the bits from stdin
        fread(input_message, 1, length, stdin);
        input_message[length + 1] = '\0';
}

void write_message()
{
        int on = input_message[9] - '0';

        char *response;
        if (on) {
                response = "{\"on\":1}";
        } else {
                response = "{\"on\":0}";
        }

        uint32_t length = strlen(response);

        // Write the 4 byte each, 1 number of 4 bytes prefix
        fwrite(&length, 4, 1, stdout);
        
        // Write the actual message, 1 byte each, length number of 1 bytes
        fwrite(response, 1, length, stdout);

        fflush(stdout);
}