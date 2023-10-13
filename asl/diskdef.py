#!/bin/env python3

# Port of CP/M DISKDEF.LIB to Python
#
# Usage:
#	disks(n)
#	diskdef(parameter_list_0)
#	diskdef(parameter_list_1)
#	diskdef(parameter_list_2)
#	diskdef(parameter_list_3)
#	...
#	endef()
#
# these calls will print the ASM code for the disk definitions.
#
# If a disk is identical to a previous disk, specify the new
# and old disk numbers in a call to diskdef2:
#     print(diskdef2(4,3))   # disk 4 is like disk 3

# Commentary from DISKDEF.LIB:
#
# 	CP/M 2.0 disk re-definition library
# 
# 	Copyright (c) 1979
# 	Digital Research
# 	Box 579
# 	Pacific Grove, CA
# 	93950
# 
# 	CP/M logical disk drives are defined using the
# 	macros given below, where the sequence of calls
# 	is:
# 
# 	disks	n
# 	diskdef	parameter-list-0
# 	diskdef	parameter-list-1
# 	...
# 	diskdef	parameter-list-n
# 	endef
# 
# 	where n is the number of logical disk drives attached
# 	to the CP/M system, and parameter-list-i defines the
# 	characteristics of the ith drive (i=0,1,...,n-1)
# 
# 	each parameter-list-i takes the form
# 		dn,fsc,lsc,[skf],bls,dks,dir,cks,ofs,[0]
# 	where
# 	dn	is the disk number 0,1,...,n-1
# 	fsc	is the first sector number (usually 0 or 1)
# 	lsc	is the last sector number on a track
# 	skf	is optional 'skew factor' for sector translate
# 	bls	is the data block size (1024,2048,...,16384)
# 	dks	is the disk size in bls increments (word)
# 	dir	is the number of directory elements (word)
# 	cks	is the number of dir elements to checksum
# 	ofs	is the number of tracks to skip (word)
# 	[0]	is an optional 0 which forces 16K/directory entry
# 
# 	for convenience, the form
# 		dn,dm
# 	defines disk dn as having the same characteristics as
# 	a previously defined disk dm.
# 
# 	a standard four drive CP/M system is defined by
# 		disks	4
# 		diskdef	0,1,26,6,1024,243,64,64,2
# 	dsk	set	0
# 		rept	3
# 	dsk	set	dsk+1
# 		diskdef	%dsk,0
# 		endm
# 		endef
# 
# 	the value of 'begdat' at the end of assembly defines the
# 	beginning of the uninitialize ram area above the bios,
# 	while the value of 'enddat' defines the next location
# 	following the end of the data area.  the size of this
# 	area is given by the value of 'datsiz' at the end of the
# 	assembly.  note that the allocation vector will be quite
# 	large if a large disk size is defined with a small block
# 	size.
# 
#	dskhdr(dn)		define a single disk header list
#	disks(nd)		define nd disks
#	dpbhdr(dn)		start dpb header (generates dpbN)
#	ddb(data,comment)	add a data byte with a comment
#	ddw(data,comment)	add a data word with a comment
#	gcd(m,n)		set gcdn to GCD of m and n
#	diskdef(dn,fsc,lsc,skf,bls,dks,dir,cks,ofs,k16)
#				insert disk definition table
#	diskdef(dn,fsc)		disk DN is like disk FSC
#	defds(lab,space)	define SPACE bytes of data area
#	lds(lb,dn,val)		allocate space called lb&dn of size val&dn
#	endef(nd)		generate ram data areas

import math

space_zero = set()

def dskhdr(dn):
    print('')
    print(f"dpe{dn}:	dw	xlt{dn},0000h	;translate table")
    print(f"	dw	0000h,0000h	;scratch area")
    print(f"	dw	dirbuf,dpb{dn}	;dir buff,parm block")
    print(f"	dw	csv{dn},alv{dn}	;check, alloc vectors")

ndisks = None
def disks(nd):
    global ndisks
    ndisks = nd
    print('')
    print(f"ndisks	set	{nd}		;for later reference")
    print(f"dpbase	equ	$		;base of disk parameter blocks")
    for dsknxt in range(nd):
        dskhdr(dsknxt)

def dpbhdr(dn):
    print(f"dpb{dn}	equ	$		;disk parm block")

def ddb(data, comment=None):
    if comment is None:
        print(f"	db	{data}")
    else:
        print(f"	db	{data}		{comment}")

def ddw(data, comment=None):
    if comment is None:
        print(f"	dw	{data}")
    else:
        print(f"	dw	{data}		{comment}")

def gcd(gcdm,gcdn):
    return math.gcd(gcdm,gcdn)

def get_blkshf_blkmsk(bls):
    blkval = bls // 128
    blkshf = 0
    blkmsk = 0;
    for i in range(16):
        if blkval == 1:
            break
        blkshf = blkshf + 1
        blkmsk = (blkmsk << 1) | 1
        blkval = blkval // 2
    return blkshf, blkmsk

def get_extmsk(bls,dks,k16):
    blkval = bls // 1024		# kilobytes per block
    extmsk = 0			# fill from right
    for i in range(16):
        if blkval == 1:
            break
        extmsk = (extmsk << 1) | 1
        blkval = blkval // 2
    if dks > 256:			# may be double byte allocation
        extmsk = extmsk >> 1
    if k16 is not None:		# may be optional [0] in last position")
        extmsk = k16
    return extmsk

def get_dirblk(dir,bls):
    dirrem = dir			# remaining to process
    dirblks = bls // 32		# number of entries per block
    dirblk = 0			# fill with 1's on each loop
    for i in range(16):
        if dirrem == 0:
            break
        dirblk = (dirblk >> 1) | 0x8000
        if dirrem > dirblks:
            dirrem = dirrem - dirblks
        else:
            dirrem = 0
    return dirblk

def diskdef(dn,fsc,lsc,skf,bls,dks,dir,cks,ofs,k16=None):
 
    print('')

    secmax = lsc - fsc
    sectors = secmax + 1

    if dks <= 0:
        space_zero.add(f'als{dn}')
    if cks <= 0:
        space_zero.add(f'css{dn}')

    print(f"als{dn}	set	{(dks+7)//8}		;size of allocation vector")
    print(f"css{dn}	set	{(cks+3)//4}		;number of checksum elements")

    blkshf, blkmsk = get_blkshf_blkmsk(bls)
    extmsk = get_extmsk(bls,dks,k16)
    dirblk = get_dirblk(dir,bls)

    print('')
    dpbhdr(dn)
    ddw(sectors,';sec per track')
    ddb(blkshf,';block shift')
    ddb(blkmsk,';block mask')
    ddb(extmsk,';extnt mask')
    ddw(dks-1,';disk size-1')
    ddw(dir-1,';directory max')
    ddb(dirblk >> 8,';alloc0')
    ddb(dirblk & 0xFF,';alloc1')
    ddw(cks//4,';check size')
    ddw(ofs,';offset')
    print('')

    if skf is None or skf <= 1:
        print(f"xlt{dn}	equ	0		;no xlate table")
        return

    nxtsec = 0		# next sector to fill
    nxtbas = 0		# moves by one on overflow
    # neltst number of elements to generate
    # before we overlap previous elements
    neltst = sectors // gcd(sectors, skf)
    nelts = neltst		# counter
    print(f"xlt{dn}	equ	$		;skew factor {skf}")
    ddbl=[]
    for i in range(sectors):		# once for each sector
        ddbl.append(f'{nxtsec+fsc}')
        nxtsec = nxtsec + skf
        if nxtsec >= sectors:
            nxtsec = nxtsec - sectors

        nelts = nelts - 1
        if nelts == 0:
            nxtbas = nxtbas + 1
            nxtsec = nxtbas
            nelts = neltst

            if len(ddbl) > 0:
                args = ",".join(ddbl)
                if sectors < 256:
                    ddb(args)
                else:
                    ddw(args)
            ddbl = []

    if len(ddbl) > 0:
        args = ",".join(ddbl)
        if sectors < 256:
            ddb(args)
        else:
            ddw(args)

def diskdef2(dn,odn):
    print('')
    print(f"dpb{dn}	equ	dpb{odn}	;equivalent parameters")
    print(f"als{dn}	equ	als{odn}	;same allocation vector size")
    print(f"css{dn}	equ	css{odn}	;same checksum vector size")
    print(f"xlt{dn}	equ	xlt{odn}	;same translate table")
    if f'als{odn}' in space_zero:
        space_zero.add(f'als{dn}')
    if f'css{odn}' in space_zero:
        space_zero.add(f'css{dn}')

def defds(lab,space):
    if space in space_zero:
        print(f"{lab}	equ	0		;{space} is 'no space'")
    else:
        print(f"{lab}:	ds	{space}")

def lds(lb,dn,val):
    defds(f'{lb}{dn}',f'{val}{dn}')

def endef():
    global ndisks
    nd = ndisks

    print('')
    print(';; define data areas')
    print(f"begdat	equ	$")
    print(f"dirbuf:	ds	128	;directory access buffer")

    for dsknxt in range(nd):
        lds('alv',dsknxt,'als')
        lds('csv',dsknxt,'css')
    print(f"enddat	equ	$")
    print(f"datsiz	equ	$-begdat")

def ibm_sssd(nd):
    disks(nd)
    diskdef(0,1,26,6,1024,243,64,64,2)
    for dsk in range(1,nd):
        diskdef2(dsk,0)
    endef()

defs = dict()

# DISKDEF parameters are:
#	fsc	is the first sector number (usually 0 or 1)
#	lsc	is the last sector number on a track
#	skf	is optional "skew factor" for sector translate
#	bls	is the data block size (1024,2048,...,16384)
#	dks	is the disk size in bls increments (word)
#	dir	is the number of directory elements (word)
#	cks	is the number of dir elements to checksum
#	ofs	is the number of tracks to skip (word)
#	[0]	is an optional 0 which forces 16K/directory entry
#
def compute_dks(tracks, fsc, lsc, bls, ofs, bps):
    """Compute the DKS value for this disk
    tracks - number of tracks (does not appear in DISKDEF)
    other parameters - match DISKDEF usage
    """
    return int(((tracks - ofs) * (lsc - fsc + 1) * bps) // bls)

# IBM Single Density 8-inch diskette
# 77 tracks of 26 sectors of 128 bytes, 2 tracks reserved: 243.75 KiB
# ... 3.25 Kib per track
# this matches the documented table.
defs['ibmsd'] = ["IBM Single Density 8-inch diskette",
                 1,26,6,1024, 
                 compute_dks(77, 1, 26, 1024, 2, 128),
                 64,64,2]

# 256 tracks of 255 sectors of 128 bytes, 1 track reserved: 8128.12 KiB
# ... 31.88 Kib per track
# These numbers seem to make sense to me.
defs['maxhd'] = ["Hypothetical Maximum Size Hard Drive",
                 1,255,0,8192,
                 compute_dks(256, 1, 255, 8192, 1, 128),
                 1024,0,1]

def main(argv):
    ddl = []
    for arg in argv[1:]:
        if arg in defs:
            args = defs[arg]
            ddl.append(args)
            continue
        i = int(arg)
        ddl.append([i])

    nd = len(ddl)

    print('')
    print(f'	;;; === === === === === === === === === === === ===')
    print(f'	;;; Disk Parameter Table for {nd} Drives')
    print(f'	;;; === === === === === === === === === === === ===')
    print('')

    disks(nd)
    for d in range(nd):
        args = ddl[d]
        if len(args) > 2:
            print('')
            print(f'	;;; === === === === === === === === === === === ===')
            print(f'	;;; 	Drive {chr(65+d)}: {args[0]}')
            args = args[1:]
            print(f'	;;; 	DISKDEF	{args!r}')
            print(f'	;;; === === === === === === === === === === === ===')
            diskdef(d, *args)
        else:
            print('')
            print(f'	;; === === === === === === === === === === === ===')
            print(f'	;; 	Drive {chr(65+d)} is the same as Drive {chr(65+args[0])}')
            diskdef2(d, *args)
    endef()

if __name__ == '__main__':
    import sys
    main(sys.argv)
