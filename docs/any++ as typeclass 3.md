\page subpage3 Concept Maps using C++23 Library Tech, Steve Downey

In 2023, Steve Downey gave a [talk at CppNow](https://www.youtube.com/watch?v=H825py1Jgfk) 
about [Concept Maps using C++23 Library Tech](https://sdowney.org/posts/index.php/2024/05/19/concept-maps-using-c23-library-tech/). 
In this talk, he used the Monoid type class as an example.

This is fine, because I only need to copy/paste his code and comment on it to contiue our discussion on the topic ;-)
To be clear once again, we are here aiming at complie time type checking and default implementations, not at runtime polymorphism, which is the other main focus of Any++!

So here is his implementation of the Monoid type class:
```cpp
template <typename T, typename M>
concept MonoidRequirements =
    requires(T i) {
      { i.identity() } -> std::same_as<M>;
    }
    ||
    requires(T i, std::ranges::empty_view<M> r1) {
      { i.concat(r1) } -> std::same_as<M>;
    };

template <class Impl>
  requires MonoidRequirements<
      Impl,
      typename Impl::value_type>
struct Monoid : protected Impl {

  auto identity(this auto&& self) {
    return self.concat(std::ranges::empty_view<typename Impl::value_type>{});
  }

  template <typename Range>
  auto concat(this auto&& self, Range r) {
    return std::ranges::fold_right(
      r, self.identity(),
      [&](auto m1, auto m2){return self.op(m1, m2);});
    }

  auto op(this auto&& self, auto a1, auto a2) {
    return self.op(a1, a2);
  }
};
```
That is the complete implementation of the Monoid type class, and is a good expressin of the indent.
Is also real close to the Haskell and Rust implementations, and it is pretty close to the original C++0x proposal for Concept Maps.

Now let us see, how Steve implemnts the mapping of a concrete type to the Monoid type class.

He starts with a general case for all types, that have an monoid "plus" operator and a default constructor, which is the identity element for the monoid:
```cpp
template <typename M>
class Plus {
public:
  using value_type = M;
  auto identity(this auto&& self) -> M {
    std::puts("Plus::identity()");
    return M{0};
  }

  auto op(this auto&& self, auto s1, auto s2) -> M {
    std::puts("Plus::op()");
    return s1 + s2;
  }
};

template<typename M>
struct PlusMonoidMap : public Monoid<Plus<M>> {
    using Plus<M>::identity;
    using Plus<M>::op;
};
```
This technique leaves an other customization point for the user, which is the choice of the operator and the identity element. 
If the user wants to use a different operator, or a different identity element, they can simply define a different mapping for their type by specializing the ``Plus`` class for their type.
Now he defines the mapping for concrete types. To this end he uses template variables, which are a C++17 feature, to define the mapping for all types
he wants to be a Monoid. In this case, he defines the mapping for ``int``, ``long`` and ``char``.
```cpp
template<class T> auto monoid_concept_map = std::false_type{};

template<>
constexpr inline auto monoid_concept_map<int> = PlusMonoidMap<int>{};

template<>
constexpr inline auto monoid_concept_map<long> = PlusMonoidMap<long>{};

template<>
constexpr inline auto monoid_concept_map<char> = PlusMonoidMap<char>{};
```
Note the use of ``std::false_type`` as the default value for the mapping, which is a way to indicate that a type has no mapping as ``monoid``.
To show, that instead of the identity element, the concatenation operation could be used to define the mapping, he also defines a mapping for ``std::string`` using the concatenation operation:
```cpp
class StringMonoid {
public:
  using value_type = std::string;

  auto op(this auto&&, auto s1, auto s2) {
    std::puts("StringMonoid::op()");
    return s1 + s2;
  }

  template <typename Range>
  auto concat(this auto&& self, Range r) {
    std::puts("StringMonoid::concat()");
    return std::ranges::fold_right(
        r, std::string{}, [&](auto m1, auto m2) {
          return self.op(m1, m2);
        });
  }
};
struct StringMonoidMap : public Monoid<StringMonoid> {
    using StringMonoid::op;
    using StringMonoid::concat;
};
template<>
constexpr inline auto monoid_concept_map<std::string> = StringMonoidMap{};
```
Now we can use the Monoid type class with its maping: 
```cpp
template<typename P>
void testP()
{
    auto d1 = monoid_concept_map<P>;

    auto x = d1.identity();
    assert(P{} == x);

    auto sum = d1.op(x, P{1});
    assert(P{1} == sum);

    std::vector<P> v = {1,2,3,4};
    auto k = d1.concat(v);
    assert(k == 10);
}

int main() {
   std::cout << "\ntest int\n";
   testP<int>();

   std::cout << "\ntest long\n";
   testP<long>();

   std::cout << "\ntest char\n";
   testP<char>();

   auto d2 = monoid_concept_map<std::string>;

   std::cout << "\ntest string\n";
   auto x2 = d2.identity();
   assert(std::string{} == x2);

   auto sum2 = d2.op(x2, "1");
   assert(std::string{"1"} == sum2);

   std::vector<std::string> vs = {"1","2","3","4"};
   auto k2 = d2.concat(vs);
   assert(k2 == std::string{"1234"});
}
```
[See it on Compiler Explorer](https://godbolt.org/z/Yh5rY43Eq)

That is a pretty good technique for complie time customization points.
But how good is it compared to other C++ compile time customization point techniques, like SFINAE, tag dispatching, or specialization and the original C++0x proposal for Concept Maps?
Here we can take look into Barry Revzin proposal "We need a language mechanism for customization points" [P2279R0](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2021/p2279r0.html#c0x-concepts) 
and use his framework for comparing different techniques for compile time customization points.

<img width="736" height="331" alt="image" src="https://github.com/user-attachments/assets/7ed7c557-3378-4e09-a884-ccb3bde54c9f" />







