/*
 * Please do not edit this file.
 * It was generated using rpcgen.
 */

#include "calc_pi.h"

bool_t
xdr_request_t (XDR *xdrs, request_t *objp)
{
	register int32_t *buf;

	 if (!xdr_int (xdrs, &objp->code))
		 return FALSE;
	 if (!xdr_int (xdrs, &objp->iters))
		 return FALSE;
	return TRUE;
}

bool_t
xdr_reply_t (XDR *xdrs, reply_t *objp)
{
	register int32_t *buf;

	 if (!xdr_int (xdrs, &objp->code))
		 return FALSE;
	 if (!xdr_double (xdrs, &objp->pi))
		 return FALSE;
	return TRUE;
}
