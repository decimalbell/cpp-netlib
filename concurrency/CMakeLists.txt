# Copyright (c) Glyn Matthews 2012.
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)

add_subdirectory(src)

if(CPP-NETLIB_BUILD_TESTS)
  enable_testing()
  add_subdirectory(test)
endif(CPP-NETLIB_BUILD_TESTS)

# propagate sources to parent directory for one-lib-build
set(CPP-NETLIB_CONCURRENCY_SRCS ${CPP-NETLIB_CONCURRENCY_SRCS} PARENT_SCOPE)
