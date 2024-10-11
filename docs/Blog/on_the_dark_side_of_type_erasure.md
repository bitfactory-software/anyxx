---
title: on the dark side of type erasure
nav_order: 3.1
---

# on the dark side of type erasure

on the dark side of type erasure

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
      
