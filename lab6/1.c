/*Initialize semaphore object SEM to VALUE.  If PSHARED then share it
    with other processes.  */
int sem_init (sem_t *__sem, int __pshared, unsigned int __value);
/* Free resources associated with semaphore object SEM.  */
// 将信号量所使用的资源全部释放
int sem_destroy (sem_t *__sem);

/* Open a named semaphore NAME with open flags OFLAG.  */
// 开启一个新信号量，并使用给定的flag来指定其标志
sem_t *sem_open (const char *__name, int __oflag, ...);

/* Close descriptor for named semaphore SEM.  */
// 将当前信号量所使用的描述符关闭
int sem_close (sem_t *__sem);

/* Remove named semaphore NAME.  */
int sem_unlink (const char *__name);

/* Wait for SEM being posted.

    This function is a cancellation point and therefore not marked with
    __THROW.  */
// 一个P操作，如果sem value > 0，则sem value--；否则阻塞直到sem value > 0
int sem_wait (sem_t *__sem);

/* Test whether SEM is posted.  */
int sem_trywait (sem_t *__sem);

/* Post SEM.  */
// 一个V操作，把指定的信号量 sem 的值加 1，唤醒正在等待该信号量的任意线程。
int sem_post (sem_t *__sem);

/* Get current value of SEM and store it in *SVAL.  */
// 获取当前信号量的值
int sem_getvalue (sem_t *__restrict __sem, int *__restrict __sval);