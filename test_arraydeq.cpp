#include <iostream>
#include <algorithm>
#include <iterator>
#include "arraydeq.hpp"

using arraylike::arraydeq;

template <std::size_t N>
void print_content(const arraydeq<int, N> &fifo) {
	for (std::size_t i = 0; i < fifo.size(); ++i) {
		std::cout << fifo[i] << ", ";
	}
	std::cout << '[' << fifo.size() << ']' << std::endl;
}

void test1() {
	arraydeq<int, 5> fifo;
	print_content(fifo);
	volatile int a = 5;
	fifo.emplace_back(a);
	print_content(fifo);
	fifo.emplace_back(3);
	print_content(fifo);
	fifo.push_front(1);
	print_content(fifo);
	fifo.push_back(2);
	print_content(fifo);
	fifo.pop_back();
	print_content(fifo);
	fifo.emplace_back(9);
	print_content(fifo);
	fifo.emplace_back(11);
	print_content(fifo);
	fifo.emplace_back(12);
	print_content(fifo);
	fifo.pop_back();
	print_content(fifo);
	fifo.pop_back();
	print_content(fifo);
	fifo.pop_back();
	print_content(fifo);
	fifo.pop_front();
	print_content(fifo);
	fifo.pop_front();
	print_content(fifo);
}

void test2() {
	arraydeq<int, 5> fifo;
	print_content(fifo);
	volatile int a = 5;
	fifo.emplace_front(a);
	print_content(fifo);
	fifo.emplace_front(3);
	print_content(fifo);
	fifo.push_front(1);
	print_content(fifo);
	fifo.push_front(2);
	print_content(fifo);
	fifo.pop_front();
	print_content(fifo);
	fifo.emplace_front(9);
	print_content(fifo);
	fifo.emplace_front(11);
	print_content(fifo);
	fifo.emplace_front(12);
	print_content(fifo);
	fifo.pop_front();
	print_content(fifo);
	fifo.pop_front();
	print_content(fifo);
	fifo.pop_front();
	print_content(fifo);
	fifo.pop_back();
	print_content(fifo);
	fifo.pop_back();
	print_content(fifo);
}

void test3() {
	const arraydeq<int, 5> fifo{2, 5, 4, 9, 8, 1};
	print_content(fifo);
	for (auto x : fifo) {
		std::cout << x << " : ";
	}
	std::cout << std::endl;
	std::for_each(
		std::make_reverse_iterator(fifo.end()),
		std::make_reverse_iterator(fifo.begin()),
		[](auto x) {
			std::cout << x << " : ";
		}
	);
	std::cout << std::endl;
	std::cout << "len: " << std::distance(fifo.begin(), fifo.end()) << std::endl;
	arraydeq<int, 10> a{1, 2, 3};
	arraydeq<int, 10> b{4, 5, 6};
	a.swap(b);
	print_content(a);
}

int main() {
	test1();
	std::cout << "---" << std::endl;
	test2();
	std::cout << "---" << std::endl;
	test3();
}

