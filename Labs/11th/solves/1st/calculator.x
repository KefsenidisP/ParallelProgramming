struct request_t {
    double val1;
    double val2;
};

struct reply_t {
    double result;
    int code;
};

program CALCULATOR {
    version CALCULATOR_V_1 {
        reply_t add(request_t) = 1;
        reply_t deduct(request_t) = 2;
        reply_t divide(request_t) = 3;
        reply_t multiply(request_t) = 4;
    } = 1;
} = 0x13371337;
