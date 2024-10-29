#include <stdio.h>
#include <string.h>
#include <fcntl.h>   // For file control
#include <errno.h>   // For error handling
#include <termios.h> // For serial port settings
#include <unistd.h>  // For read/write


// UNDERSTAND THIS CODE!!!

/* Function to open serial port */
int initSerial(const char *portname) 
{
    // Open serial port
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

/* Simple test program */
int main() 
{
    // Replace with your Arduino port 
    // Find it in Arduino IDE -> Tools -> Port
    const char *port = "/dev/cu.usbmodem1101"; 
    char input[100];
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
        printf("Enter command to send (1=on, 0=off, q=quit): ");
        if (fgets(input, sizeof(input), stdin) == NULL) break;

        // Remove newline
        input[strcspn(input, "\n")] = 0;

        // Check for quit command
        if (input[0] == 'q') break;

        // Send command to Arduino
        int bytes_written = write(serial_fd, input, strlen(input));
        if (bytes_written < 0) {
            printf("Error writing to serial port\n");
            continue;
        }

        // // Read response from Arduino
        // char buffer[100];
        // int bytes_read = read(serial_fd, buffer, sizeof(buffer) - 1);
        // if (bytes_read > 0) {
        //     buffer[bytes_read] = '\0';
        //     printf("Arduino response: %s\n", buffer);
        // }
    }

    // Close serial port
    close(serial_fd);
    return 0;
}