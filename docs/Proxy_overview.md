# Any++ Type Erased *Proxy*s

### Overview 

| Proxy type | Lifetime requirements | Concurrency considerations / Notes  | 
|------|-------|--------|
| ``cref`` | Observed object must outlive call. | Threadsafe, if observed object self is ``const`` during lifetime of observer. |
| ``mutref`` | Observed object must outlive call. | Not threadsafe. |
| ``shared`` | Same as ``std::shared_ptr``.</br>| Threadsafe.  |
| ``unique`` | Same as ``std::unique_ptr``.</br> | Threadsafe. |
| ``value`` | Same as ``std::any`` | Threadsafe. |


### Lifetime castability

#### borrow_as

| v-to\from->| ``cref`` | ``mutref``|``shared`` | ``unique`` | ``value`` | 
|:-----------|:--------:|:---------:|:----------|:----------:|:---------:|
| ``cref``   | yes      | yes       | yes       | yes        | yes	     |
| ``mutref`` | no       | yes       | no        | yes :skull:| yes	     |
| ``shared`` | no       | no        | yes       | no         | no	     |
| ``unique`` | no       | no        | no        | no         | no	     |
| ``value``  | no       | no        | yes       | no         | no	     |

#### move_to

| v-to\from->| ``cref`` | ``mutref``|``shared`` | ``unique`` | ``value`` | 
|:-----------|:--------:|:---------:|:----------|:----------:|:---------:|
| ``cref``   | yes      | yes       | no        | no         | no	     |
| ``mutref`` | no       | yes       | no        | no         | yes	     |
| ``shared`` | no       | no        | yes       | yes        | yes	     |
| ``unique`` | no       | no        | no        | no         | no	     |
| ``value``  | no       | no        | TODO      | TODO       | yes	     |

#### clone_from

| v-to\from->| ``cref`` | ``mutref``|``shared`` | ``unique`` | ``value`` | 
|:-----------|:--------:|:---------:|:----------|:----------:|:---------:|
| ``cref``   | no       | no        | no        | no         | no	     |
| ``mutref`` | no       | no        | no        | no         | no	     |
| ``shared`` | yes      | yes       | yes       | yes        | yes	     |
| ``unique`` | yes      | yes       | yes       | yes        | yes	     |
| ``value``  | yes      | yes       | yes       | yes        | yes	     |