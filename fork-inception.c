/*
 * Copyright (C) 2019 Jonathan Rajotte
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
 
#include <unistd.h>
#include <stdio.h> 
#include <sys/types.h> 
#include <unistd.h> 
#include <stdlib.h> 
#include <string.h> 

/*
 * We need to define TRACEPOINT_DEFINE in one C file in the program
 * before including provider headers.
 */
#define TRACEPOINT_DEFINE
#include "sample_component_provider.h"

int main(int argc, char **argv)
{
	int i = 0;
	int pid;

	if (argc >= 2 && strcmp(argv[1], "unload")) {
		printf("invalid parameter\n");
		printf("Usage: pass 'unload' to remove the LD_PRELOAD on exec of child\n");
		return 1;
	}

	
	pid = fork();

	if (pid == 0) {
		if (argc >= 2 && !strcmp(argv[1], "unload")) {
			printf("UNSET LD_PRELOAD\n");
			unsetenv("LD_PRELOAD");
		}
		execl("./lttng-fork", "lttng-fork", NULL);

	} else {
		while(1) {
			printf("Hello World grand-parent\n");
			tracepoint(sample_component, message, "Hello World grand_parent");
			sleep(1);
		}
	}
	return 0;
}
