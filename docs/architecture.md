# SecCage Architecture

## Components

- **CLI (`src/main.c`)**: parses arguments and coordinates the run.
- **Sandbox (`src/sandbox.c`)**: installs a libseccomp policy in the child.
- **Logger (`src/logger.c`)**: prints timestamped summary events.
- **Report (`src/report.c`)**: formats exit status and elapsed time.
- **Tests (`tests/`)**: small programs used to demonstrate policy behavior.

## Execution flow

1. Parse `--profile` and the command after `--`.
2. Fork a child process.
3. In the child, apply `no_new_privs` and load the seccomp filter.
4. Execute the requested command.
5. In the parent, wait for completion and print a report.

## Trust boundary

Seccomp limits selected system calls. It is not, by itself, filesystem isolation or a complete sandbox. The child retains the invoking user's ordinary permissions.
