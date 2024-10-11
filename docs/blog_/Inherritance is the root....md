---
title: Inherritance is the base class of all evil. Execpt if done right.
parent: blog
nav_order: 10
---

# Inherritance is the base class of all evil. Execpt if done right

If you have not yet seen [Sean Parent talking about type erasure](https://youtu.be/bIhUE5uUFOA?si=qa4nXpmz8FCNUoY0&t=577), do it now.

The quintesential scene is captured her, to clarify this blog post title:
<img width="1670" alt="image" src="https://github.com/user-attachments/assets/29ccd00e-cbdb-4ff1-bc18-174e8c46c061">

As sharp as Parent hits against missuse of inherritance, he shows in the immense experssive power of this abstraction mechanism in the same example.
Lets short  recap the quintesence of this example in terms of prox. The type erasure library on the way to c++26:

Compiler explorer proxy sample for shapes.

Some concrete objects are constructed direct at the call to an algorithm with an "type erasing" parameter type.
then the magic happens, inside the algorithm the type erasure run time dispatch takes care, that the corresponding function of concrete object is executed.

Wes see, that 
- the implementation of the inteface ("drawable" to use the algorithm is not tied to the implementation of the conrete type ("circle", "spare", .. ), and
- the implementation of the interface itsel can be very general bey use uf clever template tricks ("generic" ;-) )

And so the conclusio, ...

And here ends the the story. 


And my sorrow begins. This example should show you why... 

```c++
proxy< base > build for int value() const;
proxy derived buid for std::string to_string() const;

base f1( base a, base b ) { return  a.value() > b.value() ) ? a : b; }

void f2( derived a, derived b )
{
  auto xb = f1( a, b );
  auto xd = i_need_a_downcast_to< derived >( xb ):
  cout << xd.to_string() << "\n";
}

int main()
{
  f2( derived( 1 ), derived( 2 ) );
  return 0;
}
```

When we generalize our observations, we can say, that the expressive power, Sean shows us in his talk, get lost in the usually examples for the applictaion of type erasure.
