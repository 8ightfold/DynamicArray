#ifndef DYNAMICARRAY_HPP
#define DYNAMICARRAY_HPP

#include <cstdio>
#include <memory>
#include <cstring>

typedef signed long sint;
typedef unsigned long uint;

template <class T>
class dynArray {
public:

	uint length;

	//-----[Constructors/Destructors]-------------------------------------------------------------------

	dynArray() :		   basePtr(nullptr), length(0),      trueSize(0), reserved(false), idx(0) {}	//	Regular
	dynArray(int length) : basePtr(nullptr), length(length), trueSize(0), reserved(true),  idx(0) {		//	Reserve

		if (this->length == 0) this->length = 1;
		if (!doAlloc(this, this->length)) printf("Memory allocation failed.\n");

	}
	~dynArray() { if (this->basePtr) free(this->basePtr); }

	//-----[Stack-like functions]-----------------------------------------------------------------------

	void push(T t) {

		if (this->reserved) {

			++this->idx;
			doAlloc(this, this->idx);
			if (this->idx >= this->length) this->reserved = false;
			this->basePtr[this->idx - 1] = t;
			return;

		}

		if (!doAlloc(this, this->length + 1)) return;
		this->basePtr[this->length - 1] = t;

	}

	T pop() {

		if (!this->basePtr) { printf("Popping from empty stack."); exit(0); }
		--this->length;
		return this->basePtr[this->length];

	}

	template<size_t N>
	void append(const T(&r)[N]) {

		unsigned long oldLength;

		if (!this->basePtr) {

			if (!doAlloc(this, N)) return;
			memcpy((void*)this->basePtr, (void*)r, sizeof(T) * N);
			return;

		}
		
		if (this->reserved) {
			
			oldLength = this->idx;
			this->idx = this->idx + N;
			if(!doAlloc(this, this->idx)) return;
			if (this->idx >= this->length) this->reserved = false;
			memcpy((void*)&this->basePtr[oldLength], (void*)r, sizeof(T) * N);
			return;

		}

		oldLength = this->length;
		if (!doAlloc(this, this->length + N)) return;
		memcpy((void*)&this->basePtr[oldLength], (void*)r, sizeof(T) * N);

	};

	void append(dynArray const& r) {

		unsigned long oldLength;

		if (!this->basePtr) {

			if (this->basePtr == r.basePtr) return;
			this->basePtr = (T*)calloc(r.trueSize, sizeof(T));
			if (!this->basePtr) { printf("Memory allocation failed.\n"); exit(0); }
			memcpy((void*)this->basePtr, (void*)r.basePtr, sizeof(this->basePtr));
			return;

		}

		if (this->reserved) {

			oldLength = this->idx;
			this->idx = this->idx + r.length;
			if (!doAlloc(this, this->idx)) return;
			if (this->idx >= this->length) this->reserved = false;
			memcpy((void*)&this->basePtr[oldLength], (void*)r.basePtr, sizeof(T) * r.length);
			return;

		}

		oldLength = this->length;
		if (!doAlloc(this, this->length + r.length)) return;
		memcpy((void*)&this->basePtr[oldLength], (void*)r.basePtr, sizeof(T) * r.length);

	}

	//-----[Overloads]----------------------------------------------------------------------------------

	dynArray& operator=(dynArray const& r) {

		if (this == r) return this;
		if (this->basePtr) free(this->basePtr);
		this->basePtr = (T*) calloc(r.trueSize, sizeof(T));
		if (!this->basePtr) { printf("Memory allocation failed.\n"); exit(0); }
		memcpy((void*)this->basePtr, (void*)r.basePtr, sizeof(this->basePtr));

	}
	template<size_t N>
	dynArray& operator=(T const(&r)[N]) {

		append(r);
		return *this;
	};

	dynArray& operator+=(dynArray const& r) {
		this->append(r);
		return *this;
	}

	T operator[](int i) const {

		if ((unsigned)i >= this->length) { printf("Index is out of bounds at %i.\n", i); exit(0); }
		return this->basePtr[i];

	}
	T& operator[](int i) {

		if ((unsigned)i >= this->length) { printf("Index is out of bounds at %i.\n", i); exit(0); }
		return this->basePtr[i];

	}

	//-----[Debug functions]----------------------------------------------------------------------------

	#ifdef DEBUG_EF
	#endif

private:

	T * basePtr;
	uint trueSize;
	bool reserved; uint idx;

	//-----[Resize function]----------------------------------------------------------------------------

	bool doAlloc(dynArray* arr, uint newLength) {

		if (arr->trueSize < newLength) {

			if (!arr->basePtr) { 

				arr->trueSize = 1;
				while (arr->trueSize < newLength) arr->trueSize = arr->trueSize * 2;
				arr->basePtr = (T*)calloc(arr->trueSize, arr->trueSize * sizeof(T));
				if (!arr->basePtr) { printf("Memory allocation failed.\n"); return false; }

				arr->length = newLength;

			}
			else {

				T* tempPtr = nullptr; uint newTrueSize = arr->trueSize;
				while (newTrueSize < newLength) newTrueSize = newTrueSize * 2;
				tempPtr = (T*)calloc(arr->trueSize, newTrueSize * sizeof(T));
				if (!tempPtr) { printf("Memory allocation failed.\n"); return false; }

				memcpy((void*)tempPtr, (void*)arr->basePtr, newTrueSize * sizeof(T));
				free(arr->basePtr); arr->basePtr = tempPtr;

				arr->length = newLength;
				arr->trueSize = newTrueSize;

			}
		}

		return true;

	}

};

#endif
