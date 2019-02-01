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
 
#include <stdio.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <pty.h>
#include <termios.h>
#include <fcntl.h>

#include <sys/select.h>
#include <sys/wait.h>

#define TRACEPOINT_DEFINE
#include "sample_component_provider.h"

int main()
{

	int master;
	pid_t pid;

	pid = forkpty(&master, NULL, NULL, NULL);

	// impossible to fork
	if (pid < 0) {
		return 1;
	}

	else if (pid == 0) {
		while(1) {
			printf("Hello World child\n");
			tracepoint(sample_component, message, "Hello World child");
			sleep(1);
		}
	}

	else {
		struct termios tios;
		tcgetattr(master, &tios);
		tios.c_lflag &= ~(ECHO | ECHONL);
		tcsetattr(master, TCSAFLUSH, &tios);

		for (;;) {
			fd_set          read_fd;
			fd_set          write_fd;
			fd_set          except_fd;

			FD_ZERO(&read_fd);
			FD_ZERO(&write_fd);
			FD_ZERO(&except_fd);

			FD_SET(master, &read_fd);
			FD_SET(STDIN_FILENO, &read_fd);

			select(master+1, &read_fd, &write_fd, &except_fd, NULL);

			char input;
			char output;

			if (FD_ISSET(master, &read_fd))
			{
				// leia o que bc esta mandando
				if (read(master, &output, 1) != -1)
					// e escreva isso na saida padrao
					write(STDOUT_FILENO, &output, 1);
				else
					break;
				if (output == '\n') {
					printf("Hello World parent\n");
					tracepoint(sample_component, message, "Hello World parent");
				}
			}

			if (FD_ISSET(STDIN_FILENO, &read_fd))
			{
				read(STDIN_FILENO, &input, 1);
				write(master, &input, 1);
			}
		}
	}
	return 0;
}
