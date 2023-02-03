# CppCookbook
Cpp Cookbook / Playground for the concept's I like to remember.

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

#### Why use r-values?
r-value references expresses intent that the object being passed is **disposable**. So when it is being passes as r-value ref, it means passer no longer cares about it. They're used to improve performance and take ownership.
```c++
void func(const int& lref);
void func(const int&& rref);
int x{11};
func(x); // calls 1st
func(5); // calls 2nd
int &&ref{ 5 };
func(ref); // calls 1st !!
func(std::move(ref)); // calls 2nd
```

> **Warning**
> A named r-value is an l-value, since all named objects are l-values. Only temporary/anonymous objects are r-values.

```c++
void func(std::vector<int>&& vec) // vec is r-value ref
{ //but here vec is l-value since it designates the name of object
  // '&&' in func argument only implies its ok to dispose of object
  std::vector newVec = std::move(vec);
  // explicity use move, o/w it gets copied.
}
```

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

### typename & value_type
- Apart from defining templated funcs/classes, typename keyword is used to access nested and dependent types related to the templated class.
 ```c++
class MyClass
{
public:
    struct MyStruct{
        int sval{3};
    }
    MyStruct val;
};

template <typename T> 
void func(const T& obj)
{
    T::MyStruct value1 = obj.val; // ERROR: Compiler might think T::MyStruct is a static member.
    typename T::MyStruct value2 = obj.val; // OK
}
```
- `value_type` is mainly used to retrieve the type of data stored in the containers. It's useful when we want to define a function that involves templated containers.
```c++
template <typename Cont>
typename Cont::value_type sumElements(const Cont& cont){
    typename Cont::value_type sum = typename Cont::value_type();
    for(const auto& val: cont)
        sum += val;
    return sum;
}
// ...
auto sumList = sumElements(std::list{1,2,3,4}); // 10
auto sumQueue = sumElements(std::dequeue{1.1,2.2,3.3,4.5}); // 11.1
```
Note that `typename Cont::value_type()` is calling the default constructor of the type held in container.

### enable_if & SFINAE
- *SFINAE* stands for 'Substitution Failure Is Not An Error'. It means if the compiler fails to substitute a certain type during specialization, it will not cause a compile error **if another successful substitution is found.**
```c++
int negate(int i) {
  return -i;
}

template <typename T>
typename T::value_type negate(const T& t) {
  return -T(t);
}
// ...
auto neg = negate(42);
```
For the templated `negate`, compiler considers the substitution `int::value_type negate(const int& t);`, and sees that its invalid since `int` has no `value_type`. However since 1st version of `negate` fits into `negate(42)` usage, we get no error.

- `enable_if` is mainly used to restrict templates to types that have certain properties. 
Without enable_if, catch-all property of templates might lead to undesired behavior:
```c++
template <typename T>
class vector {
public:
    vector(size_t n, const T val);

    template <class InputIterator>
    vector(InputIterator first, InputIterator last);
};
// ...
vector<int> vec(4,8);
// calls 2nd version since 1st constructor would need a cast from int(4) to size_t
// whereas 2nd one fits perfectly with no type conversion.
```
`enable_if` returns the type `T` if condition it's provided with returns true.
```c++
template<typename T, typename = typename std::enable_if<std::is_integral<T>::value, T>::type>
T add_one(T value) {
    return value + 1;
}
// ...
add_one(1); // OK
add_one(5.0); // error since 5.0 is not integral type
```