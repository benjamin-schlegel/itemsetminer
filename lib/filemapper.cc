// Copyright (c) 2015 Benjamin Schlegel
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

///
/// @author Benjamin Schlegel
///

#include <filemapper.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <unistd.h>
#include <errno.h>
#include <fstream>
#include <fcntl.h>
#include <iostream>
#include <cstdlib>

using namespace std;

void* FileMapper::map_input(char *file_name, size_t *file_size) {
	// determine size of the file
	struct stat buffer;
	if(stat(file_name, &buffer) != 0)
	{
		cerr << "Call stat() was not successful (usually the filename is incorrect)" << endl;
		exit(-1);
	}	
	size_t input_bytes = buffer.st_size;
	
	int input;
	// load file the file
	input = open(file_name, O_RDWR);
	if(input == 0)
	{
		cerr << "Could not open input file!" << endl;
		exit(-1);
	}

	// map file into main memory
	int* file_mem = (int*) mmap64(0, input_bytes, PROT_READ, MAP_PRIVATE|MAP_NORESERVE|MAP_POPULATE, input, 0);

	if(file_mem == MAP_FAILED) {
		int errsv = errno;
		cerr << "Mapping of " << file_name << " failed (errno = " << errsv << ")" << endl;
		exit(-1);
	}
	
	*file_size = input_bytes;
	return file_mem;
}

void FileMapper::unmap_input(void *file_mem, size_t file_size) {
	munmap(file_mem, file_size);
}
