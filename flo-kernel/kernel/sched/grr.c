/* new GRR schedule */
#include "sched.h"

static void dequeue_task_grr(struct rq *rq, struct task_struct *p, int flags)
{
	printk("entering dequeue...\n");	
	struct sched_grr_entity *grr_se = &p->grr;
	struct grr_rq *grr_rq = &rq->grr;

	raw_spin_lock(&grr_rq->grr_rq_lock);
	list_del_init(&grr_se->run_list);
	--grr_rq->grr_nr_running;
	raw_spin_unlock(&grr_rq->grr_rq_lock);
	dec_nr_running(rq);
	printk("leaving dequeue...\n");
}

static void enqueue_task_grr(struct rq *rq, struct task_struct *p, int flags)
{
	printk("entering enqueue...\n");
	/* maybe this can simplified 
	struct sched_grr_entity *grr_se = &p->grr;
	struct task_struct *p = container_of(grr_se, struct task_struct, rt);
	struct rq *rq = task_rq(p);
	struct grr_rq *grr_rq = rq->grr;
	struct list_head *queue = grr_rq->grr_rq_list;
	list_add(&grr_se->run_list, queue);
	grr_rq->nr_running++; 	
	*/

	struct sched_grr_entity *grr_se = &p->grr;
	struct grr_rq *grr_rq = &rq->grr;
	struct list_head *queue = &grr_rq->grr_rq_list;

	/*Ethan: I changed list_add to list_add_tail.*/
	raw_spin_lock(&grr_rq->grr_rq_lock);
	list_add_tail(&grr_se->run_list, queue);
	grr_rq->grr_nr_running++;
	raw_spin_unlock(&grr_rq->grr_rq_lock);	
	inc_nr_running(rq);
	printk("leaving enqueue...\n");
}

static void requeue_task_grr(struct rq *rq, struct task_struct *p, int flags)
{
	printk("entering requeue...\n");
	struct sched_grr_entity *grr_se = &p->grr;
	struct grr_rq *grr_rq = &rq->grr;
	struct list_head *head = &grr_rq->grr_rq_list;

	if (sizeof(grr_rq->grr_nr_running) == 1)
		return;
	
	raw_spin_lock(&grr_rq->grr_rq_lock);
	list_move_tail(&grr_se->run_list, head);
	raw_spin_unlock(&grr_rq->grr_rq_lock);
	printk("leaving requeue...\n");
}

static void yield_task_grr(struct rq *rq)
{
	requeue_task_grr(rq, rq->curr , 0);
}

static struct task_struct *pick_next_task_grr(struct rq *rq)
{
	printk("entering pick_next_task_grr...\n");
	struct sched_grr_entity *grr_se;
	struct task_struct *p;
	struct grr_rq *grr_rq;

	grr_rq = &rq->grr;

	if(!grr_rq->grr_nr_running)
		return NULL;

	struct list_head *queue = &grr_rq->grr_rq_list;
	grr_se = list_entry(queue->next, struct sched_grr_entity, run_list);
	return container_of(grr_se, struct task_struct, grr);
	printk("leaving pick_next_task_grr...\n");
}

static void put_prev_task_grr(struct rq *rq, struct task_struct *prev)
{
}

static void task_tick_grr(struct rq *rq, struct task_struct *p, int queued)
{
	struct sched_grr_entity *grr_se = &p->grr;

	if (--p->grr.time_slice)
		return;

	p->grr.time_slice = GRR_TIMESLICE;

	if (grr_se->run_list.prev != grr_se->run_list.next) {
		requeue_task_grr(rq, p, 0);
		set_tsk_need_resched(p);
		return;
	}
}

static void set_curr_task_grr(struct rq *rq)
{
}

static void check_preempt_curr_grr(struct rq *rq,
				struct task_struct *p, int flags)
{
}

static void switched_to_grr(struct rq *rq, struct task_struct *p)
{
}

static void prio_changed_grr(struct rq *rq, struct task_struct *p, int old)
{
}

static unsigned int get_rr_interval_grr(struct rq *rq, struct task_struct *t)
{
}

void init_grr_rq(struct grr_rq *grr_rq)
{
	printk("entering init_grr_rq...\n");
	grr_rq->grr_nr_running = 0;
	INIT_LIST_HEAD(&grr_rq->grr_rq_list);
	raw_spin_lock_init(&grr_rq->grr_rq_lock);
	printk("leaving init_grr_rq...\n");
}

const struct sched_class grr_sched_class = {
	.next			= &fair_sched_class,
	.dequeue_task		= dequeue_task_grr,
	.enqueue_task		= enqueue_task_grr,
	.yield_task		= yield_task_grr,
	.check_preempt_curr	= check_preempt_curr_grr,
	.pick_next_task		= pick_next_task_grr,
	.put_prev_task		= put_prev_task_grr,

#ifdef CONFIG_SMP
	.select_task_rq		= select_task_rq_grr,
	.pre_schedule		= pre_schedule_grr,
	.post_schedule		= post_schedule_grr,
#endif

	.set_curr_task		= set_curr_task_grr,
	.task_tick		= task_tick_grr,
	
	.get_rr_interval	= get_rr_interval_grr,

	.prio_changed		= prio_changed_grr,
	.switched_to		= switched_to_grr,
};
