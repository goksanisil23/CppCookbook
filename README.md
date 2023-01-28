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