| meta data kind | description |
|-|-|
| ``no_meta_data`` | Pure void pointer, no safe cast back. Usefull if application can store meta information elsewhere. Example usage: ``open_object`` |
| ``has_type_info`` | |
| ``has_m_table`` | |


| meta data placement | description |
|-|-|
| decorated pointer | |
| decorated data | |

| type | meta data placement| lifetime requirements | description |
|------|-|-------|--------|
| ``const_observer`` | decorated pointer | | |
| ``mutable_observer`` | decorated pointer | | |
| ``shared_const_ptr`` | decorated pointer | | |
| ``shared_const`` | decorated data | | |
| ``unique_ptr`` | decorated pointer | | | 
| ``unique`` | decorated data | | |
| ``value`` | decorated data | | |
