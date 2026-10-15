# SecCage — A Syscall-Sandboxing Tool

SecCage is an educational Linux Operating Systems project written in C. It launches a child process and applies a Linux seccomp filter before executing the requested program.

## Features

- Three named syscall policies: `basic`, `restricted`, and `strict`
- Child-process execution using `fork()` and `execvp()`
- `PR_SET_NO_NEW_PRIVS` before loading the seccomp filter
- Exit status, process ID, and elapsed-time report
- Small test programs for allowed output and network syscall denial
- Modular C source, Makefile, and documented limitations

## Requirements

- Linux (Ubuntu recommended)
- GCC and Make
- libseccomp development package
- Linux kernel with seccomp support

Install dependencies on Ubuntu:

```bash
sudo apt update
sudo apt install build-essential libseccomp-dev git -y
```

## Build

From the project directory:

```bash
make
```

## Run

```bash
./seccage --help
./seccage --profile basic -- /bin/echo "Hello from SecCage"
./seccage --profile restricted -- ./tests/allowed_test
./seccage --profile restricted -- ./tests/network_test
```

The `network_test` should report that `socket()` was denied under the restricted profile on supported Linux architectures.

## Profiles

| Profile | Policy |
|---|---|
| `basic` | Denies selected administrative, kernel-management, key-management, and tracing syscalls |
| `restricted` | Basic policy plus common networking syscalls |
| `strict` | Restricted policy plus selected permission and identity-changing syscalls |

The profile names describe increasing restrictions, not a formal security certification. Syscall names and availability vary by architecture and kernel.

## How it works

1. The parent parses the profile and command.
2. `fork()` creates a child process.
3. The child sets `no_new_privs`, installs a libseccomp filter, then calls `execvp()`.
4. The parent waits for the child and reports its exit status and runtime.

## Important security limitations

SecCage is an educational syscall-filtering demonstration, not a complete isolation boundary. Its default seccomp action is allow, with a denylist of selected syscalls. It does **not** isolate filesystems, network namespaces, devices, memory, CPU usage, or process trees. A program may still access resources permitted by the user's normal Linux permissions. Do not use it to run untrusted programs as a substitute for a properly configured container or virtual machine.

The report records the child's exit status and runtime; it does not claim to log every syscall. A denied syscall commonly returns `EPERM`; this alone does not necessarily terminate the process.

## Suggested demonstration

1. Run `/bin/echo` under the basic profile.
2. Run `tests/network_test` under the restricted profile.
3. Compare the result with running the same test directly (outside SecCage).
4. Record terminal screenshots and explain the profile differences in your report.

## Project structure

```text
SecCage/
├── src/          C implementation
├── include/      Header files
├── tests/        Demonstration programs
├── profiles/     Profile descriptions
├── logs/         Reserved for future persistent logging
├── docs/         Project documentation
├── Makefile
└── README.md
```

## Future improvements

- Load policy definitions from configuration files
- Add per-syscall audit mode
- Add execution timeout and resource limits
- Add a JSON report export
- Add namespaces and filesystem isolation as a separate, carefully tested module

## License

MIT License — see `LICENSE`.
