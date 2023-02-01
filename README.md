# CppCookbook
Cpp Cookbook / Playground for the concept's I like to remember.

## Core Features

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

### Copy & Move semantics
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