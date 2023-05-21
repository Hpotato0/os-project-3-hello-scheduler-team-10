#include "sched.h"
#include <trace/events/sched.h>

#define WRR_TIME_SLICE_UNIT (10 * HZ / 1000) // TODO.. probably # of ticks in 10 ms?

static inline struct task_struct *wrr_task_of(struct sched_wrr_entity *wrr_se)
{
    return container_of(wrr_se, struct task_struct, wrr_se);
};

static inline struct rq *rq_of_wrr_rq(struct wrr_rq *wrr){
    return container_of(wrr, struct rq, wrr);
}

static inline struct rq *rq_of_wrr_se(struct sched_wrr_entity *wrr_se)
{
    // (?) struct task_struct *p = wrr_task_of(wrr_se);
    return rq_of_wrr_rq(wrr_se->wrr);
}

static inline struct wrr_rq *wrr_rq_of_wrr_se(struct sched_wrr_entity *wrr_se)
{
    struct task_struct *p = wrr_task_of(wrr_se);
    struct rq *rq = task_rq(p);
    return &rq->wrr;
}

static inline struct wrr_rq *task_wrr_rq(struct task_struct *p)
{
    return &task_rq(p)->wrr;
}

static inline struct sched_wrr_entity *curr_wrr_se(struct wrr_rq* wrr)
{
    // maybe need debuging. I'm note sure if ite will work corretly -by HS.K
    return list_first_entry(&wrr->wrr_list, struct sched_wrr_entity, list_node);
}


/*
- NOTE: Before calling dequeue_task_wrr, you have to take a lock
- Description
    - Add task to wrr_rq
    - Update total wrr_rq load, nr_running, and remain time slice of wrr_se
    - Send a reschedling requeset
*/
static void enqueue_task_wrr(struct rq *rq, struct task_struct *p, int flags)
{
    struct wrr_rq * wrr_rq = task_wrr_rq(p);
    struct sched_wrr_entity  *wrr_se = &p->wrr_se;

    list_add_tail(&wrr_se->list_node, & wrr_rq->wrr_list);

    resched_curr(rq);
    // update wrr_rq total load
    wrr_rq->load += wrr_se->weight;
    wrr_se->rem_time_slice = wrr_se->weight * WRR_TIME_SLICE_UNIT;
    add_nr_running(rq, 1);
    // maybe need to update wrr_se state or time slice?
}

/*
- NOTE: Before calling dequeue_task_wrr, you have to take a lock
- Description
    - Delete task from wrr_rq
    - Update totla wrr_rq load, nr_running, and remain time slice of wrr_se
    - Send a rescheling request 
*/
static void dequeue_task_wrr(struct rq *rq, struct task_struct *p, int flags) 
{
    struct wrr_rq *wrr_rq = task_wrr_rq(p);
    struct sched_wrr_entity  *wrr_se = &p->wrr_se;
    
    list_del_init(&wrr_se->list_node);

    resched_curr(rq);
    // update wrr_rq total load
    wrr_rq->load -= wrr_se->weight;
    wrr_se->rem_time_slice = wrr_se->weight * WRR_TIME_SLICE_UNIT;
    sub_nr_running(rq, 1);
}

/*
- NOTE:Before calling dequeue_task_wrr, you have to take a lock
- Desciption
    - Move an entity wrr_se from first slot of wrr rq to tail slot
    - As do_sched_yield from core.c calls schdule() fuction, maybe don't have to send a rescheduling request
*/
static void yield_task_wrr(struct rq *rq)
{
    // TODO: Fill me
    struct wrr_rq *wrr_rq = &rq->wrr;
    struct sched_wrr_entity *wrr_se = list_first_entry(&wrr_rq->wrr_list, struct sched_wrr_entity, list_node);
    list_move_tail(&wrr_se->list_node, &wrr_rq->wrr_list);

    // time slice update
    // TODO: need to define WRR_TIME_SLICe_UNIT
    wrr_se->rem_time_slice = wrr_se->weight * WRR_TIME_SLICE_UNIT;
}

// yield_to_task_wrr use preempt as a argument, so maybe we don't have to implement this fuction
static bool yield_to_task_wrr(struct rq *rq, struct task_struct *p, bool preempt)
{    
    yield_task_wrr(rq);
    return true;
}

static void check_preempt_wakeup(struct rq *rq, struct task_struct *p, int wake_flags)
{
    // Do not need
}

static struct task_struct * pick_next_task_wrr(struct rq *rq, struct task_struct *prev, struct rq_flags *rf)
{
    // method(1): use prev
    struct sched_wrr_entity* wrr_se = &prev->wrr_se;
    if(list_empty(&(wrr_se->list_node)))
        return NULL;
    return wrr_task_of(list_entry((wrr_se->list_node).next, struct sched_wrr_entity, list_node));

    // method(2): take the front of the queue
    // TODO.. 실행되면 queue에서 빠지나? 아님. 엥 그러면 그냥 prev 쓰고 queue_front는 enqueue/dequeue에만 사용해야 할 듯?
}

static void put_prev_task_wrr(struct rq *rq, struct task_struct *prev)
{
    // do not need
}

static int select_task_rq_wrr(struct task_struct *p, int prev_cpu, int sd_flag, int wake_flags)
{
    int cpu;
    int lowest_load = 999999;
    int lowest_load_cpu;
    int cur_load;
    lowest_load_cpu = task_cpu(p);
    if(sd_flag == SD_BALANCE_WAKE || sd_flag == SD_BALANCE_EXEC)
        return prev_cpu;
    printk(KERN_ALERT "************* select_task_rq_wrr start ************\n");
    rcu_read_lock();
    for_each_online_cpu(cpu)
    {
        cur_load = (cpu_rq(cpu)->wrr).load;
        printk(KERN_ALERT "cpu %d load: %d, is allowed?: %d\n", cpu, cur_load, cpumask_test_cpu(cpu, &p->cpus_allowed));
        if(lowest_load < cur_load && cpumask_test_cpu(cpu, &p->cpus_allowed))
        {
            printk(KERN_ALERT "Target CPU changed to %d with load %d\n", cpu, cur_load);
        }
    }
    rcu_read_unlock();
    printk(KERN_ALERT "************* select_task_rq_wrr end ************\n");
    return lowest_load_cpu;
}

static void migrate_task_rq_wrr(struct task_struct *p, int new_cpu)
{
    // TODO: Fill me
}

static void rq_online_wrr(struct rq *rq)
{
    // do not need
}

static void rq_offline_wrr(struct rq *rq)
{
    // do not need
}

static void task_dead_wrr(struct task_struct *p)
{
    // do not need
}

static void set_curr_task_wrr(struct rq *rq)
{
    // do not need
}

/*
- NOTE: Before calling dequeue_task_wrr, you have to take a lock
- Description
  - check remain time slice of current task and send rescheduling request
  - task_tick_wrr is not realated with load balance
*/
static void task_tick_wrr(struct rq *rq, struct task_struct *curr, int queued)
{
    // struct wrr_rq *wrr;
    struct sched_wrr_entity *wrr_se = &curr->wrr_se;

    wrr_se->rem_time_slice -= 1;
    if (wrr_se->rem_time_slice < 0){
        dequeue_task_wrr(rq, curr, 0);
        enqueue_task_wrr(rq, curr, 0);
    }
}

/*
    Need to take a lock in task_fork_wrr
*/
static void task_fork_wrr(struct task_struct *p)
{
    struct rq *rq = this_rq(); //? task_rq(p)
    struct rq_flags rf;

    rq_lock(rq, &rf);
	
    (p->wrr_se).weight = (p->parent->wrr_se).weight;
    (p->wrr_se).rem_time_slice = (p->wrr_se).weight * WRR_TIME_SLICE_UNIT;// Q) do we need this? when does 'enqueue' happen?

    rq_unlock(rq, &rf);
}

static void prio_changed_wrr(struct rq *rq, struct task_struct *p, int oldprio)
{
    // do not need
}

static void switched_from_wrr(struct rq *rq, struct task_struct *p)
{
    // TODO.. Q) dequeue가 있는데 필요한가?
    // rq->wrr->load -= p->wrr_se->weight; @J CHECK! if not careful might double-subtract
    (p->wrr_se).rem_time_slice = (p->wrr_se).weight * WRR_TIME_SLICE_UNIT;
}

static void switched_to_wrr(struct rq *rq, struct task_struct *p)
{
    // TODO.. Q) enqueue가 있는데 필요한가?
    (p->wrr_se).rem_time_slice = (p->wrr_se).weight * WRR_TIME_SLICE_UNIT;
}

/*
- NOTE: TODO) what should be the 'default' time slice in wrr..?
- Description
    - return the default timeslice of a process.
*/
static unsigned int get_rr_interval_wrr(struct rq *rq, struct task_struct *task)
{
    return ((task->wrr_se).weight) * WRR_TIME_SLICE_UNIT;
}

static void update_curr_wrr(struct rq *rq)
{
    // do not need
}

#ifdef CONFIG_FAIR_GROUP_SCHED   
static void task_change_group_wrr(struct task_struct *p, int type)
{
    // do not need
}
#endif
const struct sched_class wrr_sched_class = {
	.next			= &fair_sched_class, // O
	.enqueue_task		= enqueue_task_wrr,// O
	.dequeue_task		= dequeue_task_wrr, // O
	.yield_task		= yield_task_wrr, // O
	.yield_to_task		= yield_to_task_wrr, // maybe don

	.check_preempt_curr	= check_preempt_wakeup,// X: no preemption needed 

	.pick_next_task		= pick_next_task_wrr,// O
	.put_prev_task		= put_prev_task_wrr,// X: task 하나 대한 데이터 관리

#ifdef CONFIG_SMP
	.select_task_rq		= select_task_rq_wrr,// O: 가장 한가한 
	.migrate_task_rq	= migrate_task_rq_wrr, // O: load bal에서 사용

	.rq_online		= rq_online_wrr,// X: 이해불가
	.rq_offline		= rq_offline_wrr, // X: 이해불가

	.task_dead		= task_dead_wrr,// X: CFS에서 구현 X
	.set_cpus_allowed	= set_cpus_allowed_common, // O: 원래 그대로. RT도 동일
#endif
	.set_curr_task          = set_curr_task_wrr,// X: vRUNTIME 설정 관련 구현 X
	.task_tick		= task_tick_wrr, // O: load balancer 호출 예정
	.task_fork		= task_fork_wrr,// O: weight, time slice 설정

	.prio_changed		= prio_changed_wrr,// X: prio 없음
	.switched_from		= switched_from_wrr, // X: vruntime 등 update하는 함수
	.switched_to		= switched_to_wrr, // O: weight 와 time slice등 초기화
																			// core.c의 check_class_changed에서 호출되는데, 이 메서드는 context switch도중 스케줄링 클래스가 바뀌었는지 검사하는 용도

	.get_rr_interval	= get_rr_interval_wrr,// O: time slice기반 출력

	.update_curr		= update_curr_wrr,// X: vRuntime 관련 
    
    #ifdef CONFIG_FAIR_GROUP_SCHED
        .task_change_group	= task_change_group_wrr,
    #endif
};

__init void init_sched_wrr_class(void)
{
	// TODO : Fill in

	/* Example: cfs
#ifdef CONFIG_SMP
	open_softirq(SCHED_SOFTIRQ, run_rebalance_domains);

#ifdef CONFIG_NO_HZ_COMMON
	nohz.next_balance = jiffies;
	nohz.next_blocked = jiffies;
	zalloc_cpumask_var(&nohz.idle_cpus_mask, GFP_NOWAIT);
#endif
#endif // SMP */
}

void init_wrr_rq(struct wrr_rq *wrr)
{
	INIT_LIST_HEAD(&wrr->wrr_list);
	wrr->load = 0;
}
