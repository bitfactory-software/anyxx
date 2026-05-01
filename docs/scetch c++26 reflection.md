### External Polymorphism in C++26

While developing our Type Erasure library [Any++](https://github.com/bitfactory-software/anyxx) for C++23, I had to resort to a preprocessor-based EDSL to eliminate the boilerplate.

While studying the "C++26 Reflection Proposals," I was constantly searching for a way to replace this preprocessor programming.

Implementing Type Erasure requires three components:

- A "V-Table" for indirecting function calls.

- A "Facade" for the ergonomic connection between the data and the "V-Table."

- An "Adapter" to connect the functions of the "V-Table" to the specific type.

It suffices to describe one of these components. The other two will then be generated automatically.

One way to do this with C++26 is to code the default adapter and generate the V-table and facade.

C++26 allows you to generate a class using `define_aggregate`.

The key is that the class can only contain data members.

However, since a data member can have an `operator()`, member functions can also be simulated this way.

These data members can be specified so that they don't occupy any memory. This allows you to access the enclosing class in the `operator()` and use the information it manages (the V-table and the data reference).

Interestingly, this method also enables static dispatch with an elegant interface.

To coincide with the GCC16 release and its excellent reflection implementation, I've sketched out such an API:

```C++
template <typename Self>
struct stringable{
    [[=default_{}]]
    static std::string as_string(Self const& self);
};

void print(std::vector<dyn<stringable>> const& things){
    for(auto& thing : things){
        std::println("{}", thing.as_string());
    }
}

template <>
struct stringable<int>{
    static std::string as_string(int const& self) {
        return std::to_string(self);
    }
};

template <>
struct stringable<std::string>{
    static std::string as_string(std::string const& self) {
        return self;
    }
};

struct foo{ double f; };
template <>
struct stringable<foo>{
    static std::string as_string(foo const& self){
        return "foo: " + std::to_string(self.f);
    }
};

struct boo { 
    bool b = false; 
    std::string as_string(){
        return std::string{"boo? "} + (b ? "T" : "F"); 
    }
};


int main(int argc, char *argv[]) {

    // static dispatch
    auto a1 = trait_as<int, stringable>{{42}};
    auto z_from_self = a1.as_string();
    std::println("z_from_trait = {}", z_from_self);

    // dynamic dispatch, reference semantics only
    int i = 4711;
    auto dyn_stringable = dyn<stringable>{i};
    auto z_from_dyn_stringable = dyn_stringable.as_string();
    std::println("z_from_dyn_stringable = {}", z_from_dyn_stringable);

    std::string s = "hello world";
    foo a_foo{3.14};
    boo a_boo{true};
    print({dyn<stringable>{i}, dyn<stringable>{s}, dyn<stringable>{a_foo}, dyn<stringable>{a_boo}});
}
```
[Compiler Explorer](https://godbolt.org/z/vbMbq9dcP)

This is, of course, just a rough outline. The real value of a type erasure library lies in providing additional runtime capabilities (downcast, crosscast), lifetime mechanisms (shared, unique, value, etc.), and constant correctness.

Any++ provides all of this. As soon as Clang and MSVC also offer reflection, I will implement the presentated technique there.

For now, I have to thank the wizards who created this technical marvel: "C++ compile-time reflection"!

