#ifndef SJTU_DEQUE_HPP
#define SJTU_DEQUE_HPP

#include "exceptions.hpp"
#include <cmath>
#include <cstddef>

namespace sjtu {

template<class T>
class double_list {
public:

	struct node{
		T* data_ptr = nullptr;
		node* prevNode = nullptr;
		node* nextNode = nullptr;
		node(){}
		node(const T& data){data_ptr = new T(data);}
		~node() {
			if (data_ptr) {
				delete data_ptr;
			}
		}
	};

	node* frontSentinal;
	node* backSentinal;
	size_t size = 0;

	double_list() {
		frontSentinal = new node();
		backSentinal = new node();
		frontSentinal -> nextNode = backSentinal;
		backSentinal -> prevNode = frontSentinal;
	}

	double_list(const double_list<T> &other) {
		frontSentinal = new node();
		backSentinal = new node();
		frontSentinal -> nextNode = backSentinal;
		backSentinal -> prevNode = frontSentinal;
		node* tmp = other.frontSentinal -> nextNode;
		while (tmp != other.backSentinal){
			insert_tail(*(tmp -> data_ptr));
			tmp = tmp -> nextNode;
		}
	}

	double_list& operator=(const double_list<T> &other) {
        if (this == &other) {return *this;} 
        while (!empty()) {
            delete_head();
        }
        node* tmp = other.frontSentinal->nextNode;
        while (tmp != other.backSentinal) {
            insert_tail(*(tmp -> data_ptr));
            tmp = tmp->nextNode;
        }
        return *this;
    }

	~double_list() {
		node* tmp = frontSentinal;
		while (tmp != nullptr){
			node* nxt = tmp -> nextNode;
			delete tmp;
			tmp = nxt;
		}
	}

	void insert_head(const T &val) {
		node* newNode = new node(val);
		frontSentinal -> nextNode -> prevNode = newNode;
		newNode -> nextNode = frontSentinal -> nextNode;
		frontSentinal -> nextNode = newNode;
		newNode -> prevNode = frontSentinal;
		++size;
	}

	void insert_tail(const T &val) {
		node* newNode = new node(val);
		backSentinal -> prevNode -> nextNode = newNode;
		newNode -> prevNode = backSentinal -> prevNode;
		backSentinal -> prevNode = newNode;
		newNode -> nextNode = backSentinal;
		++size;
	}

	void delete_head() {
		if (empty()){
			throw container_is_empty();
		}
		node* tmp = frontSentinal -> nextNode;
		frontSentinal -> nextNode = frontSentinal -> nextNode -> nextNode;
		frontSentinal -> nextNode -> prevNode = frontSentinal;
		delete tmp;
		--size;
	}

	void delete_tail() {
		if (empty()){
			throw container_is_empty();
		}
		node* tmp = backSentinal -> prevNode;
		backSentinal -> prevNode = backSentinal -> prevNode -> prevNode;
		backSentinal -> prevNode -> nextNode = backSentinal;
		delete tmp;
		--size;
	}

	bool empty() const {return size == 0;}

	class iterator {
	public:

		node* holdingNode;

		iterator() {holdingNode = nullptr;}
		iterator(node* init) {holdingNode = init;}
		iterator(const iterator &t) : holdingNode(t.holdingNode) {}
		~iterator() {}
		
		iterator operator++(int) {
			iterator tmp = *this; 
            ++(*this);            
            return tmp;     
		}

		iterator &operator++() {
			if (!holdingNode){throw runtime_error();}
			if (!holdingNode -> nextNode){
				throw invalid_iterator();
			}
			holdingNode = holdingNode -> nextNode;
			return *this;
		}

		iterator operator--(int) {
			iterator tmp = *this;
            --(*this);
            return tmp;
		}

		iterator &operator--() {
			if (!holdingNode){throw runtime_error();}
			if (!holdingNode -> prevNode -> prevNode){
				throw invalid_iterator();
			}
			holdingNode = holdingNode -> prevNode;
			return *this;
		}
		
		T &operator*() const {
			if (!holdingNode){throw runtime_error();}
			if (!holdingNode -> nextNode){
				throw invalid_iterator();
			}//Pointing at the backSentinal node
			return *(holdingNode -> data_ptr);
		}

		T *operator->() const noexcept {
			if (!holdingNode){return nullptr;}
			if (!holdingNode -> nextNode){return nullptr;}
			return holdingNode -> data_ptr;
		}

		bool operator==(const iterator &rhs) const {return holdingNode == rhs.holdingNode;}
		bool operator!=(const iterator &rhs) const {return holdingNode != rhs.holdingNode;}
	};

	iterator begin() {return iterator(frontSentinal -> nextNode);}
	iterator end() {return iterator(backSentinal);}
	iterator cbegin() const { return iterator(frontSentinal->nextNode); }
	iterator cend() const { return iterator(backSentinal); }

	iterator erase(iterator pos) {
		if (!pos.holdingNode || !pos.holdingNode -> nextNode){return iterator();}
		node* nxt = pos.holdingNode -> nextNode;
		pos.holdingNode -> prevNode -> nextNode = pos.holdingNode -> nextNode;
		pos.holdingNode -> nextNode -> prevNode = pos.holdingNode -> prevNode;
		delete pos.holdingNode;
		pos.holdingNode = nxt;
		--size;
		return iterator(nxt);
	}

    //The following methods are added for the deque.
    const size_t listSize() const {
        return size;
    }
    void cut_tail(iterator l, iterator r, double_list<T>& lst, size_t length){
		l.holdingNode -> prevNode -> nextNode = r.holdingNode -> nextNode;
		r.holdingNode -> nextNode -> prevNode = l.holdingNode -> prevNode;
		//release
		lst.backSentinal ->prevNode -> nextNode = l.holdingNode;
		l.holdingNode -> prevNode = lst.backSentinal -> prevNode;
		lst.backSentinal -> prevNode = r.holdingNode;
		r.holdingNode -> nextNode = lst.backSentinal;
		lst.size += length;
		size -= length;
	}//Cut [l, r] and insert that part to lst tail.
	void insert_after(iterator pos,const T& val){
		if (pos == end()) {throw runtime_error();}
        node* currentNode = pos.holdingNode;
        node* newNode = new node(val);
        newNode -> prevNode = currentNode;
        newNode -> nextNode = currentNode -> nextNode;
        currentNode -> nextNode -> prevNode = newNode;
        currentNode -> nextNode = newNode;
        ++size;
    }//Insert a node after the iterator
	void insert_before(iterator pos,const T& val){
		node* currentNode = pos.holdingNode;
        node* newNode = new node(val);
        newNode -> nextNode = currentNode;
		newNode -> prevNode = currentNode -> prevNode;
		currentNode -> prevNode -> nextNode = newNode;
		currentNode -> prevNode = newNode;
        ++size;
	}//Insert a node before the iterator
};



template <class T> 
class deque {

private:
	double_list<double_list<T>*> dataList;
	size_t allSize = 0;
	size_t minBlockSize = 0;
	size_t maxBlockSize = 0;

public:
	//The iterator part was fully reinstructed.
  	class const_iterator;
  	class iterator {
		friend class deque;
		friend class const_iterator;
  	private:
	 	const deque* parent;
		typename double_list<double_list<T>*>::iterator outer;
    	typename double_list<T>::iterator inner;
  	public:
	iterator(
			const deque* p,
            typename double_list<double_list<T>*>::iterator out, 
            typename double_list<T>::iterator in) 
        : parent(p), outer(out), inner(in) {}

	iterator() : parent(nullptr), outer(), inner() {}
	iterator(const const_iterator it){
		parent = it.parent;
		outer = it.outer;
		inner = it.inner;
	}

	iterator& operator=(const iterator& other){
		parent = other.parent;
		outer = other.outer;
		inner = other.inner;
		return *this;
	}

    iterator operator+(const int &n) const {
		if (n < 0) {return *this - (-n);}
		int index = parent -> get_index(*this);
		int tar_index = index + n;
		return parent -> get_iterator(tar_index);
	}
    iterator operator-(const int &n) const {
		if (n < 0) {return *this + (-n);}
		int index = parent -> get_index(*this);
		int tar_index = index - n;
		return parent -> get_iterator(tar_index);
	}

    int operator-(const iterator &rhs) const {
		if (parent != rhs.parent){throw invalid_iterator();}
		return parent -> get_index(*this) - parent -> get_index(rhs);
	}
    iterator &operator+=(const int &n) {
		*this = *this + n;
		return *this;
	}
    iterator &operator-=(const int &n) {
		*this = *this - n;
		return *this;
	}

    iterator operator++(int) {
		iterator tmp = *this; 
        ++(*this);            
        return tmp;           
	}

    iterator &operator++() {
		++inner;
        if (inner == (*outer)->end()) {
            auto next_outer = outer;
            ++next_outer; 
            if (next_outer != (parent->dataList).cend()) {
                outer = next_outer;
                inner = (*outer)->begin();
            }
        }
        return *this;
	}

    iterator operator--(int) {
		iterator tmp = *this;
		--(*this);
		return tmp;
	}

    iterator &operator--() {
		if (inner == (*outer) -> begin()){
			if (outer == parent -> dataList.cbegin()){
				throw invalid_iterator();
			}
			--outer;
			inner = --((*outer) -> end());
		} else {
			--inner;
		}	
		return *this;
	}

    T &operator*() const {
		return *inner;
	}

    T *operator->() const noexcept {
		return &(*inner);
	}

    bool operator==(const iterator &rhs) const {return inner == rhs.inner && outer == rhs.outer;}
    bool operator==(const const_iterator &rhs) const {return inner == rhs.inner && outer == rhs.outer;}
    bool operator!=(const iterator &rhs) const {return inner != rhs.inner || outer != rhs.outer;}
    bool operator!=(const const_iterator &rhs) const {return inner != rhs.inner || outer != rhs.outer;}
  };

  	class const_iterator {
		friend class deque;
		friend class iterator;
  	private:
	 	const deque* parent;
		typename double_list<double_list<T>*>::iterator outer;
    	typename double_list<T>::iterator inner;
  	public:
	const_iterator(
			const deque* p,
            typename double_list<double_list<T>*>::iterator out, 
            typename double_list<T>::iterator in) 
        : parent(p), outer(out), inner(in) {}
	
	const_iterator() : parent(nullptr), inner(), outer() {}

	const_iterator(iterator it){
		parent = it.parent;
		outer = it.outer;
		inner = it.inner;
	}

	const_iterator& operator=(const const_iterator& other){
		parent = other.parent;
		outer = other.outer;
		inner = other.inner;
		return *this;
	}

    const_iterator operator+(const int &n) const {
		if (n < 0) {return *this - (-n);}
		int index = parent -> get_const_index(*this);
		int tar_index = index + n;
		return parent -> get_const_iterator(tar_index);
	}
    const_iterator operator-(const int &n) const {
		if (n < 0) {return *this + (-n);}
		int index = parent -> get_const_index(*this);
		int tar_index = index - n;
		return parent -> get_const_iterator(tar_index);
	}

    int operator-(const const_iterator &rhs) const {
		if (parent != rhs.parent){throw invalid_iterator();}
		return parent -> get_const_index(*this) - parent -> get_const_index(rhs);
	}
    const_iterator &operator+=(const int &n) {
		*this = *this + n;
		return *this;
	}
    const_iterator &operator-=(const int &n) {
		*this = *this - n;
		return *this;
	}

    const_iterator operator++(int) {
		const_iterator tmp = *this; 
        ++(*this);            
        return tmp;           
	}

    const_iterator &operator++() {
		++inner;
        if (inner == (*outer)->end()) {
            auto next_outer = outer;
            ++next_outer; 
            if (next_outer != parent->dataList.end()) {
                outer = next_outer;
                inner = (*outer)->begin();
            }
        }
        return *this;
	}

    const_iterator operator--(int) {
		const_iterator tmp = *this;
		--(*this);
		return tmp;
	}

    const_iterator &operator--() {
		if (inner == (*outer) -> begin()){
			if (outer == parent -> dataList.cbegin()){
				throw invalid_iterator();
			}
			--outer;
			auto tmp = (*outer)->end();
        	--tmp;
        	inner = tmp; 
		} else {
			--inner;
		}	
		return *this;
	}

    const T &operator*() const {
		return *inner;
	}

    const T *operator->() const noexcept {
		return &(*inner);
	}

    bool operator==(const iterator &rhs) const {return inner == rhs.inner && outer == rhs.outer;}
    bool operator==(const const_iterator &rhs) const {return inner == rhs.inner && outer == rhs.outer;}
    bool operator!=(const iterator &rhs) const {return inner != rhs.inner || outer != rhs.outer;}
    bool operator!=(const const_iterator &rhs) const {return inner != rhs.inner || outer != rhs.outer;}
  	};

	int get_index(iterator it) const {
    	if (it == cend()) {return allSize;}
    	int idx = 0;
    	auto out = dataList.cbegin();
    	while (out != it.outer) {
        	idx += (*out)->listSize();
        	++out;
    	}
    	auto in = (*out)->cbegin();
    	while (in != it.inner) {
        	++idx;
        	++in;
    	}
    	return idx;
	}

	iterator get_iterator(int idx) const {
 	    if (idx < 0 || idx > allSize) {throw index_out_of_bound();}
        if (idx == allSize) return cend();
    	auto out = dataList.cbegin();
    	while (out != dataList.cend()) {
        	if (idx < (*out)->listSize()) {
            	auto in = (*out)->cbegin();
            	for (int i = 0; i < idx; ++i) ++in;
            	return iterator(this, out, in);
        	}
        	idx -= (*out)->listSize();
        	++out;
    	}
    	return cend();
	}

	int get_const_index(const_iterator it) const {
    	if (it == cend()) {return allSize;}
    	int idx = 0;
    	auto out = dataList.cbegin();
    	while (out != it.outer) {
        	idx += (*out)->listSize();
        	++out;
    	}
    	auto in = (*out)->cbegin();
    	while (in != it.inner) {
        	++idx;
        	++in;
    	}
    	return idx;
	}

	const_iterator get_const_iterator(int idx) const {
 	    if (idx < 0 || idx > allSize) {throw index_out_of_bound();}
        if (idx == allSize) return cend();
    	auto out = dataList.cbegin();
    	while (out != dataList.cend()) {
        	if (idx < (*out)->listSize()) {
            	auto in = (*out)->cbegin();
            	for (int i = 0; i < idx; ++i) ++in;
            	return const_iterator(this, out, in);
        	}
        	idx -= (*out)->listSize();
        	++out;
    	}
    	return cend();
	}

	iterator begin() {
		if (!allSize){
			return iterator(this, dataList.begin(), typename double_list<T>::iterator());
		}//Use default instruction to set inner as nullptr, which is a place holder.
		typename sjtu::double_list<double_list<T>*>::iterator iter = dataList.begin();
		typename sjtu::double_list<T>::iterator subIter = (*iter) -> begin();
		return iterator(this, iter, subIter);
	}
  	const_iterator cbegin() const {
		if (!allSize){
			return const_iterator(this, dataList.cbegin(), typename double_list<T>::iterator());
		}
		typename sjtu::double_list<double_list<T>*>::iterator iter = dataList.cbegin();
		typename sjtu::double_list<T>::iterator subIter = (*iter) -> cbegin();
		return const_iterator(this, iter, subIter);
	}

 	iterator end() {
		if (!allSize){
			return iterator(this, dataList.end(), typename double_list<T>::iterator());
		}
		typename sjtu::double_list<double_list<T>*>::iterator iter = --dataList.end();
		typename sjtu::double_list<T>::iterator subIter = (*iter) -> end();
		return iterator(this, iter, subIter);
	}
  	const_iterator cend() const {
		if (!allSize){
			return const_iterator(this, dataList.cend(), typename double_list<T>::iterator());
		}
		typename sjtu::double_list<double_list<T>*>::iterator iter = --dataList.cend();
		typename sjtu::double_list<T>::iterator subIter = (*iter) -> cend();
		return const_iterator(this, iter, subIter);
	}

  	deque() {dataList = double_list<double_list<T>*>();}

  	deque(const deque &other) {
		dataList = double_list<double_list<T>*>();
		auto iter = other.dataList.cbegin();
		while(iter != other.dataList.cend()){
			double_list<T>* ptr = new double_list<T>(*(*iter));
			dataList.insert_tail(ptr);
			++iter;
		}
		allSize = other.allSize;
		updateBoundarySize();
	}

  	deque &operator=(const deque &other) {
		clear();
		auto iter = other.dataList.cbegin();
		while(iter != other.dataList.cend()){
			double_list<T>* ptr = new double_list<T>(*(*iter));
			dataList.insert_tail(ptr);
			++iter;
		}
		allSize = other.allSize;
		updateBoundarySize();
		return *this;
	}

 	~deque() {
		for (auto iter = dataList.begin(); iter != dataList.end(); ++iter){
			typename sjtu::double_list<T>* ptr = *iter;
			delete ptr;
		}
	}

  	T &at(const size_t &pos) {
		if(!validatePosition(pos)){throw index_out_of_bound();}
		size_t currentSize = 0;
		typename sjtu::double_list<double_list<T>*>::iterator iter = dataList.begin();
		while(true){
			if(currentSize + (*iter) -> listSize() > pos){
				break;
			}
			currentSize += (*iter) -> listSize();
			++iter;
		}
		typename sjtu::double_list<T>::iterator subIter = (*iter) -> begin();
		while(true){
			++currentSize;
			if (currentSize - 1 == pos){
				return *subIter;
			}
			++subIter;
		}
	}
  	const T &at(const size_t &pos) const {
		if(!validatePosition(pos)){throw index_out_of_bound();}
		size_t currentSize = 0;
		typename sjtu::double_list<double_list<T>*>::iterator iter = dataList.cbegin();
		while(true){
			if(currentSize + (*iter) -> listSize() > pos){
				break;
			}
			currentSize += (*iter) -> listSize();
			++iter;
		}
		typename sjtu::double_list<T>::iterator subIter = (*iter) -> cbegin();
		while(true){
			++currentSize;
			if (currentSize - 1 == pos){
				return *subIter;
			}
			++subIter;
		}
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
		return *(cbegin());
	}

  	const T &back() const {
		return *(--(cend()));
	}

  	bool empty() const {return allSize == 0;}

  	size_t size() const {return allSize;}

  	void clear() {
		allSize = 0;
		updateBoundarySize();
		while(!dataList.empty()){
			auto iter = dataList.begin();
			typename sjtu::double_list<T>* ptr = *iter;
			delete ptr;
			dataList.erase(iter);
		}
	}

	void split(typename sjtu::double_list<double_list<T>*>::iterator iter){
		double_list<T>* ptr = *iter;
		double_list<T>* newList = new double_list<T>();
		size_t cnt = 1;
		typename double_list<T>::iterator it = --((*ptr).end());
		auto r = it;
		while(cnt < (*ptr).listSize() / 2){
			--it;
			++cnt;
		}
		auto l = it;
		ptr -> cut_tail(l, r, *newList, cnt);
		dataList.insert_after(iter, newList);
	}//Split origin into (ori, nxt).

	void merge(typename sjtu::double_list<double_list<T>*>::iterator vicIter){
		if (vicIter == dataList.begin()){return;}
		typename sjtu::double_list<double_list<T>*>::iterator prevIter = vicIter--;
		typename sjtu::double_list<T>* ptr = *vicIter;
		auto l = ptr -> begin();
		auto r = --(ptr -> end());
		ptr -> cut_tail(l, r,*(*prevIter),ptr -> listSize());
		dataList.erase(vicIter); 
	}//Merge vic into its prev list.

  	/**
   	* insert value before pos.
   	* return an iterator pointing to the inserted value.
   	* throw if the iterator is invalid or it points to a wrong place.
   	*/
  	iterator insert(iterator pos, const T &value) {
		if (pos.parent != this) {throw invalid_iterator();}
		if (allSize == 0 && pos == end()) {
			sjtu::double_list<T>* ptr = new sjtu::double_list<T>();
			ptr ->insert_head(value);
			dataList.insert_head(ptr);
			return begin();
		}//Speical case: inserting the first element when the iterator is at the end.
		typename sjtu::double_list<T>::iterator in = pos.inner;
		typename sjtu::double_list<double_list<T>*>::iterator out = pos.outer;
		int index = get_index(pos);
		(*out) -> insert_before(in, value);
		++allSize;
		updateBoundarySize();
		if ((*out) -> listSize() > maxBlockSize){
			split(out);
		}
		return get_iterator(index);
  	}

  	/**
   	* remove the element at pos.
   	* return an iterator pointing to the following element. if pos points to
   	* the last element, return end(). throw if the container is empty,
   	* the iterator is invalid, or it points to a wrong place.
   	*/
  	iterator erase(iterator pos) {
		if (!allSize){throw runtime_error();}
		if (pos.parent != this || pos == end()){throw invalid_iterator();}
		int index = get_index(pos);
		typename sjtu::double_list<double_list<T>*>::iterator out = pos.outer;
		typename sjtu::double_list<T>::iterator in = pos.inner;
		(*out) -> erase(in);
		--allSize;
		updateBoundarySize();
		if((*out) -> listSize() < minBlockSize){
			merge(out);
		}
		return get_iterator(index);
	}

  	void push_back(const T &value) {
		if (!allSize){
			typename sjtu::double_list<T>* ptr = new sjtu::double_list<T>();
			ptr -> insert_tail(value);
			dataList.insert_tail(ptr);
			++allSize;
			updateBoundarySize();
		} else {
			typename sjtu::double_list<double_list<T>*>::iterator iter = --dataList.end();
			(*iter) -> insert_tail(value);
			++allSize;
			updateBoundarySize();
			if ((*iter) -> listSize() > maxBlockSize){
				split(iter);
			}
		}
  	}

  	void pop_back() {
		if (!allSize){
			throw runtime_error();
		} else {
			typename sjtu::double_list<double_list<T>*>::iterator iter = --dataList.end();
			(*iter) -> delete_tail();
			--allSize;
			updateBoundarySize();
			if ((*iter) -> listSize() < minBlockSize){
				merge(iter);
			}
		}
	}

  	void push_front(const T &value) {
		if (!allSize){
			typename sjtu::double_list<T>* ptr = new sjtu::double_list<T>();
			ptr -> insert_head(value);
			dataList.insert_head(ptr);
			++allSize;
			updateBoundarySize();
		} else {
			typename sjtu::double_list<double_list<T>*>::iterator iter = dataList.begin();
			(*iter) -> insert_head(value);
			++allSize;
			updateBoundarySize();
			if ((*iter) -> listSize() > maxBlockSize){
				split(iter);
			}
		}
	}

  	void pop_front() {
		if (!allSize){
			throw runtime_error();
		} else {
			typename sjtu::double_list<double_list<T>*>::iterator iter = dataList.begin();
			(*iter) -> delete_head();
			--allSize;
			updateBoundarySize();
			if ((*iter) -> listSize() < minBlockSize){
				merge(iter);
			}
		}
	}
};

} // namespace sjtu

#endif
