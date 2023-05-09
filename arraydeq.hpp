#ifndef ARRAYLIKE_ARRAYDEQ_HPP_INCLUDED
#define ARRAYLIKE_ARRAYDEQ_HPP_INCLUDED

#include <utility>
#include <cstddef>
#include <new>

namespace arraylike {

template <class T, std::size_t N>
struct arraydeq {
	arraydeq()
		: head_(0), size_(0)
	{
	}

	arraydeq(std::initializer_list<T> init)
		: arraydeq()
	{
		for (auto it = std::begin(init)
				; !is_full() && it != std::end(init)
				; ++it)
		{
			push_back(*it);
		}
	}

	template <class ...Args>
	void emplace_back(Args &&...args) {
		auto tail = (head_ + size_) % N;
		if (size_ == N) {
			// in this case, head_ == tail;
			destruct_element_at(tail);
			head_ = (head_ + 1) % N;
		} else {
			++size_;
		}
		construct_element_at(tail, std::forward<Args>(args)...);
	}

	void push_back(const T &value) {
		emplace_back(value);
	}

	void push_back(T &&value) {
		emplace_back(std::move(value));
	}

	template <class ...Args>
	void emplace_front(Args &&...args) {
		head_ = (N + head_ - 1) % N;
		if (size_ == N) {
			destruct_element_at(head_);
		} else {
			++size_;
		}
		construct_element_at(head_, std::forward<Args>(args)...);
	}

	void push_front(const T &value) {
		emplace_front(value);
	}

	void push_front(T &&value) {
		emplace_front(std::move(value));
	}

	T pop_front() {
		T item{std::move(element_at(head_))};
		destruct_element_at(head_);
		head_ = (head_ + 1) % N;
		--size_;
		return item;
	}

	T pop_back() {
		auto last = (head_ + size_ - 1) % N;
		T item{std::move(element_at(last))};
		destruct_element_at(last);
		--size_;
		return item;
	}

	std::size_t size() const {
		return size_;
	}

	bool is_full() const {
		return size_ == N;
	}

	bool is_empty() const {
		return size_ == 0;
	}

	const T &front() const {
		return operator[](0);
	}

	T &front() {
		return operator[](0);
	}

	const T &back() const {
		return operator[](size() - 1);
	}

	T &back() {
		return operator[](size() - 1);
	}

	const T &operator[](std::size_t index) const {
		return element_at((head_ + index) % N);
	}

	T &operator[](std::size_t index) {
		return element_at((head_ + index) % N);
	}

private:
	const T &element_at(std::size_t index) const {
		return storage_[index].obj;
	}

	T &element_at(std::size_t index) {
		return storage_[index].obj;
	}

	template <class ...Args>
	void construct_element_at(std::size_t index, Args &&...args) {
		new(&storage_[index].obj) T{std::forward<Args>(args)...};
	}

	void destruct_element_at(std::size_t index) {
		element_at(index).~T();
	}

	union {
		T obj;
	} storage_[N];
	std::size_t head_;
	std::size_t size_;
};

}

#endif
