 ;
 ; >>> START OF FILE
 ;
 ; telephony
 ; (__TEXT, __text)
 ;
 ;
 ;
 start:
 ;  [ MODULE ENTRY POINT ]
 ;
 +00000 00001b50 00009DE4             ldr               r0,[sp]
 +00004 00001b54 04108DE2             add               r1,sp,#0x4
 +00008 00001b58 014080E2             add               r4,r0,#0x1
 +0000c 00001b5c 042181E0             add               r2,r1,r4,lsl #2
 +00010 00001b60 07D0CDE3             bic               sp,sp,#0x7
 +00014 00001b64 0A0000EB             bl                _start
 +00018 00001b68 700020E1             bkpt              0x0000
 ;
 ;
 dyld_stub_binding_helper:
 ;
 +00000 00001b6c 04C02DE5             str               ip,[sp,#-0x4]!
 +00004 00001b70 08C09FE5             ldr               ip,[pc,#0x8]                      ; -> 0x1b80 _mh_execute_header
 +00008 00001b74 04C02DE5             str               ip,[sp,#-0x4]!
 +0000c 00001b78 04C09FE5             ldr               ip,[pc,#0x4]                      ; -> 0x1b84 dyld_content_lazy_binder
 +00010 00001b7c 00F09CE4             ldr               pc,[ip]                           ; call -> ?
 +00014 00001b80 00100000 
 +00018 00001b84 04200000 
 ;
 ;
 _dyld_func_lookup:
 ;
 ;	X-REFS:
 ;
 ;	_start
 ;
 +00000 00001b88 00C09FE5             ldr               ip,[pc,#0x0]                      ; -> 0x1b90 dyld_func_lookup_pointer "\x08\x10\xe0\x8fl\x1b"
 +00004 00001b8c 00F09CE4             ldr               pc,[ip]                           ; call -> ?
 +00008 00001b90 08200000 
 ;
 ;
 _start:
 ;
 ;	X-REFS:
 ;
 ;	start
 ;
 +00000 00001b94 80402DE9             stmdb             sp!,{r7,lr}
 +00004 00001b98 0D70A0E1             mov               r7,sp
 +00008 00001b9c 1CD04DE2             sub               sp,sp,#0x1c
 +0000c 00001ba0 18008DE5             str               r0,[sp,#0x18]
 +00010 00001ba4 14108DE5             str               r1,[sp,#0x14]
 +00014 00001ba8 10208DE5             str               r2,[sp,#0x10]
 +00018 00001bac 30319FE5             ldr               r3,[pc,#0x130]                    ; -> 0x1ce4 NXArgc
 +0001c 00001bb0 18209DE5             ldr               r2,[sp,#0x18]
 +00020 00001bb4 002083E4             str               r2,[r3]
 +00024 00001bb8 28319FE5             ldr               r3,[pc,#0x128]                    ; -> 0x1ce8 NXArgv
 +00028 00001bbc 14209DE5             ldr               r2,[sp,#0x14]
 +0002c 00001bc0 002083E4             str               r2,[r3]
 +00030 00001bc4 20319FE5             ldr               r3,[pc,#0x120]                    ; -> 0x1cec environ
 +00034 00001bc8 10209DE5             ldr               r2,[sp,#0x10]
 +00038 00001bcc 002083E4             str               r2,[r3]
 +0003c 00001bd0 14309DE5             ldr               r3,[sp,#0x14]
 +00040 00001bd4 003093E4             ldr               r3,[r3]
 +00044 00001bd8 000053E3             cmp               r3,#0x0
 +00048 00001bdc 3B00001A             bne               loc_00013c
 +0004c 00001be0 08319FE5             ldr               r3,[pc,#0x108]                    ; -> 0x1cf0 .str
 +00050 00001be4 0C308DE5             str               r3,[sp,#0xc]
 +00054 00001be8 0C309DE5 loc_000054: ldr               r3,[sp,#0xc]                      ; .str
 +00058 00001bec 00219FE5             ldr               r2,[pc,#0x100]                    ; -> 0x1cf4 __progname
 +0005c 00001bf0 003082E4             str               r3,[r2]
 +00060 00001bf4 10309DE5             ldr               r3,[sp,#0x10]
 +00064 00001bf8 08308DE5             str               r3,[sp,#0x8]
 +00068 00001bfc 020000EA             b                 loc_000078
 +0006c 00001c00 08309DE5 loc_00006c: ldr               r3,[sp,#0x8]
 +00070 00001c04 043083E2             add               r3,r3,#0x4
 +00074 00001c08 08308DE5             str               r3,[sp,#0x8]
 +00078 00001c0c 08309DE5 loc_000078: ldr               r3,[sp,#0x8]
 +0007c 00001c10 003093E4             ldr               r3,[r3]
 +00080 00001c14 000053E3             cmp               r3,#0x0
 +00084 00001c18 F8FFFF1A             bne               loc_00006c
 +00088 00001c1c 08309DE5             ldr               r3,[sp,#0x8]
 +0008c 00001c20 043083E2             add               r3,r3,#0x4
 +00090 00001c24 08308DE5             str               r3,[sp,#0x8]
 +00094 00001c28 C8309FE5             ldr               r3,[pc,#0xc8]                     ; -> 0x1cf8 mach_init_routine
 +00098 00001c2c 003093E4             ldr               r3,[r3]
 +0009c 00001c30 003093E4             ldr               r3,[r3]
 +000a0 00001c34 000053E3             cmp               r3,#0x0
 +000a4 00001c38 0300000A             beq               loc_0000b8
 +000a8 00001c3c B4309FE5             ldr               r3,[pc,#0xb4]                     ; -> 0x1cf8 mach_init_routine
 +000ac 00001c40 003093E4             ldr               r3,[r3]
 +000b0 00001c44 003093E4             ldr               r3,[r3]
 +000b4 00001c48 33FF2FE1             blx               r3                                ; call -> ?
 +000b8 00001c4c A8309FE5 loc_0000b8: ldr               r3,[pc,#0xa8]                     ; -> 0x1cfc _cthread_init_routine
 +000bc 00001c50 003093E4             ldr               r3,[r3]
 +000c0 00001c54 003093E4             ldr               r3,[r3]
 +000c4 00001c58 000053E3             cmp               r3,#0x0
 +000c8 00001c5c 0300000A             beq               loc_0000dc
 +000cc 00001c60 94309FE5             ldr               r3,[pc,#0x94]                     ; -> 0x1cfc _cthread_init_routine
 +000d0 00001c64 003093E4             ldr               r3,[r3]
 +000d4 00001c68 003093E4             ldr               r3,[r3]
 +000d8 00001c6c 33FF2FE1             blx               r3                                ; call -> ?
 +000dc 00001c70 C40000EB loc_0000dc: bl                __keymgr_dwarf2_register_sections (stub)
 +000e0 00001c74 0D10A0E1             mov               r1,sp
 +000e4 00001c78 80009FE5             ldr               r0,[pc,#0x80]                     ; -> 0x1d00 .str1 "__dyld_make_delayed_module_initializer_calls"
 +000e8 00001c7c C1FFFFEB             bl                _dyld_func_lookup
 +000ec 00001c80 00309DE4             ldr               r3,[sp]
 +000f0 00001c84 33FF2FE1             blx               r3                                ; call -> ?
 +000f4 00001c88 04108DE2             add               r1,sp,#0x4
 +000f8 00001c8c 70009FE5             ldr               r0,[pc,#0x70]                     ; -> 0x1d04 .str2 "__dyld_mod_term_funcs"
 +000fc 00001c90 BCFFFFEB             bl                _dyld_func_lookup
 +00100 00001c94 04309DE5             ldr               r3,[sp,#0x4]
 +00104 00001c98 000053E3             cmp               r3,#0x0
 +00108 00001c9c 0100000A             beq               loc_000114
 +0010c 00001ca0 04009DE5             ldr               r0,[sp,#0x4]
 +00110 00001ca4 BA0000EB             bl                atexit (stub)
 +00114 00001ca8 58309FE5 loc_000114: ldr               r3,[pc,#0x58]                     ; -> 0x1d08 errno
 +00118 00001cac 003093E4             ldr               r3,[r3]
 +0011c 00001cb0 0020A0E3             mov               r2,#0x0
 +00120 00001cb4 002083E4             str               r2,[r3]
 +00124 00001cb8 08309DE5             ldr               r3,[sp,#0x8]
 +00128 00001cbc 14109DE5             ldr               r1,[sp,#0x14]
 +0012c 00001cc0 10209DE5             ldr               r2,[sp,#0x10]
 +00130 00001cc4 18009DE5             ldr               r0,[sp,#0x18]
 +00134 00001cc8 4E0000EB             bl                main
 +00138 00001ccc B30000EB             bl                exit (stub)
 +0013c 00001cd0 14309DE5 loc_00013c: ldr               r3,[sp,#0x14]
 +00140 00001cd4 000093E4             ldr               r0,[r3]
 +00144 00001cd8 0B0000EB             bl                crt_basename
 +00148 00001cdc 0C008DE5             str               r0,[sp,#0xc]
 +0014c 00001ce0 C0FFFFEA             b                 loc_000054
 +00150 00001ce4 00300000 
 +00154 00001ce8 04300000 
 +00158 00001cec 08300000 
 +0015c 00001cf0 3C200000 
 +00160 00001cf4 0C300000 
 +00164 00001cf8 34200000 
 +00168 00001cfc 2C200000 
 +0016c 00001d00 101F0000 
 +00170 00001d04 3D1F0000 
 +00174 00001d08 30200000 
 ;
 ;
 crt_basename:
 ;
 ;	X-REFS:
 ;
 ;	_start
 ;
 +00000 00001d0c 14D04DE2             sub               sp,sp,#0x14
 +00004 00001d10 10008DE5             str               r0,[sp,#0x10]
 +00008 00001d14 00008DE4             str               r0,[sp]
 +0000c 00001d18 10309DE5             ldr               r3,[sp,#0x10]
 +00010 00001d1c 04308DE5             str               r3,[sp,#0x4]
 +00014 00001d20 090000EA             b                 loc_000040
 +00018 00001d24 04309DE5 loc_000018: ldr               r3,[sp,#0x4]
 +0001c 00001d28 0030D3E4             ldrb              r3,[r3]
 +00020 00001d2c 2F0053E3             cmp               r3,#0x2f
 +00024 00001d30 0200001A             bne               loc_000034
 +00028 00001d34 04309DE5             ldr               r3,[sp,#0x4]
 +0002c 00001d38 013083E2             add               r3,r3,#0x1
 +00030 00001d3c 00308DE4             str               r3,[sp]
 +00034 00001d40 04309DE5 loc_000034: ldr               r3,[sp,#0x4]
 +00038 00001d44 013083E2             add               r3,r3,#0x1
 +0003c 00001d48 04308DE5             str               r3,[sp,#0x4]
 +00040 00001d4c 04309DE5 loc_000040: ldr               r3,[sp,#0x4]
 +00044 00001d50 0030D3E4             ldrb              r3,[r3]
 +00048 00001d54 000053E3             cmp               r3,#0x0
 +0004c 00001d58 F1FFFF1A             bne               loc_000018
 +00050 00001d5c 00309DE4             ldr               r3,[sp]
 +00054 00001d60 08308DE5             str               r3,[sp,#0x8]
 +00058 00001d64 0C308DE5             str               r3,[sp,#0xc]
 +0005c 00001d68 0C009DE5             ldr               r0,[sp,#0xc]
 +00060 00001d6c 14D08DE2             add               sp,sp,#0x14
 +00064 00001d70 1EFF2FE1             bx                lr                                ; return
 ;
 ;
 DumpInt:
 ;  [ EXTERNAL ]
 ;
 +00000 00001d74 08D04DE2             sub               sp,sp,#0x8
 +00004 00001d78 04E08DE5             str               lr,[sp,#0x4]
 +00008 00001d7c 00708DE4             str               r7,[sp]
 +0000c 00001d80 0D70A0E1             mov               r7,sp
 +00010 00001d84 08D04DE2             sub               sp,sp,#0x8
 +00014 00001d88 0010A0E1             mov               r1,r0
 +00018 00001d8c 041007E5             str               r1,[r7,#-0x4]
 +0001c 00001d90 2C309FE5             ldr               r3,[pc,#0x2c]                     ; -> 0x1dc4
 +00020 00001d94 081007E5             str               r1,[r7,#-0x8]
 +00024 00001d98 03008FE0             add               r0,pc,r3                          ; .str "int = %d\n"
 +00028 00001d9c 8A0000EB             bl                printf (stub)
 +0002c 00001da0 081017E5             ldr               r1,[r7,#-0x8]
 +00030 00001da4 1C309FE5             ldr               r3,[pc,#0x1c]                     ; -> 0x1dc8
 +00034 00001da8 03008FE0             add               r0,pc,r3                          ; .str1 "    = 0x%08x\n"
 +00038 00001dac 860000EB             bl                printf (stub)
 +0003c 00001db0 07D0A0E1             mov               sp,r7
 +00040 00001db4 00709DE4             ldr               r7,[sp]
 +00044 00001db8 04E09DE5             ldr               lr,[sp,#0x4]
 +00048 00001dbc 08D08DE2             add               sp,sp,#0x8
 +0004c 00001dc0 1EFF2FE1             bx                lr                                ; return
 +00050 00001dc4 B3010000 
 +00054 00001dc8 AD010000 
 ;
 ;
 CreateCallback:
 ;  [ EXTERNAL ]
 ;
 +00000 00001dcc 08D04DE2             sub               sp,sp,#0x8
 +00004 00001dd0 04E08DE5             str               lr,[sp,#0x4]
 +00008 00001dd4 00708DE4             str               r7,[sp]
 +0000c 00001dd8 0D70A0E1             mov               r7,sp
 +00010 00001ddc 04D04DE2             sub               sp,sp,#0x4
 +00014 00001de0 1C309FE5             ldr               r3,[pc,#0x1c]                     ; -> 0x1e04
 +00018 00001de4 040007E5             str               r0,[r7,#-0x4]
 +0001c 00001de8 03008FE0             add               r0,pc,r3                          ; .str2 "Foo! called"
 +00020 00001dec 7E0000EB             bl                puts (stub)
 +00024 00001df0 07D0A0E1             mov               sp,r7
 +00028 00001df4 00709DE4             ldr               r7,[sp]
 +0002c 00001df8 04E09DE5             ldr               lr,[sp,#0x4]
 +00030 00001dfc 08D08DE2             add               sp,sp,#0x8
 +00034 00001e00 1EFF2FE1             bx                lr                                ; return
 +00038 00001e04 7B010000 
 ;
 ;
 main:
 ;  [ EXTERNAL ]
 ;
 ;	X-REFS:
 ;
 ;	_start
 ;
 +00000 00001e08 14D04DE2             sub               sp,sp,#0x14
 +00004 00001e0c 10E08DE5             str               lr,[sp,#0x10]
 +00008 00001e10 0C708DE5             str               r7,[sp,#0xc]
 +0000c 00001e14 08608DE5             str               r6,[sp,#0x8]
 +00010 00001e18 04508DE5             str               r5,[sp,#0x4]
 +00014 00001e1c 00408DE4             str               r4,[sp]
 +00018 00001e20 0C708DE2             add               r7,sp,#0xc
 +0001c 00001e24 43DF4DE2             sub               sp,sp,#0x10c
 +00020 00001e28 C8309FE5             ldr               r3,[pc,#0xc8]                     ; -> 0x1ef8
 +00024 00001e2c 03008FE0             add               r0,pc,r3                          ; .str3 "Create"
 +00028 00001e30 6D0000EB             bl                puts (stub)
 +0002c 00001e34 FF20A0E3             mov               r2,#0xff
 +00030 00001e38 0040A0E3             mov               r4,#0x0
 +00034 00001e3c 115E47E2             sub               r5,r7,#0x110
 +00038 00001e40 B4609FE5             ldr               r6,[pc,#0xb4]                     ; -> 0x1efc
 +0003c 00001e44 0500A0E1             mov               r0,r5
 +00040 00001e48 0410A0E1             mov               r1,r4
 +00044 00001e4c 5A0000EB             bl                memset (stub)
 +00048 00001e50 06309FE7             ldr               r3,[pc,r6]                        ; -> kCFAllocatorDefault
 +0004c 00001e54 A4209FE5             ldr               r2,[pc,#0xa4]                     ; -> 0x1f00
 +00050 00001e58 000093E4             ldr               r0,[r3]
 +00054 00001e5c 02108FE0             add               r1,pc,r2                          ; CreateCallback
 +00058 00001e60 0520A0E1             mov               r2,r5
 +0005c 00001e64 500000EB             bl                _CTServerConnectionCreate (stub)
 +00060 00001e68 140107E5             str               r0,[r7,#-0x114]
 +00064 00001e6c 184107E5             str               r4,[r7,#-0x118]
 +00068 00001e70 080000EA             b                 loc_000090
 +0006c 00001e74 183117E5 loc_00006c: ldr               r3,[r7,#-0x118]
 +00070 00001e78 112E47E2             sub               r2,r7,#0x110
 +00074 00001e7c D31092E1             ldrsb             r1,[r2,r3]
 +00078 00001e80 7C309FE5             ldr               r3,[pc,#0x7c]                     ; -> 0x1f04
 +0007c 00001e84 03008FE0             add               r0,pc,r3                          ; .str4 " %x "
 +00080 00001e88 4F0000EB             bl                printf (stub)
 +00084 00001e8c 183117E5             ldr               r3,[r7,#-0x118]
 +00088 00001e90 013083E2             add               r3,r3,#0x1
 +0008c 00001e94 183107E5             str               r3,[r7,#-0x118]
 +00090 00001e98 183117E5 loc_000090: ldr               r3,[r7,#-0x118]
 +00094 00001e9c FF0053E3             cmp               r3,#0xff
 +00098 00001ea0 F3FFFFBA             blt               loc_00006c
 +0009c 00001ea4 0A00A0E3             mov               r0,#0xa
 +000a0 00001ea8 4B0000EB             bl                putchar (stub)
 +000a4 00001eac 143117E5             ldr               r3,[r7,#-0x114]
 +000a8 00001eb0 000053E3             cmp               r3,#0x0
 +000ac 00001eb4 0300000A             beq               loc_0000c0
 +000b0 00001eb8 48309FE5             ldr               r3,[pc,#0x48]                     ; -> 0x1f08
 +000b4 00001ebc 03008FE0             add               r0,pc,r3                          ; .str5 "z"
 +000b8 00001ec0 490000EB             bl                puts (stub)
 +000bc 00001ec4 020000EA             b                 loc_0000cc
 +000c0 00001ec8 3C309FE5 loc_0000c0: ldr               r3,[pc,#0x3c]                     ; -> 0x1f0c
 +000c4 00001ecc 03008FE0             add               r0,pc,r3                          ; .str6 "nz"
 +000c8 00001ed0 450000EB             bl                puts (stub)
 +000cc 00001ed4 100017E5 loc_0000cc: ldr               r0,[r7,#-0x10]
 +000d0 00001ed8 0CD047E2             sub               sp,r7,#0xc
 +000d4 00001edc 00409DE4             ldr               r4,[sp]
 +000d8 00001ee0 04509DE5             ldr               r5,[sp,#0x4]
 +000dc 00001ee4 08609DE5             ldr               r6,[sp,#0x8]
 +000e0 00001ee8 0C709DE5             ldr               r7,[sp,#0xc]
 +000e4 00001eec 10E09DE5             ldr               lr,[sp,#0x10]
 +000e8 00001ef0 14D08DE2             add               sp,sp,#0x14
 +000ec 00001ef4 1EFF2FE1             bx                lr                                ; return
 +000f0 00001ef8 43010000 
 +000f4 00001efc E0010000 
 +000f8 00001f00 68FFFFFF 
 +000fc 00001f04 F2000000 
 +00100 00001f08 BF000000 
 +00104 00001f0c B1000000 
 ;
 ;
 ;	STUBS X-REFS:
 ;
 ; _CTServerConnectionCreate (fe000000) -> CoreTelephony
 ; _CTServerConnectionCreate (00001fac) -> CoreTelephony
 ;		main
 ; __keymgr_dwarf2_register_sections (00001f88) -> libSystem.B.dylib
 ;		_start
 ; __keymgr_dwarf2_register_sections (fe000008) -> libSystem.B.dylib
 ; _cthread_init_routine (fe000010) -> libSystem.B.dylib
 ; atexit (fe000018) -> libSystem.B.dylib
 ; atexit (00001f94) -> libSystem.B.dylib
 ;		_start
 ; errno (fe000020) -> libSystem.B.dylib
 ; exit (00001fa0) -> libSystem.B.dylib
 ;		_start
 ; exit (fe000028) -> libSystem.B.dylib
 ; kCFAllocatorDefault (fe000030) -> CoreFoundation
 ; mach_init_routine (fe000038) -> libSystem.B.dylib
 ; memset (fe000040) -> libSystem.B.dylib
 ; memset (00001fbc) -> libSystem.B.dylib
 ;		main
 ; printf (fe000048) -> libSystem.B.dylib
 ; printf (00001fcc) -> libSystem.B.dylib
 ;		DumpInt
 ;		main
 ; putchar (fe000050) -> libSystem.B.dylib
 ; putchar (00001fdc) -> libSystem.B.dylib
 ;		main
 ; puts (fe000058) -> libSystem.B.dylib
 ; puts (00001fec) -> libSystem.B.dylib
 ;		CreateCallback
 ;		main
 ;
 ; >>> END OF FILE
 ;
