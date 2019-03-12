#ifndef LINKED_LIST_H
#define LINKED_LIST_H
#include <stdint.h>
#include <functional>
#include <utility>
#include <iostream>

/*
Author: Alice L. Jones
Github: @lyriluscs
Date: 2/2/2019
*/

namespace util {
template<typename T>
class linked_list {
	using list_t = linked_list<T>;
public:
	struct node_t {
		T data;
		node_t *prev;
		node_t *next;

		inline node_t(const T &_data, node_t *_next = nullptr, node_t *_prev = nullptr) : data{ _data }, next{ _next }, prev{ _prev } { }
		inline node_t(T &&_data, node_t *_next = nullptr, node_t *_prev = nullptr) : data{ std::move(_data) }, next{ _next }, prev{ _prev } { }
	};

	using node_callback = std::function<void(node_t *)>;
	using unary_predicate = std::function<bool(T &)>;
	using binary_predicate = std::function<bool(T &, T &)>;

	template<typename Ptr, typename Ref>
	class iterator_base {
	public:
		using iterator_category = std::bidirectional_iterator_tag;
		using value_type = T;
		using difference_type = std::ptrdiff_t;
		using pointer = Ptr;
		using reference = Ref;
		using iterator_type = iterator_base<Ptr, Ref>;

	public:
		inline explicit iterator_base(node_t *node) : m_node{ node } { }

		inline iterator_type &operator++();
		inline iterator_type operator++(int);
		inline iterator_type &operator--();
		inline iterator_type operator--(int);
		inline bool operator==(const iterator_type &other) const;
		inline bool operator!=(const iterator_type &other) const;
		inline reference operator*() const;
		inline pointer operator->() const;
		inline operator pointer() const;

		inline void swap(iterator_type &other) noexcept;

	private:
		node_t *m_node = nullptr;
	};

	using const_iterator = iterator_base<const node_t *, const node_t &>;
	using iterator = iterator_base<node_t *, node_t &>;
public:
	inline linked_list() = default;
	inline linked_list(const list_t &) = delete;
	inline linked_list(size_t alloc_count, const T &value = T{});
	inline ~linked_list();

	inline list_t &operator=(const list_t &) = delete;

	inline void pop_front();
	inline void pop_back();
	inline void push_front(const T &);
	inline void push_front(T &&);
	inline void push_back(const T &);
	inline void push_back(T &&);

	template<typename ...Ts>
	inline void emplace_front(Ts &&...args);

	template<typename ...Ts>
	inline void emplace_back(Ts &&...args);

	template<typename ...Ts>
	inline void insert_before(node_t *pos, Ts &&...args);
	inline void insert_before(node_t *pos, const T &val);

	template<typename ...Ts>
	inline void insert_after(node_t *pos, Ts &&...args);
	inline void insert_after(node_t *pos, const T &val);

	template<typename ...Ts>
	inline void insert_at(size_t index, Ts &&...args);
	inline void insert_at(size_t index, const T &val);

	inline node_t *at(size_t index);
	inline const node_t *at(size_t index) const;
	inline node_t *operator[](size_t index);
	inline const node_t *operator[](size_t index) const;

	inline bool empty() const;
	inline void clear();
	inline void print(std::function<void(T &)> &&printfn = [](T &d) { std::cout << d << std::endl; });

	inline void for_each(node_callback callback);
	inline void rfor_each(node_callback callback);
	inline iterator find_if(unary_predicate pred);
	inline iterator find(const T &value);

	inline auto begin() { return iterator(m_head); }
	inline auto end() { return iterator(nullptr); }
	inline auto cbegin() const { return const_iterator(m_head); }
	inline auto cend() const { return const_iterator(nullptr); }

	inline const auto &size() const { return m_len; }
	inline const auto &length() const { return m_len; }

	inline auto back() { return m_tail; }
	inline const auto &back() const { return m_back; }
	inline auto front() { return m_head; }
	inline const auto &front() const { return m_head; }

protected:
	node_t *m_head = nullptr;
	node_t *m_tail = nullptr;
	size_t m_len = 0u;
};

template<typename T>
linked_list<T>::linked_list(size_t alloc_count, const T &value) {
	for (size_t i{}; i < alloc_count; ++i)
		push_back(value);
}

template<typename T>
linked_list<T>::~linked_list() {
	if (!empty())
		clear();	
}

template<typename T>
void linked_list<T>::clear() {
	node_t *cur = m_head, *next{};

	while (cur) {
		if (cur->next)
			next = cur->next;

		delete cur;
		cur = next;
	}

	m_len = 0u;
}

template<typename T>
void linked_list<T>::pop_front() {
	if (empty())
		return;

	if (m_tail == m_head)
		m_tail = nullptr;

	auto head = m_head;
	m_head = m_head->next;
	delete head;

	if (m_head)
		m_head->prev = nullptr;

	--m_len;
}

template<typename T>
void linked_list<T>::push_front(const T &val) {
	// allocate
	auto node = new node_t(val);

	if (m_head) {
		// link current head to new node (new head)
		m_head->prev = node;
		node->next = m_head;

		// if no link following head, head should be made tail (should already be the case)
		if (!m_head->next && !m_tail)
			m_tail = m_head;
	}

	m_head = node;

	// newly allocated node is only node in list
	if (!m_tail)
		m_tail = m_head;

	++m_len;
}

template<typename T>
void linked_list<T>::push_front(T &&val) {
	auto node = new node_t(std::move(val));

	if (m_head) {
		m_head->prev = node;
		node->next = m_head;

		if (!m_head->next && !m_tail)
			m_tail = m_head;
	}

	m_head = node;
	if (!m_tail)
		m_tail = m_head;

	++m_len;
}

template<typename T> template<typename ...Ts>
void linked_list<T>::emplace_front(Ts &&...args) {
	T tmp{ std::forward<Ts>(args)... };
	auto node = new node_t(std::move(tmp));

	if (m_head) {
		m_head->prev = node;
		node->next = m_head;

		if (!m_head->next && !m_tail)
			m_tail = m_head;
	}

	m_head = node;

	if (!m_tail)
		m_tail = m_head;

	++m_len;
}

template<typename T>
void linked_list<T>::pop_back() {
	if (empty())
		return;

	if (m_tail == m_head)
		m_head = nullptr;

	auto tail = m_tail;
	m_tail = m_tail->prev;
	delete tail;

	if (m_tail)
		m_tail->next = nullptr;

	--m_len;
}

template<typename T>
void linked_list<T>::push_back(const T &val) {
	auto node = new node_t(val);
	if (m_tail) {
		m_tail->next = node;
		node->prev = m_tail;

		if (!m_tail->prev && !m_head)
			m_head = m_tail;
	}

	m_tail = node;

	if (!m_head)
		m_head = m_tail;

	++m_len;
}

template<typename T>
void linked_list<T>::push_back(T &&val) {
	auto node = new node_t(std::move(val));
	if (m_tail) {
		m_tail->next = node;
		node->prev = m_tail;

		if (!m_tail->prev && !m_head)
			m_head = m_tail;
	}

	m_tail = node;
	if (!m_head)
		m_head = m_tail;

	++m_len;
}

template<typename T> template<typename ...Ts>
void linked_list<T>::emplace_back(Ts &&...args) {
	T tmp{ std::forward<Ts>(args)... };
	auto node = new node_t(std::move(tmp));

	if (m_tail) {
		m_tail->next = node;
		node->prev = m_tail;

		if (!m_tail->prev && !m_head)
			m_head = m_tail;
	}

	m_tail = node;

	if (!m_head)
		m_head = m_tail;

	++m_len;
}

template<typename T> template<typename ...Ts>
void linked_list<T>::insert_before(node_t *pos, Ts &&...args) {
	if (!pos)
		return;

	T tmp{ std::forward<Ts>(args)... };
	auto node = new node_t(std::move(tmp));

	// link our new node
	node->prev = pos->prev;
	node->next = pos;

	// redirect previous node to our newly created one
	if (pos->prev)
		pos->prev->next = node;

	pos->prev = node;

	if (m_head == pos)
		m_head = node;

	++m_len;
}

template<typename T>
void linked_list<T>::insert_before(node_t *pos, const T &val) {
	if (!pos)
		return;

	auto node = new node_t(val);

	// link our new node
	node->prev = pos->prev;
	node->next = pos;

	// redirect previous node to our newly created one
	if (pos->prev)
		pos->prev->next = node;

	pos->prev = node;

	if (m_head == pos)
		m_head = node;

	++m_len;
}

template<typename T> template<typename ...Ts>
void linked_list<T>::insert_after(node_t *pos, Ts &&...args) {
	if (!pos)
		return;

	T tmp{ std::forward<Ts>(args)... };
	auto node = new node_t(std::move(tmp));

	// link our new node
	node->prev = pos;
	node->next = pos->next;

	// redirect following node to our newly created one
	if (pos->next)
		pos->next->prev = node;

	pos->next = node;

	if (m_tail == pos)
		m_tail = node;

	++m_len;
}

template<typename T>
void linked_list<T>::insert_after(node_t *pos, const T &val) {
	if (!pos)
		return;

	auto node = new node_t(val);

	// link our new node
	node->prev = pos;
	node->next = pos->next;

	// redirect following node to our newly created one
	if (pos->next)
		pos->next->prev = node;

	pos->next = node;

	if (m_tail == pos)
		m_tail = node;

	++m_len;
}

template<typename T> template<typename ...Ts>
void linked_list<T>::insert_at(size_t index, Ts &&...args) {
	if (index > m_len)
		return;

	auto node_at_index = at(index);
	if (node_at_index)
		insert_before(node_at_index, std::forward<Ts>(args)...);
	else
		emplace_front(std::forward<Ts>(args)...);
}

template<typename T>
void linked_list<T>::insert_at(size_t index, const T &val) {
	if (index > m_len)
		return;

	auto node_at_index = at(index);
	if (node_at_index)
		insert_before(node_at_index, val);
	else
		push_front(val);
}

template<typename T>
typename linked_list<T>::node_t *linked_list<T>::at(size_t index) {
	if (index >= m_len)
		return nullptr;

	auto cur = m_head;
	for (size_t i{}; i < index; ++i)
		cur = cur->next;

	return cur;
}

template<typename T>
typename const linked_list<T>::node_t *linked_list<T>::at(size_t index) const {
	if (index >= m_len)
		return nullptr;

	auto cur = m_head;
	for (size_t i{}; i < index; ++i)
		cur = cur->next;

	return cur;
}

template<typename T>
typename linked_list<T>::node_t *linked_list<T>::operator[](size_t index) {
	return at(index);
}

template<typename T>
typename const linked_list<T>::node_t *linked_list<T>::operator[](size_t index) const {
	return at(index);
}

template<typename T>
bool linked_list<T>::empty() const {
	return m_len == 0;
}

template<typename T>
void linked_list<T>::for_each(node_callback callback) {
	if (empty())
		return;

	if (m_head == m_tail) {
		callback(m_head);
	} else {
		for (auto node = m_head; node; node = node->next)
			callback(node);
	}
}

template<typename T>
void linked_list<T>::rfor_each(node_callback callback) {
	if (empty())
		return;

	if (m_tail == m_head) {
		callback(m_tail);
	} else {
		for (auto node = m_tail; node; node = node->prev)
			callback(node);
	}
}

template<typename T>
typename linked_list<T>::iterator linked_list<T>::find_if(unary_predicate pred) {
	for (auto node = begin(); node != end(); ++node) {
		if (pred(node->data))
			return node;
	}

	return end();
}

template<typename T>
typename linked_list<T>::iterator linked_list<T>::find(const T &value) {
	for (auto node = begin(); node != end(); ++node) {
		if (node->data == value)
			return node;
	}

	return end();
}

template<typename T>
void linked_list<T>::print(std::function<void(T &)> &&printfn) {
	if (empty())
		return;

	for (auto node = begin(); node != end(); ++node)
		printfn(node->data);
}

template<typename T> template<typename Ptr, typename Ref>
linked_list<T>::iterator_base<Ptr, Ref> &linked_list<T>::iterator_base<Ptr, Ref>::operator++() {
	m_node = m_node->next;
	return *this;
}

template<typename T> template<typename Ptr, typename Ref>
linked_list<T>::iterator_base<Ptr, Ref> linked_list<T>::iterator_base<Ptr, Ref>::operator++(int) {
	auto ret = *this;
	operator++();
	return ret;
}

template<typename T> template<typename Ptr, typename Ref>
linked_list<T>::iterator_base<Ptr, Ref> &linked_list<T>::iterator_base<Ptr, Ref>::operator--() {
	m_node = m_node->prev;
	return *this;
}

template<typename T> template<typename Ptr, typename Ref>
linked_list<T>::iterator_base<Ptr, Ref> linked_list<T>::iterator_base<Ptr, Ref>::operator--(int) {
	auto ret = *this;
	operator--();
	return ret;
}

template<typename T> template<typename Ptr, typename Ref>
bool linked_list<T>::iterator_base<Ptr, Ref>::operator==(const iterator_type &other) const {
	return m_node == other.m_node;
}

template<typename T> template<typename Ptr, typename Ref>
bool linked_list<T>::iterator_base<Ptr, Ref>::operator!=(const iterator_type &other) const {
	return m_node != other.m_node;
}

template<typename T> template<typename Ptr, typename Ref>
Ref linked_list<T>::iterator_base<Ptr, Ref>::operator*() const {
	return *m_node;
}

template<typename T> template<typename Ptr, typename Ref>
Ptr linked_list<T>::iterator_base<Ptr, Ref>::operator->() const {
	return m_node;
}

template<typename T> template<typename Ptr, typename Ref>
linked_list<T>::iterator_base<Ptr, Ref>::operator Ptr() const {
	return m_node;
}

template<typename T> template<typename Ptr, typename Ref>
void linked_list<T>::iterator_base<Ptr, Ref>::swap(iterator_type &other) noexcept {
	std::swap(m_node, other.m_node);
}
}
#endif // LINKED_LIST_H