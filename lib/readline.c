#include "inc/stdio.h"
#include "inc/error.h"

#define BUFLEN 1024
static char buf[BUFLEN];

/**
 * 等待用户输入一个命令字符串，"回车"代表命令行结束
 */
char *
readline(const char *prompt)
{
	int i, c, echoing;

#if ALVOS_KERNEL
	if (prompt != NULL)
		cprintf("%s", prompt);
#else
	if (prompt != NULL)
		fprintf(1, "%s", prompt);
#endif

	i = 0;
	echoing = iscons(0);
	while (1)
	{
		c = getchar();
		if (c < 0)
		{
			if (c != -E_EOF)
				cprintf("read error: %e\n", c);
			return NULL;
		}
		else if ((c == '\b' || c == '\x7f') && i > 0)
		{
			if (echoing)
				cputchar('\b');
			i--;
		}
		else if (c >= ' ' && i < BUFLEN - 1)
		{
			if (echoing)
				cputchar(c);
			buf[i++] = c;
		}
		else if (c == '\n' || c == '\r')
		{
			if (echoing)
				cputchar('\n');
			buf[i] = 0;
			return buf;
		}
	}
}
