[Terms and Definitions](/terms_and_definitions.md) / [Tutorials](/tutorials/tutorials_toc.md) / [Tests and Examples](/test/)

# virtual_void: The *type tunnel* vocabulary for *programming on a large scale*

*Please refer to [Terms and Definitions](/terms_and_definitions.md) to clarify the meaning of used catchphrases in the context of this library.*

## Building block vocabulary for *programming on a large scale*

- To hide the representation with lifetime handles: [Quick start](/tutorials/tutorial___1.md/#t1) [Overview](docs/erased_data_overview.md)
- To hide the functionality with: [Overview](docs/erased_functionality.md)
    - Interfaces: [Quick start](/tutorials/tutorial___1.md/#t4) [Operators](/tutorials/tutorial__33.md/#t1)
        - Derivable
        - Upcastable
        - Downcastable
    - Open methods: [Quick start](/tutorials/tutorial___1.md/#t2)
- Allow extendable types with open_objects: [Quick start](/tutorials/tutorial___2.md)
- To obtain types with runtime casts

To enable:

- Loose coupling and
- Performance on par with virtual function calls.

They work as a foundational vocabulary to *factor out* the dependencies between the components in the system.

The library is implemented in C++20. It can be used as a static or as a dynamic library.

The name of the library originates from its fundamental design idea:

- **Hide** the concrete **data** behind a (possibly *smart*) **pointer to void**,
- Pair this with a pointer to **static meta** data to interpret and **recover** the hidden data
- Use such pairs to **connect the concrete data** via **interfaces** and **open methods**

to separate usage and implementation details as much as possible.

This makes the pointer seem not *void* but rather **virtual void**.

## Programming on a large scale

### *Runtime* versus *compile-time* **dispatch**

[Dispatch](medium.com/ingeniouslysimple/static-and-dynamic-dispatch-324d3dc890a3) on behalf of a type is - like function calls - a fundamental software construction principle to allow the building of layered abstraction levels.

Static dispatch is the dispatch of choice in C++. Foremost because of performance and binary size.

But once a program exceeds a certain size, it is no longer possible to use static dispatch for everything.

Reasons are:
- You do not know all your types all the time
- You cannot use all your types all the time because of restricted build resources: time and space.

For example, imagine a std::variant for some hundred different alternatives.
This is where **programming on a large scale** begins and runtime dispatch is the proven means.

At this point arises another problem, when it comes to "inversion of control" in regard to the type of the parameter objects passed into the callback:
- In static dispatch, you have the full type information.
- In dynamic dispatch, the original type is usually abstracted away (either via a base class or "type erasure") and must be cast back to access more concrete information.

### *Runtime* versus *compile-time*

If you have a runtime performance problem, the hot runtime path and the causing bottleneck are easily diagnosed with profilers.
When the problem is spotted, the solution is - in our experience - not far away.
In contrast, when build time has grown to a problem, there is seldom an easy way out.
While a software system grows larger, keeping the edit-compile-run loop short requires constant smart decisions.
The fundamental design principles here are **physical loose-coupling** and **information hiding**.

### *Logical* versus *physical* coupling

A C++ `template` allows a **loose coupled logical design**, because the client can be **any** `typename`. 
It requests, in contrast, a **strong physical coupling** between the client and the template, because the serving template must be fully known on the client side.

The interception of this cascading coupling is the aim of this library.

### From *OO* to *type erasure* to *type tunneling*

C++'s virtual functions have shown their versatility and usefulness for decades.
But, as the understanding of decomposing software grows, we see the "OO style" integration of the interface directly into the object as a problem.

It is now consensus that separation of the interface from the underlying object is the way to go. The type gets "erased" from the interface.
On the contrary, by doing so, we lose this connection, which had shown its usefulness in countless huge and successful software projects.

A v-table interface of an object can be used:
- To cast back to that object itself
- To "downcast" the interface to a more specific one, and
- To "crosscast" to another interface.

A vocabulary for programming on a large scale must support those casts.
Because we found no library that fulfilled these demands, we came up with this one.

We call a *lossless type erasing* technique **type tunneling**.
To symbolize the "hide the type information" and bring it "back to light."

### Coupling and Separation

Tight coupling allows information to flow easily and enhances productivity. However, if the "things" coupled are fast-moving, the coupling becomes a burden.
So we want to separate the moving parts.

***Examples***:

You will, for sure, couple with the used programming language as deeply as possible (as long as the language promises backward compatibility) to exploit its benefits.

You probably want to separate from the details of the operating system for easier portability.

You surely also want to separate different aspects of your application because you know they will change, and you want to keep the changes as local as possible.

For the decision over using this library, one has to balance the benefits of separating internal dependencies versus the tight coupling into **virtual void**.
