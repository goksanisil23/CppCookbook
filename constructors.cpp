#include <iostream>
#include <type_traits>
#include <vector>
#include <atomic>

class IntWrapper {
public:
	IntWrapper() : value_(0) {std::cout << "default const.\n";} // default constructor

	IntWrapper(int value) : value_(value), val2{value} {std::cout << "arg constructor\n";}

    // Copy constructor
	IntWrapper(const IntWrapper& other) : value_(other.value_) {std::cout << "copy constructor\n";}

    // Move constructor
	IntWrapper(IntWrapper&& other) : value_(std::move(other.value_)) {std::cout << "move const.\n";}

    // Copy assignment operator
	IntWrapper& operator=(const IntWrapper& other) {
		std::cout << "copy assignment\n";
		value_ = other.value_;
		return *this;
	}

    // Move assignment operator
	IntWrapper& operator=(IntWrapper&& other) {
		std::cout << "move assignment\n";
		value_ = std::move(other.value_);
		return *this;
	}

// private:
	int value_;
	const int val2{2};
};


void func1(const IntWrapper& wrapper)
{
	std::cout << wrapper.value_ << std::endl;
}


void func2(const IntWrapper wrapper)
{
	std::cout << wrapper.value_ << std::endl;
}

void func3(IntWrapper&& wrapper)
{
	std::cout << wrapper.value_ << std::endl;
}


int main()
{


	std::vector<IntWrapper> wrappers;

	auto wrapper = IntWrapper(22);
	// wrappers.push_back(wrapper); // Copy constructs
	// wrappers.emplace_back(wrapper); // Copy constructs
	// wrappers.push_back(std::move(wrapper)); // Move constructs
	// wrappers.emplace_back(std::move(wrapper)); // Move constructs

	// func3(IntWrapper(2));
	// wrappers.push_back(IntWrapper(2)); // move constructs
	// wrappers.emplace_back(IntWrapper(2)); // move constructs
	// wrappers.emplace_back(2); // directly constructs in place, no move or copy

	// IntWrapper newWrapper = wrapper; // copy constructor
	// IntWrapper newWrapper = std::move(wrapper); // move constructor
	IntWrapper newWrapper;
	newWrapper = wrapper; // copy assignment
	newWrapper = std::move(wrapper); // move assignment

	// func1(wrapper);


	return 0;
}