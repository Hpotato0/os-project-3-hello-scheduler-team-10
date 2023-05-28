# Project 3: hello, scheduler!
* team 10: 김현석, 홍주원, 주재형
* test code directory: `~/test/`

## 0. Running & Testing
The kernel build, test code compilation & running procedure did not change from project0 README. Optionally, the Makefile located in `test/` can be used to simplify the compilation of test code, as demonstrated in the following example.
```bash
# trial.c -> trial
/project-3-hello-scheduler-team-10/test$ make NAME=trial    			# do naive trial divistion for finite num and print turnaround time
/project-3-hello-scheduler-team-10/test$ make NAME=trial_bg 			# infinitly trial division
/project-3-hello-scheduler-team-10/test$ make NAME=printWRRloads 		# periodicly print load of each wrr rq
/project-3-hello-scheduler-team-10/test$ make NAME=sched_setweight		# 'wrapper' code of the new system call sched_setweight
/project-3-hello-scheduler-team-10/test$ make NAME=sched_getweight		# 'wrapper' code of the new system call sched getweight
```

## How to test 
kernel build and run qmeu
```bash
/project-3-hello-scheduler-team-10$ ./_build.sh
/project-3-hello-scheduler-team-10$ sudo ./setup-images.sh
/project-3-hello-scheduler-team-10$ mkdir mntdir
/project-3-hello-scheduler-team-10$ sudo moount tizen-image/rootfs.img  mntdir
/project-3-hello-scheduler-team-10$ sudo cp test/trial test/trial_bg test/log.sh test/setup.sh test/sched_setweight test/sched_getweight mntdir/root
/project-3-hello-scheduler-team-10$ sudo umount mntdir
/project-3-hello-scheduler-team-10$ ./_run.sh
// in virtual machine
```

### WRR schedling test
```base
root$ ./setup.sh			# start 20 background testing enviroonment program
root$ ./log.sh test_weight  		# test_weight will be the weight of the your performance test process
```

### Load balancing test
```base
root$ ./printWRRRloads		# periodicaly print load state of each wrr_rq
root$ ./setup.sh		# start 20 background testing enviroonment program
root$ ./log.sh test_weight  	# test_weight will be the weight of the your performance test process
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
* `wrr_list`: since `struct list_head` is a doubly linked list, an extra node is used to mark the front & rear of the queue (`wrr_list` is placed in between the front & rear of the queue)
* `load`: the sum of weights in this `wrr_rq`, used for load balancing

## 3. Implementation: Scheduler Class Functions
The following scheduler class functions of wrr_sched_class are implemented in `kernel/sched/wrr.c`.
* `enqueue_task_wrr`	:
* `dequeue_task_wrr`	:
* `yield_task_wrr`	:
* `yield_to_task_wrr`	:
* `pick_next_task_wrr`	:
* `select_task_rq_wrr`	:
* `task_tick_wrr`	:
* `task_fork_wrr`	:
* `switched_from_wrr`	:
* `switched_to_wrr`	:
* `get_rr_interval_wrr`	: (Optional) print the 

The following functions are also implemented in `kernel/sched/wrr.c`.
* `load_balance_wrr`	:
* `print_wrr_stats`	:
* `init_wrr_rq`		:
* `init_sched_wrr_class`: can be left empty

## 4. Test result
- Test envirnoment
	- We run our elasped time measurment program ("trail") with 20 competier programs ("trial_bg"), which are running infinity with different weight (1~20).
	- "trial" program performs factorization on a random number (54128) a finite number of times (100000) and outputs the elapled time.
	- We repeadted each test 1- times with different weights in the sme environment and measured the average.
	
<p align="center"><img src="https://github.com/swsnu/project-3-hello-scheduler-team-10/assets/91672190/98dfcbe8-e023-46d0-bad3-e74fbca343cc"></p>

 Before testing, we estimated the elaped time is inversely proportional the the weight protion.

<p align="center"><img src="https://github.com/swsnu/project-3-hello-scheduler-team-10/assets/91672190/a290ec6e-ef88-4596-8056-0dc25a266077"></p>
 The blue line means our testing result and orange line means the theoretical value calculated based on the weight portion. It can be observec that the elapsd time is inversely proportional to the weight portion. We calculated estimated elapsed time based on test result and weight portion. Slight discrepancies may exist, but the two graphs exhibit the same tremd.

  

## 5. Lessons Learned
* `printk` with interrupt disabling can cause deadlocks!
* stack traces are invaluable 
