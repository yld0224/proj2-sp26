#ifndef LINKED_LIST
#define LINKED_LIST
#include "exceptions.hpp"
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

    void insert_after(iterator pos, T val){
        node* currentNode = pos.holdingNode;
        node* newNode = new node(val);
        newNode -> prevNode = currentNode;
        newNode -> nextNode = currentNode -> nextNode;
        currentNode -> nextNode -> prevNode = newNode;
        currentNode -> nextNode = newNode;
        ++size;
    }//Insert a node after the iterator
};
}
#endif //LINKED_LIST