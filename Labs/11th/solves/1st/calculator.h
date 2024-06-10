/*
 * Please do not edit this file.
 * It was generated using rpcgen.
 */

#ifndef _CALCULATOR_H_RPCGEN
#define _CALCULATOR_H_RPCGEN

#include <rpc/rpc.h>


#ifdef __cplusplus
extern "C" {
#endif


struct request_t {
	double val1;
	double val2;
};
typedef struct request_t request_t;

struct reply_t {
	double result;
	int code;
};
typedef struct reply_t reply_t;

#define CALCULATOR 0x13371337
#define CALCULATOR_V_1 1

#if defined(__STDC__) || defined(__cplusplus)
#define add 1
extern  reply_t * add_1(request_t *, CLIENT *);
extern  reply_t * add_1_svc(request_t *, struct svc_req *);
#define deduct 2
extern  reply_t * deduct_1(request_t *, CLIENT *);
extern  reply_t * deduct_1_svc(request_t *, struct svc_req *);
#define divide 3
extern  reply_t * divide_1(request_t *, CLIENT *);
extern  reply_t * divide_1_svc(request_t *, struct svc_req *);
#define multiply 4
extern  reply_t * multiply_1(request_t *, CLIENT *);
extern  reply_t * multiply_1_svc(request_t *, struct svc_req *);
extern int calculator_1_freeresult (SVCXPRT *, xdrproc_t, caddr_t);

#else /* K&R C */
#define add 1
extern  reply_t * add_1();
extern  reply_t * add_1_svc();
#define deduct 2
extern  reply_t * deduct_1();
extern  reply_t * deduct_1_svc();
#define divide 3
extern  reply_t * divide_1();
extern  reply_t * divide_1_svc();
#define multiply 4
extern  reply_t * multiply_1();
extern  reply_t * multiply_1_svc();
extern int calculator_1_freeresult ();
#endif /* K&R C */

/* the xdr functions */

#if defined(__STDC__) || defined(__cplusplus)
extern  bool_t xdr_request_t (XDR *, request_t*);
extern  bool_t xdr_reply_t (XDR *, reply_t*);

#else /* K&R C */
extern bool_t xdr_request_t ();
extern bool_t xdr_reply_t ();

#endif /* K&R C */

#ifdef __cplusplus
}
#endif

#endif /* !_CALCULATOR_H_RPCGEN */
