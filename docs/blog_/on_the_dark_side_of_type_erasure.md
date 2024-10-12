---
title: as we came on the dark side of type erasure
parent: blog
nav_order: 1
---

# as we came on the dark side of type erasure

<img width="920" alt="image" src="https://github.com/user-attachments/assets/65f7f175-08d6-4c5e-b3e1-7cb45a59779b">

If you have not yet seen [Sean Parent talking about type erasure](https://www.youtube.com/watch?v=_BpMYeUFXv8), do it now.



Let us short recap the quintesence of this example. To eliminate the boilerplat code, we will use "proxy". This is the "type erasure library roposed for inclusion in c++26:

Compiler explorer proxy sample for shapes.

Some objects are constructed direct at the call site to an algorithm. 
This algorithm has a "type erasing" parameter "drawable".
The magic happens inside of the algorithm. 
The "type erasure run time dispatch", provided by proxy, takes care, that the corresponding function of concrete object is executed.

Wes see, that 
- the implementation of the inteface ("drawable") used by the algorithm is not tied to the implementation of the conrete type ("circle", "spare", .. ), 
and
- the implementation of the interface itself can be very generic by use uf clever template tricks.

These features are intruding.
So it is understanding, that type erasure is the new cool thing in regards to runtime dispatch. 
This reaches so far, that new languages, like 'rust' go full in on that idea and dissmiss the idea of inherritance as a whole.

The key messeage we get told is: Programming along classes utilizing the conventional v-table is old school and outdated. 
And here ends the the usual story. 

We where convinced too. 
So we tried to apply this techniqe to an aspect of our codebase.  
Simplified, we had this structure

```c++
class Base
{
public:
	virtual std::string ToString() const;
private: // data
};

class Dereived : Base
{
public:
	std::string ToString() override;
	int GetValue();
private: // more data
};
```

For Derevid, we wanted to seperate the implementation from the interface, becuause in many important cases, the usual implemetation of derived was too heavy.
We would like to came up with an design that looked something like that (in terms of 'proxy'):

```c++
class Base
{
public:
	std::string ToString() const;
private: // data
};

class Dereived : Base
{
public:
	int GetValue();
private: // more data
};

class DereivedLigthweight
{
public:
	std::string GetValue();
	int Scope();
private: // nearly no data
};

proxy BaseProxy ToString()
proxy DerivedProxy : BaseProxy + GetValue()
```

Until here it worked all fine. Untill we realized: Our system does not only consumes input:

```c++
proxy< base > build for int value() const;
proxy derived buid for std::string to_string() const;

base f1( base a, base b, std::function< bool( base ) > ); // do somethin clever and then select the return type

void f2( derived a, derived b )
{
  auto xb = f1( a, b, []( base x ){  return i_need_a_downcast_to< derived >( x ).value() > 3.14; };
  auto xd = i_need_a_downcast_to< derived >( xb ):
  cout << xd.value() << "\n";
}

int main()
{
  f2( derived( 1 ), derived( 2 ) );
  return 0;
}
```

The called functions
- filter for the input with predicates (callback)
and
- the have to return results

But because we have erased away ALL **type** informatiom, we have no longer access to the data we need, to 
- answer the questions asked to the predicate functions via callbcks
and
- to continue with our processing, we need the full interface we passed into the functions.

So we resorted to old some school unsexy OO-Style template mixture to solve this particular riddle:

```c++
struct IBase
{
	virtual std::string VirtualToString() const = 0;
	std::string ToString() const { return ToString(); }
};
struct IDerived : IBase
{
	virtual int VirtualGetValue() const = 0;
	int GetValue() const { return VirtualGetValue(); }
};

template< typename MODEL >
struct IBaseModel
{
	const MODEL* Model() const { return static_cast< const MODEL* >( this ); }
	virtual std::string VirtualToString() const ;
};

template< typename MODEL >
struct IDerivedModel : IBaseModel< MODEL >
{
	const MODEL* Model() const { return static_cast< const MODEL* >( this ); }
	int GetValue() const ;
};

class BaseImpl // same as original "Base"
{
public:
	std::string ToString() const override;
private: // data
};

class Base : public BaseImpl, IBaseModel< Base >
{
};

class Dereived : public BaseImpl, IDerivedModel< Dereived >
{
public:
	int GetValue() const;
private: // more data
};

class DereivedLigthweight : public BaseImpl, IDerivedModel< DereivedLigthweight >
{
public:
	std::string GetValue();
	int Scope();
private: // nearly no data
};

```

That worked.
But we saw, there is a lot of room for improvement.
We saw also, there is a pattern, that shows a general flaw in the concept of "type erasure" as we understood it.
We called that pattern the "2nd order type erasure problem".
This pattern can be reduced to this code lines.

```c++

proxy< base > build int func1() const;
proxy< derived > : base + int func2() const;

struct S
{
	int func1() const { return 1; };
	int func2() const { return 2; };
};

base f1( base b ) { return b; }

derived f2( derived d ) { return erased_2n_order_problem_downcast< derived >( xb ); }

int main()
{
  cout << f2( S{} ).func2() << "\n";
  return 0;
}
```

Next time, we will show, how we tackeled that problem on its root.

