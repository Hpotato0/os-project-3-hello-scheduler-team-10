#include "sched.h"
#include <trace/events/sched.h>

static void enqueue_task_wrr(struct rq *rq, struct task_struct *p, int flags)
{
    // TODO: Fill me
};

static void dequeue_task_wrr(struct rq *rq, struct task_struct *p, int flags) {
    // TODO: Fill me
}


static void yield_task_wrr(struct rq *rq)
{
    // TODO: Fill me
}

static bool yield_to_task_wrr(struct rq *rq, struct task_struct *p, bool preempt)
{
    // TODO: Fill me
}

static void check_preempt_wakeup(struct rq *rq, struct task_struct *p, int wake_flags)
{
    // Don't need
}

static struct task_struct * pick_next_task_wrr(struct rq *rq, struct task_struct *prev, struct rq_flags *rf)
{
    // TODO: Fill me
}

static void put_prev_task_wrr(struct rq *rq, struct task_struct *prev)
{
    // do not need
}

static int select_task_rq_wrr(struct task_struct *p, int prev_cpu, int sd_flag, int wake_flags)
{
    // TODO: Fill me
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

static void task_tick_wrr(struct rq *rq, struct task_struct *curr, int queued)
{
    // TODO: Fill me
}

static void task_fork_wrr(struct task_struct *p)
{
    // TODO: Fill me
}

static void prio_changed_wrr(struct rq *rq, struct task_struct *p, int oldprio)
{
    // do not need
}

static void switched_from_wrr(struct rq *rq, struct task_struct *p)
{
    // ?
}

static void switched_to_wrr(struct rq *rq, struct task_struct *p)
{
    // ?
}

static unsigned int get_rr_interval_wrr(struct rq *rq, struct task_struct *task)
{
    // TODO: Fill me
}

static void update_curr_wrr(struct rq *rq)
{
    // do not need
}

static void task_change_group_wrr(struct task_struct *p, int type)
{
    // do not need
}
const struct sched_class wrr_sched_class = {
	.next			= &fair_sched_class, // O
	.enqueue_task		= enqueue_task_wrr,// O
	.dequeue_task		= dequeue_task_wrr, // O
	.yield_task		= yield_task_wrr, // O
	.yield_to_task		= yield_to_task_wrr, // O

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
#endif /* SMP */
*/
}

void init_wrr_rq(struct wrr_rq *wrr_rq)
{
	INIT_LIST_HEAD(wrr_rq -> front);
	load = 0;
}
