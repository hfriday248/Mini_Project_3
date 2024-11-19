#include "types.h"
#include "user.h"
#include "fcntl.h"

#define assert(x) if (x) {} else { \
   printf(1, "%s: %d ", __FILE__, __LINE__); \
   printf(1, "assert failed (%s)\n", # x); \
   printf(1, "TEST FAILED\n"); \
   kill(getpid()); \
   exit(); \
}

// Function prototypes
int null(int);
int null2(int);
int bounds(int);

int main()
{
	int p = getpid();
	null(p);
	null2(p);
	bounds(p);
	exit();
}

int null(int p)
{
	int pid = fork();
	if(pid < 0)
	{
		printf(1, "Fork failed. TEST FAILED!\n");
		kill(getpid());
		exit();
	}
	else if(pid == 0)
	{
		uint *nullPtr = (uint*) 0;
		printf(1, "null dereference: ");
		printf(1, "%x %x\n", nullPtr, *nullPtr);
		// this process should be killed. If not, then the test failed.
		printf(1, "TEST FAILED!\n");
		kill(p);
		exit();
	}
	else
	{
		wait();
		printf(1, "TEST PASSED!\n");
		return 0;
	}
}
//any dereference from first page should fault
int null2(int p)
{
	int pid = fork();
	if(pid < 0)
	{
		printf(1, "Fork failed. TEST FAILED!\n");
		exit();
	}
	else if(pid == 0)
	{
		uint *badPtr = (uint*) 4095;
		printf(1, "bad dereference (0x0fff): ");
		printf(1, "%x %x\n", badPtr, *badPtr);
		// this process should be killed. If not, then the test failed.
		printf(1, "TEST FAILED!\n");
		kill(p);
		exit();
	}
	else
		wait();
	printf(1, "TEST PASSED!\n");
	return 0;
}

// Ensure syscall arg checks null page
int bounds(int p)
{
	char *arg;
	int fd = open("tmp", O_WRONLY|O_CREATE);
	assert(fd != -1);
	// at zero
	arg = (char*) 0x0;
	assert(write(fd, arg, 10) == -1);
	// within null page
	arg = (char*) 0x400;
	assert(write(fd, arg, 1024) == -1);
	// spanning null page and code
	arg = (char*) 0xfff;
	assert(write(fd, arg, 2) == -1);
	printf(1, "TEST PASSED!\n");
	return 0;
}

