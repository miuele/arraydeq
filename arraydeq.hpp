#ifndef ARRAYLIKE_ARRAYDEQ_HPP_INCLUDED
#define ARRAYLIKE_ARRAYDEQ_HPP_INCLUDED

#include <utility>
#include <cstddef>
#include <algorithm>
#include <new>

namespace arraylike {

template <class T, std::size_t N>
struct arraydeq_iterator;

template <class T, std::size_t N>
struct arraydeq_const_iterator;

template <class T, std::size_t N>
class arraydeq {
public:
	using value_type = T;
	using reference = T &;
	using const_reference = const T &;
	using difference_type = std::ptrdiff_t;
	using size_type = std::size_t;
	using iterator = arraydeq_iterator<T, N>;
	using const_iterator = arraydeq_const_iterator<T, N>;
	using reverse_iterator = std::reverse_iterator<iterator>;
	using const_reverse_iterator = std::reverse_iterator<const_iterator>;

	constexpr arraydeq()
		: head_(0), size_(0)
	{
	}

	arraydeq(std::initializer_list<T> init)
		: arraydeq()
	{
		for (auto it = std::begin(init)
				; !full() && it != std::end(init)
				; ++it)
		{
			push_back(*it);
		}
	}

	~arraydeq() {
		clear();
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

	void resize(std::size_t n) {
		if (n < size_) {
			const auto ndel = size_ - n;
			auto index = (head_ + size_ - 1) % N;
			for (std::size_t i = 0; i < ndel; ++i) {
				destruct_element_at(index);
				index = (N + index - 1) % N;
			}
			size_ = n;
		} else {
			const auto nadd = n - size_;
			for (std::size_t i = 0; i < nadd; ++i) {
				emplace_back();
			}
		}
	}

	void clear() {
		resize(0);
	}

	constexpr std::size_t size() const {
		return size_;
	}

	constexpr bool full() const {
		return size_ == N;
	}

	constexpr bool empty() const {
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

	auto begin() { return iterator(*this, 0); }
	auto end() { return iterator(*this, size_); }

	auto begin() const { return const_iterator(*this, 0); }
	auto end() const { return const_iterator(*this, size_); }

	auto cbegin() { return const_cast<const arraydeq &>(*this).begin(); }
	auto cend() { return const_cast<const arraydeq &>(*this).end(); }

	auto cbegin() const { return begin(); }
	auto cend() const { return end(); }

	auto rbegin() const { return reverse_iterator(end()); }
	auto rend() const { return reverse_iterator(begin()); }

	auto rbegin() { return reverse_iterator(end()); }
	auto rend() { return reverse_iterator(begin()); }

	auto crbegin() { return const_cast<const arraydeq &>(*this).rbegin(); }
	auto crend() { return const_cast<const arraydeq &>(*this).crend(); }

	auto crbegin() const { return rbegin(); }
	auto crend() const { return rend(); }

	constexpr std::size_t max_size() const { return N; }

	void swap(arraydeq &other) { std::swap(*this, other); }

	bool operator==(const arraydeq &other) const {
		return std::equal(begin(), end(), other.begin(), other.end());
	}

	bool operator!=(const arraydeq &other) const {
		return !(*this == other);
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
		storage_[index].construct(std::forward<Args>(args)...);
	}

	void destruct_element_at(std::size_t index) {
		storage_[index].destruct();
	}

	union storage_cell {
		storage_cell() {}
		~storage_cell() {}

		template <class ...Args>
		void construct(Args &&...args) {
			new(&obj) T{std::forward<Args>(args)...};
		}
		void destruct() {
			obj.~T();
		}
		T obj;
	} storage_[N];

	std::size_t head_;
	std::size_t size_;
};

template <class D>
class arraydeq_iterator_base {
public:
	using difference_type = std::ptrdiff_t;
	using value_type = typename D::value_type;
	using pointer = typename std::conditional<std::is_const<D>::value, const value_type *, value_type *>::type;
	using reference = typename std::conditional<std::is_const<D>::value, const value_type &, value_type &>::type;
	using iterator_category = std::random_access_iterator_tag;

	arraydeq_iterator_base(const arraydeq_iterator_base &) = default;
	arraydeq_iterator_base &operator=(const arraydeq_iterator_base &other) = default;

	explicit arraydeq_iterator_base(D &arrdeq, std::size_t index)
		: pointee_(&arrdeq), index_(index)
	{}

	arraydeq_iterator_base &operator++() {
		++index_;
		return *this;
	}

	arraydeq_iterator_base operator++(int) {
		auto old = *this;
		operator++();
		return old;
	}

	arraydeq_iterator_base &operator--() {
		--index_;
		return *this;
	}

	arraydeq_iterator_base operator--(int) {
		auto old = *this;
		operator--();
		return old;
	}

	reference operator*() {
		return (*pointee_)[index_];
	}

	bool operator==(const arraydeq_iterator_base &other) const {
		return pointee_ == other.pointee_ && index_ == other.index_;
	}

	bool operator!=(const arraydeq_iterator_base &other) const {
		return !(*this == other);
	}

	reference operator->() const {
		return *pointee_;
	}

	reference operator[](difference_type n) const {
		return (*pointee_)[index_ + n];
	}

	bool operator<(const arraydeq_iterator_base &other) const {
		return index_ < other.index_;
	}

	bool operator>(const arraydeq_iterator_base &other) const {
		return other < *this;
	}

	bool operator>=(const arraydeq_iterator_base &other) const {
		return !(*this < other);
	}

	bool operator<=(const arraydeq_iterator_base &other) const {
		return !(*this > other);
	}

	arraydeq_iterator_base &operator+=(difference_type n) {
		index_ += n;
		return *this;
	}

	arraydeq_iterator_base &operator-=(difference_type n) {
		index_ -= n;
		return *this;
	}

	friend arraydeq_iterator_base operator+(const arraydeq_iterator_base &lhs, difference_type n) {
		return arraydeq_iterator_base{lhs, lhs.index_ + n};
	}

	friend arraydeq_iterator_base operator+(difference_type n, const arraydeq_iterator_base &lhs) {
		return arraydeq_iterator_base{lhs, lhs.index_ + n};
	}

	friend difference_type operator-(const arraydeq_iterator_base &lhs, const arraydeq_iterator_base &rhs) {
		return lhs.index_ - rhs.index_;
	}

private:
	D *pointee_;
	std::size_t index_;
};

template <class T, std::size_t N>
struct arraydeq_iterator
	: arraydeq_iterator_base<arraydeq<T, N>>
{
	using arraydeq_iterator_base<arraydeq<T, N>>::arraydeq_iterator_base;
};

template <class T, std::size_t N>
struct arraydeq_const_iterator
	: arraydeq_iterator_base<const arraydeq<T, N>>
{
	using arraydeq_iterator_base<const arraydeq<T, N>>::arraydeq_iterator_base;
};

}

#endif
