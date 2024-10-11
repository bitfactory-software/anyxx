---
title: Inherritance is the base class of all evil. Execpt if done right.
parent: blog
nav_order: 10
---

# Inherritance is the base class of all evil. Execpt if done right

If you have not yet seen Sean Parent talking about type erasure, do it now.

<img width="1685" alt="image" src="https://github.com/user-attachments/assets/da75c051-6a95-4ed3-ac54-ef8e56fc2ac0">
[Sean Parent on youtube](https://youtu.be/bIhUE5uUFOA?si=qa4nXpmz8FCNUoY0&t=577)

For all others, the title of this bolg post, should be clear.
As sharp as Parent hits against missuse of inherritance, he shows in the immense experssive power of this abstraction mechanism in the same example.

And paradoxically these power get lost in all usually shown examples for the applictaion of type erasure. They typically look like this:

Compiler explorer proxy sample for shapes.

We see some concrete objects are constructed direct at the call to an algorithm with an "type erasing" parameter type.
then the magic happens, inside the algorithm the type erasure run time dispatch takes care, that the corresponding function of concrete object is executed.
And here ends the the story.

And my sorrow begins. This pseudoecode example should show you why... 

```c++
proxy base
proxy derived

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
