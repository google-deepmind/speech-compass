"""Bazel support for C.

Defines c_binary, c_library, c_test rules for targets written in C. They are
like their cc_* counterparts, but compile with C11 standard compatibility.
"""

load("//third_party/bazel_rules/rules_cc/cc:cc_binary.bzl", "cc_binary")
load("//third_party/bazel_rules/rules_cc/cc:cc_library.bzl", "cc_library")
load("//third_party/bazel_rules/rules_cc/cc:cc_test.bzl", "cc_test")

CONDITION_WINDOWS = "//tools/cc_target_os:windows"

WARNING_OPTS = [
    # Warn about mixed signed-unsigned integer comparisons.
    "-Wsign-compare",
    # Warn about `()` declarations. In C, but not C++, a function taking no args
    # should be declared as `void foo(void);`, not `void foo();`.
    "-Wstrict-prototypes",
    # Suppress "unused function" warnings on `static` functions in .h files.
    "-Wno-unused-function",
    "-Wno-vla",
] + select({
    CONDITION_WINDOWS: [],
    # Warn about needlessly set variables. Not supported on Windows.
    "//conditions:default": ["-Wunused-but-set-variable"],
})

# Build with C11 standard compatibility.
DEFAULT_C_OPTS = WARNING_OPTS + select({
    CONDITION_WINDOWS: ["/Za"],
    "//conditions:default": ["-std=c11"],
})

def c_binary(name = None, **kwargs):
    """cc_binary with DEFAULT_COPTS."""
    kwargs.update({"copts": DEFAULT_C_OPTS + kwargs.get("copts", [])})
    return cc_binary(name = name, **kwargs)

def c_library(name = None, **kwargs):
    """cc_library with DEFAULT_C_OPTS, and hdrs is used as textual_hrds."""
    kwargs.update({"copts": DEFAULT_C_OPTS + kwargs.get("copts", [])})

    # Use "hdrs" as "textual_hdrs". All code that cannot be standalone-compiled
    # as C++ must be listed in textual_hdrs.
    kwargs.setdefault("textual_hdrs", kwargs.pop("hdrs", None))
    return cc_library(name = name, **kwargs)

def c_test(name = None, **kwargs):
    """cc_test with DEFAULT_COPTS."""
    kwargs.update({"copts": DEFAULT_C_OPTS + kwargs.get("copts", [])})
    return cc_test(name = name, **kwargs)
