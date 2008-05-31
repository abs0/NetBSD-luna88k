/*      $NetBSD: run.c,v 1.13 2005/01/12 17:41:56 peter Exp $       */

/*
 * Copyright (c) 2000 The NetBSD Foundation, Inc.
 * All rights reserved.
 *
 * Copyright (c) 2000 Tim Rightnour <garbled@NetBSD.org>
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *        This product includes software developed by the NetBSD
 *        Foundation, Inc. and its contributors.
 * 4. Neither the name of The NetBSD Foundation nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE NETBSD FOUNDATION, INC. AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE FOUNDATION OR CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

/* XXX write return codes ignored. XXX */

#include <sys/param.h>
#include <sys/ioctl.h>
#include <sys/wait.h>
#include <sys/stat.h>

#include <curses.h>
#include <err.h>
#include <errno.h>
#include <fcntl.h>
#include <poll.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>
#include <util.h>

#include "sushi.h"
#include "run.h"

#define MAXBUF 2048

char *savelines[1000];

extern int scripting;
extern int logging;
extern FILE *logfile;
extern FILE *script;
extern nl_catd catalog;

/*
 * local prototypes 
 */
static int launch_subwin(WINDOW *, char **, struct winsize, int);
static int handleoflow(WINDOW *, int, int, int, int);

#define BUFSIZE 4096


static int
handleoflow(WINDOW *actionwin, int x, int y, int xcor, int ycor)
{
	if (ycor + 1 >= getmaxy(actionwin)) {
		if (x == 999) {
			int i;
			for (i = 1; i < 1000; i++)
				savelines[i - 1] = savelines[i];
		} else
			x++;
		savelines[x] = malloc(y);
		mvwinnstr(actionwin, 0, 0, savelines[x], y);
		wmove(actionwin, getmaxy(actionwin) - 1, 0);
		scroll(actionwin);
		wmove(actionwin, getmaxy(actionwin) - 1, 0);
	} else
		wmove(actionwin, ycor + 1, 0);
	return (x);
}

/*
 * launch a program inside a subwindow, and report it's return status when done
 */
static int
launch_subwin(WINDOW *actionwin, char **args, struct winsize win, int display)
{
	int xcor,ycor;
	int j, x;
	int pollfailed, multiloop, cols;
	int status, master, slave;
	struct pollfd set[2];
	int dataflow[2];
	pid_t child, subchild, pid;
	ssize_t n;
	char ibuf[MAXBUF], obuf[MAXBUF];
	char *command, *p, *argzero, **origargs;
	struct termios rtt, stt;
	struct termios tt;

	pipe(dataflow);

	argzero = *args;
	origargs = args;
	x = -1; /* we inc x before using it */

	command = (char *)malloc(MAXBUF * sizeof(char));
	command[0] = '\0';
	for (p = *args; p != NULL; p = *++args) {
		strlcat(command, p, MAXBUF);
		strlcat(command, " ", MAXBUF);
	}

	(void)tcgetattr(STDIN_FILENO, &tt);
	rtt = tt;
	rtt.c_lflag &= ~ECHO; 
	(void)tcsetattr(STDIN_FILENO, TCSAFLUSH, &rtt);

	if (openpty(&master, &slave, NULL, &tt, &win) == -1)
		bailout("openpty: %s", strerror(errno));

	(void)tcgetattr(slave, &stt);
	stt.c_lflag |= ICANON|ISIG|IEXTEN|ECHO;
	stt.c_iflag |= ICRNL;
	stt.c_oflag |= OPOST;
	(void)tcsetattr(slave, TCSAFLUSH, &stt);

	/* ignore tty signals until we're done with subprocess setup */
#if 0
	savetty();
	reset_shell_mode();
#endif
	endwin();
#if 0
	ttysig_ignore = 1;
#endif

	switch (child = fork()) {
	case -1:
#if 0
		ttysig_ignore = 0;
#endif
		refresh();
		bailout("fork: %s", strerror(errno));
		/* NOTREACHED */
		break;
	case 0:
		(void)close(STDIN_FILENO);
		subchild = fork();
		if (subchild == 0) {
			close(dataflow[0]);
			for (;;) {
				n = read(master, obuf, sizeof(obuf));
				if (n <= 0)
					break;
				write(dataflow[1], obuf, (size_t)n);
			} /* while spinning */
			_exit(EXIT_SUCCESS);
		} /* subchild, child forks */
		(void)close(master);
		(void)close(dataflow[1]);
		(void)close(dataflow[0]);
		login_tty(slave);
		if (logging) {
			fprintf(logfile, "%s: %s\n",
			    catgets(catalog, 3, 1, "executing"), command);
			fflush(logfile);
			fclose(logfile);
		}
		if (scripting) {
			fprintf(script, "%s\n", command);
			fflush(script);
			fclose(script);
		}
		execvp(argzero, origargs);
		/* the parent will see this as the output from the
		   child */
		warn("execvp %s", argzero);
		_exit(EXIT_FAILURE);
		break; /* end of child */
	default:
		/*
		 * we've set up the subprocess.  forward tty signals to its			 * process group.
		 */
#if 0
		ttysig_forward = child;
		ttysig_ignore = 0;
#endif
		refresh();
		break;
	}
#if 0
	resetty();
#endif
	close(dataflow[1]);
	set[0].fd = dataflow[0];
	set[0].events = POLLIN;
	set[1].fd = STDIN_FILENO;
	set[1].events = POLLIN;

	cols = 0;
	for (pollfailed = 0, multiloop = 0;;) {
again:
		if (multiloop > 10)
			goto loop; /* XXX */
		if (pollfailed) {
			char *msg = catgets(catalog, 1, 7,
			    "poll failed but no child died");
			if (logging)
				(void)fprintf(logfile, msg);
			bailout(msg);
		}
		if (poll(set, 2, INFTIM) < 0) {
			if (errno == EINTR)
				goto loop;
			perror("poll");
			if (logging)
				(void)fprintf(logfile, "%s: %s\n",
				    catgets(catalog, 1, 17, "poll failure"),
				    strerror(errno));
			++pollfailed;
		} else {
			if (set[1].revents & POLLIN) {
				n = read(STDIN_FILENO, ibuf, MAXBUF);
#if 0
				if (n < 0 && errno == EBADF)
					set[1].revents = 0;
#endif
				if (n > 0) {
					multiloop = 0;
					(void)write(master, ibuf, (size_t)n);
				}
			}
			if (set[0].revents & POLLIN) {
				n = read(dataflow[0], ibuf, MAXBUF);
#if 0
				if (n < 0 && errno == EBADF)
					set[0].revents = 0;
#endif
				if (n > 0) {
					multiloop = 0;
					if (display) {
						for (j = 0; j < n; j++) {
							cols++;
							if (cols == getmaxx(actionwin)
							    && ibuf[j] != '\n') {
								cols = 0;
								getyx(actionwin, ycor, xcor);
								x = handleoflow(actionwin, x,
								    win.ws_col, xcor, ycor);
							}
							switch (ibuf[j]) {
							case '\n':
								cols = 0;
								getyx(actionwin, ycor, xcor);
								x = handleoflow(actionwin, x,
								    win.ws_col, xcor, ycor);
								break;
							case '\r':
								getyx(actionwin, ycor, xcor);
								wmove(actionwin, ycor, 0);
								break;
							case '\b':
								getyx(actionwin, ycor, xcor);
								if (xcor <= 0)
									break;
								wmove(actionwin, ycor, xcor - 1);
								break;
							default:
								waddch(actionwin, ibuf[j]);
								break;
							}
							if (logging)
								putc(ibuf[j], logfile);
						}
						wrefresh(actionwin);
					}
					if (logging)
						fflush(logfile);
				}
			}
		}
		multiloop++;
		goto again;
loop:
		pid = wait4(child, &status, WNOHANG, 0);
 		if (pid == child && (WIFEXITED(status) || WIFSIGNALED(status)))
			break;
	}
	close(dataflow[0]); /* clean up our leaks */
	close(master);
	close(slave);
	if (logging)
		fflush(logfile);

#if 0
	/* from here on out, we take tty signals ourselves */
	ttysig_forward = 0;
#endif

	(void)tcsetattr(STDIN_FILENO, TCSAFLUSH, &tt);

	savelines[x+1] = NULL;

	if (WIFEXITED(status))
		return (WEXITSTATUS(status));
	else if (WIFSIGNALED(status))
		return (WTERMSIG(status));
	else
		return (0);
}

/*
 * generic program runner.  display can be set to
 * 1 if you wish the output to be displayed.
 */
int
run_prog(int display, char **args)
{
	struct winsize win;
	int ret, n, i, done, numlines, x, y, curline;
	WINDOW *actionwin, *statuswin, *boxwin;
	char *command;
	char buf2[MAXBUF];

	command = buf2;
	snprintf(buf2, sizeof(buf2), "%s ", args[0]);
	for (i = 1; args[i] != NULL; i++) {
		strlcat(buf2, args[i], sizeof(buf2));
		strlcat(buf2, " ", sizeof(buf2));
	}

	(void)ioctl(STDIN_FILENO, TIOCGWINSZ, &win);
	/* Apparently, we sometimes get 0x0 back, and that's not useful */
	if (win.ws_row == 0)
		win.ws_row = 24;
	if (win.ws_col == 0)
		win.ws_col = 80;

	if (!display) {
		ret = launch_subwin(NULL, args, win, 0);
		return (ret);
	}
	wclear(stdscr);
	clearok(stdscr, 1);
	refresh();

	statuswin = subwin(stdscr, 4, win.ws_col, 0, 0);
	if (statuswin == NULL)
		bailout(catgets(catalog, 1, 8,
		    "failed to allocate status window"));

	boxwin = subwin(stdscr, 1, win.ws_col, 4, 0);
	if (boxwin == NULL)
		bailout(catgets(catalog, 1, 9,
		    "failed to allocate status box"));

	actionwin = subwin(stdscr, win.ws_row - 5, win.ws_col, 5, 0);
	if (actionwin == NULL)
		bailout(catgets(catalog, 1, 10,
		    "failed to allocate output window"));

	scrollok(actionwin, TRUE);

	win.ws_row -= 5;

	wmove(statuswin, 0, 11 - (int)strlen(catgets(catalog, 3, 2, "Status")));
	waddstr(statuswin, catgets(catalog, 3, 2, "Status"));
	waddstr(statuswin, ": ");
	wstandout(statuswin);
	waddstr(statuswin, catgets(catalog, 3, 3, "Running"));
	wstandend(statuswin);
	wmove(statuswin, 1, 11 - (int)strlen(catgets(catalog, 3, 4, "Command")));
	waddstr(statuswin, catgets(catalog, 3, 4, "Command"));
	waddstr(statuswin, ": ");
	wstandout(statuswin);
	if (strlen(command) > (win.ws_col - 14)) {
		command[win.ws_col - 17] = '.';
		command[win.ws_col - 16] = '.';
		command[win.ws_col - 15] = '.';
		command[win.ws_col - 14] = '\0';
	}
	waddstr(statuswin, command);
	wstandend(statuswin);
	mvwaddstr(statuswin, 0,
	    win.ws_col - 12 - (int)strlen(catgets(catalog, 3, 5, "Lines")),
	    catgets(catalog, 3, 5, "Lines"));
	waddstr(statuswin, " (   0/   0)");
	wstandout(statuswin);
	mvwaddstr(statuswin, 3, 0, catgets(catalog, 3, 9,
	    "Use HOME,END,PGUP,PDGN,UP/DOWN Arrow keys to scroll.  ESC,"
	    " F3 exits, F10 quits."));
	wstandend(statuswin);
	wrefresh(statuswin);

	wmove(boxwin, 0, 0);
	for (n = 0; n < win.ws_col; n++)
		waddch(boxwin, ACS_HLINE);
	wrefresh(boxwin);

	wrefresh(actionwin);

	ret = launch_subwin(actionwin, args, win, 1);

	wmove(statuswin, 0, 13);
	wstandout(statuswin);
	waddstr(statuswin, ret ? catgets(catalog, 3, 6, "Failed") :
	    catgets(catalog, 3, 7, "Finished"));
	wstandend(statuswin);
	waddstr(statuswin, "  ");
	wmove(statuswin, 2, 5);
	waddstr(statuswin, catgets(catalog, 3, 8, "Press any key to continue"));
	wrefresh(statuswin);

	/* process the scroller */
	noecho();
	keypad(stdscr, TRUE);
	done = 0;
	for (numlines = 0; savelines[numlines] != NULL; numlines++)
		continue;
	for (x = 0; x < win.ws_row; x++) {
		if (numlines == 999) {
			for (y = 1; y < 1000; y++)
				savelines[y-1] = savelines[y];
		} else
			savelines[numlines] = malloc(win.ws_col);
		mvwinnstr(actionwin, x, 0, savelines[numlines], win.ws_col);
		numlines++;
	}
	savelines[numlines] = NULL;
	numlines--;
	curline = numlines - win.ws_row+1;
	mvwprintw(statuswin, 0, win.ws_col - 10, "%4d/%4d", curline, numlines);
	wrefresh(statuswin);
	while (!done) {
		i = getch();
		switch (i) {
		case KEY_F(10):
			endwin();
			exit(EXIT_SUCCESS);
			break;
		case KEY_HOME:
			wclear(actionwin);
			for (x = 0; x < win.ws_row - 1; x++)
				if (savelines[x] != NULL)
					mvwaddstr(actionwin, x, 0,
					    savelines[x]);
			curline = 0;
			mvwprintw(statuswin, 0, win.ws_col-10, "%4d",
			    curline);
			wrefresh(statuswin);
			wrefresh(actionwin);
			break;
		case KEY_END:
			wclear(actionwin);
			curline = numlines - win.ws_row + 1;
			for (x = numlines; x > numlines - win.ws_row; x--)
				if (savelines[x] != NULL)
					mvwaddstr(actionwin, x - curline,
					    0, savelines[x]);
			mvwprintw(statuswin, 0, win.ws_col-10, "%4d",
			    curline);
			wrefresh(statuswin);
			wrefresh(actionwin);
			break;
		case KEY_UP:
			if (curline == 0)
				break;
			wclear(actionwin);
			curline--;
			for (x = 0; x < win.ws_row - 1; x++, curline++)
				if (savelines[curline] != NULL)
					mvwaddstr(actionwin, x, 0,
					    savelines[curline]);
			curline -= win.ws_row - 1;
			mvwprintw(statuswin, 0, win.ws_col-10, "%4d", curline);
			wrefresh(statuswin);
			wrefresh(actionwin);
			break;
		case KEY_DOWN:
			if (curline + win.ws_row - 1 >= numlines)
				break;
			wclear(actionwin);
			curline++;
			for (x = curline; x < curline + win.ws_row; x++)
				if (savelines[x] != NULL)
					mvwaddstr(actionwin, x-curline, 0,
					    savelines[x]);
			mvwprintw(statuswin, 0, win.ws_col-10, "%4d", curline);
			wrefresh(statuswin);
			wrefresh(actionwin);
			break;
		case KEY_PPAGE:
			if (curline - win.ws_row < 0) {
				wclear(actionwin);
				for (x = 0; x < win.ws_row - 1; x++)
					if (savelines[x] != NULL)
						mvwaddstr(actionwin, x, 0,
						    savelines[x]);
				curline = 0;
				mvwprintw(statuswin, 0, win.ws_col-10, "%4d",
				    curline);
				wrefresh(statuswin);
				wrefresh(actionwin);
			} else {
				wclear(actionwin);
				curline -= win.ws_row - 1;
				for (x = 0; x < win.ws_row-1; x++, curline++)
					if (savelines[x] != NULL)
						mvwaddstr(actionwin, x, 0,
						    savelines[curline]);
				curline -= win.ws_row - 1;
				mvwprintw(statuswin, 0, win.ws_col-10, "%4d",
				    curline);
				wrefresh(statuswin);
				wrefresh(actionwin);
			}
			break;
		case KEY_NPAGE:
			if (curline + win.ws_row == numlines) {
				;
			} else if (curline + win.ws_row * 2 - 2 > numlines) {
				wclear(actionwin);
				curline = numlines - win.ws_row + 1;
				for (x = numlines; x > numlines-win.ws_row; x--)
					if (savelines[x] != NULL)
						mvwaddstr(actionwin, x-curline,
						    0, savelines[x]);
				mvwprintw(statuswin, 0, win.ws_col-10, "%4d",
				    curline);
				wrefresh(statuswin);
				wrefresh(actionwin);
			} else {
				wclear(actionwin);
				for (x = 0; x < win.ws_row - 1; x++, curline++)
					if (savelines[x] != NULL) {
						mvwaddstr(actionwin, x, 0,
						 savelines[curline+win.ws_row]);
						wrefresh(actionwin);
					}
				mvwprintw(statuswin, 0, win.ws_col-10, "%4d",
				    curline);
				wrefresh(statuswin);
				wrefresh(actionwin);
			}
			break;
		default:
			done = 1;
			break;
		}
	}
	/* clean things up */
	delwin(actionwin);
	delwin(boxwin);
	delwin(statuswin);

	wclear(stdscr);
	clearok(stdscr, 1);
	refresh();

	return (ret);
}
