// This program writes game name / game code to an N64 ROM.

#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define GAME_NAME_LENGTH 20
#define GAME_NAME_OFFSET 0x20
#define GAME_CODE_LENGTH 4
#define GAME_CODE_OFFSET 0x3B
#define ROM_VERSION_OFFSET 0x3F
#define MINIMUM_FILE_SIZE (ROM_VERSION_OFFSET + 1)

int	main(int argc, char* argv[])
{
	FILE* file;
	char game_code[GAME_CODE_LENGTH + 1];
	char game_name[GAME_NAME_LENGTH + 1];
	unsigned long rom_version = 0;
	char *file_buffer, *file_buffer_pos, *file_buffer_end;
	long file_length;
	size_t temp;

	if (argc <= 3) {
		printf("Usage: header64 <rom.n64> <game code> <game name> [rom version]\n");
		printf("Example: header64 frogger2.n64 NGVE \"Frogger2            \" 1\n");
		printf("The game code must be four characters, and the game name must be no longer than 20 characters.\n");
		exit(1);
	}

	// Read game code.
	if (GAME_CODE_LENGTH <= strlen(argv[2])) {
		memset(&game_code[0], 0, (GAME_CODE_LENGTH + 1) * sizeof(char));
		memcpy(&game_code[0], argv[2], strlen(argv[2]) * sizeof(char));
	} else {
		printf("The game code was too long: '%s'. (Length: %d)\n", argv[2], strlen(argv[2]));
		exit(-1);
	}

	// Read game name.
	if (GAME_NAME_LENGTH <= strlen(argv[3])) {
		memset(&game_name[0], 0, (GAME_NAME_LENGTH + 1) * sizeof(char));
		memcpy(&game_name[0], argv[3], strlen(argv[3]) * sizeof(char));
	} else {
		printf("The game name was too long: '%s'. (Length: %d)\n", argv[3], strlen(argv[3]));
		exit(-1);
	}

	// Read rom version
	if (argc > 4) {
		rom_version = strtoul(argv[4], NULL, 0);
		if (rom_version > 255) {
			printf("The ROM version cannot exceed 255.\n");
			exit(-1);
		}
	}

	// Open file.
	file = fopen(argv[1], "rb");
	if (file == NULL) {
		perror("Failed to open ROM file for reading.");
		exit(-1);
	}

	// Verify file size.
	fseek(file, 0, SEEK_END);
	file_length = ftell(file);
	if (MINIMUM_FILE_SIZE > file_length) {
		printf("The provided ROM file is too small. (Size: %d)\n", file_length);
		exit(-1);
	}

	// Determine ROM file length.
	fseek(file, 0, SEEK_SET);
	file_buffer_pos = file_buffer = (char*) malloc(file_length);
	if (!file_buffer) {
		printf("Failed to allocate %ld bytes for the file buffer.\n", file_length);
		exit(-1);
	}

	// Read file contents to the buffer.
	file_buffer_end = file_buffer + file_length;
	while (file_buffer_end > file_buffer_pos)
		file_buffer_pos += fread(file_buffer_pos, sizeof(char), file_buffer_end - file_buffer_pos, file);
	fclose(file);

	// Writes data to the ROM buffer.
	memcpy(&file_buffer[GAME_NAME_OFFSET], &game_name[0], GAME_NAME_LENGTH * sizeof(char)); // Writes the game name to the buffer.
	memcpy(&file_buffer[GAME_CODE_OFFSET], &game_code[0], GAME_CODE_LENGTH * sizeof(char)); // Writes the game code to the buffer.
	file_buffer[ROM_VERSION_OFFSET] = (char)rom_version; // Writes the ROM version to the buffer.

	// Open output file for writing.
	file = fopen(argv[1], "wb");
	if (file == NULL) {
		perror("Failed to open ROM file for writing.");
		exit(-1);
	}

	// Write buffer.
	file_buffer_pos = file_buffer;
	while (file_buffer_end > file_buffer_pos)
		file_buffer_pos += fwrite(file_buffer_pos, sizeof(char), file_buffer_end - file_buffer_pos, file);
	fclose(file);
	
	// Finished.
	free(file_buffer);
	printf("Successfully updated ROM header to %s/%s\n", game_code, game_name);
	return 0;
}
