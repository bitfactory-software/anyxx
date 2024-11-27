# Comparison of *erased functionality* in ``virtual_void``  

| namespace | ``interface`` | ``open_method::via_type_info``  | ``open_method::via_m_table`` |
|-|-|-|-|
| meta data requirement | none |  ``via_type_info`` </or>``via_m_table`` | ``via_m_table`` |
| Runtime penalty</br>``virtual`` function = 1 |  1 | 2.5 | 1.8 |
| Memory Footprint</br>per function(f) and implemeting type(t) [byte] | O(1) | O(1 + x) </br> Ø(x) ~ 0.3 | O( ``all domains`` + ``functions_in_domain``)</br> see bellow |
| static meta data | yes (v-table) | no | yes (m-table) |
| coupling in comparison | strong(-) </br>- Type of interface</br>- Type of lifetime holder  | week(+)</br>- pointer to erased data</br>- typeinfo | week(+)</br>- pointer to erased data</br>- pointer | 
| cermony overhead | no | seal_for_runtime() must be called before usage.</br>Typically in controlled from ``main`` after all *.dll/so* are loaded. | no |
| Alternatives</br>(With other tradeoffs) | </br>proxy, </br>Dyno, </br>AnyAny, </br>Zoo, </br>boost::type_erasure | yomm2 | <-|
