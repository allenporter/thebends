#ifndef	_zoo_user_
#define	_zoo_user_

/* Module zoo */

#include <string.h>
#include <mach/ndr.h>
#include <mach/boolean.h>
#include <mach/kern_return.h>
#include <mach/notify.h>
#include <mach/mach_types.h>
#include <mach/message.h>
#include <mach/mig_errors.h>
#include <mach/port.h>

#ifdef AUTOTEST
#ifndef FUNCTION_PTR_T
#define FUNCTION_PTR_T
typedef void (*function_ptr_t)(mach_port_t, char *, mach_msg_type_number_t);
typedef struct {
        char            *name;
        function_ptr_t  function;
} function_table_entry;
typedef function_table_entry 	*function_table_t;
#endif /* FUNCTION_PTR_T */
#endif /* AUTOTEST */

#ifndef	zoo_MSG_COUNT
#define	zoo_MSG_COUNT	1
#endif	/* zoo_MSG_COUNT */

#include <mach/std_types.h>
#include <mach/mig.h>

#ifdef __BeforeMigUserHeader
__BeforeMigUserHeader
#endif /* __BeforeMigUserHeader */

#include <sys/cdefs.h>
__BEGIN_DECLS


/* Routine add_two_numbers */
#ifdef	mig_external
mig_external
#else
extern
#endif	/* mig_external */
kern_return_t add_two_numbers
(
	mach_port_t server_port,
	int32_t a,
	int32_t b,
	int32_t *c
);

__END_DECLS

/********************** Caution **************************/
/* The following data types should be used to calculate  */
/* maximum message sizes only. The actual message may be */
/* smaller, and the position of the arguments within the */
/* message layout may vary from what is presented here.  */
/* For example, if any of the arguments are variable-    */
/* sized, and less than the maximum is sent, the data    */
/* will be packed tight in the actual message to reduce  */
/* the presence of holes.                                */
/********************** Caution **************************/

/* typedefs for all requests */

#ifndef __Request__zoo_subsystem__defined
#define __Request__zoo_subsystem__defined

#ifdef  __MigPackStructs
#pragma pack(4)
#endif
	typedef struct {
		mach_msg_header_t Head;
		NDR_record_t NDR;
		int32_t a;
		int32_t b;
	} __Request__add_two_numbers_t;
#ifdef  __MigPackStructs
#pragma pack()
#endif
#endif /* !__Request__zoo_subsystem__defined */

/* union of all requests */

#ifndef __RequestUnion__zoo_subsystem__defined
#define __RequestUnion__zoo_subsystem__defined
union __RequestUnion__zoo_subsystem {
	__Request__add_two_numbers_t Request_add_two_numbers;
};
#endif /* !__RequestUnion__zoo_subsystem__defined */
/* typedefs for all replies */

#ifndef __Reply__zoo_subsystem__defined
#define __Reply__zoo_subsystem__defined

#ifdef  __MigPackStructs
#pragma pack(4)
#endif
	typedef struct {
		mach_msg_header_t Head;
		NDR_record_t NDR;
		kern_return_t RetCode;
		int32_t c;
	} __Reply__add_two_numbers_t;
#ifdef  __MigPackStructs
#pragma pack()
#endif
#endif /* !__Reply__zoo_subsystem__defined */

/* union of all replies */

#ifndef __ReplyUnion__zoo_subsystem__defined
#define __ReplyUnion__zoo_subsystem__defined
union __ReplyUnion__zoo_subsystem {
	__Reply__add_two_numbers_t Reply_add_two_numbers;
};
#endif /* !__RequestUnion__zoo_subsystem__defined */

#ifndef subsystem_to_name_map_zoo
#define subsystem_to_name_map_zoo \
    { "add_two_numbers", 1000 }
#endif

#ifdef __AfterMigUserHeader
__AfterMigUserHeader
#endif /* __AfterMigUserHeader */

#endif	 /* _zoo_user_ */
