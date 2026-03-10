#ifndef SJTU_DEQUE_HPP
#define SJTU_DEQUE_HPP

#include "exceptions.hpp"
#include "linkedlist.hpp"
#include <cmath>
#include <cstddef>

namespace sjtu {

template <class T> 
class deque {
private:
	double_list<double_list<T>> dataList;
	size_t allSize = 0;
	size_t minBlockSize = 0;
	size_t maxBlockSize = 0;
public:

  	class const_iterator;
  	class iterator {
  	private:
		double_list<T>* parentBlock;
		typename double_list<T>::iterator holdingIter;
  	public:
    /**
     * return a new iterator which points to the n-next element.
     * if there are not enough elements, the behaviour is undefined.
     * same for operator-.
     */
    iterator operator+(const int &n) const {}
    iterator operator-(const int &n) const {}

    /**
     * return the distance between two iterators.
     * if they point to different vectors, throw
     * invaild_iterator.
     */
    int operator-(const iterator &rhs) const {}
    iterator &operator+=(const int &n) {}
    iterator &operator-=(const int &n) {}

    /**
     * iter++
     */
    iterator operator++(int) {}
    /**
     * ++iter
     */
    iterator &operator++() {}
    /**
     * iter--
     */
    iterator operator--(int) {}
    /**
     * --iter
     */
    iterator &operator--() {}

    /**
     * *it
     */
    T &operator*() const {}
    /**
     * it->field
     */
    T *operator->() const noexcept {}

    /**
     * check whether two iterators are the same (pointing to the same
     * memory).
     */
    bool operator==(const iterator &rhs) const {}
    bool operator==(const const_iterator &rhs) const {}
    /**
     * some other operator for iterators.
     */
    bool operator!=(const iterator &rhs) const {}
    bool operator!=(const const_iterator &rhs) const {}
  };

  class const_iterator {
    /**
     * it should has similar member method as iterator.
     * you can copy them, but with care!
     * and it should be able to be constructed from an iterator.
     */
  };

  	deque() {dataList = double_list<double_list<T>>();}
  	deque(const deque &other) : dataList(other.dataList), allSize(other.allSize), minBlockSize(other.minBlockSize), maxBlockSize(other.maxBlockSize) {}

 	~deque() {}

  	deque &operator=(const deque &other) {
		dataList = other.dataList;
		allSize = other.allSize;
		minBlockSize = other.minBlockSize;
		maxBlockSize = other.maxBlockSize;
	}

  	T &at(const size_t &pos) {
		if(!validatePosition(pos)){throw index_out_of_bound();}
		size_t currentSize = 0;
		typename sjtu::double_list<double_list<T>>::iterator iter = dataList.begin();
		while(true){
			if(currentSize + iter -> listSize() > pos){
				break;
			}
			currentSize += iter -> listSize();
			++iter;
		}
		typename sjtu::double_list<T>::iterator subIter = iter -> begin();
		while(true){
			++currentSize;
			if (currentSize - 1 == pos){
				return *subIter;
			}
			++subIter;
		}
		return T();
	}
  	const T &at(const size_t &pos) const {
		if(!validatePosition(pos)){throw index_out_of_bound();}
		size_t currentSize = 0;
		typename sjtu::double_list<double_list<T>>::iterator iter = dataList.begin();
		while(true){
			if(currentSize + iter -> listSize() > pos){
				break;
			}
			currentSize += iter -> listSize();
			++iter;
		}
		typename sjtu::double_list<T>::iterator subIter = iter -> begin();
		while(true){
			++currentSize;
			if (currentSize - 1 == pos){
				return *subIter;
			}
			++subIter;
		}
		return T();
	}
  	T &operator[](const size_t &pos) {return at(pos);}
  	const T &operator[](const size_t &pos) const {return at(pos);}

	bool validatePosition(const size_t& pos) const {
		return (pos >= 0 && pos < allSize);
	}

	void updateBoundarySize(){
		minBlockSize = static_cast<int>((sqrt(allSize) / 2));
		maxBlockSize = static_cast<int>((sqrt(allSize) * 2));
	}

  	const T &front() const {
		if (!allSize){throw container_is_empty();}
		typename sjtu::double_list<double_list<T>>::iterator iter = dataList.begin();
		typename sjtu::double_list<T>::iterator subIter = iter -> begin();
		return *subIter;
	}

  	const T &back() const {
		if (!allSize){throw container_is_empty();}
		typename sjtu::double_list<double_list<T>>::iterator iter = --dataList.end();
		typename sjtu::double_list<T>::iterator subIter = --(iter -> end());
		return *subIter;
	}

  /**
   * return an iterator to the beginning.
   */
  	iterator begin() {}
  	const_iterator cbegin() const {}

  /**
   * return an iterator to the end.
   */
 	iterator end() {}
  	const_iterator cend() const {}


  	bool empty() const {return allSize == 0;}

  	size_t size() const {return allSize;}

  	void clear() {
		allSize = 0;
		updateBoundarySize();
		while(!dataList.empty()){
			dataList.delete_head();
		}
	}

	void split(typename sjtu::double_list<double_list<T>>::iterator iter){
		double_list<T>& originList = *iter;
		double_list<T> newList = new double_list<T>();
		size_t cnt = 0;
		size_t originSize = originList.listSize();
		typename sjtu::double_list<T>::iterator subIter = --(originList.end());
		while(cnt < originSize / 2){
			newList.insert_head(*subIter);
			--subIter;
			originList.delete_tail();
			++cnt;
		}
		dataList.insert_after(iter, newList);
	}//Split origin into (ori, nxt).

	void merge(typename sjtu::double_list<double_list<T>>::iterator vicIter, typename sjtu::double_list<double_list<T>>::iterator prevIter){
		double_list<T> vicList = *vicIter;
		double_list<T>& lst = *prevIter;
		dataList.erase(vicIter);
		for (auto it = vicList.begin(); it != vicList.end(); ++it){
			lst.insert_tail(*it);
		}
	}//Merge vic into its prev list.

  /**
   * insert value before pos.
   * return an iterator pointing to the inserted value.
   * throw if the iterator is invalid or it points to a wrong place.
   */
  iterator insert(iterator pos, const T &value) {}

  /**
   * remove the element at pos.
   * return an iterator pointing to the following element. if pos points to
   * the last element, return end(). throw if the container is empty,
   * the iterator is invalid, or it points to a wrong place.
   */
  iterator erase(iterator pos) {}

  /**
   * add an element to the end.
   */
  void push_back(const T &value) {}

  /**
   * remove the last element.
   * throw when the container is empty.
   */
  void pop_back() {}

  /**
   * insert an element to the beginning.
   */
  void push_front(const T &value) {}

  /**
   * remove the first element.
   * throw when the container is empty.
   */
  void pop_front() {}
};

} // namespace sjtu

#endif
