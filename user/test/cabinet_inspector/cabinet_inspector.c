#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "cabinet.h"

union pagemap {
	struct {
		uint64_t pfn : 55;
		uint64_t _ : 9;
	} present;

	struct {
		uint64_t type : 5;
		uint64_t offset : 50;
		uint64_t _ : 9;
	} swapped;

	struct {
		uint64_t _ : 56;
		uint64_t x_map : 1;
		uint64_t zero : 4;
		uint64_t file_page : 1;
		uint64_t swapped : 1;
		uint64_t present : 1;
	} meta;
};

void ppagemap(union pagemap *pm, unsigned long vaddr)
{
	if (pm->meta.present) {
		unsigned long paddr, pf_paddr;

		pf_paddr = pm->present.pfn * sysconf(_SC_PAGESIZE);
		paddr = pf_paddr | (vaddr & (sysconf(_SC_PAGESIZE) - 1));

		printf("paddr: %p\n",		(void *) paddr);
		printf("pf_paddr: %p\n",	(void *) pf_paddr);
	} else if (pm->meta.swapped) {
		printf("swap type: %d\n",	pm->swapped.type);
		printf("swap offset: %lx\n",	(uint64_t) pm->swapped.offset);
	} else {
		printf("(neither present nor swapped\n");
	}

	printf("exclusively mapped: %s\n",	strbool(pm->meta.x_map));
	printf("file-page: %s\n",		strbool(pm->meta.file_page));
	printf("swapped: %s\n",			strbool(pm->meta.swapped));
	printf("present: %s\n",			strbool(pm->meta.present));
}


void read_pagemap(union pagemap *pm, pid_t pid, unsigned long vaddr)
{
	char path[64];
	FILE *pagemap;
	unsigned long offset;

	if (pid == -1)
		snprintf(path, sizeof(path), "/proc/self/pagemap");
	else
		snprintf(path, sizeof(path), "/proc/%d/pagemap", pid);

	pagemap = fopen(path, "r");

	if (!pagemap)
		die("could not open pagemap: %s\n", path);

	offset = (vaddr / sysconf(_SC_PAGESIZE)) * sizeof(uint64_t);

	if (fseek(pagemap, offset, SEEK_SET))
		die("fseek failed\n");

	if (fread(pm, sizeof(*pm), 1, pagemap) != 1)
		die("fread failed\n");

	fclose(pagemap);
}

int main(int argc, char **argv)
{
	pid_t pid = -1;
	unsigned long vaddr = (unsigned long) &pid;
	struct cab_info inventory;
	union pagemap pm;

	switch (argc) {
	case 3:
		pid = atoi(argv[2]);
		if (!pid && strcmp(argv[2], "0"))
			die("error parsing pid: %s\n", argv[2]);
		/* FALLTHRU */
	case 2:
		vaddr = strtoul(argv[1], NULL, 16);
		/* FALLTHRU */
	case 1:
		break;
	default:
		die("usage: %s [-r] [<vaddr> [<pid>]] (as root)\n", argv[0]);
	}

	printf("inspecting cabinet for %p of pid=%d\n", (void *) vaddr, pid);

	printf("\n[405] inspect_cabinet():\n");
	if (inspect_cabinet(pid, vaddr, &inventory))
		perror("inspect_cabinet");
	else
		pcab_info(&inventory);

	printf("\n[proc] pagemap:\n");
	read_pagemap(&pm, pid, vaddr);
	ppagemap(&pm, vaddr);

	return 0;
}
