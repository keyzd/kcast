#include <stdio.h>
#include <stdlib.h>
#include "kcast.h"

void load_map_file(const char *path)
{
	FILE *map_file = NULL;
	int c;
	long grid_h = 0;	
	long map_size = 0;
	long grid_w = 0;

	map_file = fopen(path, "r");

	if(!map_file)
	{
		perror(path);
		exit(1);
	}

	while( (c = fgetc(map_file)) != '/' )
	{
		if(c == EOF)
		{
			printf("Wrong map file\n");
			exit(1);
		}

		if(c == '\n')
			grid_h++;
		else
			map_size++;
	}

	grid_w = map_size / grid_h;

	map.grid = malloc( (map_size + 1) * sizeof(char) );

	fseek(map_file, 0, SEEK_SET);
	long i = 0;

	while( (c = fgetc(map_file)) != '/' )
	{
		if(c == EOF)
		{
			printf("Wrong map file\n");
			exit(1);
		}

		if(c != '\n')
		{
			map.grid[i] = c;
			i++;
		}
	}

	map.grid[i] = '\0';

	map.grid_w = grid_w;
	map.grid_h = grid_h;
	map.block = DEFAULT_BLOCK_SIZE;
	map.w = map.grid_w * map.block;
	map.h = map.grid_h * map.block;

	fclose(map_file);
}

