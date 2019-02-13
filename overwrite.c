// Copyright (c) 2019 Xavier Perrin. All rights reserved.
//
// This work is licensed under the terms of the MIT license.
// For a copy, see <https://opensource.org/licenses/MIT>.

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <limits.h>

#define MAX_LINESIZE 4096

typedef struct s_info {
	int		limit;
	char	*target;
	FILE	*file;
}			t_param;

void	handle_error(t_param info) {
	if (errno) {
		fprintf(stderr, "[ERROR] %s\n", strerror(errno));
	}
	free(info.target);
	fclose(info.file);
	exit(1);
}

int		prepend_in_file(t_param info, char *target) {
	char	buf[MAX_LINESIZE];
	char	line = 0;
	fpos_t	pos;
	fpos_t	tmp_pos;

	while (fgets(buf, MAX_LINESIZE, info.file) && line < info.limit) {
		// If string matches rewind and prepend
		if (strstr(buf, info.target)) {
			if (strlen(buf) < strnlen(target, MAX_LINESIZE)) {
				fprintf(stderr,
						"[WARN] l%d: Target string longer than source string, not overwrote\n",
						line);
				continue;
			}

			fgetpos(info.file, &tmp_pos);

			fsetpos(info.file, &pos);
			fprintf(info.file, "%s", target);
			printf("Overwrote l%d: %s", line + 1, buf);

			fsetpos(info.file, &tmp_pos);
		}

		fgetpos(info.file, &pos);
		line++;
	}

	if (errno) {
		handle_error(info);
	}

	return (1);
}

int		main(int ac, char **av) {
	t_param	info;

	if (ac < 4) {
		printf("Usage: %s [file] [string to overwrite] [string to overwrite it with] (line limit)\n", av[0]);
		return (1);
	}

	// Parsing the arguments & Reading the file
	if (ac > 4) {
		info.limit = atoi(av[4]);
		if (info.limit <= 0) {
			fprintf(stderr, "[ERROR] Limit can't be null or negative!\n");
			return (1);
		}
	} else {
		info.limit = INT_MAX;
	}
	if (!(info.file = fopen(av[1], "r+"))) {
		fprintf(stderr, "[ERROR] Cannot open file '%s': %s\n", av[1], strerror(errno));
		return (1);
	}
	if (!(info.target = strdup(av[2]))) {handle_error(info);}

	return ((prepend_in_file(info, av[3])) != 0);
}
