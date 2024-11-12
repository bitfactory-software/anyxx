| meta data kind | description |
|-|-|
| ``no_meta_data`` | - Pure ``void`` pointer.</br>-No memory overhead.</br>-No safe cast back.</br>- Usefull if application can store meta information elsewhere.</br>- **usage**:  implementation of ``open_object``, ad hoc ``interface`` usage. |
| ``has_type_info`` | - Pointer to ``type_info`` of type known at construction.</br>- enables *safe cast* back to type.</br>- enables ``open_method::via_type_info``- enables ``std::any``like types |
| ``has_m_table`` | - Pointer to ``m_table`` and indirect to ``type_info`` of type known at construction.</br>- enables *safe cast* back to type.</br>- enables ``open_method::via_type_info``</br>- enables ``open_method::via_m_table``  |


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
