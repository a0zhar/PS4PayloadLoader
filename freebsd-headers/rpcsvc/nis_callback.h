#pragma once
/*
 * Please do not edit this file.
 * It was generated using rpcgen.
 */

#ifndef _NIS_CALLBACK_H_RPCGEN
#define	_NIS_CALLBACK_H_RPCGEN

#include <rpc/rpc.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Sun RPC is a product of Sun Microsystems, Inc. and is provided for
 * unrestricted use provided that this legend is included on all tape
 * media and as a part of the software program in whole or part.  Users
 * may copy or modify Sun RPC without charge, but are not authorized
 * to license or distribute it to anyone else except as part of a product or
 * program developed by the user or with the express written consent of
 * Sun Microsystems, Inc.
 *
 * SUN RPC IS PROVIDED AS IS WITH NO WARRANTIES OF ANY KIND INCLUDING THE
 * WARRANTIES OF DESIGN, MERCHANTIBILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE, OR ARISING FROM A COURSE OF DEALING, USAGE OR TRADE PRACTICE.
 *
 * Sun RPC is provided with no support and without any obligation on the
 * part of Sun Microsystems, Inc. to assist in its use, correction,
 * modification or enhancement.
 *
 * SUN MICROSYSTEMS, INC. SHALL HAVE NO LIABILITY WITH RESPECT TO THE
 * INFRINGEMENT OF COPYRIGHTS, TRADE SECRETS OR ANY PATENTS BY SUN RPC
 * OR ANY PART THEREOF.
 *
 * In no event will Sun Microsystems, Inc. be liable for any lost revenue
 * or profits or other special, indirect and consequential damages, even if
 * Sun has been advised of the possibility of such damages.
 *
 * Sun Microsystems, Inc.
 * 2550 Garcia Avenue
 * Mountain View, California  94043
 */
#include <rpcsvc/nis.h>

typedef nis_object *obj_p;

struct cback_data {
	struct {
		u_int entries_len;
		obj_p *entries_val;
	} entries;
};
typedef struct cback_data cback_data;

#define	CB_PROG ((unsigned long)(100302))
#define	CB_VERS ((unsigned long)(1))

extern  void cb_prog_1(struct svc_req *rqstp, SVCXPRT *transp);
#define	CBPROC_RECEIVE ((unsigned long)(1))
extern  bool_t * cbproc_receive_1(cback_data *, CLIENT *);
extern  bool_t * cbproc_receive_1_svc(cback_data *, struct svc_req *);
#define	CBPROC_FINISH ((unsigned long)(2))
extern  void * cbproc_finish_1(void *, CLIENT *);
extern  void * cbproc_finish_1_svc(void *, struct svc_req *);
#define	CBPROC_ERROR ((unsigned long)(3))
extern  void * cbproc_error_1(nis_error *, CLIENT *);
extern  void * cbproc_error_1_svc(nis_error *, struct svc_req *);
extern int cb_prog_1_freeresult(SVCXPRT *, xdrproc_t, caddr_t);

/* the xdr functions */
extern  bool_t xdr_obj_p(XDR *, obj_p*);
extern  bool_t xdr_cback_data(XDR *, cback_data*);

#ifdef __cplusplus
}
#endif

#endif /* !_NIS_CALLBACK_H_RPCGEN */
