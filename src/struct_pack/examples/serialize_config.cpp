/*
 * Copyright (c) 2023, Alibaba Group Holding Limited;
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
 */

#include <cassert>

#define STRUCT_PACK_OPTIMIZE  // add this macro to speed up
                              // serialize/deserialize but it will cost more
                              // time to compile
#define STRUCT_PACK_ENABLE_UNPORTABLE_TYPE  // add this macro to enable
                                            // unportable type like wchar_t,
                                            // std::wstring
// #define STRUCT_PACK_ENABLE_INT128
// add this macro to enable support of int128/uint128

#include <ylt/struct_pack.hpp>
using var_int = struct_pack::var_int32_t;

struct rect {
  var_int a, b, c, d;
  bool operator==(const rect& o) const {
    return a == o.a && b == o.b && c == o.c && d == o.d;
  }
};

//clang-format off
void serialize_config() {
  // serialize with config
  rect r{1, -1, 0, 5};
  auto buffer = struct_pack::serialize<struct_pack::DISABLE_ALL_META_INFO>(r);
  // only need 4 bytes
  assert(buffer.size() == 4);
  // deserialize with config
  auto result =
      struct_pack::deserialize<struct_pack::DISABLE_ALL_META_INFO, rect>(
          buffer);
  assert(result.value() == r);
}

// or, you can also use ADL helper function to config it. this function should
// in the same namespace of type
inline constexpr struct_pack::sp_config set_sp_config(rect*) {
  return struct_pack::DISABLE_ALL_META_INFO;
}

void serialize_config_by_ADL() {
  // serialize with config
  rect r{1, -1, 0, 5};
  auto buffer = struct_pack::serialize(r);
  // only need 4 bytes
  assert(buffer.size() == 4);
  // deserialize with config
  auto result = struct_pack::deserialize<rect>(buffer);
  assert(result.value() == r);
}
