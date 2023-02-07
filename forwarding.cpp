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


template<typename T>
void valAssign(T&& valIn)
{
	// IntWrapper val = valIn; // calls copy constructor
	// IntWrapper val = std::forward<T>(valIn); // calls move constructor
	IntWrapper val = std::move(valIn); // calls move constructor
	std::cout << "value: " << val.value_ << std::endl;

}

int main()
{
	IntWrapper val(2);
	valAssign(std::move(val));

	return 0;
}