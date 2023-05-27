#include "sched.h"
#include <trace/events/sched.h>
#include <linux/list.h>

#define WRR_TIME_SLICE_UNIT (10 * HZ / 1000) // 10ms

// utility functions 
static inline struct task_struct *wrr_task_of(struct sched_wrr_entity *wrr_se)
{
    return container_of(wrr_se, struct task_struct, wrr_se);
};

static inline struct rq *rq_of_wrr_rq(struct wrr_rq *wrr){
    return container_of(wrr, struct rq, wrr);
}

static inline struct rq *rq_of_wrr_se(struct sched_wrr_entity *wrr_se)
{
    return rq_of_wrr_rq(wrr_se->wrr);
}


static inline struct wrr_rq *task_wrr_rq(struct task_struct *p)
{
    return &task_rq(p)->wrr;
}


/*
- NOTE: Before calling this function, you have to take a lock
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
    wrr_rq->load += wrr_se->weight;
    wrr_se->rem_time_slice = wrr_se->weight * WRR_TIME_SLICE_UNIT;
    add_nr_running(rq, 1);
}

/*
- NOTE: Before calling this function, you have to take a lock
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
    wrr_rq->load -= wrr_se->weight;
    wrr_se->rem_time_slice = wrr_se->weight * WRR_TIME_SLICE_UNIT;
    sub_nr_running(rq, 1);
}

/*
- NOTE:Before calling this function, you have to take a lock
- Desciption
    - Move an entity wrr_se from first slot of wrr rq to tail slot
    - As do_sched_yield from core.c calls schdule() fuction, maybe don't have to send a rescheduling request
*/
static void yield_task_wrr(struct rq *rq)
{
    struct wrr_rq *wrr_rq = &rq->wrr;
    struct sched_wrr_entity *wrr_se = list_first_entry(&wrr_rq->wrr_list, struct sched_wrr_entity, list_node);
    
    list_move_tail(&wrr_se->list_node, &wrr_rq->wrr_list);
    wrr_se->rem_time_slice = wrr_se->weight * WRR_TIME_SLICE_UNIT;
}

// yield_to_task_wrr use preempt as a argument, so maybe we don't have to implement this fuction
static bool yield_to_task_wrr(struct rq *rq, struct task_struct *p, bool preempt)
{    
    yield_task_wrr(rq);
    return true;
}

static void check_preempt_wakeup_wrr(struct rq *rq, struct task_struct *p, int wake_flags)
{
    // Do not need
}

/*
- NOTE:Before calling this function, you have to take a lock
- Desciption
    - The frontmost task in the wrr_rq is next task for schedling
*/
static struct task_struct * pick_next_task_wrr(struct rq *rq, struct task_struct *prev, struct rq_flags *rf)
{
    struct wrr_rq* wrr = &rq->wrr;
    struct task_struct *curr = wrr_task_of(list_first_entry(&wrr->wrr_list, struct sched_wrr_entity, list_node));

    if(list_empty(&wrr->wrr_list))
        return NULL;
    return curr;
}

static void put_prev_task_wrr(struct rq *rq, struct task_struct *prev)
{
    // do not need
}

/*
- Desciption
    - Find the wrr_rq with the smallest weight
*/
static int select_task_rq_wrr(struct task_struct *p, int prev_cpu, int sd_flag, int wake_flags)
{
    int cpu;
    int lowest_load = 999999;
    int lowest_load_cpu;
    int cur_load;
    lowest_load_cpu = task_cpu(p);

    rcu_read_lock();
    for_each_online_cpu(cpu)
    {
        cur_load = (cpu_rq(cpu)->wrr).load;
        if(lowest_load > cur_load && cpumask_test_cpu(cpu, &p->cpus_allowed))
        {
            lowest_load = cur_load;
            lowest_load_cpu = cpu;
        }
    }
    rcu_read_unlock();
    return lowest_load_cpu;
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
    wrr_se->rem_time_slice--;
    if (wrr_se->rem_time_slice < 0){
        dequeue_task_wrr(rq, curr, 0);
        enqueue_task_wrr(rq, curr, 0);
        resched_curr(rq);
    }
}

/*
- Description
  - Initialize the weight and rem_time_slice of forked task_struct
*/
static void task_fork_wrr(struct task_struct *p)
{
    rcu_read_lock(); // p->parent could be in another cpu, other locks not needed since p not enqueued yet
    (p->wrr_se).weight = (p->parent->wrr_se).weight;
    (p->wrr_se).rem_time_slice = (p->wrr_se).weight * WRR_TIME_SLICE_UNIT;// for safety
    rcu_read_unlock();
}


static void prio_changed_wrr(struct rq *rq, struct task_struct *p, int oldprio)
{
    // do not need
}

/*
- Description
  - Switched form wrr scheduler to antoerh scheduler
*/
static void switched_from_wrr(struct rq *rq, struct task_struct *p)
{
    (p->wrr_se).rem_time_slice = (p->wrr_se).weight * WRR_TIME_SLICE_UNIT;
}

/*
- Description
  - Switched form another scheduler to wrr scheduler
*/
static void switched_to_wrr(struct rq *rq, struct task_struct *p)
{
    // overwrites possible garbage after enqueue
    (p->wrr_se).weight = 10;
    (p->wrr_se).rem_time_slice = (p->wrr_se).weight * WRR_TIME_SLICE_UNIT;
}

/*
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
	.next			= &fair_sched_class, 
	.enqueue_task		= enqueue_task_wrr,
	.dequeue_task		= dequeue_task_wrr, 
	.yield_task		= yield_task_wrr, 
	.yield_to_task		= yield_to_task_wrr, 

	.check_preempt_curr	= check_preempt_wakeup_wrr,

	.pick_next_task		= pick_next_task_wrr,
	.put_prev_task		= put_prev_task_wrr,

#ifdef CONFIG_SMP
	.select_task_rq		= select_task_rq_wrr, 
	//.migrate_task_rq	= migrate_task_rq_wrr, 

	.rq_online		= rq_online_wrr,
	.rq_offline		= rq_offline_wrr,

	.task_dead		= task_dead_wrr,
	.set_cpus_allowed	= set_cpus_allowed_common, 
#endif
	.set_curr_task          = set_curr_task_wrr,
	.task_tick		= task_tick_wrr, 
	.task_fork		= task_fork_wrr,

	.prio_changed		= prio_changed_wrr,
	.switched_from		= switched_from_wrr, 
	.switched_to		= switched_to_wrr, 
											

	.get_rr_interval	= get_rr_interval_wrr,

	.update_curr		= update_curr_wrr,
    
#ifdef CONFIG_FAIR_GROUP_SCHED
    .task_change_group	= task_change_group_wrr,
#endif
};

__init void init_sched_wrr_class(void)
{
	// Do not need
}


void init_wrr_rq(struct wrr_rq *wrr)
{
	INIT_LIST_HEAD(&wrr->wrr_list);
	wrr->load = 0;
}

/*
- Description
    - Balance the load of each wrr rq 
    - Find the most and least weighted rq and move the first tranferable task
*/
void load_balance_wrr()
{
    struct rq *src_rq, *dst_rq;
    struct task_struct *migrate_task = (struct task_struct *)(NULL);
    struct task_struct *cur_task = (struct task_struct *)(NULL);
    struct sched_wrr_entity *temp_wrr_entity;
    struct sched_wrr_entity *cur_wrr_entity;
    int min_cpu = 0;
    int max_cpu = 0;
    int max_load = 0;
    int min_load = 999999;
    int cur_load  = 0;
    int cpu = 0;

    preempt_disable();
    rcu_read_lock();
    for_each_online_cpu(cpu)
    {
        cur_load = (cpu_rq(cpu)->wrr).load;
        if(cur_load > max_load)
        {
            max_load = cur_load;
            max_cpu = cpu;
        }
        if(cur_load < min_load)
        {
            min_load = cur_load;
            min_cpu = cpu;
        }
    }
    rcu_read_unlock();

    if(max_cpu == min_cpu || max_load <= min_load)
        goto balance_end;
    src_rq = cpu_rq(max_cpu);
    dst_rq = cpu_rq(min_cpu);

    
    list_for_each_entry_safe(cur_wrr_entity, temp_wrr_entity, &((src_rq->wrr).wrr_list), list_node)
    {
        if(cur_wrr_entity){
            cur_task = wrr_task_of(cur_wrr_entity);
            raw_spin_lock(&cur_task->pi_lock);
            if(((src_rq->wrr).load - (dst_rq->wrr).load > ((cur_wrr_entity->weight)*2)) && cpumask_test_cpu(min_cpu, &cur_task->cpus_allowed) && src_rq->curr != cur_task)
            {
                migrate_task = cur_task;
                break;
            }
            raw_spin_unlock(&cur_task->pi_lock);
        } 
    }
    if(migrate_task)
    {   
        double_rq_lock(src_rq, dst_rq);
        deactivate_task(src_rq, migrate_task, 0);
		set_task_cpu(migrate_task, min_cpu);
		activate_task(dst_rq, migrate_task, 0);
        printk(KERN_DEBUG "[WRR LOAD BALANCING] jiffies: %Ld\n"
                "[WRR LOAD BALANCING] max_cpu: %d, total weight: %u\n"
                "[WRR LOAD BALANCING] min_cpu: %d, total weight: %u\n"
                "[WRR LOAD BALANCING] migrated task name: %s, task weight: %u\n",
        (long long)(jiffies), max_cpu, max_load, min_cpu, min_load,
        migrate_task->comm, migrate_task->wrr_se.weight);
        double_rq_unlock(src_rq, dst_rq);
        raw_spin_unlock(&migrate_task->pi_lock);
    }
balance_end:
    preempt_enable();
}

