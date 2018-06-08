# find icu
#set (ICU_INCLUDE_DIR "/home/ytliu/.conda/envs/loc/include")
set (ICU_LIBRARY "/home/ytliu/.conda/envs/loc/lib")
find_package (ICU REQUIRED)
if (ICU_FOUND)
    include_directories (${ICU_INCLUDE_DIRS})
    link_directories (${ICU_LIBARARY_DIRS})
else ()
    message (FATAL_ERROR "ERROR: libicu not found.")
endif ()

# find boost
find_package (Boost REQUIRED
    COMPONENTS timer chrono program_options regex filesystem)
if (Boost_FOUND)
    include_directories (${Boost_INCLUDE_DIR})
    link_directories (${Boost_LIBRARY_DIR})
else ()
    message (FATAL_ERROR "ERROR: Boost not found.")
endif ()

# find libtiff
find_package (TIFF REQUIRED)
if (TIFF_FOUND)
    include_directories (${TIFF_INCLUDE_DIR})
    link_directories (${TIFF_LIBRARIES})
else ()
    message (FATAL_ERROR "ERROR: libtiff not found.")
endif ()

# compile main executable
add_executable (stacktr main.cpp files.cpp tiff.cpp)
target_link_libraries (stacktr 
    Boost::timer Boost::chrono Boost::program_options Boost::regex Boost::filesystem
    TIFF::TIFF)
