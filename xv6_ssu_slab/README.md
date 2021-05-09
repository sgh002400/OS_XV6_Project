# Byte-level Memory Allocator



## Implement a byte-level memory allocator on xv6



## Individual allocator per fixed size of bytes

- 8, 16, 32, 64, 128, 256, 512, 1024 and 2048 bytes



## Slab

- size : size of a slab allocator
- num_pages : number of pages of a slab allocator
- num_free_objects : number of free objects of a slab allocator
- num_used_objects : number of used objects of a slab allocator
- bitmap : allocation bitmap for a slab allocator
- page : page array for a slab allocator



## Slab.c

### void slabinit()

- Initialization function
- Allocate one page for bitmap
- Allocate one page for slab cache



### char *kmalloc(int size)

- Byte-level allocation function
- 0 < size <- 2048
- Allocate one more page when free space runs out
- Return the address of the allocated objects



### void kmfree(char *addr, int size)

- Free function
- Memory should be allocated by kmalloc() first
- DO NOT IMPLEMENT "page deallocation"



## Test

``` make qemu-nox ```

``` slabtest ```



![image](https://user-images.githubusercontent.com/62228195/117566385-57b4b680-b0f1-11eb-9ab3-047a9f65e03e.png)

(skip)
