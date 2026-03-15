\page subpage2 Monoid, the "Hello World" of type classes

> The Monoid is a simple algebraic structure that consists of a set of elements, an associative binary operation, and an identity element.<br>
It is a fundamental concept in abstract algebra and has many applications in computer science, especially in functional programming.

If you have ever heard of the term "type class", you have probably heard of the Monoid as well and you may skip this section. 

But if you are new to the concept of type classes, the Monoid is a great example to start with, because it is simple and easy to understand and because 
it is inherrent self refernetial, it shows the obstacles an implementation of type has to overcome.

Lets start with  a colloquial definition of the Monoid:
When a type wants to be a Monoid, 
1. it has to provide a way to combine two values of that type that result in an other value of the same type. This operation is called the binary operation.<br>
Often the symbol "<>" is used to denote the binary operation, but we will use the symbol + for simplicity.
2. it must have a value that can be used with the binary operation and the result is the other value. This value is called the identity element.
3. the binary operation must be associative, which means that the order of operations does not matter.<br>
For example, if we have three values a, b, and c of a Monoid type, then (a + b) + c must be equal to a + (b + c).
4. Haskell adds a fourth requirement, which is redundant, the concatenation operation. It states that if we have a list of values of a Monoid type, then we can combine them all using the binary operation and the identity element. 
For example: if we have a list of values [a, b, c] of a Monoid type, then we can combine them all using the binary operation and the identity element like this: a + b + c + identity.
This requirement is redundant because it can be derived from the first three requirements. If we have a binary operation that is associative and has an identity element, then we can combine any number of values using the binary operation and the identity element.
It is included in Haskell for convenience and performance reasons that do not matter for our discussion here.

The Hskell definition of the Monoid type class looks like this:
```haskell
class Semigroup a => Monoid a where
  mempty :: a
  mempty = mconcat []

  mappend :: a -> a -> a
  mappend = (<>)

  mconcat :: [a] -> a
  mconcat = foldr mappend mempty
```
We can see that the Monoid type class inherits from the Semigroup type class, which is a type class that only requires the binary operation. 
This is for our discussion here not relevant, but it is a common pattern in Haskell to have a hierarchy of type classes, where each type class inherits from one or more other type classes.
mempty is the identity element, mappend is the binary operation, and mconcat is the concatenation operation.
``mapapend`` requires the binary operation.
``mempty`` and ``mconcat`` have default implementations, witch reference each other. ``mconcat`` is defined in terms of ``mappend`` and ``mempty``, and ``mempty`` is defined as the result of concatenating an empty list.
For example, if a type provides an implementation for mappend and mempty, then it can use the default implementation for mconcat, which is defined in terms of mappend and mempty.
This is a powerful feature of type classes, because it allows to define a lot of functionality for a type class, wher a concrete type only has to provide a few implementations to get all the other functionality for free.

In Rust , the Monoid trait looks like this:
```rust
trait Monoid {
    fn empty() -> Self { Self::concat(vec![]) }

    // 2. You MUST implement this (the binary operation)
    fn append(self, other: Self) -> Self;

    fn concat<I: IntoIterator<Item = Self>>(iter: I) -> Self 
    where 
        Self: Sized 
    {
        // This relies on empty() as the starting point for the fold
        iter.into_iter().fold(Self::empty(), Self::append)
    }
}
```
[See a complete example on Compiler Explorer](https://godbolt.org/z/rnh9azK9K)
We can see that the Monoid trait with it's three requirements, which are the same as the Haskell type class.
The identity element is provided by the empty() function, the binary operation is provided by the append() function, and the concatenation operation is provided by the concat() function, which has a default implementation that relies on the empty() function as the starting point for the fold.
''IntoIterator'' is a trait that allows to convert a type into an iterator, which is a type that can be used to iterate over a sequence of values.
In the case of an self referential type class like the Monoid, you can not use this trait as a type, because it is not object safe. 

How far can we get there with standrd C++23?

Gracyfully to Steve Downey, we can get pretty far, let's see his approach to \ref subpage3 "C++0x Concepts with Concept Maps using C++23 Library Tech" in the next chapter.


