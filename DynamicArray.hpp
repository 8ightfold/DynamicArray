#ifndef DYNAMICARRAY_HPP
#define DYNAMICARRAY_HPP

#include <cstdio>
#include <memory>
#include <cstring>
#include <assert.h>

template <class T>
class dynArray {
private:

	//~~~~~[MEMBERS]~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	const char* m_name;
	size_t m_size;					//	"Public" size
	size_t m_trueSize;				//	Size of the underlying array
	T* m_basePtr;					//	The underlying array

public:

	//~~~~~[FUNCTIONS]~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	//-----[Constructors/Destructors]-------------------------------------------------------------------

	dynArray() : m_basePtr(nullptr), m_size(0), m_trueSize(0) {}
	dynArray(int g_size)
		: m_basePtr(nullptr), m_size(0), m_trueSize(0)
	{
		if (!doAlloc(this, g_size)) printf("Memory allocation failed.");
	}
	template <size_t N>
	dynArray(T(&r)[N])  : m_basePtr(nullptr), m_size(0), m_trueSize(0) {
		this->append(r);
	};
	template <size_t N>
	dynArray(T(&&r)[N]) : m_basePtr(nullptr), m_size(0), m_trueSize(0) {
		this->append(r);
	};
	~dynArray() { if (this->m_basePtr) free(this->m_basePtr); }

	//-----[Size functions]-----------------------------------------------------------------------------

	size_t size() { return this->m_size; }

	void resize(size_t n_size, bool clamp = false) {

		if (!doResize(this, n_size, clamp)) printf("Resize failed.");

	}

	//-----[Stack-like functions]-----------------------------------------------------------------------

	void push(T t) {

		if (!doAlloc(this, this->m_size + 1)) { printf("Memory allocation failed."); return; }
		this->m_basePtr[this->m_size - 1] = t;

	}

	T pop() {

		assert(this->m_size != 0);
		--this->m_size;
		return this->m_basePtr[this->m_size];

	}

	void append(T& t)  { this->push(t); }
	void append(T&& t) { this->push(t); }
	void append(dynArray& r) {

		if (this->m_basePtr == r.m_basePtr || !r.m_basePtr) return;

		size_t o_size = this->m_size;
		if (!doAlloc(this, this->m_size + r.m_size)) return;
		memcpy((void*)&this->m_basePtr[o_size], (void*)r.m_basePtr, r.m_size * sizeof(T));

	}
	void append(dynArray&& r) {

		if (this->m_basePtr == r.m_basePtr || !r.m_basePtr) return;

		size_t o_size = this->m_size;
		if (!doAlloc(this, this->m_size + r.m_size)) return;
		memcpy((void*)&this->m_basePtr[o_size], (void*)r.m_basePtr, r.m_size * sizeof(T));

	}
	template <size_t N>
	void append(T(&r)[N])  {

		if (this->m_basePtr == r) return;

		size_t o_size = this->m_size;
		if (!doAlloc(this, this->m_size + N)) return;
		memcpy((void*)&this->m_basePtr[o_size], (void*)r, N * sizeof(T));

	};
	template <size_t N>
	void append(T(&&r)[N]) {

		if (this->m_basePtr == r) return;

		size_t o_size = this->m_size;
		if (!doAlloc(this, this->m_size + N)) return;
		memcpy((void*)&this->m_basePtr[o_size], (void*)r, N * sizeof(T));

	};

	//-----[Overloads]----------------------------------------------------------------------------------

	dynArray& operator=(dynArray& r) {

		this->resize(0);
		this->append(r);
		return *this;

	}
	template <size_t N>
	dynArray& operator=(T(&r)[N])  { 

		this->resize(0);
		this->append(r); 
		return *this;

	};
	template <size_t N>
	dynArray& operator=(T(&&r)[N]) { 

		this->resize(0);
		this->append(r);
		return *this;

	};

	dynArray& operator+=(dynArray& r) { this->append(r); return *this; }
	template <size_t N>
	dynArray& operator+=(T(& r)[N])  { this->append(r); return *this; };
	template <size_t N>
	dynArray& operator+=(T(&& r)[N]) { this->append(r); return *this; };

	T operator[](int i) const {

		assert(i < this->m_size);
		return this->m_basePtr[i];

	}
	T& operator[](int i) {

		assert(i < this->m_size);
		return this->m_basePtr[i];

	}

	//-----[Debug functions]----------------------------------------------------------------------------

	#ifdef DEBUG_EF

	T* basePtr()				     { return this->m_basePtr;  }
	bool directAllocCall(size_t n)   { return doAlloc(this, n); }
	size_t trueSize()				 { return this->m_trueSize; }

	#endif

private:

	//-----[Resize functions]---------------------------------------------------------------------------

	bool doAlloc(dynArray* arr, size_t n_size) {

		//	Space will only be allocated if the new size forces the underlying array to grow.
		//	If this is the case, it will grow to be 2x the size of the original.

		if (arr->m_size == n_size) return true;
		if (arr->m_size < n_size) {

			if (!arr->m_basePtr) {

				size_t n_trueSize = 1;
				while (n_trueSize < n_size) n_trueSize = n_trueSize * 2;

				arr->m_basePtr = (T*)calloc(n_trueSize, sizeof(T));
				if (!arr->m_basePtr) return false;
				arr->m_size = n_size;
				arr->m_trueSize = n_trueSize;

			}
			else {

				size_t n_trueSize = arr->m_trueSize;
				while (n_trueSize < n_size) n_trueSize = n_trueSize * 2;

				T* n_basePtr = nullptr;
				n_basePtr = (T*)calloc(n_trueSize, sizeof(T));
				if (!n_basePtr) return false;
				memcpy((void*)n_basePtr, (void*)arr->m_basePtr, arr->m_trueSize * sizeof(T));

				free(arr->m_basePtr); arr->m_basePtr = n_basePtr;
				arr->m_size = n_size;
				arr->m_trueSize = n_trueSize;

			}
		}

		return true;

	}

	bool doResize(dynArray* arr, size_t n_size, bool clamp = false) {

		//	Extension of doAlloc, allowing the size to be decreased.
		//	If clamping is enabled, the size of the underlying array will also decrease.
		//	Clamping may not work well with more complex classes.

		if (n_size == 0) {

			if (this->m_basePtr) free(this->m_basePtr);
			this->m_size = 0; this->m_trueSize = 0;
			return true;

		}
		if (arr->m_size == n_size) return true;
		if (arr->m_size > n_size) {

			if (clamp) {

				size_t n_trueSize = arr->m_trueSize;
				while (n_trueSize > n_size) n_trueSize = n_trueSize / 2;
				n_trueSize = (n_trueSize != 0) ? n_trueSize * 2 : 1;

				T* n_basePtr = nullptr;
				n_basePtr = (T*)calloc(n_trueSize, sizeof(T));
				if (!n_basePtr) return false;
				memcpy((void*)n_basePtr, (void*)arr->m_basePtr, n_trueSize * sizeof(T));

				free(arr->m_basePtr); arr->m_basePtr = n_basePtr;
				arr->m_trueSize = n_trueSize;

			}

			arr->m_size = n_size;

		}
		else return doAlloc(arr, n_size);

		return true;

	}

};

#endif
