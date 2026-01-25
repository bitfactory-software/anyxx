# Any++ *Proxy* types overview



| Proxy type | Lifetime requirements | Concurrency considerations / Notes  | 
|------|-------|--------|
| ``cref`` | Observed object must outlive call. | Threadsave, if observed object is ``const`` turing lifetime of observer. |
| ``mutref`` | Observed object must outlive call. | Not threadsave. |
| ``shared`` | ``..._data`` | Same as ``std::shared_ptr``.</br>| Threadsave.  |
| ``unique`` | ``..._data`` | Same as ``std::unique_ptr``.</br> | Threadsave, if no new ref' where taken. |
| ``value`` | ``..._data`` | Same as ``std::any`` | Threadsafe. |
