typedef struct monitor monitor_t;

typedef struct condvar{
    semaphore_t sem;        // 条件变量所对应的信号量
    int count;              // 等待当前条件变量的等待进程总数
    monitor_t * owner;      // 当前条件变量的父管程
} condvar_t;

typedef struct monitor{
    semaphore_t mutex;      // 管程锁，每次只能有一个进程执行管程代码。该值初始化为1
    semaphore_t next;       // the next semaphore is used to down the signaling proc itself, and the other OR wakeuped waiting proc should wake up the sleeped signaling proc.
    int next_count;         // the number of of sleeped signaling proc
    condvar_t *cv;          // 当前管程中存放所有条件变量的数组
} monitor_t;


// Initialize monitor.
void monitor_init (monitor_t * mtp, size_t num_cv) {
    int i;
    assert(num_cv>0);
    mtp->next_count = 0;
    mtp->cv = NULL;
    // 初始化管程锁为1
    sem_init(&(mtp->mutex), 1); //unlocked
    sem_init(&(mtp->next), 0);// 注意这里的0
    // 分配当前管程内的条件变量
    mtp->cv =(condvar_t *) kmalloc(sizeof(condvar_t)*num_cv);
    assert(mtp->cv!=NULL);
    // 初始化管程内条件变量的各个属性
    for(i=0; i<num_cv; i++){
        mtp->cv[i].count=0;
        sem_init(&(mtp->cv[i].sem),0);
        mtp->cv[i].owner=mtp;
    }
}



// Unlock one of threads waiting on the condition variable.
void cond_signal (condvar_t *cvp) {
    //LAB7 EXERCISE1: YOUR CODE
    cprintf("cond_signal begin: cvp %x, cvp->count %d, cvp->owner->next_count %d\n", cvp, cvp->count, cvp->owner->next_count);
/*
    *      cond_signal(cv) {
    *          if(cv.count>0) {
    *             mt.next_count ++;
    *             signal(cv.sem);
    *             wait(mt.next);
    *             mt.next_count--;
*          }
    *       }
    */
    if(cvp->count>0) {
        cvp->owner->next_count ++;
        up(&(cvp->sem));
        down(&(cvp->owner->next));
        cvp->owner->next_count --;
    }
    cprintf("cond_signal end: cvp %x, cvp->count %d, cvp->owner->next_count %d\n", cvp, cvp->count, cvp->owner->next_count);
}


// Suspend calling thread on a condition variable waiting for condition Atomically unlocks
// mutex and suspends calling thread on conditional variable after waking up locks mutex. Notice: mp is mutex semaphore for monitor's procedures
void
cond_wait (condvar_t *cvp) {
    //LAB7 EXERCISE1: YOUR CODE
    cprintf("cond_wait begin:  cvp %x, cvp->count %d, cvp->owner->next_count %d\n", cvp, cvp->count, cvp->owner->next_count);
    /*
    *         cv.count ++;
    *         if(mt.next_count>0)
    *            signal(mt.next)
    *         else
    *            signal(mt.mutex);
    *         wait(cv.sem);
    *         cv.count --;
    */
    cvp->count++;
    if(cvp->owner->next_count > 0)
        up(&(cvp->owner->next));
    else
        up(&(cvp->owner->mutex));
    down(&(cvp->sem));
    cvp->count--;

    cprintf("cond_wait end:  cvp %x, cvp->count %d, cvp->owner->next_count %d\n", cvp, cvp->count, cvp->owner->next_count);
}


void monitorFunc() {
    down(&(mtp->mutex));
//--------into routine in monitor--------------
    // ...
//--------leave routine in monitor--------------
    if(mtp->next_count>0)
        up(&(mtp->next));
    else
        up(&(mtp->mutex));
}



void phi_take_forks_condvar(int i) {
    down(&(mtp->mutex));
//--------into routine in monitor--------------
    // LAB7 EXERCISE1: YOUR CODE
    // I am hungry
    state_condvar[i]=HUNGRY; /* 记录下哲学家i饥饿的事实 */
    // try to get fork
    phi_test_condvar(i);
    if (state_condvar[i] != EATING) {
        cprintf("phi_take_forks_condvar: %d didn't get fork and will wait\n",i);
        cond_wait(&mtp->cv[i]);
    }
//--------leave routine in monitor--------------
    if(mtp->next_count>0)
        up(&(mtp->next));
    else
        up(&(mtp->mutex));
}



void phi_put_forks_condvar(int i) {
    down(&(mtp->mutex));
//--------into routine in monitor--------------
    // LAB7 EXERCISE1: YOUR CODE
    // I ate over
    state_condvar[i]=THINKING; /* 哲学家进餐结束 */
    // test left and right neighbors
    phi_test_condvar(LEFT); /* 看一下左邻居现在是否能进餐 */
    phi_test_condvar(RIGHT); /* 看一下右邻居现在是否能进餐 */
//--------leave routine in monitor--------------
    if(mtp->next_count>0)
        up(&(mtp->next));
    else
        up(&(mtp->mutex));
}


void phi_test_condvar (i) {
    if(state_condvar[i]==HUNGRY&&state_condvar[LEFT]!=EATING
            &&state_condvar[RIGHT]!=EATING) {
        cprintf("phi_test_condvar: state_condvar[%d] will eating\n",i);
        state_condvar[i] = EATING ;
        cprintf("phi_test_condvar: signal self_cv[%d] \n",i);
        cond_signal(&mtp->cv[i]) ;
    }
}



