#define  _GNU_SOURCE
#include <stdio.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

int main()
{
	FILE* obj = fopen("adder.o", "rb");
	struct stat st;

	if(obj == NULL){
		printf("unable to open file\n");
		return -1;
	}
	
	stat((const char*)obj, &st);
	int size = st.st_size;
	int fd = memfd_create("obj_in_mem", MFD_ALLOW_SEALING);
	
	if(fd == -1){
		return -1;
	}
	
	if(ftruncate(fd, size) == -1){
		printf("ftruncate failed\n");
		return -1;
	}

	printf("PID %jd \n", (int) getpid());
	printf("FD  %d  \n", fd);

	getchar();

	fclose(obj);

	return 0;
}
