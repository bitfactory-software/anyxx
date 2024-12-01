# Comparison of *erased functionality* in `virtual_void`

| Kind                    | REFERENCE:</br>`C++ virtual`      | `virtual_void`::</br>`interface`   | `virtual_void`::</br>`open_method::via_type_info`                    | `virtual_void`::</br>`open_method::via_m_table`                                        |
|:-                                  |:- |:-               |:-                                                |:-                                                                  |
| meta data requirement              | Derived from base class | none            | `with_type_info` </br>`with_m_table`            | `with_m_table`                                                    |
| Runtime penalty |  1 |  1              | 2.5                                              | 1.8                                                                 |
| Memory Footprint</br>per function(f) and implementing type(t) [byte] | O(1) | O(1)           | O(1 + x) </br> Ø(x) ~ 0.3                         | O( `all domains` + `methods_in_domain`)[^1]      |
| static meta data                   | yes (v-table) | yes (v-table)   | no                                               | yes (m-table)                                                      |
| intrusive?             | Yes: All implementing classes must derive from a common base | no |  no| no | 
| coupling in comparison             | </br>- Type of interface</br>- All implementing classes must derive from a common base | </br>- Type of interface</br>- Type of lifetime holder | minimal(+) </br>- pointer to erased data</br>- typeinfo | minimal(+) </br>- pointer to erased data</br>- pointer to m-table  | 
| general boilerplate?          | no | no              | `seal_for_runtime()` must be called before usage.</br>Typically from `main` after all *.dll/so* are loaded. | no                                    |
| dynamic library **load** handling boilerplate?  | no | no              | `seal_for_runtime()` must be called after all *.dll/so* are loaded.  | no                                                                 |
| dynamic library **unload** _safe_ before 'normal' termination phase? | yes | yes           | not supported                                     | yes.                                                                |
| Alternatives | virtual_void or -> | </br>proxy, </br>Dyno, </br>AnyAny, </br>Zoo, </br>boost::type_erasure</br>- All have no downcast</br>- They can be faster because data and v-table are inlined | yomm2, a little faster</br>needs `this` with `v-table`. | ?                      |

[^1]: How m_tables work:
    - Every `class` defining an `open_method::via_m_table` has an `m_table`.
    - Every `open_method::via_m_table` belongs to a domain.
    - Every `domain` has a unique index.
    - Every `open_method::via_m_table` has a unique index in its domain.
    - A pointer to the `m_table` is passed to the call of the `open_method`.
    - The function called is `m_table[domain_index][method_index]`.
    - Consider a 'domain' an analogy to an 'interface'.
