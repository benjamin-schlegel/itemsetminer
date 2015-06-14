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

#ifndef OUTPUTFILEBATCH
#define OUTPUTFILEBATCH

#include <ta_batch.h>
#include <iostream>
#include <fstream>

using namespace std;

template < typename ItemType >
class FileBatch : public TaBatch<ItemType> {
private:
	/// file used for writing batches out
	ofstream ofile;
public:
	/**
	 * \brief Default constructor
	 */
	FileBatch(char *filename);
	
	/**
	 * \brief Default destructor
	 */
	virtual ~FileBatch();

	/**
	 * \brief Writes the batch into a file
	 */
	virtual void write();
	
	/**
	 * \brief Adds a transaction to the file batch
	 * 
	 * \details If the batch is full, it is written out and reset
	 */
	virtual bool add_ta(Ta<ItemType> *ta);
};

template < typename ItemType >
FileBatch<ItemType>::FileBatch(char *filename) {
	ofile.open(filename, ios_base::out | ios_base::binary);
}

template < typename ItemType >
FileBatch<ItemType>::~FileBatch() {
	ofile.close();
}

template < typename ItemType >
bool FileBatch<ItemType>::add_ta(Ta<ItemType> *ta) {
	bool succ = TaBatch<ItemType>::add_ta(ta);
	if(!succ) {
		// write out the batch
		write();
		// reset the batch
		TaBatch<ItemType>::reset();
		// add the transaction into the empty batch
		TaBatch<ItemType>::add_ta(ta);
	}
	return true;
}

template < typename ItemType >
void FileBatch<ItemType>::write() {
	ofile.write((char*)this, TaBatch<ItemType>::size());
}

#endif // OUTPUTFILEBATCH