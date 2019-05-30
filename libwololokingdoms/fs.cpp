#if defined(__cplusplus) && __cplusplus >= 201703L &&                          \
    defined(__has_include) && __has_include(<filesystem>) && !defined(USE_USERLAND_FS)
// nothing
#else
#include <ghc/fs_impl.hpp>
#endif
