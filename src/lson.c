/****************************************************************************
Copyright (c) 2022 Isaponsoft (Isao Shibuya)

This software is provided 'as-is', without any express or implied warranty.
In no event will the authors be held liable for any damages arising from the
use of this software.

Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it
freely, subject to the following restrictions:

   1. The origin of this software must not be misrepresented; you must not
      claim that you wrote the original software. If you use this software
      in a product, an acknowledgment in the product documentation would be
      appreciated but is not required.

   2. Altered source versions must be plainly marked as such, and must not be
      misrepresented as being the original software.

   3. This notice may not be removed or altered from any source distribution.
*****************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/lson.h"

int main(int argc, char** argv)
{
	int			mode	= 0;
	int			noret	= 0;
	char*		in_buff;
	char*		dst_buff;
	char const*	in_pos;
	char*		dst_pos;
	long		in_size;
	long		dst_size;
	long		read_size;
	int			ret;
	int			i;
	FILE*		fp;

	if (argc == 1)
	{
		printf("usage)\n");
		printf("lson [-n] -jl or -lj filename\n");
		printf("   -n  No output return.\n");
		printf("   -jl JSON to LSON\n");
		printf("   -lj LSON to JSON\n");
		return	0;
	}

	for (i = 1; i < argc; ++i)
	{
		char const*	opt	= argv[i];
		if (opt[0] != '-')
		{
			break;
		}
		if (opt[1] == 0)
		{
			break;
		}
		if (!strcmp(opt, "-n"))
		{
			noret	= 1;
			continue;
		}
		if (!strcmp(opt, "-jl"))
		{
			mode	= 1;
			continue;
		}
		if (!strcmp(opt, "-lj"))
		{
			mode	= 2;
			continue;
		}
		fprintf(stderr, "Unkown option '%s'.", opt);
		return	1;
	}

	if (mode == 0)
	{
		fprintf(stderr, "nothing '-jl' or '-lj'.\n");
		return	1;
	}

	if (i != argc-1)
	{
		fprintf(stderr, "filename nothing %d %d.\n", i, argc);
		return	1;
	}

	if (!strcmp(argv[i], "-"))
	{
		int	len	= 3;
		int	off	= 0;
		in_buff	= malloc(len);
		in_size	= 0;
		if (!in_buff)
		{
			fprintf(stderr, "memory allocate failed.\n");
			return	1;
		}
		for (;;)
		{
			int	reqest	= len - off - 1;
			read_size	= fread(in_buff + off, 1, reqest, stdin);
			if (read_size <= 0)
			{
				break;
			}
			in_size		= in_size + read_size;
			off			= off     + read_size;
			if (read_size < reqest)
			{
				break;
			}
			len		= len + 3;
			in_buff	= realloc(in_buff, len);
			if (!in_buff)
			{
				fprintf(stderr, "memory allocate failed.\n");
				return	1;
			}
		}
		in_buff[in_size]	= 0;
	}
	else
	{
		fp	= fopen(argv[i], "rb");
		if (!fp)
		{
			fprintf(stderr, "Can't open %s.\n", argv[i]);
			return	1;
		}

		fseek(fp, 0, SEEK_END);
		in_size		= ftell(fp);
		fseek(fp, 0, SEEK_SET);

		in_buff 	=  malloc(in_size + 1);
		if (!in_buff)
		{
			fprintf(stderr, "memory allocate failed.\n");
			return	1;
		}
		read_size = fread(in_buff, 1, in_size, fp);
		if (read_size != in_size)
		{
			fprintf(stderr, "file read error(%d/%d).\n", (int)read_size, (int)in_size);
			return	1;
		}
		fclose(fp);
		in_buff[in_size] = 0;
	}

	dst_size	= (in_size + 100) * 10;
	dst_buff 	=  malloc(dst_size + 1);
	if (!dst_buff)
	{
		fprintf(stderr, "memory allocate failed.\n");
		return	1;
	}

	memset(dst_buff, 0, dst_size+1);
	if (mode == 1)
	{
		dst_pos	= dst_buff;
		in_pos	= in_buff;
		ret	= json_to_lson((char**)&dst_pos, dst_buff + dst_size, (char const**)&in_pos, in_buff + in_size);
		if ((in_pos = lson_find_not_blunk(in_pos, in_buff + in_size)) != NULL)
		{
			ret	= LSON_RESULT_UNFORMAT;
		}
	}
	if (mode == 2)
	{
		dst_pos	= dst_buff;
		in_pos	= in_buff;
		ret	= lson_to_json((char**)&dst_pos, dst_buff + dst_size, (char const**)&in_pos, in_buff + in_size);
		if ((in_pos = lson_find_not_blunk(in_pos, in_buff + in_size)) != NULL)
		{
			ret	= LSON_RESULT_UNFORMAT;
		}
	}
	if (ret < 0)
	{
		int			line_number;
		char const*	line_start;
		char const*	cur;
		lson_compute_location(&line_number, &line_start, in_buff, in_buff + in_size, in_pos);
		cur	= line_start;
		while (cur != (in_buff + in_size) && *cur != '\n' && *cur != '\r')
		{
			++cur;
		}
		*(char*)cur	= 0;
		fprintf(stderr, "line %d : %s\n%s\n", line_number, ret == LSON_RESULT_UNFORMAT ? "format error" : "buffer overfllow", line_start);
		if (line_start != in_pos)
		{
			do
			{
				fprintf(stderr, " ");
				++line_start;
			} while (line_start != in_pos);
			fprintf(stderr, "^\n");
		}

		return	1;
	}
	printf(noret ? "%s" : "%s\n", dst_buff);

	free(in_buff);
	free(dst_buff);
	return	0;
}
