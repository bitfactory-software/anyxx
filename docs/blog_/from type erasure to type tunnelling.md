---
title: From "type erasure" to "type tunneling".
parent: blog
nav_order: 10
---

# "type erasure" to "type tunneling"

As we recognized the subtile differnce between "tyoe erasure" and "type tunneling", std::any came tou our mind. As simple at it is, it is nevertheless the prototypical "type tunnel".
That is also all you can do with it. But with some love, that can be a lot.

To show you a litte example, lets immagine a simple "DB-System". An application can register factories for "types".
The Types are identified through a str::string. The factory gets a second string paramter, containig serialized date for that type.

https://github.com/andreaspfaffenbichler/anypp/blob/master/test/05_Sink_TypeErased_w_any_cast.cpp

An application can attach a "file" to that DB-System. That is a list of pairs of strings. first cantains the type and second the data of a serialized object.
When the Application queries the known objects of the file, the db calls for each pair the fitting factory and passes afterward the constructed object to the application.
In this first implementation, we will use std::any to hold the deserialized object.
To access the real object, we must use any cast, to test if we know the tunneled type, and if so we get the object for further processing.

Just for illustration purposes we take also a short look on an OO-Style implementation of this scenario.
https://github.com/andreaspfaffenbichler/anypp/blob/master/test/03_Sink_Inheritance_modern.cpp

If we compare the two distict strategies, we see, the have the downcast pattern in common. 
Form our type tunneling perspective, we can say the inherritance, the v-table and the "dynamic_cast" work together to form a "type tunnel".
If we look from the aspect of coupling, we see a difference. All the application classes are coupled to the IAny any_base. But after the "upcast" to "Data", the v-table run time dispatch decouples the types in the application layer-
````
void ReportSink(const DB::IAny* any) {
  if (auto data = dynamic_cast<const Data*>(any))
    std::cout << data->ToString() << std::endl;
}
````
With std::any have no dependency in the classes, but we have a strong coupling in the function "ReportSink" to all know types in the Application. With the OO is only a depenency to the highest abstraction level "Data".
This seams fine, because we look at only one sink for one query. Are thre many query sinks, and you want stay at this patter, one virtual function will not be enough and you will add virtual functions proprtional to the different sink you hav to serve with "Data". 

From this perspective, that that resembles the [visitor pattern](https://en.wikipedia.org/wiki/Visitor_pattern).
Visitor pattern is notorious for discomfort.
Interestingly, for the any case, there is a solution, that jummps in your eye:
Do the dispatch via typeid!

The first naiive aproach will look like this:
[test/05_Sink_TypeErased_w_any_dispach_simple.cpp](https://github.com/andreaspfaffenbichler/anypp/blob/master/test/05_Sink_TypeErased_w_any_dispach_simple.cpp)

From the standpoint of decoupling is this the best you can get. But this comes with a large get_meta_data penalty...
Nevertheless, this aproach looks to appealing.
So will in the next step investigate, how big is the overhead we generate with map, function and any in comparison to plain v-tables.
For this we will use another example: An expression tree!
In OO-style ths goes like this:
https://github.com/andreaspfaffenbichler/anypp/blob/master/test/20_Tree_OO.cpp
We will use the expression tree to compute its value and to show a representation in forth and in lisp.
With a "catch" benchamrk we watch the get_meta_data performance.

Now to the sample with our [naiive any dispatch] (https://github.com/andreaspfaffenbichler/anypp/blob/master/test/21_Tree_TE_dispatch_via_any.cpp)

On my laptop, i got this number with standard releas mode and MSVC 17
```
-------------------------------------------------------------------------------
20_Tree_OO
-------------------------------------------------------------------------------
D:\BitFactory\Blog\anypp/test\20_Tree_OO.cpp(63)
...............................................................................

benchmark name                       samples       iterations    est run time
                                     mean          low mean      high mean
                                     std dev       low std dev   high std dev
-------------------------------------------------------------------------------
20_Tree_OO value                               100          2797     1.3985 ms
                                        4.56024 ns    4.52199 ns    4.63389 ns
                                       0.264757 ns    0.15925 ns   0.390666 ns

20_Tree_OO as_lisp                             100           148     1.5244 ms
                                        92.6486 ns    92.1149 ns    93.5676 ns
                                        3.48399 ns    2.35465 ns    6.28842 ns

-------------------------------------------------------------------------------
21_Tree_TE_dispach_via_any_dispatch
-------------------------------------------------------------------------------
D:\BitFactory\Blog\anypp/test\21_Tree_TE_dispatch_via_any.cpp(88)
...............................................................................

benchmark name                       samples       iterations    est run time
                                     mean          low mean      high mean
                                     std dev       low std dev   high std dev
-------------------------------------------------------------------------------
21_Tree_TE_dispach_via_any_dispatch
value                                          100           230      1.518 ms
                                        55.1478 ns    54.8478 ns    55.8826 ns
                                         2.2301 ns    1.13441 ns    4.53223 ns

21_Tree_TE_dispach_via_any_dispatch
as_lisp                                        100           113     1.5255 ms
                                        125.407 ns    123.372 ns    128.301 ns
                                        12.3265 ns    9.77748 ns    17.1408 ns
```

The value case shows the overhead cler. Not a surprise computers today are realy fast the in first rules of arithmetic, and the time neccessary to find the right function to call is a big part of the whole get_meta_data.
As soon as there is a little relevant work todo in the found function, the time neccessary for dispatch is a smaller fraction. 
The overhead goes down from ~1.400% to ~30%.

30% overhead is still a lot, but worth a consideration, if it could help to escape visitor pattern hell or to get a hot header file out of the build time bottleneck.
We have not even tryed to speed up the things, so that's what we have to do next!






  
