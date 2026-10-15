# Testing Guide

Build:

```bash
make clean
make
```

Test ordinary execution:

```bash
./seccage --profile basic -- /bin/echo "SecCage is running"
```

Test standard output:

```bash
./seccage --profile restricted -- ./tests/allowed_test
```

Test network syscall restriction:

```bash
./seccage --profile restricted -- ./tests/network_test
```

Compare with running the network test outside the sandbox:

```bash
./tests/network_test
```

Expected behavior depends on Linux architecture and kernel support. Under `restricted`, the `socket()` call should fail with `EPERM`. A denial is expected to be handled by the test program; it should not be interpreted as proof that every possible network path is blocked.
