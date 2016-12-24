#include <errno.h>
int personality(unsigned long);
extern char **environ;

#define MY_ADDR_NO_RANDOMIZE 0x0040000
int main (int argc, char**argv) {
	int mypersonality = personality (0xffffffff);
	if (errno == 0 && !(mypersonality & MY_ADDR_NO_RANDOMIZE)) {
		personality (mypersonality | MY_ADDR_NO_RANDOMIZE);
		execve(argv[1], &argv[1], environ);
	}
}
