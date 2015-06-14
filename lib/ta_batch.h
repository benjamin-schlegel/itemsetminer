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

#ifndef TABATCH
#define TABATCH

#include <transaction.h>
#include <iostream>

template < typename ItemType >
class TaBatch {
protected:
	/// Maximum size of a single batch
	static const unsigned MAX_SIZE = 128 * 1024;
	
	/// Actual used number of bytes
	unsigned bytes_;
	
	/// Memory of a batch
	ItemType mem_[MAX_SIZE / sizeof(ItemType)];
	
	/**
	 * \brief Resets a batch to an empty state
	 */
	void reset();
	
	/**
	 * \brief returns actual size of the batch in bytes
	 */
	unsigned size() const;
public:
	/**
	 * \brief Default constructor
	 */
	TaBatch();
	
	/**
	 * \brief Adds a transaction to the batch
	 * 
	 * \param ta Transaction being added
	 * 
	 * \return true if the tranaction could be added - otherwise false
	 */
	bool add_ta(Ta<ItemType> *ta);
	
	/**
	 * \brief Prints a batch to the console
	 */
	void print() const;
	
};

template < typename ItemType >
TaBatch<ItemType>::TaBatch() {
	bytes_ = 0;
}

template < typename ItemType >
bool TaBatch<ItemType>::add_ta(Ta<ItemType> *ta) {
	if(ta->get_size() <= MAX_SIZE - bytes_) {
		if(ta->n_items_ > 0) {
			// do only copy transaction with at least on item
			unsigned ta_bytes = ta->get_size();
			memcpy(&mem_[bytes_/sizeof(ItemType)], ta, ta_bytes);
			bytes_ += ta_bytes;
		}
		return true;
	}
	else {
		return false;
	}
}

template < typename ItemType >
void TaBatch<ItemType>::print() const {
	unsigned b = 0;
	while(b * sizeof(ItemType) < bytes_) {
		ItemType len = mem_[b++];
		for(ItemType i = 0; i < len; i++) {
			std::cout << mem_[b++] << "\t";
		}
		std::cout << std::endl;
	}
}

template < typename ItemType >
void TaBatch<ItemType>::reset() {
	bytes_ = 0;
}

#include <iostream>
using namespace std;

template < typename ItemType >
unsigned TaBatch<ItemType>::size() const {
	return sizeof(TaBatch<ItemType>) - MAX_SIZE + bytes_;
}

#endif // TABATCH
