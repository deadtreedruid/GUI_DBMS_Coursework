#ifndef TYPEDEFS_H
#define TYPEDEFS_H

#include <stddef.h>

#ifndef NO_STDINT_H
    #include <stdint.h>
#endif

// These all suck
#ifdef _WIN32
    #undef min
    #undef max
    #undef ERROR
    #undef DELETE
    #undef MIN
    #undef MAX
    #undef CLAMP 
    #undef OK

	#include <Windows.h>
#endif

// sometimes you have to undef these twice, no clue why
#ifdef min 
#undef min 
#endif

#ifdef max 
#undef max 
#endif

// Int types
#ifdef _MSC_VER
    using i8 = signed __int8;
    using u8 = unsigned __int8;
    using i16 = signed __int16;
    using u16 = unsigned __int16;
    using i32 = signed __int32;
    using u32 = unsigned __int32;
    using i64 = signed __int64;
    using u64 = unsigned __int64;
#else
    using i8 = signed char;
    using u8 = unsigned char;
    using i16 = signed short;
    using u16 = unsigned short;
    using i32 = signed int;
    using u32 = unsigned int;
    using i64 = signed long long;
    using u64 = unsigned long long;
#endif

using ulong = unsigned long;


template<typename T>
inline void safe_delete(T *ptr) {
	if (ptr) {
		delete ptr;
		ptr = nullptr;
	}
}

template<typename T, typename Fn>
inline void safe_delete(T *ptr, Fn &&pre_delete_fn) {
	if (ptr) {
		pre_delete_fn(ptr);
		delete ptr;
		ptr = nullptr;
	}
}

#endif // TYPEDEFS_H