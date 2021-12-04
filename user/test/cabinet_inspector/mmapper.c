#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <string.h>

#define die(...)		{ fprintf(stderr, __VA_ARGS__); exit(1); }
#define perror_die(e)		{ perror(e); exit(1); }
#define USAGE			"usage: %s [-m] <filename>\n"
#define MODFLAG			"-m"

int main(int argc, char **argv)
{
	struct stat s;
	int modify, o_flags, prot_flags, fd, status;
	char *path, *f;

	switch (argc) {
	case 3:
		if (strcmp(MODFLAG, argv[1]))
			die("error: option '%s' not recognised\n" USAGE,
					argv[1], argv[0])
		modify = 1;
		o_flags = O_CREAT | O_RDWR;
		prot_flags = PROT_READ | PROT_WRITE;
		path = argv[2];
		break;
	case 2:
		modify = 0;
		o_flags = O_RDONLY;
		prot_flags = PROT_READ;
		path = argv[1];
		break;
	default:
		die("error: wrong number of arguments\n" USAGE, argv[0]);
	}

	fd = open(path, o_flags);
	if (fd < 0)
		perror_die("open failed");

	status = fstat(fd, &s);
	if (status < 0)
		perror_die("stat failed");

	f = (char *) mmap(NULL, s.st_size, prot_flags, MAP_SHARED, fd, 0);
	if (f < 0)
		perror_die("mmap failed");

	printf("PID %d: mmap()ed %s to %p with size = %ldB\n",
			getpid(), path, f, s.st_size);

	if (modify) {
		*f = 'C';
		printf("wrote first byte: %c\n", *f);
	} else {
		printf("read first byte: %c\n", *f);
	}

	printf("to test, run:\n\tsudo ./cabinet_inspector %p %d\n\n",
			f, getpid());
	printf("to quit, press CTRL+C or run:\n\tkill -s INT %d\n",
			getpid());

	pause();
	close(fd); /* not technically necessary */

	return 0;
}
