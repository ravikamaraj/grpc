/*
 *
 * Copyright 2015 gRPC authors.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#include "src/core/lib/security/credentials/credentials.h"

#include <grpc/support/alloc.h>

#include <string.h>

#include "src/core/lib/slice/slice_internal.h"

static void mdelem_list_ensure_capacity(grpc_credentials_mdelem_list *list,
                                        size_t space_needed) {
  size_t target_size = list->size + space_needed;
  size_t new_size = list->size == 0 ? 2 : list->size * 2;
  while (new_size < target_size) {
    new_size *= 2;
  }
  list->md = gpr_realloc(list->md, sizeof(grpc_mdelem) * new_size);
}

void grpc_credentials_mdelem_list_add(grpc_credentials_mdelem_list *list,
                                      grpc_mdelem md) {
  mdelem_list_ensure_capacity(list, 1);
  list->md[list->size++] = GRPC_MDELEM_REF(md);
}

void grpc_credentials_mdelem_list_append(grpc_credentials_mdelem_list *dst,
                                         grpc_credentials_mdelem_list *src) {
  mdelem_list_ensure_capacity(dst, src->size);
  for (size_t i = 0; i < src->size; ++i) {
    dst->md[dst->size++] = GRPC_MDELEM_REF(src->md[i]);
  }
}

void grpc_credentials_mdelem_list_destroy(grpc_exec_ctx *exec_ctx,
                                          grpc_credentials_mdelem_list *list) {
  for (size_t i = 0; i < list->size; ++i) {
    GRPC_MDELEM_UNREF(exec_ctx, list->md[i]);
  }
  gpr_free(list->md);
}
