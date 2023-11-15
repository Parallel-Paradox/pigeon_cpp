#ifndef PIGEON_FRAMEWORK_DEFINE
#define PIGEON_FRAMEWORK_DEFINE

#if defined(_WIN32) && defined(PIGEON_SHARED)  // Build shared with MSVC

#if defined(BUILD_PIGEON)
#define PIGEON_API __declspec(dllexport)
#else
#define PIGEON_API __declspec(dllimport)
#endif
#define AUTO_PTR_INSTANTIATE(PtrType, ValueType)             \
  template class PIGEON_API std::function<void(ValueType*)>; \
  template class PIGEON_API pigeon::PtrType<ValueType>;

#else
#define PIGEON_API
#define AUTO_PTR_INSTANTIATE(PtrType, ValueType)
#endif

#endif  // PIGEON_FRAMEWORK_DEFINE
