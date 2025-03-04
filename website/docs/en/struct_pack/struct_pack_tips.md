# struct_pack Tips
This document introcude some tips about struct_pack
## requirements

struct_pack need compiler at least support C++17. We have test it on GCC/Clang/MSVC.

| compiler      | tested min version    | 
| ----------- | ------------------   |
| GCC         | 9.1                               |
| Clang/Apple Clang   | 6.0                        |
| MSVC        | Visual Studio 2019 (MSVC19.20)    |

struct_pack is cross-platform. struct_pack can work in both big-endian and little-endian. struct_pack support 32bit arch or 64bit arch. 

When C++20 standard was enabled, struct_pack has better performance by support memcpy user-defined trivial continuous container. In C++17, struct_pack only support memcpy container `std::array<T>`, `std::vector<T>`, `std::basic_string<T>` and `std::basic_string_view<T>`.

## endian

struct_pack will save the data as little-endian even if the arch is big-endian. The trivial-copy optimize and zero-copy optimize will be disable when the object is width than 1 byte. so struct_pack performance in little-endian is better than big-endian.

## macro
| macro      | description |
| ----------- | ------------------ |
| STRUCT_PACK_OPTIMIZE               | Allow more extremed loop unrolling to get better performance, but it cost more compile time.    |
| STRUCT_PACK_ENABLE_UNPORTABLE_TYPE | Enable serialize unportable type, such as wchar_t and wstring. Deserialize them in other platform is ubdefined bevaior. |
| STRUCT_PACK_ENABLE_INT128 | Enable serialize __int128 and __uint128. Not all compiler support it.
## How to speed up serialization/deserialization
1. use string_view instead of string, use span instead of vector/array.
2. move trivial field to a standlone struct, so that struct_pack could copy it faster. 
3. use std::vector instead of std::list.
4. define macro `STRUCT_PACK_OPTIMIZE`.
## Type Check
struct_pack will generate a name for the serialized type during compilation, and get a 32-bit MD5 based on the string, then take its upper 31 bits for type check. When deserializing, it will check whether the hash code stored is the same as the type to be deserialized. 
In order to alleviate possible hash collisions, in debug mode, struct_pack will store the complete type name instead of hash code. Therefore, the binary size in debug mode is slightly larger than release. 
## serialization config
struct_pack allows user to configure the content of the metadata via `struct_pack::sp_config`, which currently has the following settings:
| enum value      | description |
| ----------- | ------------------ |
| DEFAULT               |default value          |
| DISABLE_TYPE_INFO  | Prohibit storing full type information in serialized data, even in DEBUG mode|
| ENABLE_TYPE_INFO | Prohibit storing full type information in serialized data, even in DEBUG mode|
| DISABLE_META_INFO| Force full type information to be stored in serialized data, even if not in DEBUG mode|

Note that when serialization is configured with the DISABLE_META_INFO option, it must be ensured that deserialization also uses this option, otherwise the behavior is undefined and the probability is that deserialization will fail.

For example：
```cpp
struct rect {
  var_int a, b, c, d;
};
```

### Using ADL as a Serialization Configuration

we can declare a function `set_sp_config` in the same namespace of type `rect`: 

```cpp
inline constexpr struct_pack::sp_config set_sp_config(rect*) {
  return struct_pack::DISABLE_ALL_META_INFO;
}
```

### Configuration using template parameters

For example

```cpp
auto buffer = struct_pack::serialize<struct_pack::DISABLE_ALL_META_INFO>(rect{});
auto result = struct_pack::deserialize<struct_pack::DISABLE_ALL_META_INFO,rect>(buffer);
```

When both are configured, the template parameter takes precedence over the ADL.

Currently we do not allow the `DISABLE_ALL_META_INFO` configuration to be enabled with a compatible field.

## Type requires

1. The type to serialize should be legal struct_pack type.。See document：[struct_pack type system](https://alibaba.github.io/yalantinglibs/en/struct_pack/struct_pack_type_system.html)。
2. struct_pack support update protocol by add struct_pack::compatible field, which is forward backward compatibility. User should make sure the version number is increment for each update. It's not allow to delete/modify exist field. See : [document](https://alibaba.github.io/yalantinglibs/en/struct_pack/struct_pack_type_system.html#%E5%85%BC%E5%AE%B9%E7%B1%BB%E5%9E%8B)

