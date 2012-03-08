################################
##############ZeOS #############
################################
########## Makefile ############
################################

# package dev86 is required
AS86	= as86 -0 -a
LD86	= ld86 -0

HOSTCFLAGS = -Wall -Wstrict-prototypes
HOSTCC 	= gcc
CC      = gcc -Wall -g -c
AS      = as
LD      = ld
OBJCOPY = objcopy -O binary -R .note -R .comment -S

INCLUDEDIR = include

JP = #-DWRITE #-DREAD -DFORK -DSEM

#cflag a afegir en la ultima versio d'Ubuntu: -fno-stack-protector
CFLAGS = -O2 -fno-stack-protector -enable-port-e9-hack -DZEOS  $(JP) -ffreestanding -Wall -I$(INCLUDEDIR) 

ASMFLAGS = -I$(INCLUDEDIR)
SYSLDFLAGS = -T system.lds
USRLDFLAGS = -T user.lds
LINKFLAGS = -g 

SYSOBJ = interrupt.o entry.o io.o sys.o sched.o mm.o list.o devices.o utils.o hardware.o fd.o bufCirc.o zeosFat.o
USROBJ = libc.o userProcNostre.o #libjp_linux.o

all:zeos.bin

zeos.bin: bootsect system build user
	$(OBJCOPY) system system.out
	$(OBJCOPY) user user.out
	./build bootsect system.out user.out > zeos.bin

build: build.c
	$(HOSTCC) $(HOSTCFLAGS) -o $@ $<

bootsect: bootsect.o
	$(LD86) -s -o $@ $<

bootsect.o: bootsect.s
	$(AS86) -o $@ $<

bootsect.s: bootsect.S Makefile
	$(CPP) $(ASMFLAGS) -traditional $< -o $@

entry.s: entry.S $(INCLUDEDIR)/asm.h $(INCLUDEDIR)/segment.h
	$(CPP) $(ASMFLAGS) -o $@ $<

libjp_linux.o:libjp_linux.c $(INCLUDEDIR)/tools.h $(INCLUDEDIR)/libcjp.h

user.o:user.c $(INCLUDEDIR)/libc.h

interrupt.o:interrupt.c $(INCLUDEDIR)/interrupt.h $(INCLUDEDIR)/segment.h $(INCLUDEDIR)/types.h

io.o:io.c $(INCLUDEDIR)/io.h

libc.o:libc.c $(INCLUDEDIR)/libc.h

list.o:list.c $(INCLUDEDIR)/list.h

mm.o:mm.c $(INCLUDEDIR)/types.h $(INCLUDEDIR)/mm.h

sched.o:sched.c $(INCLUDEDIR)/sched.h

sys.o:sys.c $(INCLUDEDIR)/devices.h

utils.o:utils.c $(INCLUDEDIR)/utils.h

fd.o:fd.c $(INCLUDEDIR)/fd.h

bufCirc.o:bufCirc.c $(INCLUDEDIR)/bufCirc.h

zeosFat.o:zeosFat.c $(INCLUDEDIR)/bufCirc.h

system.o:system.c $(INCLUDEDIR)/hardware.h system.lds $(SYSOBJ) $(INCLUDEDIR)/segment.h $(INCLUDEDIR)/types.h $(INCLUDEDIR)/interrupt.h $(INCLUDEDIR)/system.h $(INCLUDEDIR)/sched.h $(INCLUDEDIR)/mm.h $(INCLUDEDIR)/io.h $(INCLUDEDIR)/mm_address.h


system: system.o system.lds $(SYSOBJ)
	$(LD) $(LINKFLAGS) $(SYSLDFLAGS) -o $@ $< $(SYSOBJ) 

user: user.o user.lds $(USROBJ) 
	$(LD) $(LINKFLAGS) $(USRLDFLAGS) -o $@ $< $(USROBJ)


clean:
	rm -f *.o *.s bochsout.txt parport.out system.out system bootsect zeos.bin user user.out *~ build

disk: zeos.bin
	dd if=zeos.bin of=/dev/fd0

emul: zeos.bin
	bochs -q -f .bochsrc
