project(mining_tools)

# include headers
include_directories(../lib)

# flags
add_definitions( "-Wall -g" )

# ascii to binary conversion tool
add_executable(ascii2bin ascii2bin.cc ../lib/filemapper.cc)
# binary to ascii conversion tool
add_executable(bin2ascii bin2ascii.cc ../lib/filemapper.cc)