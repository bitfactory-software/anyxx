---
title: From "type erasure" to "type tunneling".
parent: blog
nav_order: 10
---

# "type erasure" to "type tunneling"

As we recognized the subtile differnce between "tyoe erasure" and "type tunneling", std::any came tou our mind. As simple at it is, it is nevertheless the prototypical "type tunnel".
That is also all you can do with it. But with some love, that can be a lot.

To show you a litte example, lets immagine a simple "DB-System". An application can register factories for "types". The Types are identified through a str::string. The factory gets a second string paramter, containig serialized date for that type.
An application can attach a "file" to that DB-System. That is a list of pairs of strings. first cantains the type and second the data of a serialized object.
When the Application queries the known objects of the file, the db calls for each pair the fitting factory and passes afterward the constructed object to the application.
In this first implementation, we will use std::any to hold the deserialized object.
To access the real object, we must use any cast, to test if we know the tunneled type, and if so we get the object for further processing.

Just for illustration purposes we take also a short look on an OO-Style implementation of this scenario.

If we compare the two distict strategies, we see, the have the downcast pattern in common. 
Form our type tunneling perspective, we can say the inherritance, the v-table and the "dynamic_cast" work together to form a "type tunnel".
If we look from the aspect of coupling, we see a difference. All the application classes are coupled to the IAny base. 
std::any implies not this dependency.
In both examples is a close coupling to all the types in the sink of the query.
At a closer look, that resembles a visitor pattern.
Visitor pattern are notorious for the requrements on coupling.
Interestingly, for the any case, there is a solution, that jummps in your eye:
Dispatch via typeid!



  
