typedef int groupid_t;

struct worker_args_t {
    int rank;
    int thread_iters;
    int iters_max;
};

struct master_args_t {
    groupid_t groupid;
    int thread_num;
    double sum;
};

program MAS_WOR {
    version MAS_WOR_V_1 {
        groupid_t get_groupid () = 1;
        worker_args_t assign_to_task (master_args_t) = 2;
        int send_sum (master_args_t) = 3;
        int mark_as_finished (groupid_t) = 4;
    } = 1;
} = 0x123456;
