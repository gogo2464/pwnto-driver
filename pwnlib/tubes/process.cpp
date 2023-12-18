#ifdef _WIN32
    #include <windows.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
	   
#include "process.hpp"

#define TUBE_PTY 1

Process::Process(char *cmd)
{
	/**
	 * @brief open a file.
	 *
	 * @param file , the file to execute.
	 * @return Nothing.
	 *
	 * @test
	 * Tube * p = process("python");
	 * p.pwnto_send("print('Hello')\n");
	 * CHECK(p.pwnto_recv(5) == "Hello");
	 */
	
	
	#ifdef _WIN32
    HANDLE hStdInRead, hStdInWrite;
    SECURITY_ATTRIBUTES saAttr = {sizeof(SECURITY_ATTRIBUTES), NULL, TRUE};

    // Create a pipe for the child process's stdin
    if (!CreatePipe(&hStdInRead, &hStdInWrite, &saAttr, 0)) {
        fprintf(stderr, "CreatePipe failed (%lu)\n", GetLastError());
        exit(-1);
    }

    // Ensure the write handle to the pipe for stdin is not inherited
    SetHandleInformation(hStdInWrite, HANDLE_FLAG_INHERIT, 0);

    PROCESS_INFORMATION piProcInfo;
    STARTUPINFO siStartInfo;

    ZeroMemory(&piProcInfo, sizeof(PROCESS_INFORMATION));
    ZeroMemory(&siStartInfo, sizeof(STARTUPINFO));

    siStartInfo.cb = sizeof(STARTUPINFO);
    siStartInfo.hStdInput = hStdInRead;
    siStartInfo.hStdOutput = GetStdHandle(STD_OUTPUT_HANDLE); // Optionally redirect stdout
    siStartInfo.hStdError = GetStdHandle(STD_ERROR_HANDLE);   // Optionally redirect stderr
    siStartInfo.dwFlags |= STARTF_USESTDHANDLES;


    if (!CreateProcess(
            NULL,               // No module name (use command line)
            cmd, // Replace with the actual child process command
            NULL,               // Process handle not inheritable
            NULL,               // Thread handle not inheritable
            TRUE,               // Set handle inheritance to TRUE
            0,                  // No creation flags
            NULL,               // Use parent's environment block
            NULL,               // Use parent's starting directory
            &siStartInfo,       // Pointer to STARTUPINFO structure
            &piProcInfo)) {     // Pointer to PROCESS_INFORMATION structure
        fprintf(stderr, "CreateProcess failed (%lu)\n", GetLastError());
        exit(-1);
    }
	
	printf("debugging\n");
	
	
	// Close the unused handles
    /*CloseHandle(hStdInRead);
    CloseHandle(hStdInWrite);
	*/
	
	//Process *proc = malloc(sizeof(Process));
	
	
	
	
	
	
	this->process_handle = piProcInfo;
	this->process_startup_info = siStartInfo;
	
	
	
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
	
    //return NULL;
}

void * Process::recv(int size) {
	printf("recv!");
	return (void*) "abc";
}

void Process::send(void *input) {
    // Simulate sending input to the child process
    //const char* inputData = "print('python has been done')";
    DWORD bytesWritten;
    WriteFile(this->process_startup_info.hStdInput, input, strlen((char *)input), &bytesWritten, NULL);
	
	
	printf("testing\n");
	
	//printf("%s response got\n", bytesWritten);
}

void Process::libs() {
	
}

void Process::libc() {
	
}

void Process::bin() {
	
}

void Process::close() {
    WaitForSingleObject(this->process_handle.hProcess, INFINITE);
    CloseHandle(this->process_handle.hProcess);
    CloseHandle(this->process_handle.hThread);
	
	// Close the write handle to indicate the end of input
    CloseHandle(this->process_startup_info.hStdInput);
}