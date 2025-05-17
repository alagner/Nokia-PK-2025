#pragma once

#if defined(__GNUC__) || defined(__clang__)
# if defined(DEBUG) && !defined(NDEBUG)
#  define STR(X) \
       #X
#  define DEFER(F, ...) \
       F(__VA_ARGS__)
#  define DO_PRAGMA(X) \
       _Pragma(#X)
#  define TODO(MESSAGE) \
       DO_PRAGMA(message ("\n    \033[1;33mTODO\033[0m [" DEFER(STR, __FILE__) ":" DEFER(STR, __LINE__) "]: \033[1;37m" #MESSAGE "\033[0m"))
# else
#  define TODO(...)
# endif
#endif