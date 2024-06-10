#define OK 0 
#define ERROR 1
#define BUFFER_SIZE 64

struct request_t {
    int code;
    int iters;
};

struct reply_t {
    int code;
    double pi;
};

program PI_CALC {
    version PI_CALC_V_1 {
        reply_t calc_pi(request_t) = 1;
    } = 1;
} = 0x13371337;
