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
#include <unistd.h>
#include <filemapper.h>
#include <cfile_batch.h>

using namespace std;

#define MAX_TA_LENGTH 256 * 1024 * 1024

void print_help(char *cmd) {
	cout << "Usage: " << cmd << " <file> [-u] [-h]" << endl;
	cout << endl;
	cout << "\t-u Generate an uncompressed '.bin' file" << endl;
	cout << "\t   Standard is a compressed '.cta' file" << endl;
	cout << "\t-h Print this help" << endl;
	exit(0);
}

int main(int argc, char *argv[])
{
	// check the arguments
	if(argc < 2) {
		print_help(argv[0]);
	}
	
	/* check the set flags after the path */ 
	bool compressed = true;
	if(argc > 2) {
		int c;
		
		while ((c = getopt(argc - 1, argv + 1, "uh")) != -1) {
			switch(c) {
				case 'u':
					compressed = false;
					break;
				case 'h':
					print_help(argv[0]);
					break;
				default:
					print_help(argv[0]);
					break;
			}
		}
	}
	
	// build the output name by replacing the extension of the input file
	char   *ipath     = argv[1];
	size_t  ipath_len = strlen(ipath); 
	char   *opath     = new char[ipath_len + 5];
	strcpy(opath, ipath);
	
	char   *file_ext  = strrchr(opath, '.');
	if(file_ext != NULL) {
		*file_ext = '\0';
	}
	if(compressed) {
		strcat(opath, ".cta");
	}
	else {
		strcat(opath, ".bin");
	}
	
	if(strcmp(ipath, opath) == 0) {
		cerr << "Input file must not have '.bin' extension!" << endl; 
		exit(-1);
	}
	
	// map the input file into memory
	size_t input_bytes;
	unsigned char *file_mem = (unsigned char*) FileMapper::map_input(ipath, &input_bytes);
	unsigned char *file_mem_cur = file_mem;
	unsigned char *file_mem_end = file_mem + input_bytes;
	
	// check if the file contains at least one transaction
	if (file_mem_cur >= file_mem_end) {
		exit(0);
	}
	
	Ta<unsigned> *ta = (Ta<unsigned>*) new unsigned[MAX_TA_LENGTH];
	FileBatch<unsigned> *file_batch;
	if(compressed) {
		file_batch = new CFileBatch<unsigned>(opath);
	}
	else {
		file_batch = new FileBatch<unsigned>(opath);
	}
	unsigned n_items = 0;
	
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
			ta->items_[n_items++] = value;
			assert(n_items < MAX_TA_LENGTH - 1);
		}
		else if(*file_mem_cur == '\n') {
			file_mem_cur++;
			ta->n_items_ = n_items;
			file_batch->add_ta(ta);
			n_items = 0;
		}
		else {
			// go over the separators
			assert(*file_mem_cur == ' ');
			file_mem_cur++;
		}
		assert(file_mem_cur <= file_mem_end);
		// check if the end of the file is reached
		if(file_mem_cur == file_mem_end) {
			if(n_items) {
				// write the transaction to the file
				ta->n_items_ = n_items;
				file_batch->add_ta(ta);
			}
			break;
		}
	}
	// flush the last batch out
	file_batch->write();
	
	// unmap the input file
	FileMapper::unmap_input(file_mem, input_bytes);
	
	delete file_batch;
	delete[] opath;
	delete[] ta;
	
	return 0;
}
