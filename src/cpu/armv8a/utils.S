.global halt_core
halt_core:
    wfe
    b halt_core

.global get_el
get_el:
	mrs x0, CurrentEL
	lsr x0, x0, #2
	ret

.global cpuid
cpuid:
    mrs x0, mpidr_el1
    and x0, x0, #0xff
	ret
