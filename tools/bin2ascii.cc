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

using namespace std;

#define MAX_LINE_LENGTH	256 * 1024 * 1024
#define MAX_INT_LENGTH 256 * 1024 * 1024

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
	
	if(file_ext == NULL || strcmp(file_ext, ".bin")) {
		cerr << "Input file has to have the '.bin' extension!" << endl;
		exit(-1);
	}
	
	// replace the extension
	strncpy(opath, ipath, file_ext - ipath);
	strcat(opath, ".dat");
	
	// input and output file cannot be the same at this time
	assert(strcmp(ipath, opath));
	
	FILE* input;
	int a;
	int b;
	input = fopen(ipath,"r");
	if(input == NULL) {
		cerr << "Could not open input file: " << argv[1] << endl;
		exit(-1);
	}

	FILE* output;
	output = fopen(opath,"wb");
	
	// TODO
	
	fclose(input);
	fclose(output);
	delete[] opath;
	
	return 0;
}