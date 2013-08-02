/**
 * @file utils.h
 *
 * @author Jan Dusek <xdusek17@stud.fit.vutbr.cz>
 * @date 2013
 */

#ifndef UTILS_H
#define UTILS_H

#include <cstdlib>
#include <map>
#include <vector>
#include <cassert>

/**
 * Utility class that allows std::map initialization.
 * Required because !!!!STUPID!!!! MSVC11 doesn't support initializer lists
 */
template <typename K, typename T>
class create_map 
{
public:
	create_map(const K& key, const T& val) {
		map[key] = val;
	}

	create_map<K, T>& operator()(const K& key, const T& val) {
		map[key] = val;
		return *this;
	}

	operator std::map<K, T>() {
		return map;
	}
private:
	std::map<K, T> map;
};

/**
 * Utility class that allows std::vector initialization.
 * Required because !!!!STUPID!!!! MSVC11 doesn't support initializer lists
 */
template <typename T>
class create_vector
{
public:
	create_vector(const T& val) {
		vec.push_back(val);
	}

	create_vector<T>& operator()(const T& val) {
		vec.push_back(val);
		return *this;
	}

	operator std::vector<T>() {
		return vec;
	}
private:
	std::vector<T> vec;
};

/**
 * Convenient class for passing non-const references to arrays.
 * Use this class to pass arrays to functions, it's intended to
 * pass by value, don't worry its just pointer and size.
 */
template <typename T>
class ArrayRef
{
public:
	typedef T value_type;
	typedef T* pointer;
	typedef size_t size_type;

	ArrayRef(pointer ptr, size_type n) : ptr(ptr), n(n) { }
	ArrayRef(std::vector<T>& vec) : ptr(vec.data()), n(vec.size()) { }

	pointer data() {
		return ptr;
	}

	const pointer data() const {
		return ptr;
	}

	size_type size() const {
		return n;
	}

	value_type& operator[](size_type i) {
		assert(i < n);
		return ptr[i];
	}

	const value_type& operator[](size_type i) const {
		assert(i < n);
		return ptr[i];
	}
private:
	T* ptr;
	size_type n;
};

/**
 * Convenient class for passing const references to arrays.
 * Use this class to pass arrays to functions, it's intended to
 * pass by value, don't worry its just pointer and size.
 */
template <typename T>
class ConstArrayRef
{
public:
	typedef T value_type;
	typedef T* pointer;
	typedef size_t size_type;

	ConstArrayRef(const pointer ptr, size_type n) : ptr(ptr), n(n) { }
	ConstArrayRef(const std::vector<T>& vec) : ptr(vec.data()), n(vec.size()) { }
	ConstArrayRef(ArrayRef<T>& other) : ptr(other.data()), n(other.size()) { }

	const pointer data() const {
		return ptr;
	}

	size_type size() const {
		return n;
	}

	const value_type& operator[](size_type i) const {
		assert(i < n);
		return ptr[i];
	}
private:
	const T* ptr;
	size_type n;
};

/**
 * Naive implementation of sign function
 */
template <typename T>
bool sign(T val) {
	return val >= 0 ? true : false;
}

#endif // !UTILS_H
