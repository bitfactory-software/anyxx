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

https://github.com/andreaspfaffenbichler/virtual_void/blob/master/test/05_Sink_TypeErased_w_any_cast.cpp

An application can attach a "file" to that DB-System. That is a list of pairs of strings. first cantains the type and second the data of a serialized object.
When the Application queries the known objects of the file, the db calls for each pair the fitting factory and passes afterward the constructed object to the application.
In this first implementation, we will use std::any to hold the deserialized object.
To access the real object, we must use any cast, to test if we know the tunneled type, and if so we get the object for further processing.

Just for illustration purposes we take also a short look on an OO-Style implementation of this scenario.
https://github.com/andreaspfaffenbichler/virtual_void/blob/master/test/03_Sink_Inheritance_modern.cpp

If we compare the two distict strategies, we see, the have the downcast pattern in common. 
Form our type tunneling perspective, we can say the inherritance, the v-table and the "dynamic_cast" work together to form a "type tunnel".
If we look from the aspect of coupling, we see a difference. All the application classes are coupled to the IAny base. But after the "upcast" to "Data", the v-table run time dispatch decouples the types in the application layer-
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




  
