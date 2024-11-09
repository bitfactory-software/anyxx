[Terms and Definitions](/docs/terms_and_definitions.md) / [Tutorials](/tutorials/tutorial___1.md) / [Tests and Examples](/test/)

# virtual_void : The *type tunnel* vocabulary for programming in the large

This is a library with building blocks for large software systems:

- lifetimehandles
- interfaces 
  - deriveable
  - upcastable
  - downcastable
- open methods
- object casts
- open types

They offer

- loose coupling and
- performance on par with virtual function calls.

They work as a foundating vocabulary, to *factor out* the dependencies between the components in the system.  

The library is **header only** 

The name of the library originates in its fundamental design idea:

- **hide** the concrete **data** behind a (possible *smart**) **pointer to void**,
- pair this with a pointer to **static meta** data to interprete and **recover** the hidden data
- use such pairs to **connect the concrete data** via **interfaces** and **open methods**

to seperate usage and implementation details as much as possible. 

This makes the pointer seeming not *void* rather **virtual void**. 

## Programming in the large

### *Runtime* versus *compiletime* **dispatch**

Dispatch on behalf of a type is - like function calls - a fundamental software construction principle to allow the building of layerd abstraction levels.

Static dispatch is the dispatch of chioce in C++. Foremost because of performance and binary size.

But eceeds a program a certain size, it is no longer possible to use static disapch for all and everything. 

Reasons are:
- you do not know all your types all the time
- you can not use all your types all the time because of restricted build resources: time and space.

For example, imagine a std::variant for some hundred different alternatives.
This is where **programming in the large** begins and runtime dispatch is the proven means.

At this point arises another problem, when it comes to "inversion of control" in regard to the type of the parameter objectds passed into the callback:
- In static dispatch, the have usually the full type information.
- In dynamic dispatch, the original type is usualy abstracted away (either via a base class or "type erasure") and must be casted back to acces more concrete inforamtion.

If the callback would not need more concrete (type) information abount the passed in paramter, there would be no need for inversion control.

### *Runtime* versus *compiletime*

In the general case, the "hot" runtime path and the bottleneck are easy diagnosed with profilers.
When the problem is spottet, the soultion is - so our expirience - not so far away.  
In contrary, when build time has grown to a problem, there is seldom an easy way out. 
While a software system grows lager, keeping the edit-compile-run loop short, needs constant smart decisssions.
The fundamental design principals here are **loose-coupling** and **information hiding**.

### From *OO* to *type erasure** to *type tunneling**

C++'s virtual functions have shown their versaitilty and usefullness for decades.
But, as the understanding for decomopising software grews, we see the "OO style" integration of the interface direct into the object as a problem.

It is now consens, that separation of the interface from the underlying object is the way to go. The type gets "erased" from the interface.
On the contrary, by doing so, we lose this connection, whitch had shown is usefullnes in a countless homongos and succesful software projects.
So can a v-table interface of an object to be used
- to cast back to that object itself
- to "downcast" the interface to a more specific one, and
- to "crosscast" to an other interface.

A vocabulary for programming in the large to must support those casts.
Because we found no library, which fullfilled this demands, we came up with this one.

We call a *lossless type erasing** technique **type tunneling**.
To symbolize the "hide the type information" and bring it "back to light".

### Coupling and Separation

Thight coupling allows information easy to flow and enhances productivity. Contrary, if the "Things" coupled are fast moving, the coupling gets a burden. 
So we want to separate the moving parts.

***Examples***:

You will for sure couple with the used Progrmming Language as deep as possible to exploit its benefit. 

You probably want to seperat from the details of the operating system, to easy portation.

You surly also want to separate different aspects of your application, because you know, they will change, and yout want to keep the changes as localy as possible.

For the decission over using this library, once have to ballance the benefits of sepaerating internal dependencies versus the tight coupling into **virtual void**.




