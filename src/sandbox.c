#include "sandbox.h"

#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <sys/prctl.h>
#include <seccomp.h>

static int deny_syscall(scmp_filter_ctx ctx, const char *name) {
    int nr = seccomp_syscall_resolve_name(name);
    if (nr == __NR_SCMP_ERROR) {
        /* A syscall may not exist on every architecture/kernel. */
        return 0;
    }
    return seccomp_rule_add(ctx, SCMP_ACT_ERRNO(EPERM), nr, 0);
}

int sandbox_profile_exists(const char *profile) {
    return profile &&
        (!strcmp(profile, "basic") ||
         !strcmp(profile, "restricted") ||
         !strcmp(profile, "strict"));
}

int sandbox_apply_profile(const char *profile) {
    if (!sandbox_profile_exists(profile)) {
        errno = EINVAL;
        return -1;
    }

    scmp_filter_ctx ctx = seccomp_init(SCMP_ACT_ALLOW);
    if (!ctx) {
        errno = ENOMEM;
        return -1;
    }

    /*
     * All profiles deny selected high-risk administrative and tracing calls.
     * This is a syscall filter, not a complete container or VM.
     */
    const char *common_denied[] = {
        "ptrace", "mount", "umount2", "pivot_root", "reboot",
        "kexec_load", "kexec_file_load", "init_module", "finit_module",
        "delete_module", "open_by_handle_at", "bpf", "userfaultfd",
        "add_key", "request_key", "keyctl"
    };

    for (size_t i = 0; i < sizeof(common_denied) / sizeof(common_denied[0]); ++i) {
        if (deny_syscall(ctx, common_denied[i]) < 0) {
            seccomp_release(ctx);
            return -1;
        }
    }

    if (!strcmp(profile, "restricted") || !strcmp(profile, "strict")) {
        const char *network_denied[] = {
            "socket", "socketpair", "connect", "accept", "accept4",
            "bind", "listen", "sendto", "recvfrom", "sendmsg", "recvmsg"
        };
        for (size_t i = 0; i < sizeof(network_denied) / sizeof(network_denied[0]); ++i) {
            if (deny_syscall(ctx, network_denied[i]) < 0) {
                seccomp_release(ctx);
                return -1;
            }
        }
    }

    if (!strcmp(profile, "strict")) {
        const char *extra_denied[] = {
            "chmod", "fchmod", "fchmodat", "chown", "fchown",
            "fchownat", "setuid", "setgid", "setreuid", "setregid",
            "setresuid", "setresgid", "setgroups"
        };
        for (size_t i = 0; i < sizeof(extra_denied) / sizeof(extra_denied[0]); ++i) {
            if (deny_syscall(ctx, extra_denied[i]) < 0) {
                seccomp_release(ctx);
                return -1;
            }
        }
    }

    if (prctl(PR_SET_NO_NEW_PRIVS, 1, 0, 0, 0) < 0) {
        seccomp_release(ctx);
        return -1;
    }

    int rc = seccomp_load(ctx);
    seccomp_release(ctx);
    return rc;
}
