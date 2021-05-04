![image](https://user-images.githubusercontent.com/62228195/116965935-03bd6280-acea-11eb-9f5a-efedff206df3.png)



# Setnice / Getnice

### A system call that sets/gets a priority of process by using nice value(0~10)



### int setnice(int pid, int nice)

- Set the process priority

- Argument: process ID, nice value

- Return value: failure -1 / success 0



### int getnice(int pid)

- Get the process priority

- Argument: process ID, nice value

- Return value: failure -1 / success process' nice value





# PS

### A system call that prints out the process information

- Pid

- State: RUNNING, RUNNABLE, SLEEPING
- Priority(nice value)
- Runtime
- Tick



# Test

``` make qemu-nox ```

``` ps ```

``` test_nice ```

``` ctrl+a, x ```


![image](https://user-images.githubusercontent.com/62228195/116965697-59453f80-ace9-11eb-9471-40f72f9eaa96.png)

