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
~/./sudo setup-images.sh
~/./mkdir mntdir
~/./sudo moount tizen-image/rootfs.img  mntdir
~/./sudo cp test/factorizLoop test/factorizLoop_hide test/log.sh test/setup.sh mntdir/root
~/./_run.sh
```
### WRR schedling test
```bash
root/./setup.sh
root/./log.sh test_weight // test_weight will be the weight of the your performance test process
```
### Load balancing test
```base
root/./printWRRRloads
root/./setup.sh
root/./log.sh
```
### Error check
```bash
```
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

## 3. Implementation: Scheduler Class Functions

in `kernel/sched/wrr.c`
```C
const struct sched_class wrr_sched_class = {
	.next			    = &fair_sched_class, 
	
    .enqueue_task		= enqueue_task_wrr,
	.dequeue_task		= dequeue_task_wrr, 
	
    .yield_task		    = yield_task_wrr, 
	.yield_to_task		= yield_to_task_wrr, 

	.pick_next_task		= pick_next_task_wrr,

#ifdef CONFIG_SMP
	.select_task_rq		= select_task_rq_wrr, 
#endif

	.task_tick		    = task_tick_wrr, 
	.task_fork		    = task_fork_wrr,

	.switched_from		= switched_from_wrr, 
	.switched_to		= switched_to_wrr, 

	.get_rr_interval	= get_rr_interval_wrr,
};
```
## 4. Performance test

## 5. Lessons Learned
* `printk`s with interrupt disabling can cause deadlocks!
* stack traces are invaluable 
