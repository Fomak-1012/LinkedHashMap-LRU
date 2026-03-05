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
		node() : prev(nullptr), next(nullptr), val(nullptr) {};
		node(const T& v, node* p = nullptr, node* n = nullptr): val(new T(v)), prev(p), next(n) {}
		~node() { delete val; }
	};
	node *head, *tail;

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
		iterator(node* ptr) : p(ptr) {}
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
		if (head->next == tail) return;
		node* tmp = head->next;
		head->next = tmp->next;
		tmp->next->prev = head;
		delete tmp;
	}
	void delete_tail() {
		if (tail->prev == head) return;
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
	using list_iterator = typename double_list<value_type>::iterator;
	/**
	 * elements
	 * add whatever you want
	 */
	double_list<value_type> *buckets;
	size_t capacity;
	size_t size;
	double load_factor = 0.75;
	Hash hash_func;
	Equal equal_func;

	// --------------------------

	/**
	 * the follows are constructors and destructors
	 * you can also add some if needed.
	 */
	hashmap() : capacity(16), size(0) {
		buckets = new double_list<value_type>[capacity];
	}
	hashmap(const hashmap &other) : capacity(other.capacity), size(0) {
		buckets = new double_list<value_type>[capacity]; 
		for (size_t i = 0; i < capacity; i++) {
			 for (auto it = other.buckets[i].begin(); it != other.buckets[i].end(); it++) {
				this->insert(*it);
			}
		}
	}
	~hashmap() {
		delete[] buckets;
	}
	hashmap &operator=(const hashmap &other) {
		if (this == &other) return *this;
		hashmap tmp(other);
		std::swap(buckets, tmp.buckets);
		std::swap(capacity, tmp.capacity);
		std::swap(size, tmp.size);
		return *this;
	}

	class iterator {
	public:
		/**
		 * elements
		 * add whatever you want
		 */
		list_iterator iter;
		hashmap* map_ptr;
		size_t index;
		// --------------------------
		/**
		 * the follows are constructors and destructors
		 * you can also add some if needed.
		 */
		iterator() : iter(), map_ptr(nullptr), index(0) {}
		iterator(size_t idx, list_iterator it, hashmap* ptr) : index(idx), iter(it), map_ptr(ptr) {}
		iterator(const iterator &t) : iter(t.iter), map_ptr(t.map_ptr), index(t.index) {}
		~iterator() {}

		/**
		 * if point to nothing
		 * throw
		 */
		value_type &operator*() const {
			if (map_ptr == nullptr || index >= map_ptr->capacity) {
				throw "point to nothing";
			}
			return *iter;
		}

		/**
		 * other operation
		 */
		value_type *operator->() const noexcept {
			return &(*iter);
		}
		bool operator==(const iterator &rhs) const {
			return map_ptr == rhs.map_ptr 
				&& index == rhs.index 
				&& iter == rhs.iter;
		}
		bool operator!=(const iterator &rhs) const {
			return !(*this == rhs);
		}
	};

	void clear() {
		size = 0;
		for (size_t i = 0; i < capacity; i++) {
			while (!buckets[i].empty()) {
				buckets[i].delete_head();
			}
		}
	}
	/**
	 * you need to expand the hashmap dynamically
	 */
	void expand() {
		size_t old_capacity = capacity;
		capacity <<= 1;
		double_list<value_type> *old_buckets = buckets;
		buckets = new double_list<value_type>[capacity];

		for (size_t i = 0; i < old_capacity; i++) {
			for (auto it = old_buckets[i].begin();
				it != old_buckets[i].end(); ++it) {

				size_t idx = getIndex((*it).first);
				buckets[idx].insert_tail(*it);
			}
		}

		delete[] old_buckets;
	}

	/**
	 * the iterator point at nothing
	 */
	iterator end() const {
		if (capacity == 0) return iterator(capacity, list_iterator(), const_cast<hashmap*>(this));
		return iterator(capacity - 1, buckets[capacity-1].end(), const_cast<hashmap*>(this));
	}
	/**
	 * find, return a pointer point to the value
	 * not find, return the end (point to nothing)
	 */
	iterator find(const Key &key) const {
		if (capacity == 0) return end();
		size_t idx = getIndex(key);
		for (auto it = buckets[idx].begin(); it != buckets[idx].end(); it++) {
			if (equal_func((*it).first, key)) {
				return iterator(idx, it, const_cast<hashmap*>(this));
			}
		}
		return end();
	}
	/**
	 * already have a value_pair with the same key
	 * -> just update the value, return false
	 * not find a value_pair with the same key
	 * -> insert the value_pair, return true
	 */
	sjtu::pair<iterator, bool> insert(const value_type &value_pair) {
		iterator it = find(value_pair.first);

		if(it != end()) {
			const_cast<value_type&>(*it.iter).second = value_pair.second;
			return sjtu::pair<iterator, bool>(it, false);
		}

		if(static_cast<double>(size + 1) / capacity > load_factor) {
			expand();
		}
		size_t idx = getIndex(value_pair.first);
		buckets[idx].insert_tail(value_pair);
		size++;

		auto lit = buckets[idx].end();
		lit--;
		return sjtu::pair<iterator, bool>(iterator(idx, lit, this), true);
	}
	/**
	 * the value_pair exists, remove and return true
	 * otherwise, return false
	 */
	bool remove(const Key &key) {
		iterator it = find(key);
		if(it == end()) return false;
		auto& list = buckets[it.index];
		list.erase(it.iter);
		size--;
		return true;
	}
private:
	size_t getIndex(const Key &key) const {
		size_t idx = hash_func(key) % capacity;
		return idx;
	}
};

template<class Key, class T, class Hash = std::hash<Key>, class Equal = std::equal_to<Key>>
class linked_hashmap : public hashmap<Key, typename double_list<pair<const Key, T>>::iterator, Hash, Equal> { 
public:
	using value_type = pair<const Key, T>;
	using list_iterator = typename double_list<value_type>::iterator;
	using base_hashmap = hashmap<Key, list_iterator, Hash, Equal>;

	/**
	 * elements
	 * add whatever you want
	 */
	double_list<value_type> global_list;
	// --------------------------
	class const_iterator;
	class iterator {
	public:
		/**
		 * elements
		 * add whatever you want
		 */
		list_iterator lit;
		linked_hashmap* map_ptr;
	// --------------------------
		// --------------------------
		iterator() : lit(list_iterator()), map_ptr(nullptr) {}
		iterator(list_iterator it, linked_hashmap* ptr) : lit(it), map_ptr(ptr) {}
		iterator(const iterator &other) : lit(other.lit), map_ptr(other.map_ptr) {}
		~iterator() {}

		/**
		 * iter++
		 */
		iterator operator++(int) {
			if(map_ptr == nullptr || lit == map_ptr->global_list.end()) 
				throw "invalid";
			iterator tmp = *this;
			++(*this);
			return tmp;
		}
		/**
		 * ++iter
		 */
		iterator &operator++() {
			if(map_ptr == nullptr || lit == map_ptr->global_list.end()) 
				throw "invalid";
			lit++;
			return *this;
		}
		/**
		 * iter--
		 */
		iterator operator--(int) {
			if(map_ptr == nullptr || lit == map_ptr->global_list.begin()) 
				throw "invalid";
			iterator tmp = *this;
			--(*this);
			return tmp;
		}
		/**
		 * --iter
		 */
		iterator &operator--() {
			if(map_ptr == nullptr || lit == map_ptr->global_list.begin()) 
				throw "invalid";
			lit--;
			return *this;
		}

		/**
		 * if the iter didn't point to a value
		 * throw "star invalid"
		 */
		value_type &operator*() const {
			if (map_ptr == nullptr || lit == map_ptr->global_list.end()) 
				throw "star invalid";
			return *lit;
		}
		value_type *operator->() const noexcept {
			return &(*lit);
		}

		/**
		 * operator to check whether two iterators are same (pointing to the same memory).
		 */
		bool operator==(const iterator &rhs) const {
			return lit == rhs.lit;
		}
		bool operator!=(const iterator &rhs) const {
			return lit != rhs.lit;
		}
		bool operator==(const const_iterator &rhs) const {
			return lit == rhs.lit;
		}
		bool operator!=(const const_iterator &rhs) const {
			return lit != rhs.lit;
		}
	};

	class const_iterator {
	public:
		/**
		 * elements
		 * add whatever you want
		 */
		list_iterator lit;
		const linked_hashmap* map_ptr;
		// --------------------------
		const_iterator() : lit(list_iterator()), map_ptr(nullptr) {}
		const_iterator(list_iterator it, const linked_hashmap* ptr) : lit(it), map_ptr(ptr) {}
		const_iterator(const iterator &other) : lit(other.lit), map_ptr(other.map_ptr) {}

		const_iterator &operator=(const iterator &rhs) {
			lit = rhs.lit;
			map_ptr = rhs.map_ptr;
			return *this;
		}

		/**
		 * iter++
		 */
		const_iterator operator++(int) {
			if(map_ptr == nullptr || lit == map_ptr->global_list.end()) 
				throw "invalid";
			const_iterator tmp = *this;
			++(*this);
			return tmp;
		}
		/**
		 * ++iter
		 */
		const_iterator &operator++() {
			if(map_ptr == nullptr || lit == map_ptr->global_list.end()) 
				throw "invalid";
			lit++;
			return *this;
		}
		/**
		 * iter--
		 */
		const_iterator operator--(int) {
			if(map_ptr == nullptr || lit == map_ptr->global_list.begin()) 
				throw "invalid";
			const_iterator tmp = *this;
			--(*this);
			return tmp;
		}
		/**
		 * --iter
		 */
		const_iterator &operator--() {
			if(map_ptr == nullptr || lit == map_ptr->global_list.begin()) 
				throw "invalid";
			lit--;
			return *this;
		}

		/**
		 * if the iter didn't point to a value
		 * throw
		 */
		const value_type &operator*() const {
			if (map_ptr == nullptr || lit == map_ptr->global_list.end()) 
				throw "invalid";
			return *lit;
		}
		const value_type *operator->() const noexcept {
			return &(*lit);
		}

		/**
		 * operator to check whether two iterators are same (pointing to the same memory).
		 */
		bool operator==(const iterator &rhs) const {
			return lit == rhs.lit;
		}
		bool operator!=(const iterator &rhs) const {
			return lit != rhs.lit;
		}
		bool operator==(const const_iterator &rhs) const {
			return lit == rhs.lit;
		}
		bool operator!=(const const_iterator &rhs) const {
			return lit != rhs.lit;
		}
	};

	linked_hashmap() {}
	linked_hashmap(const linked_hashmap &other) {
		for (auto it = other.cbegin(); it != other.cend(); ++it) {
			this->insert(*it);
		}
	}
	~linked_hashmap() {
		clear();
	}
	linked_hashmap &operator=(const linked_hashmap &other) {
		if (this == &other) return *this;
		clear();
		for (auto it = other.cbegin(); it != other.cend(); ++it) {
			this->insert(*it);
		}
		return *this;
	}

	/**
	 * return the value connected with the Key(O(1))
	 * if the key not found, throw
	 */

	T &at(const Key &key) {
		auto hash_it = base_hashmap::find(key);
		if (hash_it == base_hashmap::end())
			throw "invalid";
		list_iterator lit = (*hash_it).second;
		value_type value = *lit;
		global_list.erase(lit);
		global_list.insert_tail(value);
		(*hash_it).second = global_list.end();
		--((*hash_it).second);
		return (*((*hash_it).second)).second;
	}
	const T &at(const Key &key) const {
		auto it = base_hashmap::find(key);
		if (it == base_hashmap::end())
			throw "invalid";
		return (*((*it).second)).second;
	}
	T &operator[](const Key &key) {
		return at(key);
	}
	const T &operator[](const Key &key) const {
		return at(key);
	}

	/**
	 * return an iterator point to the first
	 * inserted and existed element
	 */
	iterator begin() {
		return iterator(global_list.begin(), this);
	}
	const_iterator cbegin() const {
		return const_iterator(global_list.begin(), this);
	}
	/**
	 * return an iterator after the last inserted element
	 */
	iterator end() {
		return iterator(global_list.end(), this);
	}
	const_iterator cend() const {
		return const_iterator(global_list.end(), this);
	}
	/**
	 * if didn't contain anything, return true,
	 * otherwise false.
	 */
	bool empty() const {
		return global_list.empty();
	}

	void clear() {
		this->base_hashmap::clear();
		while (!global_list.empty()) {
			global_list.delete_head();
		}
	}

	size_t size() const {
		return this->base_hashmap::size;
	}
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
	pair<iterator, bool> insert(const value_type &value) {
		auto hash_it = this->base_hashmap::find(value.first);

		if (hash_it != this->base_hashmap::end()) {
			global_list.erase((*hash_it).second);
			global_list.insert_tail(value);
			(*hash_it).second = global_list.end();
			--((*hash_it).second);
			return pair<iterator, bool>(iterator((*hash_it).second, this), false);
		}
		else {
			global_list.insert_tail(value);
			auto it = global_list.end();
			--it;
			this->base_hashmap::insert(sjtu::pair<const Key, list_iterator>(value.first, it));
			return pair<iterator, bool>(iterator(it, this), true);
		}
	}
	/**
	 * erase the value_pair pointed by the iterator
	 * if the iterator points to nothing
	 * throw
	 */
	void remove(iterator pos) {
		if (pos == end() || pos.map_ptr != this) 
			throw "invalid remove";
		this->base_hashmap::remove(pos->first);
		global_list.erase(pos.lit);
	}
	/**
	 * return how many value_pairs consist of key
	 * this should only return 0 or 1
	 */
	size_t count(const Key &key) const {
		auto it = base_hashmap::find(key);
		return (it == base_hashmap::end()) ? 0 : 1;
	}
	/**
	 * find the iterator points at the value_pair
	 * which consist of key
	 * if not find, return the iterator
	 * point at nothing
	 */
	iterator find(const Key &key) {
		auto hash_it = base_hashmap::find(key);
		if (hash_it == base_hashmap::end()) return end();
		return iterator((*hash_it).second, this);
	}
};

class lru {
	using lmap = sjtu::linked_hashmap<Integer, Matrix<int>, Hash, Equal>;
	using value_type = sjtu::pair<const Integer, Matrix<int>>;
private:
	lmap data;
	int max_size;
public:
	lru(int size) : max_size(size) {}
	~lru() {
		data.clear();
	}
	/**
	 * save the value_pair in the memory
	 * delete something in the memory if necessary
	 */
	void save(const value_type &v) {
		auto it = data.find(v.first);
		if (it != data.end()) {
			data.insert(v);
		}
		else {
			data.insert(v);
			if (data.size() > max_size) {
				data.remove(data.begin());
			}
		}
	}
	/**
	 * return a pointer contain the value
	 */
	Matrix<int> *get(const Integer &v) {
		auto it = data.find(v);
		if(it == data.end()){
			return nullptr;
		}

		return &(data.at(v));
	}
	/**
	 * just print everything in the memory
	 * to debug or test.
	 * this operation follows the order, but don't
	 * change the order.
	 */
	void print() {
		sjtu::linked_hashmap<Integer, Matrix<int>, Hash, Equal>::iterator it;
		for (it = data.begin(); it != data.end(); it++) {
			std::cout << (*it).first.val << " "
					<< (*it).second << std::endl;
    	}
	}
};
} // namespace sjtu

#endif
