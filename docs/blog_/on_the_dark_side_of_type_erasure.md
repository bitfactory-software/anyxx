---
title: as we came on the dark side of type erasure
parent: blog
nav_order: 1
---

# as we came on the dark side of type erasure

<img width="920" alt="image" src="https://github.com/user-attachments/assets/65f7f175-08d6-4c5e-b3e1-7cb45a59779b">

If you have not yet seen [Sean Parent talking about type erasure](https://www.youtube.com/watch?v=_BpMYeUFXv8), do it now.



Lets short  recap the quintesence of this example in terms of "proxy", the type erasure library roposed for inclusion in c++26:

Compiler explorer proxy sample for shapes.

Some concrete objects are constructed direct at the call to an algorithm with an "type erasing" parameter type.
Then the magic happens, inside the algorithm the type erasure run time dispatch takes care, that the corresponding function of concrete object is executed.

Wes see, that 
- the implementation of the inteface ("drawable" to use the algorithm is not tied to the implementation of the conrete type ("circle", "spare", .. ), and
- the implementation of the interface itsel can be very general bey use uf clever template tricks ("generic" ;-) )

These features are intruding. So it is understanding, that type erasure is the new cool thing in down, when it comes to runtime dispatch. 
This reaches so far, that new languages, like 'rust' go full in on that idea and dissmiss the idea of inherritance as a whole.

The key messeage we get told is, that programming along classes utilizing the conventional v-table is old school and outdated. 
And here ends the the usual story. 

We where convinced to. So we tried this techniqe to an aspect of our codebase.  
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

proxy BaseProxy GetValue()
proxy DerivedProxy : BaseProxy Scope()
```

Until here it worked all fine. Untill we realized, our system not only consumes input:

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
- filter for the input with predicates, and
- return results

But because we have erased away ALL **type** informatiom, we have no longer access to the data we need, to 
- answer the questions asked to the predicate functions, and
- continue with our processing.

So in this case we resorted to old some school unsexy OO-Style template mixture to solve the riddle:

```c++
struct IBase
{
	virtual std::string VirtualToString() const = 0;
	std::string ToString() const { return ToString(); }
};
struct IDerived : IBase
{
	virtual int VirtualGetValue() const = 0;
	std::string GetValue() const { return VirtualGetValue(); }
};

template< typename MODEL, typename INTERFACE >
struct IBaseModel
{
	const MODEL* Model() const { return static_cast< const MODEL* >( this ); }
	virtual std::string VirtualToString() const ;

};


class Base
{
public:
	std::string ToString() const override;
private: // data
};

class Dereived : Base
{
public:
	std::string ToString() const override;
	int GetValue() const override;
private: // more data
};

class DereivedLigthweight
{
public:
	std::string GetValue();
	int Scope();
private: // nearly no data
};

proxy BaseProxy GetValue()
proxy DerivedProxy : BaseProxy Scope()
```

When we generalize our observations, we can say, that the expressive power, Sean shows us in his talk, get lost in the usually examples for the applictaion of type erasure.


# on the dark side of type erasure

  what we see…
  
    void algorithm( ersased );
    
    boundary caller-callee
    
    container of  erased
    
  to Show 
  
    loose coupling
    
    no inherritance dependency imposed
    
    type erasure is the new runtime polymorhism kid in the down
    
    also adopted by rust ("traits")
    
  what is often implied
  
    (between the lines or loud)
    
    classes with "conventional" v-tables  are somewhat old school and outdatetd
    
  lets ummarize, what we can do with v-tables…
  
    Control Lifetime via virtual destructor
    
    do superfast runtime dispatch
    
    dynamic/static downcast
    
    dynamic typesafe crosscast
    
  type erasure as we  see in the exaples 
  
    Shines at 1 + 2
    
    pondering silence
    
    if you think, dynamic_cast's are a code smell
    
      please give me one more Minute, to give you my Point
      
      ersased  = algorithm( ersased  );
  
  but 
  
    so, we need it!
    
      same in rust ( https://github.com/rust-lang/rust/issues/65991 )
        
    to do all we can do with v-tables
    
    so type erasure can be a fundamental Building block for large scale Software
    
      what is "large scale Software"?
      
      when you need runtime dispatch for Inversion of Control, because the compiler can not know all your types everywhere.
      
      when you do not know all the interfaces a type supports everywhere
      
    like v-tables
    
    only better
    
  so
    
    let us explore…
    
    what are typical Problems?
    
    what could a soultion look like?
    
      Show samlpe ?
      
      interesting?  next time more!
      
