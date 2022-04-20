#ifndef DYNAMICARRAY_HPP
#define DYNAMICARRAY_HPP

#include <cstdio>
#include <memory>
#include <cstring>

typedef signed long sint;
typedef unsigned long uint;

template <class T>
class dynArray {
private:

	//~~~~~[MEMBERS]~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	uint m_size;					//	"Public" size
	T* m_basePtr;					//	The array
	uint m_trueSize;				//	Size of the underlying array
	bool m_reserved; uint m_idx;	//	Info used when reserving space

public:

	//~~~~~[FUNCTIONS]~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	//-----[Constructors/Destructors]-------------------------------------------------------------------

	dynArray() : m_basePtr(nullptr), m_size(0), m_trueSize(0), m_reserved(false), m_idx(0) {}
	dynArray(int length, bool t = false) :	m_basePtr(nullptr), m_size(0), m_trueSize(0),  m_idx(0) {			
		
		this->m_reserved = t;	//	Regular if false, reserves if true
		if ((uint)length == 0) {}
		else if (!doAlloc(this, (uint)length)) printf("Memory allocation failed.\n");

	}
	template <size_t N>
	dynArray(T(&r)[N]) : m_basePtr(nullptr), m_size(0), m_trueSize(0), m_reserved(false), m_idx(0) {
		this->append(r);
	};
	template <size_t N>
	dynArray(T(&&r)[N]) : m_basePtr(nullptr), m_size(0), m_trueSize(0), m_reserved(false), m_idx(0) {
		this->append(r);
	};
	~dynArray() { if (this->m_basePtr) free(this->m_basePtr); }

	//-----[Size functions]-----------------------------------------------------------------------------

	uint size() {

		if (this->m_reserved) return this->m_idx;
		return this->m_size;

	}

	//	Calling this will cancel out any previous reserve calls
	//	All clamped data will be lost
	void resize(int i, bool clamp = false) {

		if (this->m_size == i) return;

		if (this->m_reserved) {
			this->m_size = this->m_idx;
			this->m_reserved = false;
		}

		if (!doAlloc(this, (uint)i, clamp)) return;

	}

	//	"Reserves" extra space
	void reserve(int i) {

		if (i == 0) return;

		if (this->m_reserved) {}
		else this->m_idx = this->m_size;

		if (!doAlloc(this, this->length + (uint)i)) return;
		this->m_reserved = true;

	}

	//-----[Stack-like functions]-----------------------------------------------------------------------

	void push(T& t) {

		if (this->m_reserved) {

			++this->m_idx;
			if (!doAlloc(this, this->m_idx)) return;
			if (this->m_idx >= this->m_size) this->m_reserved = false;
			this->m_basePtr[this->m_idx - 1] = t;
			return;

		}

		if (!doAlloc(this, this->m_size + 1)) return;
		this->m_basePtr[this->m_size - 1] = t;

	}
	void push(T&& t) {

		if (this->m_reserved) {

			++this->m_idx;
			if (!doAlloc(this, this->m_idx)) return;
			if (this->m_idx >= this->m_size) this->m_reserved = false;
			this->m_basePtr[this->m_idx - 1] = t;
			return;

		}

		if (!doAlloc(this, this->m_size + 1)) return;
		this->m_basePtr[this->m_size - 1] = t;

	}

	T pop() {

		if (!this->m_basePtr || this->m_size == 0) { printf("Popping from empty array."); exit(0); }
		if (this->m_reserved) {
			--this->m_idx;
			return this->m_basePtr[this->m_idx];
		}
		--this->m_size;
		return this->m_basePtr[this->m_size];

	}

	template<size_t N>
	void append(const T(&r)[N]) {

		unsigned long oldLength;
		if (N == 0) return;

		if (!this->m_basePtr) {

			if (!doAlloc(this, N)) return;
			if (!this->m_basePtr) return;
			memcpy((void*)this->m_basePtr, (void*)r, sizeof(T) * N);
			return;

		}
		
		if (this->m_reserved) {
			
			oldLength = this->m_idx;
			this->m_idx = this->m_idx + N;
			if(!doAlloc(this, this->m_idx)) return;
			if (this->m_idx >= this->m_size) this->m_reserved = false;
			memcpy((void*)&this->m_basePtr[oldLength], (void*)r, sizeof(T) * N);
			return;

		}

		oldLength = this->m_size;
		if (!doAlloc(this, this->m_size + N)) return;
		memcpy((void*)&this->m_basePtr[oldLength], (void*)r, sizeof(T) * N);

	};
	template<size_t N>
	void append(const T(&&r)[N]) {

		unsigned long oldLength;
		if (N == 0) return;

		if (!this->m_basePtr) {

			if (!doAlloc(this, N)) return;
			if (!this->m_basePtr) return;
			memcpy((void*)this->m_basePtr, (void*)r, sizeof(T) * N);
			return;

		}

		if (this->m_reserved) {

			oldLength = this->m_idx;
			this->m_idx = this->m_idx + N;
			if (!doAlloc(this, this->m_idx)) return;
			if (this->m_idx >= this->m_size) this->m_reserved = false;
			memcpy((void*)&this->m_basePtr[oldLength], (void*)r, sizeof(T) * N);
			return;

		}

		oldLength = this->m_size;
		if (!doAlloc(this, this->m_size + N)) return;
		memcpy((void*)&this->m_basePtr[oldLength], (void*)r, sizeof(T) * N);

	};

	void append(dynArray const& r) {

		unsigned long oldLength;
		if (this->m_basePtr == r.m_basePtr || !r.m_basePtr) return;

		if (!this->m_basePtr) {

			if(!doAlloc(this, r.m_size)) return;
			memcpy((void*)this->m_basePtr, (void*)r.m_basePtr, r.m_size * sizeof(T));
			return;

		}

		if (this->m_reserved) {

			oldLength = this->m_idx;
			this->m_idx = this->m_idx + r.m_size;
			if (!doAlloc(this, this->m_idx)) return;
			if (this->m_idx >= this->m_size) this->m_reserved = false;
			memcpy((void*)&this->m_basePtr[oldLength], (void*)r.m_basePtr, r.m_size * sizeof(T));
			return;

		}

		oldLength = this->m_size;
		if (!doAlloc(this, this->m_size + r.m_size)) return;
		memcpy((void*)&this->m_basePtr[oldLength], (void*)r.m_basePtr, r.m_size * sizeof(T));

	}

	//-----[Overloads]----------------------------------------------------------------------------------

	dynArray& operator=(dynArray const& r) {

		if (this->m_basePtr  == r.m_basePtr  &&
			this->m_size   == r.m_size   &&
			this->m_trueSize == r.m_trueSize &&
			this->m_reserved == r.m_reserved &&
			this->m_idx	   == r.m_idx
		   ) 
		{ return *this; }
		if (this->m_basePtr) { free(this->m_basePtr); this->m_basePtr = nullptr; }
		this->m_basePtr = (T*) calloc(r.m_trueSize, sizeof(T));
		if (!this->m_basePtr) { printf("Memory allocation failed.\n"); exit(0); }
		memcpy((void*)this->m_basePtr, (void*)r.m_basePtr, r.m_trueSize * sizeof(T));

		this->m_size = r.m_size;
		this->m_trueSize = r.m_trueSize;
		this->m_reserved = r.m_reserved;
		this->m_idx = r.m_idx;
		return *this;

	}
	template<size_t N>
	dynArray& operator=(T const(&r)[N]) {

		if (!doAlloc(this, N)) return *this;
		memcpy((void*)this->m_basePtr, (void*)r, N * sizeof(T));
		this->m_size = N;
		return *this;

	}

	//	Corrupts stack, don't use for now
	/*friend dynArray operator+(dynArray& l, dynArray& r) {

		dynArray<T> tmp;

		if (!l.m_basePtr && !r.m_basePtr) return tmp;
		else if (!l.m_basePtr) return r;
		else if (!r.m_basePtr) return l;

		tmp.append(l);
		tmp.append(r);
		return tmp;

	}*/

	dynArray& operator+=(dynArray const& r) {
		
		this->append(r);
		return *this;

	}
	template<size_t N>
	dynArray& operator+=(T const(&r)[N]) {

		this->append(r);
		return *this;

	};

	T operator[](int i) const {

		if ((unsigned)i >= this->m_size ||
			(unsigned)i >= this->m_trueSize ||
			((unsigned)i >= this->m_idx && this->m_reserved)
			) { printf("Index is out of bounds at %i.\n", i); exit(0); }
		return this->m_basePtr[i];

	}
	T& operator[](int i) {


		if ((unsigned)i >= this->m_size ||
			(unsigned)i >= this->m_trueSize ||
			((unsigned)i >= this->m_idx && this->m_reserved)
			) { printf("Index is out of bounds at %i.\n", i); exit(0); }
		return this->m_basePtr[i];

	}

	//-----[Debug functions]----------------------------------------------------------------------------

	#ifdef DEBUG_EF

	T* basePtr()				 { return this->m_basePtr; }
	bool directAllocCall(uint n) { return doAlloc(this, n); }

	uint fullSize()				 { return this->m_size; }
	uint trueSize()				 { return this->m_trueSize; }

	#endif

private:

	//-----[Resize functions]---------------------------------------------------------------------------
	
	bool doAlloc(dynArray* arr, uint newLength, bool clamp = false) {
		
		if (newLength == 0) {

			arr->m_size = 0;
			if (clamp == false) return true;

			if (arr->m_basePtr) free(arr->m_basePtr);
			arr->m_basePtr = nullptr;
			arr->m_trueSize = 0;
			return true;

		}
		if (arr->m_size > newLength && !arr->m_reserved) { 

			if (clamp) {

				T* tempPtr = nullptr; uint newTrueSize = arr->m_trueSize;
				while (newTrueSize > newLength) newTrueSize = newTrueSize / 2;
				newTrueSize = newTrueSize * 2;
				tempPtr = (T*)calloc(newTrueSize, sizeof(T));
				if (!tempPtr) { printf("Memory allocation failed.\n"); return false; }

				memcpy((void*)tempPtr, (void*)arr->m_basePtr, newTrueSize * sizeof(T));
				free(arr->m_basePtr); arr->m_basePtr = tempPtr;

				arr->m_size = newLength;
				arr->m_trueSize = newTrueSize;

				return true;

			}
			arr->m_idx = newLength;
			if(!arr->m_reserved) arr->m_size = newLength;
			return true;
		}
		else if (arr->m_trueSize < newLength || !arr->m_basePtr) {

			if (!arr->m_basePtr) { 

				//	Create new array if the current base pointer is null
				arr->m_trueSize = 1;
				while (arr->m_trueSize < newLength) arr->m_trueSize = arr->m_trueSize * 2;
				arr->m_basePtr = (T*)calloc(arr->m_trueSize, sizeof(T));
				if (!arr->m_basePtr) { printf("Memory allocation failed.\n"); return false; }

				arr->m_size = newLength;
				return true;

			}

			//	Attempt to allocate new memory
			T* tempPtr = nullptr; uint newTrueSize = arr->m_trueSize;
			while (newTrueSize < newLength) newTrueSize = newTrueSize * 2;
			tempPtr = (T*)calloc(newTrueSize, sizeof(T));
			if (!tempPtr) { printf("Memory allocation failed.\n"); return false; }

			memcpy((void*)tempPtr, (void*)arr->m_basePtr, newTrueSize * sizeof(T));
			free(arr->m_basePtr); arr->m_basePtr = tempPtr;

			arr->m_size = newLength;
			arr->m_trueSize = newTrueSize;

		}
		
		return true;

	}

};

#endif
