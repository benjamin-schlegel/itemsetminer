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

#ifndef FILECBATCH
#define FILECBATCH

#include <file_batch.h>
#include <algorithm>

template < typename ItemType >
class CFileBatch : public FileBatch<ItemType> {
private:
	/**
	* \brief Compresses the underlying batch
	*/
	void compress();
public:
	/**
	 * \brief Default constructor
	 */
	CFileBatch(char *filename);
	
	/**
	 * \brief Writes the batch into a file
	 */
	void write();
		
	/**
	 * \brief Adds a transaction to the file batch
	 * 
	 * \details If the batch is full, it is written out and reset
	 */
	bool add_ta(Ta<ItemType> *ta);
};

template < typename ItemType >
CFileBatch<ItemType>::CFileBatch(char *filename) : 
  FileBatch<ItemType>(filename) {
}

template < typename ItemType >
void CFileBatch<ItemType>::write() {
	// compress the batch
	compress();
	// write the batch to disk
	FileBatch<ItemType>::write();
}

template < typename ItemType >
bool CFileBatch<ItemType>::add_ta(Ta<ItemType> *ta) {
	// sort the transaction
	std::sort(ta->items_, ta->items_ + ta->n_items_);
	// delta compress the transaction
	ItemType prev = 0;
	for(unsigned i = 0 ; i < ta->n_items_; i++) {
		ItemType v = ta->items_[i];
		ta->items_[i] = v - prev;
		prev = v + 1;
	}
	// add the transaction to the batch
	return FileBatch<ItemType>::add_ta(ta);
}

template < typename ItemType >
void CFileBatch<ItemType>::compress() {
	// apply null suppression
	ItemType *input = TaBatch<ItemType>::mem_;
	unsigned char * output = (unsigned char*) TaBatch<ItemType>::mem_;
	unsigned out_bytes = 0;
	
	for(unsigned i = 0; i * sizeof(ItemType) < TaBatch<ItemType>::bytes_; i+=4) {
		ItemType cur0 = input[i];
		ItemType cur1 = input[i+1];
		ItemType cur2 = input[i+2];
		ItemType cur3 = input[i+3];

		ItemType zero_bytes0 = __builtin_clz(cur0|1) >> 3;
		ItemType zero_bytes1 = __builtin_clz(cur1|1) >> 3;
		ItemType zero_bytes2 = __builtin_clz(cur2|1) >> 3;
		ItemType zero_bytes3 = __builtin_clz(cur3|1) >> 3;

		assert(zero_bytes0 >= 0 && zero_bytes0 < 4);
		assert(zero_bytes1 >= 0 && zero_bytes1 < 4);
		assert(zero_bytes2 >= 0 && zero_bytes2 < 4);
		assert(zero_bytes3 >= 0 && zero_bytes3 < 4);
		
		unsigned char compression_mask = (zero_bytes3 << 6)|(zero_bytes2 << 4)|(zero_bytes1 << 2)|(zero_bytes0);
		
		// write the compression mask
		*((unsigned char*)output + out_bytes)=compression_mask;
		out_bytes++;
			
		// write the non-zero bytes and increase the output-byte pointer
		*(int*)(&output[out_bytes])=cur0;
		out_bytes += 4 - zero_bytes0;
		*(int*)(&output[out_bytes])=cur1;
		out_bytes += 4 - zero_bytes1;
		*(int*)(&output[out_bytes])=cur2;
		out_bytes += 4 - zero_bytes2;
		*(int*)(&output[out_bytes])=cur3;
		out_bytes += 4 - zero_bytes3;
	}
	TaBatch<ItemType>::bytes_ = out_bytes;
}

#endif // FILECBATCH