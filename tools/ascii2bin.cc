// Copyright (c) 2009 Benjamin Schlegel
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

#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <cassert>
#include <filemapper.h>

using namespace std;

#define MAX_TA_LENGTH 256 * 1024 * 1024

int main(int argc, char *argv[])
{
	// check the arguments
	if(argc != 2) {
		cerr << "Usage: " << argv[0] << " <file>\n" << endl;
		exit(-1);
	}
	
	// build the output name by replacing the extension of the input file
  char   *ipath     = argv[1];
	char   *file_ext  = strrchr(ipath, '.');
	size_t  ipath_len = strlen(ipath); 
	char   *opath     = new char[ipath_len + 5];

	strcpy(opath, ipath);
	if(file_ext == NULL) {
		*file_ext = '\0';
	}
	strcat(opath, ".bin");
	
	if(strcmp(ipath, opath) == 0) {
		cerr << "Input file must not have '.bin' extension!" << endl; 
		exit(-1);
	}
	
	// map the input file into memory
	size_t input_bytes;
	unsigned char *file_mem = (unsigned char*) FileMapper::map_input(ipath, &input_bytes);
	unsigned char *file_mem_cur = file_mem;
	unsigned char *file_mem_end = file_mem + input_bytes;
	
	FILE* output;
	output = fopen(opath,"wb");
	
	// check if the file contains at least one transaction
	if (file_mem_cur >= file_mem_end) {
		exit(0);
	}
	
	int *ta = new int[MAX_TA_LENGTH];
	unsigned length = 0;
	
	// parse the transactions
	while (true) {
		if(*file_mem_cur >= '0') {
			unsigned value = 0;
			do {
				value *= 10;
				value += *file_mem_cur - '0';
				file_mem_cur++;
			} 
			while (*file_mem_cur >= '0' && file_mem_cur < file_mem_end);
			// decoded the value
			length++;
			ta[length] = value;
			assert(length < MAX_TA_LENGTH);
		}
		else if(*file_mem_cur == '\n') {
			file_mem_cur++;
			if(length) {
			  // write the transaction to the file
				ta[0] = length;
			  fwrite(ta, sizeof(int), length + 1, output);
				// reset the length
				length = 0;
			}
		}
		else {
			// go over the separators
			assert(*file_mem_cur == ' ');
			file_mem_cur++;
		}
		assert(file_mem_cur <= file_mem_end);
		// check if the end of the file is reached
		if(file_mem_cur == file_mem_end) {
			if(length) {
				// write the transaction to the file
				ta[0] = length;
				fwrite(ta, sizeof(int), length + 1, output);
			}
			break;
		}
	}
	
	// unmap the input file
	FileMapper::unmap_input(file_mem, input_bytes);
	
	fclose(output);
	delete[] opath;
	delete[] ta;
	
	return 0;
}
