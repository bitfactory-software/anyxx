| meta data kind | description |
|-|-|
| ``no_meta_data`` | - Pure ``void`` pointer.</br>-No memory overhead.</br>-No safe cast back.</br>- Usefull if application can store meta information elsewhere.</br>- **Example** usage:</br> - ``open_object``</br> - simple ``interface`` usage. |
| ``has_type_info`` | - Pointer to ``type_info`` of type known at construction.</br>- Safe cast back to type.</br>|
| ``has_m_table`` | |


| meta data placement | description |
|-|-|
| ``decorated_ptr`` | |
| ``decorated_data`` | |

| type | meta data placement| lifetime requirements | description |
|------|-|-------|--------|
| ``const_observer`` | ``decorated_ptr`` | | |
| ``mutable_observer`` | ``decorated_ptr`` | | |
| ``shared_const_ptr`` | ``decorated_ptr`` | | |
| ``shared_const`` | ``decorated_data`` | | |
| ``unique_ptr`` | ``decorated_ptr`` | | | 
| ``unique`` | ``decorated_data`` | | |
| ``value`` | ``decorated_data`` | | |
