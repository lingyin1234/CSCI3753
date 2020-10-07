#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <stdbool.h>

#define DEVICENAME "/dev/simple_character_device"
#define BUFF_SIZE 1024


int main(){
	char command;
	int length, whence, new_offset;
	char buffer[BUFF_SIZE];
	int file = open(DEVICENAME, O_RDWR);
	bool running = true;
	while(running){
		printf("\n\nLingyin's Program\n");
		printf("  COMMANDS:\n");
		printf("    'r' to read from device\n");
		printf("    'w' to write to device\n");
		printf("    's' to seek from device\n");
		printf("    'e' to exit from device\n");
		printf("    anything else brings up main menu\n");
		printf("/command$> ");
		scanf("%c", &command);

		switch(command){
			case 'r':
				printf("/read$> How many bytes to read: "); 
				scanf("%d", &length); 
				read(file, buffer, length); /* reads from the file, puts it to the buffer for x-length */
				printf("/read$> %s\n", buffer); /* prints the buffer */
				while(getchar() != '\n'); /* while its not a new line, continue */
				break;
			case 'w':
				printf("/write$> "); 
				
				scanf("%s", buffer);
				write(file, buffer, BUFF_SIZE); /* writes the buffer to file */
				while (getchar() != '\n'); /* while its not a new line, continue */
				break;
			case 's':
				printf("SEEK COMMANDS:\n");
				printf("	'0' seek set\n");
				printf("	'1' seek cur\n");
				printf("	'2' seek end\n");
				printf("	anything else brings up main menu\n");
				printf("/seek$> Enter whence: ");
				scanf("%d", &whence);
				printf("\n/write$> Enter an offset value: ");
				scanf("%d", &new_offset);
				llseek(file, new_offset, whence);
				break;
			case 'e':
				printf("/exit$> Exiting\n");
				running = false;
				break;
			default:
				printf("\n/error$> error: not a valid command\n");
				break;
		}
	}
	close(file);
	return 0;
}
