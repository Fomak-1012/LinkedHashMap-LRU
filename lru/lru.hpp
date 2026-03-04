#ifndef SJTU_LRU_HPP
#define SJTU_LRU_HPP

#include "class-integer.hpp"
#include "class-matrix.hpp"
#include "exceptions.hpp"
#include "utility.hpp"

class Hash {
public:
    unsigned int operator()(Integer lhs) const {
        int val = lhs.val;
        return std::hash<int>()(val);
    }
};

class Equal {
public:
    bool operator()(const Integer &lhs, const Integer &rhs) const { return lhs.val == rhs.val; }
};

namespace sjtu {
template<class T>
class double_list {
public:
	/**
	 * elements
	 * add whatever you want
	 */
	struct node {
		T* val;
		node *prev, *next;
		node() = delete;
		node(const T& v, node* p = nullptr, node* n = nullptr): val(new T(v)), prev(p), next(n) {}
		~node() { delete val; }
	};

	// --------------------------
	/**
	 * the follows are constructors and destructors
	 * you can also add some if needed.
	 */
	double_list() {
		head = new node();
		tail = new node();
		head->next = tail;
		tail->prev = head;
	}
	double_list(const double_list<T> &other) {
		head = new node();
		tail = new node();
		head->next = tail;
		tail->prev = head;

		node* cur = other.head->next;
		while (cur != other.tail) {
			insert_tail(*cur->val);
			cur = cur->next;
		}
	}
	~double_list() {
		node* cur = head->next;
		while (cur != tail) {
			node* tmp = cur;
			cur = cur->next;
			delete tmp;
		}
		delete head;
		delete tail;
	}

	class iterator {
	public:
		/**
		 * elements
		 * add whatever you want
		 */
		node *p;
		// --------------------------
		/**
		 * the follows are constructors and destructors
		 * you can also add some if needed.
		 */
		iterator() : p(nullptr) {}
		iterator(const iterator &t) : p(t.p) {}
		~iterator() {}
		
		/**
		 * iter++
		 */
		iterator operator++(int) {
			if(p->next == nullptr)
				throw "It's the end!";
			iterator tmp(*this);
			p = p->next;
			return tmp;
		}
		/**
		 * ++iter
		 */
		iterator &operator++() {
			if(p->next == nullptr)
				throw "It's the end!";
			p = p->next;
			return *this;
		}
		/**
		 * iter--
		 */
		iterator operator--(int) {
			if(p->prev->prev == nullptr)
				throw "It's the beginning!";
			iterator tmp(*this);
			p = p->prev;
			return tmp;
		}
		/**
		 * --iter
		 */
		iterator &operator--() {
			if(p->prev->prev == nullptr)
				throw "It's the beginning!";
			p = p->prev;
			return *this;
		}
		
		/**
		 * if the iter didn't point to a value
		 * throw " invalid"
		 */
		T &operator*() const {
			if(p == nullptr || p->prev == nullptr || p->next == nullptr || p->val == nullptr)
				throw " invalid";
			return *p->val;
		}
		
		/**
		 * other operation
		 */
		T *operator->() const noexcept {
			return p->val;
		}
		bool operator==(const iterator &rhs) const {
			return p == rhs.p;
		}
		bool operator!=(const iterator &rhs) const {
			return p != rhs.p;
		}
	};
	/**
	 * return an iterator to the beginning
	 */
	iterator begin() const {
		return iterator(head->next);
	}
	/**
	 * return an iterator to the ending
	 * in fact, it returns the iterator point to nothing,
	 * just after the last element.
	 */
	// 需支持 end()--
	iterator end() const {
		return iterator(tail);
	}
	/**
	 * if the iter didn't point to anything, do nothing,
	 * otherwise, delete the element pointed by the iter
	 * and return the iterator point at the same "index"
	 * e.g.
	 * 	if the origin iterator point at the 2nd element
	 * 	the returned iterator also point at the
	 *  2nd element of the list after the operation
	 *  or nothing if the list after the operation
	 *  don't contain 2nd elememt.
	 */
	iterator erase(iterator pos) {
		if (pos.p == nullptr || pos.p == head || pos.p == tail)
			return pos;
		node* to_delete = pos.p;
		pos.p = pos.p->next;
		to_delete->prev->next = to_delete->next;
		to_delete->next->prev = to_delete->prev;
		delete to_delete;
		return pos;
	}

	/**
	 * the following are operations of double list
	 */
	void insert_head(const T &val) {
		node* tmp = new node(val, head, head->next);
		head->next->prev = tmp;
		head->next = tmp;
	}
	void insert_tail(const T &val) {
		node* tmp = new node(val, tail->prev, tail);
		tail->prev->next = tmp;
		tail->prev = tmp;
	}
	void delete_head() {
		node* tmp = head->next;
		head->next = tmp->next;
		tmp->next->prev = head;
		delete tmp;
	}
	void delete_tail() {
		node* tmp = tail->prev;
		tail->prev = tmp->prev;
		tmp->prev->next = tail;
		delete tmp;
	}
	/**
	 * if didn't contain anything, return true,
	 * otherwise false.
	 */
	bool empty()  const {
		return head->next == tail;
	}
};

template<class Key, class T, class Hash = std::hash<Key>, class Equal = std::equal_to<Key>>
class hashmap {
public:
	using value_type = pair<const Key, T>;
	/**
	 * elements
	 * add whatever you want
	 */

	// --------------------------

	/**
	 * the follows are constructors and destructors
	 * you can also add some if needed.
	 */
	hashmap() {}
	hashmap(const hashmap &other) {}
	~hashmap() {}
	hashmap &operator=(const hashmap &other) {}

	class iterator {
	public:
		/**
		 * elements
		 * add whatever you want
		 */
		// --------------------------
		/**
		 * the follows are constructors and destructors
		 * you can also add some if needed.
		 */
		iterator() {}
		iterator(const iterator &t) {}
		~iterator() {}

		/**
		 * if point to nothing
		 * throw
		 */
		value_type &operator*() const {}

		/**
		 * other operation
		 */
		value_type *operator->() const noexcept {}
		bool operator==(const iterator &rhs) const {}
		bool operator!=(const iterator &rhs) const {}
	};

	void clear() {}
	/**
	 * you need to expand the hashmap dynamically
	 */
	void expand() {}

	/**
	 * the iterator point at nothing
	 */
	iterator end() const {}
	/**
	 * find, return a pointer point to the value
	 * not find, return the end (point to nothing)
	 */
	iterator find(const Key &key) const {}
	/**
	 * already have a value_pair with the same key
	 * -> just update the value, return false
	 * not find a value_pair with the same key
	 * -> insert the value_pair, return true
	 */
	sjtu::pair<iterator, bool> insert(const value_type &value_pair) {}
	/**
	 * the value_pair exists, remove and return true
	 * otherwise, return false
	 */
	bool remove(const Key &key) {}
};

template<class Key, class T, class Hash = std::hash<Key>, class Equal = std::equal_to<Key>>
class linked_hashmap : public hashmap<Key, T, Hash, Equal> {
public:
	typedef pair<const Key, T> value_type;
	/**
	 * elements
	 * add whatever you want
	 */
	// --------------------------
	class const_iterator;
	class iterator {
	public:
		/**
		 * elements
		 * add whatever you want
		 */
		// --------------------------
		iterator() {}
		iterator(const iterator &other) {}
		~iterator() {}

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
		 * if the iter didn't point to a value
		 * throw "star invalid"
		 */
		value_type &operator*() const {}
		value_type *operator->() const noexcept {}

		/**
		 * operator to check whether two iterators are same (pointing to the same memory).
		 */
		bool operator==(const iterator &rhs) const {}
		bool operator!=(const iterator &rhs) const {}
		bool operator==(const const_iterator &rhs) const {}
		bool operator!=(const const_iterator &rhs) const {}
	};

	class const_iterator {
	public:
		/**
		 * elements
		 * add whatever you want
		 */
		// --------------------------
		const_iterator() {}
		const_iterator(const iterator &other) {}

		/**
		 * iter++
		 */
		const_iterator operator++(int) {}
		/**
		 * ++iter
		 */
		const_iterator &operator++() {}
		/**
		 * iter--
		 */
		const_iterator operator--(int) {}
		/**
		 * --iter
		 */
		const_iterator &operator--() {}

		/**
		 * if the iter didn't point to a value
		 * throw
		 */
		const value_type &operator*() const {}
		const value_type *operator->() const noexcept {}

		/**
		 * operator to check whether two iterators are same (pointing to the same memory).
		 */
		bool operator==(const iterator &rhs) const {}
		bool operator!=(const iterator &rhs) const {}
		bool operator==(const const_iterator &rhs) const {}
		bool operator!=(const const_iterator &rhs) const {}
	};

	linked_hashmap() {}
	linked_hashmap(const linked_hashmap &other) {}
	~linked_hashmap() {}
	linked_hashmap &operator=(const linked_hashmap &other) {}

	/**
	 * return the value connected with the Key(O(1))
	 * if the key not found, throw
	 */
	T &at(const Key &key) {}
	const T &at(const Key &key) const {}
	T &operator[](const Key &key) {}
	const T &operator[](const Key &key) const {}

	/**
	 * return an iterator point to the first
	 * inserted and existed element
	 */
	iterator begin() {}
	const_iterator cbegin() const {}
	/**
	 * return an iterator after the last inserted element
	 */
	iterator end() {}
	const_iterator cend() const {}
	/**
	 * if didn't contain anything, return true,
	 * otherwise false.
	 */
	bool empty() const {}

	void clear() {}

	size_t size() const {}
	/**
	 * insert the value_piar
	 * if the key of the value_pair exists in the map
	 * update the value instead of adding a new element，
	 * then the order of the element moved from inner of the
	 * list to the head of the list
	 * and return false
	 * if the key of the value_pair doesn't exist in the map
	 * add a new element and return true
	 */
	pair<iterator, bool> insert(const value_type &value) {}
	/**
	 * erase the value_pair pointed by the iterator
	 * if the iterator points to nothing
	 * throw
	 */
	void remove(iterator pos) {}
	/**
	 * return how many value_pairs consist of key
	 * this should only return 0 or 1
	 */
	size_t count(const Key &key) const {}
	/**
	 * find the iterator points at the value_pair
	 * which consist of key
	 * if not find, return the iterator
	 * point at nothing
	 */
	iterator find(const Key &key) {}
};

class lru {
	using lmap = sjtu::linked_hashmap<Integer, Matrix<int>, Hash, Equal>;
	using value_type = sjtu::pair<const Integer, Matrix<int>>;

public:
	lru(int size) {}
	~lru() {}
	/**
	 * save the value_pair in the memory
	 * delete something in the memory if necessary
	 */
	void save(const value_type &v) const {}
	/**
	 * return a pointer contain the value
	 */
	Matrix<int> *get(const Integer &v) const {}
	/**
	 * just print everything in the memory
	 * to debug or test.
	 * this operation follows the order, but don't
	 * change the order.
	 */
	void print() {}
};
} // namespace sjtu

#endif
