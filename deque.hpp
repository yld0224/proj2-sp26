#ifndef SJTU_DEQUE_HPP
#define SJTU_DEQUE_HPP

#include "exceptions.hpp"
#include <cmath>
#include <cstddef>

namespace sjtu {

template<class T>
class double_list {
	//The following part is repeatedly using code before, which is error free.
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

    //The following methods are added for the deque, bugs may occur.
    const size_t listSize() const {
        return size;
    }
    void cut_tail(iterator l, iterator r, double_list<T>& lst, int length){
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
	void insert_after(iterator pos, T val){
        node* currentNode = pos.holdingNode;
        node* newNode = new node(val);
        newNode -> prevNode = currentNode;
        newNode -> nextNode = currentNode -> nextNode;
        currentNode -> nextNode -> prevNode = newNode;
        currentNode -> nextNode = newNode;
        ++size;
    }//Insert a node after the iterator
	void insert_before(iterator pos, T val){
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

public:
	class Node{
		T val;
		double_list<Node>* parentBlock;
		Node(T value, double_list<Node>* ptr) : val(value), parentBlock(ptr) {}
	};

private:
	double_list<double_list<Node>*> dataList;
	size_t allSize = 0;
	size_t minBlockSize = 0;
	size_t maxBlockSize = 0;

public:
  	class const_iterator;
  	class iterator {
  	private:
		typename double_list<Node>::iterator holdingIter;
  	public:
	iterator(typename double_list<Node>::iterator i) : holdingIter(i) {}

    iterator operator+(const int &n) const {
		typename double_list<Node>::iterator tmp = holdingIter;
		for (int i = 0; i < n; ++i){
			++tmp;
		}
		return iterator(tmp);
	}
    iterator operator-(const int &n) const {
		typename double_list<Node>::iterator tmp = holdingIter;
		for (int i = 0; i < n; ++i){
			--tmp;
		}
		return iterator(tmp);
	}

    int operator-(const iterator &rhs) const {
		if (holdingIter -> parentBlock != rhs.holdingIter -> parentBlock){
			throw invalid_iterator();
		}
		auto tmp1 = holdingIter; 
		auto tmp2 = holdingIter;
		size_t cnt1 = 0, cnt2 = 0;
		while(true){
			if (tmp1 == rhs.holdingIter){return cnt1;}
			--tmp1;
			++cnt1;
			if (tmp1 == holdingIter -> parentBlock -> begin()){break;}
		}
		while(tmp2 != holdingIter -> parentBlock -> end()){
			if (tmp2 == rhs.holdingIter){return cnt2;}
			++tmp2;
			++cnt2;
		}
		return -1;
	}
    iterator &operator+=(const int &n) {
		for (int i = 0; i < n; ++i){
			++holdingIter;
		}
		return *this;
	}
    iterator &operator-=(const int &n) {
		for (int i = 0; i < n; ++i){
			--holdingIter;
		}
		return *this;
	}

    iterator operator++(int) {
		return iterator(holdingIter++);
	}

    iterator &operator++() {
		++holdingIter;
		return *this;
	}

    iterator operator--(int) {
		return iterator(holdingIter--);
	}

    iterator &operator--() {
		--holdingIter;
		return *this;
	}

    T &operator*() const {
		return *holdingIter;
	}

    T *operator->() const noexcept {
		return holdingIter;
	}

    bool operator==(const iterator &rhs) const {return holdingIter == rhs.holdingIter;}
    bool operator==(const const_iterator &rhs) const {return holdingIter == rhs.holdingIter;}
    bool operator!=(const iterator &rhs) const {return holdingIter != rhs.holdingIter;}
    bool operator!=(const const_iterator &rhs) const {return holdingIter != rhs.holdingIter;}

	typename double_list<Node>::iterator getIter(){return holdingIter;}
  };

  class const_iterator {
	private:
		typename double_list<Node>::iterator holdingIter;
	public:
	const_iterator(typename double_list<Node>::iterator i) : holdingIter(i) {}
	const_iterator(iterator it) : holdingIter(it.getIter()) {}
   
	const_iterator operator+(const int &n) const {
		typename double_list<Node>::iterator tmp = holdingIter;
		for (int i = 0; i < n; ++i){
			++tmp;
		}
		return const_iterator(tmp);
	}
    const_iterator operator-(const int &n) const {
		typename double_list<Node>::iterator tmp = holdingIter;
		for (int i = 0; i < n; ++i){
			--tmp;
		}
		return const_iterator(tmp);
	}

    int operator-(const const_iterator &rhs) const {
		if (holdingIter -> parentBlock != rhs.holdingIter -> parentBlock){
			throw invalid_iterator();
		}
		auto tmp1 = holdingIter; 
		auto tmp2 = holdingIter;
		size_t cnt1 = 0, cnt2 = 0;
		while(true){
			if (tmp1 == rhs.holdingIter){return cnt1;}
			--tmp1;
			++cnt1;
			if (tmp1 == holdingIter -> parentBlock -> begin()){break;}
		}
		while(tmp2 != holdingIter -> parentBlock -> end()){
			if (tmp2 == rhs.holdingIter){return cnt2;}
			++tmp2;
			++cnt2;
		}
		return -1;
	}
    const_iterator &operator+=(const int &n) {
		for (int i = 0; i < n; ++i){
			++holdingIter;
		}
		return *this;
	}
    const_iterator &operator-=(const int &n) {
		for (int i = 0; i < n; ++i){
			--holdingIter;
		}
		return *this;
	}

    const_iterator operator++(int) {
		return const_iterator(holdingIter++);
	}

    const_iterator &operator++() {
		++holdingIter;
		return *this;
	}

    const_iterator operator--(int) {
		return const_iterator(holdingIter--);
	}

    const_iterator &operator--() {
		--holdingIter;
		return *this;
	}

    T &operator*() const {
		return *holdingIter;
	}

    T *operator->() const noexcept {
		return holdingIter;
	}

    bool operator==(const iterator &rhs) const {return holdingIter == rhs.holdingIter;}
    bool operator==(const const_iterator &rhs) const {return holdingIter == rhs.holdingIter;}
    bool operator!=(const iterator &rhs) const {return holdingIter != rhs.holdingIter;}
    bool operator!=(const const_iterator &rhs) const {return holdingIter != rhs.holdingIter;}

	typename double_list<Node>::iterator getIter(){return holdingIter;}
  };

  	deque() {dataList = double_list<double_list<Node>*>();}

  	deque(const deque &other) {
		dataList = double_list<double_list<Node>*>();
		auto iter = other.dataList.begin();
		while(iter != other.dataList.end()){
			double_list<Node>* ptr = new double_list<Node>(*(*iter));
			dataList.insert_tail(ptr);
			++iter;
		}
		allSize = other.allSize;
		updateBoundarySize();
	}

  	deque &operator=(const deque &other) {
		clear();
		auto iter = other.dataList.begin();
		while(iter != other.dataList.end()){
			double_list<Node>* ptr = new double_list<Node>(*(*iter));
			dataList.insert_tail(ptr);
			++iter;
		}
		allSize = other.allSize;
		updateBoundarySize();
	}

 	~deque() {
		for (auto iter = dataList.begin(); iter != dataList.end(); ++iter){
			typename sjtu::double_list<Node>* ptr = *iter;
			delete ptr;
		}
	}

  	T &at(const size_t &pos) {
		if(!validatePosition(pos)){throw index_out_of_bound();}
		size_t currentSize = 0;
		typename sjtu::double_list<double_list<Node>*>::iterator iter = dataList.begin();
		while(true){
			if(currentSize + (*iter) -> listSize() > pos){
				break;
			}
			currentSize += (*iter) -> listSize();
			++iter;
		}
		typename sjtu::double_list<Node>::iterator subIter = (*iter) -> begin();
		while(true){
			++currentSize;
			if (currentSize - 1 == pos){
				return subIter -> val;
			}
			++subIter;
		}
		return T();
	}
  	const T &at(const size_t &pos) const {
		if(!validatePosition(pos)){throw index_out_of_bound();}
		size_t currentSize = 0;
		typename sjtu::double_list<double_list<Node>*>::iterator iter = dataList.begin();
		while(true){
			if(currentSize + (*iter) -> listSize() > pos){
				break;
			}
			currentSize += (*iter) -> listSize();
			++iter;
		}
		typename sjtu::double_list<Node>::iterator subIter = (*iter) -> begin();
		while(true){
			++currentSize;
			if (currentSize - 1 == pos){
				return subIter -> val;
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
		typename sjtu::double_list<double_list<Node>*>::iterator iter = dataList.begin();
		typename sjtu::double_list<Node>::iterator subIter = (*iter) -> begin();
		return *subIter;
	}

  	const T &back() const {
		if (!allSize){throw container_is_empty();}
		typename sjtu::double_list<double_list<Node>*>::iterator iter = --dataList.end();
		typename sjtu::double_list<Node>::iterator subIter = --((*iter) -> end());
		return *subIter;
	}

  	iterator begin() {
		if (!allSize){throw container_is_empty();}
		typename sjtu::double_list<double_list<Node>*>::iterator iter = dataList.begin();
		typename sjtu::double_list<Node>::iterator subIter = (*iter) -> begin();
		return iterator(subIter);
	}
  	const_iterator cbegin() const {
		if (!allSize){throw container_is_empty();}
		typename sjtu::double_list<double_list<Node>*>::iterator iter = dataList.begin();
		typename sjtu::double_list<Node>::iterator subIter = (*iter) -> begin();
		return const_iterator(subIter);
	}

 	iterator end() {
		if (!allSize){throw container_is_empty();}
		typename sjtu::double_list<double_list<Node>*>::iterator iter = --dataList.end();
		typename sjtu::double_list<Node>::iterator subIter = --((*iter) -> end());
		return iterator(subIter);
	}
  	const_iterator cend() const {
		if (!allSize){throw container_is_empty();}
		typename sjtu::double_list<double_list<Node>*>::iterator iter = --dataList.end();
		typename sjtu::double_list<Node>::iterator subIter = --((*iter) -> end());
		return const_iterator(subIter);
	}


  	bool empty() const {return allSize == 0;}

  	size_t size() const {return allSize;}

  	void clear() {
		allSize = 0;
		updateBoundarySize();
		while(!dataList.empty()){
			auto iter = dataList.begin();
			typename sjtu::double_list<Node>* ptr = *iter;
			delete ptr;
			dataList.erase(iter);
		}
	}

	void split(typename sjtu::double_list<double_list<Node>*>::iterator iter){
		double_list<Node>* ptr = *iter;
		double_list<Node>* newList = new double_list<Node>();
		size_t cnt = 1;
		typename double_list<Node>::iterator it = --((*ptr).end());
		auto r = it;
		while(cnt < (*ptr).listSize() / 2){
			it -> parentBlock = newList;
			--it;
			++cnt;
		}
		auto l = it;
		l -> parentBlock = newList;
		ptr -> cut_tail(l, r, *newList, cnt);
		dataList.insert_after(iter, newList);
	}//Split origin into (ori, nxt).

	void merge(typename sjtu::double_list<double_list<Node>*>::iterator vicIter){
		if (vicIter == dataList.begin()){return;}
		typename sjtu::double_list<double_list<Node>*>::iterator prevIter = vicIter--;
		typename sjtu::double_list<Node>* ptr = *vicIter;
		auto l = ptr -> begin();
		auto r = --(ptr -> end());
		for (auto it = l; it != ptr -> end(); ++it){
			it -> parentBlock = *prevIter;
		}
		ptr -> cut_tail(l, r,*(*prevIter),ptr -> listSize());
		dataList.erase(vicIter); 
	}//Merge vic into its prev list.

  /**
   * insert value before pos.
   * return an iterator pointing to the inserted value.
   * throw if the iterator is invalid or it points to a wrong place.
   */
  	iterator insert(iterator pos, const T &value) {
		typename double_list<Node>::iterator iter = pos.getIter();
		double_list<Node>& block = *(iter -> parentBlock);
		block.insert_before(iter, Node(value, &block));
		auto ret = iter--;
		++allSize;
		updateBoundarySize();
		if (block.listSize() > maxBlockSize){
			dataList.split(double_list<double_list<Node>*>::iterator(&block));
		}
		return iterator(ret);
  	}

  /**
   * remove the element at pos.
   * return an iterator pointing to the following element. if pos points to
   * the last element, return end(). throw if the container is empty,
   * the iterator is invalid, or it points to a wrong place.
   */
  	iterator erase(iterator pos) {
		if (!allSize){throw runtime_error();}
		typename double_list<Node>::iterator iter = pos.getIter();
		double_list<Node>& block = *(iter -> parentBlock);
		auto ret = iter++;
		if (iterator(ret) != end()){
			if(ret == block.end()){
				auto dataListIter = double_list<double_list<Node>*>::iterator(&block);
				++dataListIter;
				ret = (*(*dataListIter)).begin();
			}//Move to the next block if the last node of current block is removed.
		}
		block.erase(iter);
		allSize--;
		updateBoundarySize();
		if(block.listSize() < minBlockSize){
			dataList.merge(double_list<double_list<Node>*>::iterator(&block));
		}
		return iterator(ret) == end() ? end() : iterator(ret);
	}

  	void push_back(const T &value) {
		if (!allSize){
			typename sjtu::double_list<Node>* ptr = new sjtu::double_list<Node>();
			ptr -> insert_tail(Node(value, ptr));
			dataList.insert_tail(ptr);
			++allSize;
			updateBoundarySize();
		} else {
			typename sjtu::double_list<double_list<Node>*>::iterator iter = --dataList.end();
			(*iter) -> insert_tail(Node(value, *iter));
			++allSize;
			updateBoundarySize();
			if ((*iter) -> listSize() > maxBlockSize){
				dataList.split(double_list<double_list<Node>*>::iterator(&(*iter)));
			}
		}
  	}

  	void pop_back() {
		if (!allSize){
			throw runtime_error();
		} else {
			typename sjtu::double_list<double_list<Node>*>::iterator iter = --dataList.end();
			(*iter) -> delete_tail();
			--allSize;
			updateBoundarySize();
			if ((*iter) -> listSize() < minBlockSize){
				dataList.merge(double_list<double_list<Node>*>::iterator(&(*iter)));
			}
		}
	}

  	void push_front(const T &value) {
		if (!allSize){
			typename sjtu::double_list<Node>* ptr = new sjtu::double_list<Node>();
			ptr -> insert_head(Node(value, ptr));
			dataList.insert_head(ptr);
			++allSize;
			updateBoundarySize();
		} else {
			typename sjtu::double_list<double_list<Node>*>::iterator iter = dataList.begin();
			(*iter) -> insert_head(Node(value, *iter));
			++allSize;
			updateBoundarySize();
			if ((*iter) -> listSize() > maxBlockSize){
				dataList.split(double_list<double_list<Node>*>::iterator(&(*iter)));
			}
		}
	}

  	void pop_front() {
		if (!allSize){
			throw runtime_error();
		} else {
			typename sjtu::double_list<double_list<Node>*>::iterator iter = dataList.begin();
			(*iter) -> delete_head();
			--allSize;
			updateBoundarySize();
			if ((*iter) -> listSize() < minBlockSize){
				dataList.merge(double_list<double_list<Node>*>::iterator(&(*iter)));
			}
		}
	}
};

} // namespace sjtu

#endif
