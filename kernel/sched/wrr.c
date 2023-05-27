#include "sched.h"
#include <trace/events/sched.h>
#include <linux/list.h>

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

    // resched_curr(rq);
    // update wrr_rq total load
    wrr_rq->load += wrr_se->weight;
    wrr_se->rem_time_slice = wrr_se->weight * WRR_TIME_SLICE_UNIT;
    add_nr_running(rq, 1);
    // display_wrr_rq(wrr_rq);
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
    // debug
    yield_task_wrr(rq);
    return true;
}

static void check_preempt_wakeup_wrr(struct rq *rq, struct task_struct *p, int wake_flags)
{
    // Do not need
}

static struct task_struct * pick_next_task_wrr(struct rq *rq, struct task_struct *prev, struct rq_flags *rf)
{
    // front of the queue
    struct wrr_rq* wrr = &rq->wrr;
    // debug
    // struct sched_wrr_entity *wrr_se = list_first_entry(&wrr->wrr_list, struct sched_wrr_entity, list_node);
    struct task_struct *curr = wrr_task_of(list_first_entry(&wrr->wrr_list, struct sched_wrr_entity, list_node));
    if(list_empty(&wrr->wrr_list))
        return NULL;
    return curr;
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

    //printk(KERN_ALERT "************* select_task_rq_wrr start ************\n");
    rcu_read_lock();
    for_each_online_cpu(cpu)
    {
        cur_load = (cpu_rq(cpu)->wrr).load;
        //printk(KERN_ALERT "cpu %d load: %d, is allowed?: %d\n", cpu, cur_load, cpumask_test_cpu(cpu, &p->cpus_allowed));
        if(lowest_load > cur_load && cpumask_test_cpu(cpu, &p->cpus_allowed))
        {
            //printk(KERN_ALERT "Target CPU changed to %d with load %d\n", cpu, cur_load);
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
    // struct rq *rq = task_rq(p);
    // struct sched_wrr_entity *wrr_se = &p->wrr_se;
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

static void switched_from_wrr(struct rq *rq, struct task_struct *p)
{
    // not necessary.. just to be safe
    (p->wrr_se).rem_time_slice = (p->wrr_se).weight * WRR_TIME_SLICE_UNIT;
}

static void switched_to_wrr(struct rq *rq, struct task_struct *p)
{
    // overwrites possible garbage after enqueue
    (p->wrr_se).weight = 10;
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

	.check_preempt_curr	= check_preempt_wakeup_wrr,// X: no preemption needed 

	.pick_next_task		= pick_next_task_wrr,// O
	.put_prev_task		= put_prev_task_wrr,// X: task 하나 대한 데이터 관리

#ifdef CONFIG_SMP
	.select_task_rq		= select_task_rq_wrr,// O: 가장 한가한 
	//.migrate_task_rq	= migrate_task_rq_wrr, // X: 거추장스러운건 떼버려!

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
    // printk("[%s]", __func__);
    preempt_disable();
    rcu_read_lock();
    // printk(KERN_ALERT "Finding min & max cpus\n");
    for_each_online_cpu(cpu)
    {
        cur_load = (cpu_rq(cpu)->wrr).load;
        //printk(KERN_ALERT "cpu %d load: %d,", cpu, cur_load);
        if(cur_load > max_load)
        {
            // printk(KERN_ALERT "max CPU changed to %d with load %u\n", cpu, cur_load);
            max_load = cur_load;
            max_cpu = cpu;
        }
        if(cur_load < min_load)
        {
            // printk(KERN_ALERT "min CPU changed to %d with load %u\n", cpu, cur_load);
            min_load = cur_load;
            min_cpu = cpu;
        }
    }
    rcu_read_unlock();
    // printk(KERN_ALERT "min_cpu: %d, min_load: %u, max_cpu: %d, max_load: %u\n", min_cpu, min_load, max_cpu, max_load);

    if(max_cpu == min_cpu || max_load <= min_load)
        goto balance_end;
    src_rq = cpu_rq(max_cpu);
    dst_rq = cpu_rq(min_cpu);

    
    list_for_each_entry_safe(cur_wrr_entity, temp_wrr_entity, &((src_rq->wrr).wrr_list), list_node)
    {
        if(cur_wrr_entity){
            cur_task = wrr_task_of(cur_wrr_entity);
            raw_spin_lock(&cur_task->pi_lock);
            // printk(KERN_ALERT "task weight: %d, is allowed?: %d\n", cur_wrr_entity->weight, cpumask_test_cpu(min_cpu, &cur_task->cpus_allowed));
            if(((src_rq->wrr).load - (dst_rq->wrr).load > ((cur_wrr_entity->weight)*2)) && cpumask_test_cpu(min_cpu, &cur_task->cpus_allowed) && src_rq->curr != cur_task)
            {
                // printk(KERN_ALERT "select this task for load balancing!\n");
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


/*
reference from core.c
	 * The caller should hold either p->pi_lock or rq->lock, when changing
	 * a task's CPU. ->pi_lock for waking tasks, rq->lock for runnable tasks.
	 *
	 * sched_move_task() holds both and thus holding either pins the cgroup,
	 * see task_group().
	 *
	 * Furthermore, all task_rq users should acquire both locks, see
	 * task_rq_lock().
*/

/*
static int migrate_swap_stop(void *data)
{
	struct migration_swap_arg *arg = data;
	struct rq *src_rq, *dst_rq;
	int ret = -EAGAIN;

	if (!cpu_active(arg->src_cpu) || !cpu_active(arg->dst_cpu))
		return -EAGAIN;

	src_rq = cpu_rq(arg->src_cpu);
	dst_rq = cpu_rq(arg->dst_cpu);

	double_raw_lock(&arg->src_task->pi_lock,
			&arg->dst_task->pi_lock);
	double_rq_lock(src_rq, dst_rq);

	if (task_cpu(arg->dst_task) != arg->dst_cpu)
		goto unlock;

	if (task_cpu(arg->src_task) != arg->src_cpu)
		goto unlock;

	if (!cpumask_test_cpu(arg->dst_cpu, &arg->src_task->cpus_allowed))
		goto unlock;

	if (!cpumask_test_cpu(arg->src_cpu, &arg->dst_task->cpus_allowed))
		goto unlock;

	__migrate_swap_task(arg->src_task, arg->dst_cpu);
	__migrate_swap_task(arg->dst_task, arg->src_cpu);

	ret = 0;

unlock:
	double_rq_unlock(src_rq, dst_rq);
	raw_spin_unlock(&arg->dst_task->pi_lock);
	raw_spin_unlock(&arg->src_task->pi_lock);

	return ret;
}
static void __migrate_swap_task(struct task_struct *p, int cpu)
{
	if (task_on_rq_queued(p)) {
		struct rq *src_rq, *dst_rq;
		struct rq_flags srf, drf;

		src_rq = task_rq(p);
		dst_rq = cpu_rq(cpu);

		rq_pin_lock(src_rq, &srf);
		rq_pin_lock(dst_rq, &drf);

		p->on_rq = TASK_ON_RQ_MIGRATING;
		deactivate_task(src_rq, p, 0);
		set_task_cpu(p, cpu);
		activate_task(dst_rq, p, 0);
		p->on_rq = TASK_ON_RQ_QUEUED;
		check_preempt_curr(dst_rq, p, 0);

		rq_unpin_lock(dst_rq, &drf);
		rq_unpin_lock(src_rq, &srf);

	} else {
		
		 * Task isn't running anymore; make it appear like we migrated
		 * it before it went to sleep. This means on wakeup we make the
		 * previous CPU our target instead of where it really is.
		 
		p->wake_cpu = cpu;
	}
}
*/
