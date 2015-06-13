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

#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <cassert>
#include <algorithm>
#include <filemapper.h>

using namespace std;

#define MAX_TA_LENGTH 256 * 1024 * 1024

/**
 * \brief Compresses a transaction including the length field
 * 
 * \param ta Transaction being compressed - used to return the compressed transaction
 * \param length Length of the transaction including the length field
 * 
 * \return Number of bytes of the compressed transaction
 */
int compress_ta(int *ta, int length)
{
	return sizeof(int) * (length + 1);
}

int main(int argc, char *argv[])
{
	// check the arguments
	if(argc != 2) {
		cerr << "Usage: " << argv[0] << " <file>\n" << endl;
		exit(-1);
	}
	
	// build the output name by replacing the extension of the input file
  char   *ipath     = argv[1];
	size_t  ipath_len = strlen(ipath); 
	char   *opath     = new char[ipath_len + 5];
	strcpy(opath, ipath);
	
	// replace the extension
	char   *file_ext  = strrchr(opath, '.');
	if(file_ext == NULL || strcmp(file_ext, ".bin")) {
		cerr << "Input file has to have the '.bin' extension!" << endl;
		exit(-1);
	}
	*file_ext = '\0';
	strcat(opath, ".cta");
	
	// input and output file cannot be the same at this time
	assert(strcmp(ipath, opath));
	
	// map the input file into memory
	size_t input_bytes;
	int *file_mem = (int*) FileMapper::map_input(ipath, &input_bytes);
	
	FILE* output;
	output = fopen(opath,"wb");
	
	int *ta = new int[MAX_TA_LENGTH];
	
	int *cur_trans = file_mem;
	while((char*)cur_trans < (char*)file_mem + input_bytes) {
		int ta_len = cur_trans[0];
		// copy the transaction (should not sort the memory mapped file)
		memcpy(ta, cur_trans, (ta_len + 1) * sizeof(int));
		// sort the transaction
		sort(ta + 1, ta + ta_len + 1);
		// write the data compressed
		int bytes = compress_ta(ta, ta_len);
		// write the compressed transaction into the file
		fwrite(ta, sizeof(char), bytes, output);
		// goto the next transaction
		cur_trans += ta_len + 1;
	}
	
	FileMapper::unmap_input(file_mem, input_bytes);
	
	fclose(output);
	delete[] opath;
	
	return 0;
}