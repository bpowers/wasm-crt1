#include <stdint.h>
#include <string.h>
#include <unistd.h>

#define INIT_MSG "test initializing\n"
#define FINI_MSG "test cleaning up\n"
#define MAIN_MSG "fib done calculating\n"

__attribute__((constructor))
static void
init(void)
{
	write(STDOUT_FILENO, INIT_MSG, strlen(INIT_MSG));
}

__attribute__((destructor))
static void
fini(void)
{
	write(STDOUT_FILENO, FINI_MSG, strlen(FINI_MSG));
}

int64_t
fib(int64_t n)
{
	if (n <= 1)
		return 1;
	else
		return fib(n-1) + fib(n-2);
}

int
main(int argc, const char *argv[])
{
	int64_t result = fib(5);

	write(STDOUT_FILENO, MAIN_MSG, strlen(MAIN_MSG));

	return result;
}
