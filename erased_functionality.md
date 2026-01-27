# Comparison of *erased functionality* in `virtual_void`

| Kind                    | REFERENCE:</br>`C++ virtual`      | `virtual_void`::</br>`any`   | `virtual_void`::</br>`open_method::via_type_info`                    | `virtual_void`::</br>`open_method::via_m_table`                                        |
|:-                                  |:- |:-               |:-                                                |:-                                                                  |
| meta data requirement              | Derived from any class | none            | `with_type_info` </br>`with_m_table`            | `with_m_table`                                                    |
| Runtime penalty |  1 |  1              | 2.5                                              | 1.3                                                                 |
| Memory Footprint</br>per function(f) and implementing type(t) [byte] | O(1) | O(1)           | O(1 + x) </br> Ø(x) ~ 0.3                         | O(1), but... [^1]      |
| static meta data                   | yes (v-table) | yes (v-table)   | no                                               | yes (m-table)                                                      |
| intrusive?             | Yes: All implementing classes must derive from a common any | no |  no| no | 
| coupling in comparison             | </br>- Type of any</br>- All implementing classes must derive from a common any | </br>- Type of any</br>- Type of lifetime holder | minimal(+) </br>- pointer to erased data</br>- typeinfo | minimal(+) </br>- pointer to erased data</br>- pointer to m-table  | 
| general boilerplate?          | no | no              | `seal_for_runtime()` must be called before usage.</br>Typically from `main` after all *.dll/so* are loaded. | no                                    |
| enforced implementation?          | yes | yes              | no, see [^2] | no, see [^2] |
| dynamic library **load** handling boilerplate?  | no | no              | `seal_for_runtime()` must be called after all *.dll/so* are loaded.  | no                                                                 |
| dynamic library **unload** _safe_ before 'normal' termination phase? | yes | yes           | not supported                                     | yes.                                                                |
| Alternatives | virtual_void or -> | </br>proxy, </br>Dyno, </br>AnyAny, </br>Zoo, </br>boost::type_erasure</br>- All have no downcast</br>- They can be faster because data and v-table are inlined | yomm2, a little faster</br>needs `this` with `v-table`. | ?                      |

[^1]: How m_tables work:
    - Every `open_method::via_m_table` has a unique index.
    - Every `class` defining an `open_method::via_m_table` has an `m_table` with an entry for every `open_method`.
    - A pointer to the `m_table` must be passed to invoke the `open_method`.
    - The function called is `m_table[method_index]`.

[^2]: If for an invoking type no target is found, the `default_target` gets called. Its default is to throw an exception. This can be overridden by the `define_default(...)` member function of the `open_method`. If it is not possible, there is an algorithm `open_method::interpolate`. This algorithm uses the meta data of the classes registered with `declare_classes` to set the target to the implementation of the *nearest* any class with a defined function. The fallback is again the `default_target`.
