# Aspects of *erased data* in ``virtual_void``  

## Meta data kind

| Meta data kind | Description |
|-|-|
| ``no_meta_data`` | - Pure ``void`` pointer.</br>- No memory overhead.</br>- No safe cast back.</  br>- Usefull if application can store meta information elsewhere.</br>- **Usage**:  implementation of ``open_object``, ad hoc ``any`` usage. |
| ``has_type_info`` | - Pointer to ``type_info`` of type known at construction.</br>- **Enables** *safe cast* back to type.</br>- **Enables** ``open_method::via_type_info``</br>- **Enables** ``std::any``like types |
| ``has_m_table`` | - Pointer to ``m_table`` and indirect to ``type_info`` of type known at construction.</br>- **Enables** same options as ``has_type_info``</br>- **Enables** ``open_method::via_m_table``  |

## Meta data decoration

| Meta data decoration | Description |
|-|-|
| ``decorated_ptr`` | Pointer to *meta data* beneth *(smart)pointer* to *erased data*.</br>One copy per pointer. |
| ``decorated_data`` | Meta allocated with erased data.</br>One copy per erased data. |

## A 'Lifetime handle' with a 'Meta data decoration' make a ``is_erased_data`` model: 

| Lifetime type | Meta data decoration | Lifetime requirements | Concurrency considerations / Notes  | 
|------|-|-------|--------|
| ``const_observer`` | ``..._ptr`` | Observed object must outlive call. | Threadsave, if observed object is ``const`` turing lifetime of observer. |
| ``mutable_observer`` | ``..._ptr`` | Observed object must outlive call. | Not threadsave. |
| ``shared_const_ptr`` | ``..._ptr`` | Same as ``std::shared_ptr``.</br>Use, if </br>- you can not control construction, or</br>- you interact with code unaware of ``virtual_void``. | Threadsave.</br>Makes additional *type erased* shared_ptr to original *shared_ptr control block*. |
| ``shared`` | ``..._data`` | Same as ``std::shared_ptr``.</br>Use, if you can control full lifetime in ``virtual_void`` aware code. | Threadsave.  |
| ``unique_ptr`` | ``..._ptr`` | Same as ``std::unique_ptr``.</br>Use, if </br>- you can not control construction, or</br>- you interact with code unaware of ``virtual_void``. | Takes *type erased* ownership original data. | 
| ``unique`` | ``..._data`` | Same as ``std::unique_ptr``.</br>Use, if you can control full lifetime in ``virtual_void`` aware code. | |
| ``value`` | ``..._data`` | Same as ``std::any`` | Threadsafe. |
