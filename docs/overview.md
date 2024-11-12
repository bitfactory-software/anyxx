| Meta data kind | Description |
|-|-|
| ``no_meta_data`` | - Pure ``void`` pointer.</br>-No memory overhead.</br>-No safe cast back.</  br>- Usefull if application can store meta information elsewhere.</br>- **Usage**:  implementation of ``open_object``, ad hoc ``interface`` usage. |
| ``has_type_info`` | - Pointer to ``type_info`` of type known at construction.</br>- **Enables** *safe cast* back to type.</br>- **Enables** ``open_method::via_type_info``</br>- **Enables** ``std::any``like types |
| ``has_m_table`` | - Pointer to ``m_table`` and indirect to ``type_info`` of type known at construction.</br>- **Enables** same options as ``has_type_info``</br>- **Enables** ``open_method::via_m_table``  |


| Meta data decoration | Description |
|-|-|
| ``decorated_ptr`` | Pointer to *meta data* beneth *(smart)pointer* to *erased data*.</br>One copy per pointer. |
| ``decorated_data`` | Meta allocated with erased data.</br>One copy per erased data. |

| Lifetime type | Meta data decoration | Lifetime requirements | Description |
|------|-|-------|--------|
| ``const_observer`` | ``..._ptr`` | | |
| ``mutable_observer`` | ``..._ptr`` | | |
| ``shared_const_ptr`` | ``..._ptr`` | | |
| ``shared_const`` | ``..._data`` | | |
| ``unique_ptr`` | ``..._ptr`` | | | 
| ``unique`` | ``..._data`` | | |
| ``value`` | ``..._data`` | | |
