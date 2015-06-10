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

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cassert>

#define MAX_LINE_LENGTH	256 * 1024 * 1024
#define MAX_INT_LENGTH 256 * 1024 * 1024

int main(int argc, char *argv[])
{
	char filename[200];
	
	if(argc != 2) {
		printf("Usage: %s <file>\n",argv[0]);
		abort();
	}
	
	strcpy(filename,argv[1]);
	strcat(filename,"_TwoInt");
	
	FILE* input;
	int a;
	int b;
	input = fopen(argv[1],"r");
	if(input == NULL) {
		fprintf(stderr,"Could not open input file %s!\n",argv[1]);
		abort();
	}

	FILE* output;
	output = fopen(filename,"wb");
	
	int ta = 1;

	char * str = (char*)malloc(MAX_LINE_LENGTH);
	char * line;
	int * writeline = (int*)malloc(MAX_INT_LENGTH * sizeof(int));

	while(line = fgets(str,MAX_LINE_LENGTH, input)) {
		int size = 0;
		assert(line[MAX_LINE_LENGTH-2] == 0x0);
		char * pch = strtok(line,"\t "); 
		do {
			writeline[++size] = atoi(pch);
			pch = strtok(NULL,"\t ");
			assert(size < MAX_INT_LENGTH - 1);
		}
		while(pch != NULL && *pch != '\n' && *pch != '\0');
		writeline[0] = size;
		fwrite(writeline,sizeof(int)*(size+1),1,output);
	}
	fclose(input);
	fclose(output);
}
