#ifdef _WIN32
    #include <windows.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
	   
#include "process.h"

#define TUBE_PTY 1

Process proc;



Tube *process(char *cmd)
{
	/**
	 * @brief open a file.
	 *
	 * @param file , the file to execute.
	 * @return Nothing.
	 *
	 * @test
	 * Tube * p = process("python");
	 * p.send("print('Hello')\n");
	 * CHECK(p.recv(5) == "Hello");
	 */
	
	
	#ifdef _WIN32
		const char* processName = cmd;
		
		// Open the target process
		
		
		STARTUPINFO info={sizeof(info)};
		PROCESS_INFORMATION processInfo;
		if (CreateProcess(NULL, cmd, NULL, NULL, TRUE, 0, NULL, NULL, &info, &processInfo))
		{
			WaitForSingleObject(processInfo.hProcess, INFINITE);
			CloseHandle(processInfo.hProcess);
			CloseHandle(processInfo.hThread);
		}

		HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, GetProcessId(processInfo.hProcess));
		if (hProcess == NULL) {
			printf("Failed to open process. Error %lu\n", GetLastError());
			exit(-1);
		}

		// Replace this with the target address you want to read
		uintptr_t targetAddress = 0x00400000;
		SIZE_T bytesRead;
		char buffer[100];

		// Read memory from the target process
		if (ReadProcessMemory(hProcess, (LPCVOID)targetAddress, buffer, sizeof(buffer), &bytesRead)) {
			printf("Read %lu bytes from target process:\n", bytesRead);
			// Print or manipulate the data as needed
			// ...
		} else {
			printf("Failed to read process memory. Error %lu\n", GetLastError());
		}

		// Close the process handle
		CloseHandle(hProcess);
    #elif __linux__

    int fdm, fds;
    // for pipe
    int fd_out[2];
    int fd_in[2];
    pid_t pid;
	
    if (TUBE_PTY)
    {
        // Open a new pseudoterminal
        if ((fdm = posix_openpt(O_RDWR | O_NOCTTY)) == -1)
        {
            perror("posix_openpt");
            exit(EXIT_FAILURE);
        }

        // Grant access to the slave pseudoterminal
        if (grantpt(fdm) == -1)
        {
            perror("grantpt");
            exit(EXIT_FAILURE);
        }

        // Get the current terminal attributes
        if (tcgetattr(fdm, &tios) < 0)
        {
            perror("tcgetattr");
            exit(EXIT_FAILURE);
        }

        // https://viewsourcecode.org/snaptoken/kilo/02.enteringRawMode.html
        // raw mode
        tios.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);
        tios.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
        tios.c_cflag &= ~(CSIZE | PARENB);
        tios.c_cflag |= CS8;
        tios.c_oflag &= ~(OPOST);
        tios.c_oflag &= ~ONLCR; // disable \r

        // Set the new attributes
        if (tcsetattr(fdm, TCSANOW, &tios) < 0)
        {
            perror("tcsetattr");
            exit(EXIT_FAILURE);
        }

        // Unlock the slave pseudoterminal
        if (unlockpt(fdm) == -1)
        {
            perror("unlockpt");
            exit(EXIT_FAILURE);
        }

        char *slave_name;
        // get slave pty name
        if ((slave_name = ptsname(fdm)) == NULL)
        {
            perror("ptsname_r");
            exit(EXIT_FAILURE);
        }

        // open slave pty
        if ((fds = open(slave_name, O_RDWR | O_NOCTTY)) == -1)
        {
            perror("open slave");
            exit(EXIT_FAILURE);
        }
    }
    else
    {

        if (pipe(fd_out) == -1 || pipe(fd_in) == -1)
        {
            perror("pipe");
        }
    }

    // Fork a child process
    if ((pid = fork()) == -1)
    {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    // Child process
    if (pid == 0)
    {
        if (TUBE_PTY)
        {
            close(fdm); // Close unused file descriptor

            // Make the current process a new session leader
            setsid();

            // Set the controlling terminal to the slave pseudoterminal
            ioctl(fds, TIOCSCTTY, 0);

            close(0);
            close(1);
            close(2);

            dup2(fds, STDIN_FILENO);
            dup2(fds, STDOUT_FILENO);
            dup2(fds, STDERR_FILENO);
        }
        else
        {
            /* Duplicate FD to FD2, closing FD2 and making it open on the same file */
            close(fd_in[WRITE_END]);
            close(fd_out[READ_END]);

            dup2(fd_in[READ_END], STDIN_FILENO);
            dup2(fd_out[WRITE_END], STDOUT_FILENO);
            dup2(fd_out[WRITE_END], STDERR_FILENO);

            close(fd_in[READ_END]);
            close(fd_out[WRITE_END]);
        }

        /* Executes same as popen() syscall */
        if (execl(_PATH_BSHELL, "sh", "-c", cmd, (char *)NULL) == -1)
        {
            perror("execl");
        }

        exit(EXIT_FAILURE);

        // Parent
    }
    else
    {

        // Add forked process to a pidNode linked list to keep track of all forked processes
        Tube *tube = malloc(sizeof(Tube));
        tube->type = PROCESS_TUBE;

        /* init empty buffer to tube struct */
        tube->buffer = str_new("");

        /* Stores file descriptors in current struct */
        if (TUBE_PTY)
        {
            close(fds);
            ///TODO: tube->fd_out = fdm;
            //TODO: tube->fd_in = fdm;
        }
        else
        {
            close(fd_in[READ_END]);
            close(fd_out[WRITE_END]);

            tube->fd_out = fd_out[READ_END];
            tube->fd_in = fd_in[WRITE_END];
        }

        // Add forked process to a pidNode linked list to keep track of all forked processes
        pidNode *pid_node = malloc(sizeof(pidNode));
        if (!pid_node)
        {
            perror("malloc");
            return NULL;
        }

        pid_node->pid = pid;
        pid_node->next = head;
        pid_node->prev = NULL;

        tube->pid_node = pid_node;

        // Register to kill all processes when program exits
        if (head == NULL)
        {
            atexit(kill_processes);
        }

        if (head != NULL)
        {
            head->prev = pid_node;
        }
        head = pid_node;

        LOG_INFO("Starting local process '%s': pid %d", cmd, pid);

        return tube;
    }
    #else
        printf("This code is compiled on an unknown operating system.\n");
    #endif
	
    return NULL;
}

#define recv win_recv
void *recv(int size) {
	char *buff;
	fgets(buff, sizeof(size), proc.process_PID);
}
#undef recv


#define send win_send
void send() {
	printf("hey!");
}
#undef send

void libs() {
	
}

void libc() {
	
}

void bin() {
	
}