# Project 3: hello, scheduler!
* team 10: 김현석, 홍주원, 주재형
* main implementation: ./kernel/rotation.c
* test code: ./test/{rotd.c,professor.c,student.c}

## 0. Running & Testing
The kernel build, test code compilation & running procedure did not change from project0 README. Optionally, the Makefile located in `test/` can be used to simplify the compilation of test code, as demonstrated in the following example.

```bash
# trial.c -> trial
/project-3-hello-scheduler-team-10/test$ make NAME=trial
```

## How to test 
kernel build and run qmeu
```bash
~/sudo ./_build.sh
./sudo setup-images.sh
./mkdir mntdir
./sudo moount tizen-image/rootfs.img  mntdir
./sudo cp test/factorizLoop test/factorizLoop_hide test/log.sh test/setup.sh mntdir/root
./_run.sh
// in virtual machine
./setup.sh
./log.sh "test weight"
```
### WRR schedling test
### Load balancing test

printWRRloads

### Error check


## 1. Implementation Overview
WRR(Weighted Round Robin)

## 2. Implementation: Registering WRR into Structs
**include/linux/sched.h**
```C
struct sched_wrr_entity {
	struct list_head list_node;
	int weight;
	int rem_time_slice; // unit: ms
	struct wrr_rq * wrr;
};
...
struct task_struct {
    ...
    struct sched_wrr_entity 	wrr_se;
    ...
};
```
* `list_node`: linked list node for implementing the queue
* `weight`: weight(for WRR)
* `rem_time_slice`: remaining timeslice(for round-robin)
* `wrr`: pointer to the WRR queue that the task belongs

**kernel/sched/sched.h**
```C
struct wrr_rq {
	struct list_head wrr_list;
	int load;
};
...
struct rq {
    ...
    struct wrr_rq		wrr;
    ...
}
```
* `wrr_list`: since `struct list_head` is a doubly linked list, an extra node is used to mark the front & rear of the queue (`wrr_list` is placed in between the front & rear of the queue)
* `load`: the sum of weights in this `wrr_rq`, used for load balancing

## 3. Implementation: Scheduler Class Functions
The following scheduler class functions of wrr_sched_class are implemented in `kernel/sched/wrr.c`.
* `enqueue_task_wrr`
* `dequeue_task_wrr`
* `yield_task_wrr`
* `yield_to_task_wrr`
* `pick_next_task_wrr`
* `select_task_rq_wrr`
* `task_tick_wrr`
* `task_fork_wrr`
* `switched_from_wrr`
* `switched_to_wrr`
* `get_rr_interval_wrr`

## 4. Lessons Learned
* `printk` with interrupt disabling can cause deadlocks!
* stack traces are invaluable 
