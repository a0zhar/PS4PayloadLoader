#pragma once
/*
 * Please do not edit this file.
 * It was generated using rpcgen.
 */

#ifndef _KLM_PROT_H_RPCGEN
#define	_KLM_PROT_H_RPCGEN

#include <rpc/rpc.h>

#ifdef __cplusplus
extern "C" {
#endif

#define	LM_MAXSTRLEN 1024

enum klm_stats {
	klm_granted = 0,
	klm_denied = 1,
	klm_denied_nolocks = 2,
	klm_working = 3
};
typedef enum klm_stats klm_stats;

struct klm_lock {
	char *server_name;
	netobj fh;
	int pid;
	u_int l_offset;
	u_int l_len;
};
typedef struct klm_lock klm_lock;

struct klm_holder {
	bool_t exclusive;
	int svid;
	u_int l_offset;
	u_int l_len;
};
typedef struct klm_holder klm_holder;

struct klm_stat {
	klm_stats stat;
};
typedef struct klm_stat klm_stat;

struct klm_testrply {
	klm_stats stat;
	union {
		struct klm_holder holder;
	} klm_testrply_u;
};
typedef struct klm_testrply klm_testrply;

struct klm_lockargs {
	bool_t block;
	bool_t exclusive;
	struct klm_lock alock;
};
typedef struct klm_lockargs klm_lockargs;

struct klm_testargs {
	bool_t exclusive;
	struct klm_lock alock;
};
typedef struct klm_testargs klm_testargs;

struct klm_unlockargs {
	struct klm_lock alock;
};
typedef struct klm_unlockargs klm_unlockargs;

#define	KLM_PROG ((unsigned long)(100020))
#define	KLM_VERS ((unsigned long)(1))

extern  void klm_prog_1(struct svc_req *rqstp, SVCXPRT *transp);
#define	KLM_TEST ((unsigned long)(1))
extern  klm_testrply * klm_test_1(struct klm_testargs *, CLIENT *);
extern  klm_testrply * klm_test_1_svc(struct klm_testargs *, struct svc_req *);
#define	KLM_LOCK ((unsigned long)(2))
extern  klm_stat * klm_lock_1(struct klm_lockargs *, CLIENT *);
extern  klm_stat * klm_lock_1_svc(struct klm_lockargs *, struct svc_req *);
#define	KLM_CANCEL ((unsigned long)(3))
extern  klm_stat * klm_cancel_1(struct klm_lockargs *, CLIENT *);
extern  klm_stat * klm_cancel_1_svc(struct klm_lockargs *, struct svc_req *);
#define	KLM_UNLOCK ((unsigned long)(4))
extern  klm_stat * klm_unlock_1(struct klm_unlockargs *, CLIENT *);
extern  klm_stat * klm_unlock_1_svc(struct klm_unlockargs *, struct svc_req *);
extern int klm_prog_1_freeresult(SVCXPRT *, xdrproc_t, caddr_t);

/* the xdr functions */
extern  bool_t xdr_klm_stats(XDR *, klm_stats*);
extern  bool_t xdr_klm_lock(XDR *, klm_lock*);
extern  bool_t xdr_klm_holder(XDR *, klm_holder*);
extern  bool_t xdr_klm_stat(XDR *, klm_stat*);
extern  bool_t xdr_klm_testrply(XDR *, klm_testrply*);
extern  bool_t xdr_klm_lockargs(XDR *, klm_lockargs*);
extern  bool_t xdr_klm_testargs(XDR *, klm_testargs*);
extern  bool_t xdr_klm_unlockargs(XDR *, klm_unlockargs*);

#ifdef __cplusplus
}
#endif

#endif /* !_KLM_PROT_H_RPCGEN */
