# Comparison of *erased functionality* in ``virtual_void``  

| namespace                          | ``interface``   | ``open_method::via_type_info``                    | ``open_method::via_m_table``                                        |
|:-                                  |:-               |:-                                                |:-                                                                  |
| meta data requirement              | none            | ``via_type_info`` </br>``via_m_table``            | ``via_m_table``                                                    |
| Runtime penalty</br>``virtual`` function = 1 |  1              | 2.5                                              | 1.8                                                                 |
| Memory Footprint</br>per function(f) and implemeting type(t) [byte] | O(1)           | O(1 + x) </br> Ø(x) ~ 0.3                         | O( ``all domains`` + ``functions_in_domain``)</br> see bellow      |
| static meta data                   | yes (v-table)   | no                                               | yes (m-table)                                                      |
| coupling in comparison             | low(0) </br>- Type of interface</br>- Type of lifetime holder  | minimal(+) </br>- pointer to erased data</br>- typeinfo | minimal(+) </br>- pointer to erased data</br>- pointer   | 
| general boilerplate?          | no              | seal_for_runtime() must be called before usage.</br>Typically in controlled from ``main`` after all *.dll/so* are loaded. | no                                    |
| dynamic library load handling boilerplate?  | no              | seal_for_runtime() must be called after all *.dll/so* are loaded.  | no                                                                 |
| dynamic library unload safe before 'normal' termination phase? | yes           | not supported                                     | yes.                                                                |
| Alternatives</br>(With other tradeoffs) | </br>proxy, </br>Dyno, </br>AnyAny, </br>Zoo, </br>boost::type_erasure</br>- All have no downcast</br>- They can be faster because data and v-table are inlined | yomm2, a little faster, but needs ``this`` with ``v-table``.    | ?                      |
