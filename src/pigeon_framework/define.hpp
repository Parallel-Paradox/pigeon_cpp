#ifndef PIGEON_FRAMEWORK_DEFINE
#define PIGEON_FRAMEWORK_DEFINE

#if defined(_WIN32) && defined(PIGEON_SHARED)  // Build shared with MSVC
#define PIGEON_API __declspec(dllexport)
#else
#define PIGEON_API
#endif

#endif  // PIGEON_FRAMEWORK_DEFINE
