# Priority Scheduling



## Implement priority scheduler in xv6

- The lower nice value, the higher priority
- The highest priority process should be chosen for next running
  - choose randomly if equal



## Scheduler runs only when a change occurs in process priorities

- DO NOT call the scheduler on the timer interrupt

  -> trap.c

![image](https://user-images.githubusercontent.com/62228195/116967883-41bc8580-acee-11eb-8ce8-c20d56d7341e.png)
  

- When a process calls fork(), the nice value of child process is set to 5





## Test

``` make qemu-nox ```

``` test_sched1 ```

``` test_sched2 ```


![image](https://user-images.githubusercontent.com/62228195/116968590-bd6b0200-acef-11eb-88c7-ba622fc5d3d3.png)

