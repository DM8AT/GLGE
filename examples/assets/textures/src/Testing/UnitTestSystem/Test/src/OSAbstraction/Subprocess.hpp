/**
 * @file Subprocess.hpp
 * @author DM8AT
 * @brief define a simple abstraction of subprocesses
 * @version 0.1
 * @date 2026-05-20
 * 
 * @copyright Copyright (c) 2026
 * 
 */
//header guard
#pragma once

#ifndef OS_ABSTRACTION_DETECT_OS_MSG
#define OS_ABSTRACTION_DETECT_OS_MSG
    #if defined(_WIN32)
        #pragma message "Detected platform: Windows"
    #elif defined(__linux__)
        #pragma message "Detected platform: Linux (assuming posix)"
    #elif defined(__APPLE__)
        #pragma message "Detected platform: MaCOS"
    #else
        #error Unsupported platform
    #endif
#endif

//include the OS-Dependent headers
#ifdef _WIN32
    //add the windows API
    #define NOMINMAX
    #include <windows.h>
#else
    //add all libs for linux
    #include <spawn.h>
    #include <sys/wait.h>
    #include <signal.h>
    #include <unistd.h>
    #include <fcntl.h>

    extern char** environ;

#endif

//add the filesystem
#include <filesystem>
//add vectors
#include <vector>
//add threading
#include <thread>
//add arrays
#include <array>

/**
 * @brief a simple class that wraps subprocesses
 * 
 * This class is not thread safe
 */
class Subprocess {
public:

    /**
     * @brief store the default exit code value (meaning no process finished)
     */
    inline static constexpr int EXIT_CODE_DEFAULT = -1;
    /**
     * @brief store the exit code for a problematic exit
     */
    inline static constexpr int EXIT_CODE_ERROR = -2;

    /**
     * @brief Construct a new Subprocess
     */
    Subprocess() = default;

    //subprocesses cannot be copied
    Subprocess(const Subprocess&) = delete;
    Subprocess& operator=(const Subprocess&) = delete;

    /**
     * @brief Construct a new Subprocess
     * 
     * Move constructor
     * 
     * @param other the subprocess to move from
     */
    Subprocess(Subprocess&& other) noexcept {
    #ifdef _WIN32

        m_process = other.m_process;
        m_thread = other.m_thread;

        m_stdoutRead = other.m_stdoutRead;
        m_stderrRead = other.m_stderrRead;

        m_pid = other.m_pid;

        other.m_process = nullptr;
        other.m_thread = nullptr;

        other.m_stdoutRead = nullptr;
        other.m_stderrRead = nullptr;

        other.m_pid = 0;

    #else

        m_pid = other.m_pid;

        m_stdoutFd = other.m_stdoutFd;
        m_stderrFd = other.m_stderrFd;

        other.m_pid = -1;

        other.m_stdoutFd = -1;
        other.m_stderrFd = -1;

    #endif

        m_stdOutCache = std::move(other.m_stdOutCache);
        m_stdErrCache = std::move(other.m_stdErrCache);

        m_exitCode = other.m_exitCode;

        other.m_exitCode = -1;
    }
    /**
     * @brief Move operator
     * 
     * @param other the subprocess to move from
     * @return `Subprocess&` a reference to the subprocess after move
     */
    Subprocess& operator=(Subprocess&& other) noexcept {
        //stop if moving to self
        if (this == &other)
        {return *this;}

        //clean up
        cleanup();

    #ifdef _WIN32

        //copy all handles over
        m_process = other.m_process;
        m_thread = other.m_thread;

        m_stdoutRead = other.m_stdoutRead;
        m_stderrRead = other.m_stderrRead;

        m_pid = other.m_pid;

        //clean up the other
        other.m_process = nullptr;
        other.m_thread = nullptr;

        other.m_stdoutRead = nullptr;
        other.m_stderrRead = nullptr;

        other.m_pid = 0;

    #else

        //copy over the handles
        m_pid = other.m_pid;

        m_stdoutFd = other.m_stdoutFd;
        m_stderrFd = other.m_stderrFd;

        //clean up the other
        other.m_pid = -1;

        other.m_stdoutFd = -1;
        other.m_stderrFd = -1;

    #endif

        //move over the caches
        m_stdOutCache = std::move(other.m_stdOutCache);
        m_stdErrCache = std::move(other.m_stdErrCache);

        //move over the exit codes
        m_exitCode = other.m_exitCode;

        //clean up the other exit code
        other.m_exitCode = -1;

        //return a reference to self
        return *this;
    }

    /**
     * @brief Destroy the Subprocess
     * 
     * If the subprocess is running it will be terminated
     */
    ~Subprocess() {
        //stop the subprocess
        terminate();
        //clean up
        cleanup();
    }

    /**
     * @brief get if the subprocess has a running subprocess
     * 
     * @return `true` if a subprocess is being run, `false` otherwise
     */
    bool isRunning() const {
    #ifdef _WIN32

        //if no process ID is set, it is not running
        if (!m_process)
        {return false;}

        //query from the process if the process is running
        DWORD code{};
        if (!GetExitCodeProcess(m_process, &code))
        {return false;}

        //if the code is the alive marker, return it
        return code == STILL_ACTIVE;

    #else

        //if no process id is set, there is no subprocess running
        if (m_pid <= 0)
        {return false;}

        //get the status
        int status{};
        pid_t result = waitpid(
            m_pid,
            &status,
            WNOHANG
        );

        //return if the result is 0 (0 = still running marker)
        return result == 0;

    #endif
    }

    /**
     * @brief launch an executable
     * 
     * This function starts the subprocess and returns immediately. The subprocess will run in parallel. 
     * 
     * If a subprocess is running on this structure this call will return `false`. 
     * 
     * @param executable a path to the executable to run
     * @param args the arguments to pass to the executable
     * @return `true` if the launching was successful, `false` on failure
     */
    bool launch(const std::filesystem::path& executable, const std::vector<std::string>& args) {
        //if a process is running, stop
        if (isRunning())
        {return false;}

        //cleanup should be done by terminate, the subprocess must be in a clean state

        #ifdef _WIN32

            //create a job object
            m_job = CreateJobObjectW(NULL, NULL);
            if (m_job == NULL) {return false;}

            //setup the job limitations
            JOBOBJECT_EXTENDED_LIMIT_INFORMATION jeli = { 0 };
            jeli.BasicLimitInformation.LimitFlags = JOB_OBJECT_LIMIT_KILL_ON_JOB_CLOSE;
            if (!SetInformationJobObject(m_job, JobObjectExtendedLimitInformation, &jeli, sizeof(jeli))) {
                cleanup();
                return false;
            }

            //setup some security attributes
            SECURITY_ATTRIBUTES sa{};
            sa.nLength = sizeof(sa);
            sa.bInheritHandle = TRUE;

            //setup the pipes
            HANDLE stdoutWrite = nullptr;
            HANDLE stderrWrite = nullptr;
            if (!CreatePipe(&m_stdoutRead, &stdoutWrite, &sa, 0))
            {return false;}
            if (!CreatePipe(&m_stderrRead, &stderrWrite, &sa, 0)) {
                cleanup();
                return false;
            }
            SetHandleInformation(m_stdoutRead, HANDLE_FLAG_INHERIT, 0);
            SetHandleInformation(m_stderrRead, HANDLE_FLAG_INHERIT, 0);

            //get a path to the executable as a wstring
            std::wstring cmd = executable.wstring();

            //add all arguments to the command
            for (const auto& arg : args) {
                //parse the arguments
                cmd += L" \"";
                cmd += std::wstring(arg.begin(), arg.end()) + L"\"";
            }

            //fill out the process startup info
            STARTUPINFOW si{};
            si.cb = sizeof(si);
            si.dwFlags |= STARTF_USESTDHANDLES;
            si.hStdOutput = stdoutWrite;
            si.hStdError = stderrWrite;

            //store the process information
            PROCESS_INFORMATION pi{};

            //launch the process
            BOOL ok = CreateProcessW(nullptr, cmd.data(), nullptr, nullptr, TRUE, CREATE_NO_WINDOW, nullptr, nullptr, &si, &pi);
            //assign to the job
            AssignProcessToJobObject(m_job, m_process);
            //resume the process main thread
            ResumeThread(m_thread);

            //close the write handles
            CloseHandle(stdoutWrite);
            CloseHandle(stderrWrite);

            //check for failures
            if (!ok) {
                //clean up
                cleanup();
                return false;
            }

            //store the process and process main thread
            m_process = pi.hProcess;
            m_thread = pi.hThread;
            //store the process ID
            m_pid = pi.dwProcessId;

        #else

            //store the standard input and error pipe
            int stdoutPipe[2]{};
            int stderrPipe[2]{};

            //create the std out pipe
            if (pipe(stdoutPipe) != 0)
            {return false;}

            //create the error pipe
            if (pipe(stderrPipe) != 0) {
                //clean up the close pipe
                close(stdoutPipe[0]);
                close(stdoutPipe[1]);

                return false;
            }

            //create an action to prepare to add the output streams, but ignore the input streams
            posix_spawn_file_actions_t actions{};
            posix_spawn_file_actions_init(&actions);
            //add the output pipes to the action
            posix_spawn_file_actions_adddup2(&actions, stdoutPipe[1], STDOUT_FILENO);
            posix_spawn_file_actions_adddup2(&actions, stderrPipe[1], STDERR_FILENO);
            //close the unused pipes
            posix_spawn_file_actions_addclose(&actions, stdoutPipe[0]);
            posix_spawn_file_actions_addclose(&actions, stderrPipe[0]);

            //combine the argument values into a vector
            std::vector<char*> argv;
            argv.push_back(const_cast<char*>(executable.c_str()));
            for (const auto& arg : args) 
            {argv.push_back(const_cast<char*>(arg.c_str()));}
            //terminate the argument list by adding a nullptr
            argv.push_back(nullptr);

            //spawn the new process
            int rc = posix_spawn(&m_pid, executable.c_str(), &actions, nullptr, argv.data(), environ);

            //clean up the action
            posix_spawn_file_actions_destroy(&actions);

            //close the input pipes
            close(stdoutPipe[1]);
            close(stderrPipe[1]);

            //check for success
            if (rc != 0) {
                //on failure clean up
                close(stdoutPipe[0]);
                close(stderrPipe[0]);
                cleanup();

                return false;
            }

            //else, store the output pipes
            m_stdoutFd = stdoutPipe[0];
            m_stderrFd = stderrPipe[0];
            //setup the output pipes for nonblocking
            fcntl(m_stdoutFd, F_SETFL, O_NONBLOCK);
            fcntl(m_stderrFd, F_SETFL, O_NONBLOCK);

        #endif

            //reset the exit code
            m_exitCode = -1;

            //success
            return true;
    }

    /**
     * @brief wait until the subprocess stops or the `timeout` milliseconds passed
     * 
     * @param timeout the amount of milliseconds to wait before the function will return
     * @return `true` if the wait was returned because the subprocess exited, `false` if the wait timed out
     */
    bool wait(uint64_t timeout) {
    #ifdef _WIN32

        //if no process exists, the wait finishes instantly
        if (!m_process)
        {return true;}

        //wait for the process to signal shutdown
        DWORD rc = WaitForSingleObject(m_process, static_cast<DWORD>(timeout));

        //check for a timeout
        if (rc == WAIT_TIMEOUT)
        {return false;}

        //retrieve the exit code
        DWORD code{};
        GetExitCodeProcess(m_process, &code);
        m_exitCode = static_cast<int>(code);
        //get the result
        getResult(0);

        //get the last pipe output
        flush();

        //wait successful
        return true;

    #else

        //if no process exists, the wait finishes instantly
        if (m_pid <= 0)
        {return true;}

        //store the starting time of the function
        auto start = std::chrono::steady_clock::now();

        //loop while the pid runs
        while (true) {
            //wait for the pid, then quarry the state
            int status{};
            pid_t result = waitpid(m_pid, &status, WNOHANG);

            if (result == m_pid) {
                //get the ending
                getResult(status);
                //success
                return true;
            }

            //compute the elapsed time in milliseconds
            auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - start).count();
            //check for a timeout
            if (static_cast<uint64_t>(elapsed) >= timeout)
            {return false;}

            //get the data from the output pipes
            flush();

            //wait a bit
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }

    #endif
    }

    /**
     * @brief force the subprocess to stop
     * 
     * This sends the terminate signal and waits for the subprocess to terminate before returning. 
     */
    void terminate() {
        //if no process is running, just stop
        if (!isRunning())
        {return;}

    #ifdef _WIN32

        //send the terminate call to the process
        TerminateProcess(m_process, 1);
        //wait for the process to stop
        WaitForSingleObject(m_process, INFINITE);
        //get the result
        getResult(0);

    #else

        //kill the process using sigkill
        kill(m_pid, SIGKILL);
        int status{};
        //wait for the process to stop
        waitpid(m_pid, &status, 0);
        //get the result
        getResult(status);

    #endif
    }

    /**
     * @brief a function to get the data from the standard output and error stream and cache it
     * 
     * This function should be called periodically while the subprocess runs. 
     * 
     * If this function is not called periodically the subprocess may block if the stdout / stderr buffers fill up. 
     */
    void flush() {
        //define the buffer size
        constexpr size_t BUFFER_SIZE = 4096;
        //store the buffer to pull to
        std::array<char, BUFFER_SIZE> buffer{};

    #ifdef _WIN32

        //store if data is available
        DWORD available{};

        //repeatedly pull from the std out pipe
        while (PeekNamedPipe(m_stdoutRead, nullptr, 0, nullptr, &available, nullptr) && available > 0) {
            //read from the pipe
            DWORD read{};
            if (!ReadFile(m_stdoutRead, buffer.data(), static_cast<DWORD>(buffer.size()), &read, nullptr))
            {break;}

            //add the data to the cache
            m_stdOutCache.append(buffer.data(), read);
        }

        //repeatedly pull from the error out pipe
        while (PeekNamedPipe(m_stderrRead, nullptr, 0, nullptr, &available, nullptr) && available > 0) {
            //read from the pipe
            DWORD read{};
            if (!ReadFile(m_stderrRead, buffer.data(), static_cast<DWORD>(buffer.size()), &read, nullptr))
            {break;}

            //add the data to the cache
            m_stdErrCache.append(buffer.data(), read);
        }

    #else

        //store the amount of read bytes
        ssize_t readBytes{};

        //read from the std out pipe
        while ((readBytes = read(m_stdoutFd, buffer.data(), buffer.size())) > 0) {
            //store the data
            m_stdOutCache.append(buffer.data(), static_cast<size_t>(readBytes));
        }

        //read from the std err pipe
        while ((readBytes = read(m_stderrFd, buffer.data(), buffer.size())) > 0) {
            //store the data
            m_stdErrCache.append(buffer.data(), static_cast<size_t>(readBytes));
        }

    #endif
    }

    /**
     * @brief Get the messages from the standard output stream
     * 
     * This message clears the internal cache of the stdout stream. 
     * 
     * @return `std::string` all messages gotten since the last call to this function
     */
    std::string getStandardOutMessages() {
        //move the data into an output string
        std::string result = std::move(m_stdOutCache);
        //cleanup the cache
        m_stdOutCache.clear();
        //return the old cached data
        return result;
    }

    /**
     * @brief Get the messages from the standard error output stream
     * 
     * This message clears the internal cache of the stderr stream. 
     * 
     * @return `std::string` all messages gotten since the last call to this function
     */
    std::string getErrorOutMessages() {
        //move the data into an output string
        std::string result = std::move(m_stdErrCache);
        //cleanup the cache
        m_stdErrCache.clear();
        //return the old cached data
        return result;
    }

    /**
     * @brief Get the exit code reported by the subprocess
     * 
     * The returned exit code is the exit code reported by the last finished subprocess. If no subprocess finished it will be `EXIT_CODE_DEFAULT`. If the process exited with problems the exit code will be `EXIT_CODE_ERROR`
     * 
     * @return `int` the reported exit code
     */
    int getExitCode() const
    {return m_exitCode;}

    /**
     * @brief check if the process closed normally
     * 
     * @return `true` if the process closed normally, `false` if not
     */
    inline bool didCloseNormally() const 
    {return m_exitedNormally;}

    /**
     * @brief check if the process was closed by a signal
     * 
     * @return `true` if closed by a signal, `false` if not
     */
    inline bool didCloseBySignal() const
    {return m_wasSignaled;}

    /**
     * @brief check if the core was dumped
     * 
     * @return `true` if the core was dumped, `false` if not
     */
    inline bool didCoreDump() const
    {return m_coreDumped;}

    /**
     * @brief Get the Termination Signal
     * 
     * @return `int` the termination signal
     */
    inline int getTerminationSignal() const
    {return m_terminationSignal;}

    static std::filesystem::path getCurrentExecutablePath() {
    #if defined(_WIN32)
        //store a buffer to write to
        char buffer[MAX_PATH]{};

        //read the own module file path
        DWORD len = GetModuleFileNameA(nullptr, buffer, MAX_PATH);

        //on failure return an empty path
        if (len == 0)
        {return "";}

        //return as a filesystem absolute path
        return std::filesystem::canonical(buffer);
    #elif defined(__linux__)
        //store a buffer to write to (on the heap because I got stack overflow on the stack)
        std::vector<char> buffer(4096);

        //read the own process data
        ssize_t len = readlink("/proc/self/exe", buffer.data(), buffer.size() - 1);

        //on failure return an empty path
        if (len <= 0)
        {return "";}

        //terminate with 0
        buffer[len] = '\0';

        //return as a filesystem absolute path
        return std::filesystem::canonical(buffer.data());
    #elif defined(__APPLE__)
        //store a buffer to write to (on the heap because I got stack overflow on the stack)
        std::vector<char> buffer(4096);

        //read the own process data
        ssize_t len = _NSGetExecutablePath(buffer.data(), buffer.size() - 1);

        //on failure return an empty path
        if (len <= 0)
        {return "";}

        //terminate with 0
        buffer[len] = '\0';

        //return as a filesystem absolute path
        return std::filesystem::canonical(buffer.data());
    #endif
    }

protected:

    void getResult(int status) {
    #ifdef _WIN32
        //query exit code from the process
        DWORD exitCode = 0;
        if (GetExitCodeProcess(m_process, &exitCode)) {
            //Check if the process was closed through an unhandeld exception
            if (exitCode == STATUS_ACCESS_VIOLATION || exitCode == 0xC0000005 || exitCode == 0xC0000374) {
                m_wasSignaled = true;
                //store windows exception 
                m_wasSignaled = true;
                m_coreDumped = true;
                m_terminationSignal = 11; //translate to linux segfault err code
                m_exitedNormally = false;
            } else if (exitCode == STILL_ACTIVE) {
                //process is running, but it shoudn't
                m_terminationSignal = EXIT_CODE_ERROR;
                m_exitedNormally = false;
            }
            else if (exitCode == 0x1) {
                //terminated
                m_exitedNormally = false;
                m_terminationSignal = 9; //translate to killed code
                m_wasSignaled = true;
                m_coreDumped = false;
            } else if (exitCode == 0x0) {
                //normal closing
                m_exitedNormally = true;
                m_terminationSignal = 0;
                m_wasSignaled = false;
                m_coreDumped = false;
            }
            else {
                //unknown
                m_exitedNormally = false;
                m_terminationSignal = static_cast<int>(exitCode);
                m_wasSignaled = false;
                m_coreDumped = false;
            }
        }
        else {
            //API-Error
            m_terminationSignal = EXIT_CODE_ERROR;
            m_exitedNormally = false;
        }

        //flush queued up stuff
        flush();
    #else
        //check if the exit was normal
        if (WIFEXITED(status)) {
            //store that the exit was done normally
            m_exitedNormally = true;
            m_exitCode = WEXITSTATUS(status);
        } else if (WIFSIGNALED(status)) {
            //else, check if the process was signaled to stop
            m_wasSignaled = true;
            m_terminationSignal = WTERMSIG(status);

            //check if the core was dumped
            #ifdef WCOREDUMP
            m_coreDumped = WCOREDUMP(status);
            #endif

            //always use exit code of `EXIT_CODE_ERROR`
            m_exitCode = EXIT_CODE_ERROR;
        } else { //general error
            m_exitCode = EXIT_CODE_ERROR;
        }

        //flush the queued up stuff
        flush();
    #endif
    }

    /**
     * @brief internal cleanup helper
     * 
     * This assumes the process is terminated. 
     * This cleans up, but does NOT terminate. 
     */
    void cleanup() {
    #ifdef _WIN32

        //close all handles and set the handles to nullptr
        if (m_job) {
            CloseHandle(m_job);
            m_job = nullptr;
        }
        if (m_process) {
            CloseHandle(m_process);
            m_process = nullptr;
        }
        if (m_thread) {
            CloseHandle(m_thread);
            m_thread = nullptr;
        }
        if (m_stdoutRead) {
            CloseHandle(m_stdoutRead);
            m_stdoutRead = nullptr;
        }
        if (m_stderrRead) {
            CloseHandle(m_stderrRead);
            m_stderrRead = nullptr;
        }

        //reset the pid to 0
        m_pid = 0;

    #else

        //close the streams
        if (m_stdoutFd >= 0) {
            close(m_stdoutFd);
            m_stdoutFd = -1;
        }
        if (m_stderrFd >= 0) {
            close(m_stderrFd);
            m_stderrFd = -1;
        }

        //reset the pid to -1
        m_pid = -1;

    #endif

        //clear the output caches
        m_stdOutCache.clear();
        m_stdErrCache.clear();
    }

    #ifdef _WIN32

    /**
     * @brief store the process handle of the subprocess
     */
    HANDLE m_process = nullptr;
    /**
     * @brief store the job handle
     */
    HANDLE m_job = nullptr;
    /**
     * @brief store the handle of the primary thread the process runs on (process main thread)
     */
    HANDLE m_thread = nullptr;

    /**
     * @brief store the std out read handle
     */
    HANDLE m_stdoutRead = nullptr;
    /**
     * @brief store the std err read handle
     */
    HANDLE m_stderrRead = nullptr;

    /**
     * @brief store the process ID
     */
    DWORD m_pid = 0;

    #else

    /**
     * @brief store the process ID of the subprocess
     */
    pid_t m_pid = -1;

    /**
     * @brief store the std-out filedescriptor
     */
    int m_stdoutFd = -1;
    /**
     * @brief store the std-err filedescriptor
     */
    int m_stderrFd = -1;

    #endif

    /**
     * @brief store a cache of the output stream since the last query
     */
    std::string m_stdOutCache;
    /**
     * @brief store a a cache of the error output stream since the last query
     */
    std::string m_stdErrCache;

    /**
     * @brief store the exit code of the process
     */
    int m_exitCode = EXIT_CODE_DEFAULT;

    /**
     * @brief `true` if the process exited normally, `false` if not
     */
    bool m_exitedNormally = false;
    /**
     * @brief store if the processed was signaled to stop
     */
    bool m_wasSignaled = false;
    /**
     * @brief store the termination signal
     */
    int m_terminationSignal = 0;
    /**
     * @brief store if the core was dumped
     */
    bool m_coreDumped = false;

};