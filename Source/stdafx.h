#pragma once

#include <cstddef>
#include <iostream>
#include <string>
#include <vector>

#define BOOST_FILESYSTEM_VERSION 3

// Do not use deprecated features.
#ifndef BOOST_FILESYSTEM_NO_DEPRECATED 
#define BOOST_FILESYSTEM_NO_DEPRECATED
#endif
#ifndef BOOST_SYSTEM_NO_DEPRECATED 
#define BOOST_SYSTEM_NO_DEPRECATED
#endif

#include <boost/filesystem.hpp>
namespace fs = boost::filesystem;