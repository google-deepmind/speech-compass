// Copyright 2025 Google LLC
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//    http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// ==============================================================================


/* A logging utility for C tests. */
#ifndef THIRD_PARTY_MULTI_MIC_AUDIO_LOGGING_H_
#define THIRD_PARTY_MULTI_MIC_AUDIO_LOGGING_H_

#include <stdio.h>
#include <stdlib.h>

#define LOG_ERROR(...) fprintf(stderr, __VA_ARGS__)

static void _fatal_error(const char* message) {
  LOG_ERROR("%s", message);
  exit(EXIT_FAILURE);
}

#define AS_STRING(x) AS_STRING_INTERNAL(x)
#define AS_STRING_INTERNAL(x) #x

#ifdef CHECK
#error CHECK is already defined.
#endif

#define CHECK(condition)                                                      \
  if (!(condition)) {                                                         \
    _fatal_error(__FILE__                                                     \
                 ":" AS_STRING(__LINE__) ": Check failed: " #condition "\n"); \
  }

#define CHECK_NOTNULL(ptr) \
  _check_notnull(__FILE__ ":" AS_STRING(__LINE__) ": Pointer is null\n", ptr)

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif  // THIRD_PARTY_MULTI_MIC_AUDIO_LOGGING_H_
