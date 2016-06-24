add_library(hotpass MODULE
    # List your source files here.
    hot.cpp
)

# Use C++11 to compile our pass (i.e., supply -std=c++11).
target_compile_features(hotpass PRIVATE cxx_range_for cxx_auto_type)

# LLVM is (typically) built with no C++ RTTI. We need to match that.
set_target_properties(hotpass PROPERTIES
    COMPILE_FLAGS "-fno-rtti"
)

# Get proper shared-library behavior (where symbols are not necessarily
# resolved when the shared library is linked) on OS X.
if(APPLE)
    set_target_properties(hotpass PROPERTIES
        LINK_FLAGS "-undefined dynamic_lookup"
    )
endif(APPLE)
