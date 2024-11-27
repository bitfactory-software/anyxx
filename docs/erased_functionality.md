| First Header  | Second Header | Third Header         |
| :------------ | :-----------: | -------------------: |
| First row     | Data          | Very long data entry |
| Second row    | **Cell**      | *Cell*               |
| Third row     | Cell that spans across two columns  ||

| Type | **Interface** | **Open method**  | **Open method** |
|-|-|-|-|
| meta data requirement | none |  via ``type_info`` |  ``via m_table`` |
| Runtime penalty</br>``virtual`` function = 1 |  1 | 2.5 | 1.8 |
| Memory Footprint</br>per function(f) and implemeting type(t) [byte] | O(1) | O(1 + x) </br> Ø(x) ~ 0.3 | O( ``all domains`` + ``functions_in_domain``)</br> see bellow |
| static meta data | yes (v-table) | no | yes (m-table) |
| coupling in comparison | strong(-) </br>- Type of interface</br>- Type of lifetime holder  | week(+)</br>- pointer to erased data</br>- typeinfo  | | |
| Sumary - | ti | | | |
