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
class Foo
{
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
