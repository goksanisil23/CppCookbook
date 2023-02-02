# CppCookbook
Cpp Cookbook / Playground for the concept's I like to remember.

## Core Features

### l-value & r-value
- `l-value`: expression whose address can be taken.
```c++
int a;
a = 1; // a is an l-value
```
```c++
int x; // global
int& getRef() {return x;}
getRef() = 4;
```
Here `getRef` returns a reference to a value that is stored in permenant location, hence has an address.

- `r-value`: expression that results in a temporary object
```c++
int x; // global
int getVal() {return x;}
int val = getVal();
```
`getVal()` is an r-value, value being returned is not a reference to x, just a temporary value.
```c++
std::string getName() {return "Jordan";}
std::string name = getName();
```
Here `getName()` returns a string constructed inside the function, hence temporary object. getName() is an r-value. 

#### r-value references
Allows to bind a reference to an r-value but NOT to an l-value (so that we can differentiate between the 2).
```c++
std::string&& name = getName(); // name is r-value ref.
std::string& name2 = getName(); // ERROR, name2 is l-value ref.
```
To *cast* an l-value to an r-value, `std::move()` is used, to tell the compiler that we will only temporarily need that moved object, and not anymore further down the line.

> **Note**
> Do not return r-value ref. from a function using std::move. When returning a local function variable, compilers after C++11 will use RVO to automatically move them. So use
```c++
std::vector<int> GenVec(){
    std::vector<int> vec{1,2,3};
    return vec;
}
// RVO
```


### `auto` Usage

- Helps avoiding faulty implicit conversions:
```c++
auto sz1 = vec.size();
int sz2 = vec.size();
std::cout << std::boolalpha << std::is_same_v<decltype(sz1),decltype(sz2)> << std::endl; // not same

// Receives warning for narrowing conversion
auto sz3 = int{vec.size()};
```

- When used with reference types, `&` needs to be added explicitly:
```c++
int a = 42;
int& b = a;
auto c = b;
c++;
std::cout << a; // still 42
auto& d = b;
d++;
std::cout << a; // now 43
```

- Similarly, to return a reference from a function with auto, decltype need to be used:
```c++
class Foo {
	int x_;
	Foo(int x) : x_{x}{}
	int& Get() {return _x;}
};

auto ProxyGet(Foo& foo) {return f.get();}

auto f = Foo{ 42 };
auto& x = ProxyGet(f); // ERROR: Cannot convert int& to int
```

Instead, use decltype(auto):

```c++
decltype(auto) ProxyGet(Foo& foo) {return f.get();}
decltype(auto) x = ProxyGet(f);
```

### Initializer List Usage

- Initializing an object with initializer list by default is good practice since compiler *should* give an error if narrowing conversion happens:
```c++
double doub = 12/14; 
float flo{f}; // ERROR
```

### Memory alignment

- Processors do not access memory 1 byte at a time, but rather in largers chunks of powers of 2s. Hence, compilers align data in memory for more efficient access. With structs, alignment is determined by the largest member.

```c++
struct MyStruct
{
    char x;
    char y[4];
    float z;
    // implicit padding: char pad[3] for x to match largest mem: 4
};
cout << sizeof(MyStruct); // (1+3)+(4)+(4) = 12
cout << alignof(MyStruct); // 4 due to float or char[4]
```
One could try to enforce a different alignment size, and the compiler will chose the largest one.
```c++
struct alignas(8) MyStruct
{
    char x;
    char y[4];
    float z;
    // implicit padding: char pad[7] because
    // sizeof(y)+sizeof(z) = 8, so x needs 7 to match 8.
};
cout << sizeof(MyStruct); // (1+7)+(4+4)+(4+4) = 16
cout << alignof(MyStruct); // 8 due to alignas
```

In order to control the padding behavior of the compiler (which would come at a performance cost due to increased cache misses), once could use `#pragma pack(n)`. 
```c++
#pragma pack(1)
struct MyStruct
{
    char x;
    char y[4];
    float z;
};
cout << sizeof(MyStruct); // 1 + 4 + 4 = 9
cout << alignof(MyStruct); // 1
```

### Scoped Enumerators
- Prefer scoped enumerators in general to unscoped enumerators. They avoid name clashes and ambigous implicit conversions to int.
```c++
enum MyEnum{ OK, NOK};
int var = MyEnum::OK; // unscoped Enum converts to int
enum class MyEnumScoped{ OK, NOK};
int var2  = MyEnumScoped::OK; // ERROR, needs static_cast<int>()
```

### Inlined namespaces
A member of an inline namespace is treated as a member of the surrounding namespace. This is useful for:
- Symbol versioning for different versions of the library 
```c++
namespace mylib
{
    #ifdef VERSION_2
    inline namespace version_2
    {
        void myfunc(){cout << "2\n";};
    }
    #else
    inline namespace version_1
    {
        void myfunc(){cout << "1\n";};
    }    
    #endif
}
// ...
mylib::myfunc(); // works for both version_1 and version_2
```
Depending on build flags `VERSION_1` and `VERSION_2`, inlined namespace allows us to see that version info in the library symbol. Note that the user do not need to change usage of API call.
```bash
nm mylib | grep myfunc # will show version_2 or version_1 in symbol name
```
- Finer grained access to features within namespaces.
```c++
namespace top
{
    inline namespace mid
    {
        inline namespace bot
        {
            void myfunc(){std::cout << "myfunc\n";}
        }
    }
}
// ...
top::mid::bot::myfunc(); // finer grained
// OR
using top;
myfunc(); // convenient
```

### Rule of 5
- Rule of 5: If *any* of class special member functions are explicitly defined, either explicitly define or default all of them.

```c++
class Example
{
public:
    Example(): dataPtr_{nullptr}, var_(0) {}

    Example(const Example& other) // Copy Constructor
    {
        dataPtr_ = new int(*(other.dataPtr_)); // Deep copy
        var_ = other.var_;
    }

    Example& operator= (const Example& other) // Copy Assign. Op.
    {
        if(this != &other)
        {
            delete dataPtr_;
            dataPtr_ = new int(*(other.dataPtr_)); // Deep Copy
            var_ = other.var_;
        }
        return *this;
    }

    Example(Example&& other) noexcept // Move Constructor
    {
        dataPtr_ = other.dataPtr_; // transfer ownership
        var_ = other.var_;
        other.dataPtr_ = nullptr; // release the other
        other.var_ = 0.0;
    }

    Example& operator= (Example&& other) // Move Assign. Op.
    {
        if(this != &other)
        {
            delete dataPtr_;
            dataPtr_ = other.dataPtr_; 
            var_ = other.var_;
            other.dataPtr_ = nullptr; // release the other
            other.var_ = 0.0;
        }
        return *this;
    }

    ~Example()
    {
        delete dataPtr_;
    }

private:
    int* dataPtr_;
    double var_;
}; 
```

If we are providing both move assignment and move constructor for the class, we can avoid redundant code by having move constructor call move assignment:
```c++
    Example(Example&& other) noexcept // Move Constructor
    {
        *this = std::move(other);
    }
```
Here `std::move` converts lvalue `other` to an rvalue.

### Variadic Function Templates
Allows to define non-fixed number of input arguments to functions. The compiler recursively generates deterministic versions in compile time.
```c++
template <typename T>
T add(T value)
{
    return value;
}

template <typename T, typename ... Ts>
T add (T head, Ts ... rest)
{
    std::cout << __PRETTY_FUNCTION__ << std::endl;
    //std::cout << sizeof...(rest) << std::endl;
    return head + add(rest...);
}
// ...
auto r1 = add(1,2,3,4);
auto r2 = add("hey","teacher"," ", "leave'em");
```

Prints out:
```c++
T add(T, Ts...) [T = unsigned int, Ts = <double, float, int, long>]
T add(T, Ts...) [T = double, Ts = <float, int, long>]
T add(T, Ts...) [T = float, Ts = <int, long>]
T add(T, Ts...) [T = int, Ts = <long>]
7
T add(T, Ts...) [T = std::__cxx11::basic_string<char>, Ts = <std::__cxx11::basic_string<char>, std::__cxx11::basic_string<char>>]
T add(T, Ts...) [T = std::__cxx11::basic_string<char>, Ts = <std::__cxx11::basic_string<char>>]
aa bbb
```
Note that if we change the return type of variadic template function to auto, return type can be inferred from the return expression, and in this case since we have double along the additions, it will return a double:
```c++
template <typename T, typename ... Ts>
auto add (T head, Ts ... rest)
{...}
```
Prints out:
```c++
auto add(T, Ts...) [T = unsigned int, Ts = <double, float, int, long>]
auto add(T, Ts...) [T = double, Ts = <float, int, long>]
auto add(T, Ts...) [T = float, Ts = <int, long>]
auto add(T, Ts...) [T = int, Ts = <long>]
7.2
```

### Fold expressions
Fold expressions are related to variadic arguments of templates. It enables compiler to apply the same binary operation `(+,-,>,*,/=,...)` to all arguments of parameter pack. The expression must to be surrounded by parenthesis `(...)`
- Note that associtivity matters when folding:
```c++
template <typename ... Ts>
auto minus(Ts const& ... vals)
{
    return (values - ...);
    // OR
    return (... - values);
}
```
The example above will give different results for `minus(1,2,3)`, depending on left or right associtivity.
