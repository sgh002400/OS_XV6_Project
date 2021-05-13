# Dynamic Process Allocator

## Implement dynamic process allocation with slab cache

![image](https://user-images.githubusercontent.com/62228195/118091722-1cbec580-b406-11eb-9fac-0fb63879fb68.png)


## Modify proc.c
### Allocate
- allocproc
- fork, userinit

### Dealloc
- wait

### Traverse
- exit, wait, scheduler
- wakeup1, kill, procdump
- ps
- We will test with ps(system call)


## Ps

### Current result with NPROC = 16
![image](https://user-images.githubusercontent.com/62228195/118092821-930ff780-b407-11eb-9d3c-96e53106f0e4.png)

### After implementation result with NPROC = 16
![image](https://user-images.githubusercontent.com/62228195/118093026-cd799480-b407-11eb-96d7-17557fb468d2.png)


### __Changed slab.c__
