#include <stdio.h>
#include <string.h>
#include <fcntl.h>   // For file control
#include <errno.h>   // For error handling
#include <termios.h> // For serial port settings
#include <unistd.h>  // For read/write

#define PIPE_PATH "./tmp/tens_control"

void print_arduino_response(int serial_fd);
void write_to_arduino(int serial_fd, char *input);

// Open serial port
int initSerial(const char *portname) 
{
    // O_RDWR: Read/Write mode
    // O_NOCTTY: Don't allow controlling terminal
    int fd = open(portname, O_RDWR | O_NOCTTY);

    if (fd == -1) {
        printf("Error opening port: %s\n", strerror(errno));
        return -1;
    }

    // Configure port settings
    struct termios tty;
    memset(&tty, 0, sizeof(tty));

    // Get current settings
    if(tcgetattr(fd, &tty) != 0) {
        printf("Error from tcgetattr: %s\n", strerror(errno));
        return -1;
    }

    // Set Baud Rate (9600)
    cfsetspeed(&tty, B9600);

    // Arduino default settings: 8 data bits, no parity, one stop bit.
    tty.c_cflag &= ~PARENB;         // No parity
    tty.c_cflag &= ~CSTOPB;         // 1 stop bit, since CSTOPB means 2 stop bits
    tty.c_cflag &= ~CSIZE;          // Clear Size bits first since could be default size bits
    tty.c_cflag |= CS8;             // 8 bits per byte

    // Save settings
    if (tcsetattr(fd, TCSANOW, &tty) != 0) {
        printf("Error from tcsetattr: %s\n", strerror(errno));
        return -1;
    }

    return fd;
}

int main()
{
    // Pipe Connection
    printf("Connecting to pipe... \n");

    int pipe_rd = open(PIPE_PATH, O_RDONLY | O_NONBLOCK);
    if (pipe_rd < 0)
    {
            printf("Failed to open open pipe \n");
            return -1;
    }

    printf("PIPE OPENED\n");

    // Arduino port
    const char *port = "/dev/cu.usbmodem101"; 
    char input[100] = {0};
    int serial_fd;

    // Open serial connection
    serial_fd = initSerial(port);
    if (serial_fd < 0) {
        printf("Failed to open serial port\n");
        return 1;
    }

    printf("Serial port opened successfully!\n");

    // Main communication loop
    while(1) {
        int bytes_read = read(pipe_rd, input, 1); // No cost here
        input[1] = 0;

        if (bytes_read > 0)
        {
            // From Chrome Browser
            printf("Received: %c\n", input[0]);

            // Send command to Arduino
            write_to_arduino(serial_fd, input); // Cost here 

            usleep(10000); // 0.1 seconds: Consideration for hardware speed at 9600 baud 
            // + Arduino Processing

            // Read response from Arduino
            // print_arduino_response(serial_fd); 
        }
    }

    // Close serial port
    close(serial_fd);
    return 0;
}

void write_to_arduino(int serial_fd, char *input)
{
        // Send command to Arduino
        int bytes_written = write(serial_fd, input, strlen(input));
        if (bytes_written < 0) {
            printf("Error writing to serial port\n");
        }
}

void print_arduino_response(int serial_fd) 
{
    char buffer[100];
    int bytes_read = read(serial_fd, buffer, sizeof(buffer) - 1);
    if (bytes_read > 0) {
        buffer[bytes_read] = '\0';
        printf("Arduino response: %s\n", buffer);
    }
}
