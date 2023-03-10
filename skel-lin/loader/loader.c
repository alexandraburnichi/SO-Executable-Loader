/*
 * Loader Implementation
 *
 * 2022, Operating Systems
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>

#include "exec_parser.h"

static so_exec_t *exec;
static int fd; //se retine fisierul pentru a putea fi folosit in segv_handler
static struct sigaction old_handler;
struct sigaction action;

static void segv_handler(int signum, siginfo_t *info, void *context)
{
	int i;
	char *fault = info->si_addr; // adresa la care se gaseste seg fault-ul
	int pageSize = getpagesize(); // functia intoarce dimensiunea unei pagini

	for (i = 0; i < exec->segments_no; i++) { //parcurgerea fisierului segment cu segment
		if ((char *)exec->segments[i].vaddr + exec->segments[i].mem_size > fault) //se verifica daca adresa de seg fault apartine segmentului
			break;
	}

	if (i == exec->segments_no || info->si_code != SEGV_MAPERR) //in cazul in care adresa nu se gaseste in niciun segment se foloseste handler-ul vechi
		old_handler.sa_sigaction(signum, info, context); 

	char *vaddr = (char *)exec->segments[i].vaddr;
	char *filesize = vaddr + exec->segments[i].file_size;
	int pagenr = (fault - vaddr) / pageSize; //numarul paginii se obtine prin impartirea la marimea unei pagini

	char *aligned = (char *)ALIGN_DOWN((uintptr_t)fault, pageSize);
	char *addr = mmap(aligned, pageSize, PROT_WRITE, //se mapeaza o pagina, de la inceputul ei
			MAP_ANONYMOUS | MAP_FIXED | MAP_SHARED, 0, 0);

	if (addr == MAP_FAILED)
		exit(-1);

	int length = pageSize;

	if (aligned + pageSize > filesize) { 
		if (aligned < filesize)
			length = filesize - aligned;
		else
			length = 0;
	}

	lseek(fd, exec->segments[i].offset + pagenr * pageSize, SEEK_SET);
	read(fd, addr, length);

	if (mprotect(addr, pageSize, exec->segments[i].perm) == -1)
		exit(-1);
}

int so_init_loader(void)
{
	int rc;
	struct sigaction sa;

	memset(&sa, 0, sizeof(sa));
	sa.sa_sigaction = segv_handler;
	sa.sa_flags = SA_SIGINFO;
	rc = sigaction(SIGSEGV, &sa, NULL);
	if (rc < 0) {
		perror("sigaction");
		return -1;
	}
	return 0;
}

int so_execute(char *path, char *argv[])
{
	exec = so_parse_exec(path);
	if (!exec)
		return -1;

	fd = open(path, O_RDONLY);
	so_start_exec(exec, argv);

    close(fd);
	return -1;
}
