GIT_VERSION   = $(shell git rev-parse HEAD 2>/dev/null)
ABOUT         = $(shell cat ABOUT 2>/dev/null)

define ABOUT
$(shell cat ABOUT 2>/dev/null)
endef

DEST	      = /home/sysadmin/dest/

LIBDIR 	      = $(DEST)

#CC_ENVIRON    = -Wf,-XNh2000 -mips2
#CC_ENVIRON    = -O2 -mips4 -n32 -r10000

INCLUDES      = -I/usr/include/X11 -I/usr/include/X11/extensions \
		-I/usr/include/GL \
		-I.

LIBS          = $(MORE) -lGL -lGLU -lXm -lXt -lXi -lXext -lX11 

MORE          = -lm


BSD	          = # -cckr

CFLAGS        = $(BSD) -g $(CC_ENVIRON) $(INCLUDES) $(DEBUG) -DLIB_DIR=\"$(LIBDIR)\"

ifdef GIT_VERSION
CFLAGS += -DGIT_VERSION=\"$(GIT_VERSION)\"
endif

ifdef ABOUT
CFLAGS += -DABOUT="\"$(ABOUT)\""
endif

LDFLAGS	      = $(DEBUG)

EXTHDRS	      = ./XPick.h \
		/usr/include/X11/Composite.h \
		/usr/include/X11/Constraint.h \
		/usr/include/X11/Core.h \
		/usr/include/X11/Intrinsic.h \
		/usr/include/X11/Object.h \
		/usr/include/X11/RectObj.h \
		/usr/include/X11/Vendor.h \
		/usr/include/X11/X.h \
		/usr/include/X11/Xlib.h \
		/usr/include/X11/Xos.h \
		/usr/include/X11/Xresource.h \
		/usr/include/X11/Xutil.h \
		/usr/include/X11/keysym.h \
		/usr/include/X11/keysymdef.h \
		/usr/include/Xm/BulletinB.h \
		/usr/include/Xm/CascadeB.h \
		/usr/include/Xm/DrawingA.h \
		/usr/include/Xm/FileSB.h \
		/usr/include/Xm/LabelG.h \
		/usr/include/Xm/MessageB.h \
		/usr/include/Xm/PushB.h \
		/usr/include/Xm/PushBG.h \
		/usr/include/Xm/RowColumn.h \
		/usr/include/Xm/SelectioB.h \
		/usr/include/Xm/Separator.h \
		/usr/include/Xm/Text.h \
		/usr/include/Xm/ToggleBG.h \
		/usr/include/Xm/Xm.h \
		/usr/include/ctype.h \
		/usr/include/fcntl.h \
		/usr/include/math.h \
		/usr/include/string.h \
		/usr/include/sys/fcntl.h \
		/usr/include/sys/file.h \
		/usr/include/sys/types.h \
		/usr/include/time.h \
		/usr/include/unistd.h 

HDRS	      = paleodefs.h \
		paleoexterns.h \
		paleotypes.h \
		pex.h

LINKER	      = cc

MAKEFILE      = Makefile

OBJS	      = callbacks.o \
		choice.o \
		compare.o \
		data.o \
		deletor.o \
		draw.o \
		enter_pole.o \
		fkeys.o \
		labels.o \
		lith.o \
		misc.o \
		motif.o \
		move_plate.o \
		pmt.o \
		pick.o \
		picture_maker.o \
		pmag.o \
		quick_send_rot.o \
		reconstructor.o \
		send_vector.o \
		timeline.o \
		valuator.o \
		view.o \
		xmainloop.o \
		usbmidi.o

PRINT	      = pr

PROGRAM	      = pmt

SRCS	      = callbacks.c \
		choice.c \
		compare.c \
		data.c \
		deletor.c \
		draw.c \
		enter_pole.c \
		fkeys.c \
		labels.c \
		lith.c \
		misc.c \
		motif.c \
		move_plate.c \
		pmt.c \
		pick.c \
		picture_maker.c \
		pmag.c \
		quick_send_rot.c \
		reconstructor.c \
		send_vector.c \
		timeline.c \
		valuator.c \
		view.c \
		xmainloop.c \
		usbmidi.c

all:		$(PROGRAM)

$(PROGRAM): $(OBJS)
		@echo -n "Linking $(PROGRAM)... "
				
		@$(LINKER) $(LDFLAGS) $(OBJS) $(LIBS) -o $(PROGRAM)
		@echo "done"

clean:;		@rm -f $(OBJS)

depend:;	@mkmf -f $(MAKEFILE) PROGRAM=$(PROGRAM) DEST=$(DEST)

index:;		@ctags -wx $(HDRS) $(SRCS)

install: $(PROGRAM)
		@echo Installing $(PROGRAM) in $(DEST)...
		@rm -rf $(DEST)
		@mkdir -p $(DEST)
		@mv $(PROGRAM) $(DEST)
#		@chown bin $(DEST)/$(PROGRAM)
#		@chmod 755 $(DEST)/$(PROGRAM)
		@mkdir -p $(LIBDIR)
		@echo Copying data files...
		@cp agedata $(LIBDIR)
		@cp world.dat $(LIBDIR)
		@cp -r *.rot $(LIBDIR)
		@cp -r data $(LIBDIR)
		@cp -r pmag $(LIBDIR)
		@echo Done!
# 		@cp Xdefaults /usr/lib/X11/app-defaults/Paleomap

print:;		@$(PRINT) $(HDRS) $(SRCS)

program:        $(PROGRAM)

tags:           $(HDRS) $(SRCS); @ctags $(HDRS) $(SRCS)

update:		$(DEST)/$(PROGRAM)

$(DEST)/$(PROGRAM): $(SRCS) $(LIBS) $(HDRS) $(EXTHDRS)
		make -f $(MAKEFILE) DEST=$(DEST) install
###
callbacks.o:  paleodefs.h pex.h
choice.o: paleotypes.h pex.h 
compare.o: paleotypes.h paleodefs.h paleoexterns.h
data.o: paleotypes.h paleodefs.h
deletor.o: paleotypes.h paleodefs.h paleoexterns.h
draw.o: paleotypes.h paleodefs.h paleoexterns.h
enter_pole.o: paleotypes.h paleodefs.h paleoexterns.h
fkeys.o: paleotypes.h paleodefs.h paleoexterns.h
labels.o: paleotypes.h paleodefs.h paleoexterns.h
lith.o: paleotypes.h paleodefs.h paleoexterns.h
misc.o: paleotypes.h paleodefs.h paleoexterns.h
motif.o: paleodefs.h usbmidi.h
move_plate.o: paleotypes.h paleodefs.h paleoexterns.h
pmt.o: paleotypes.h paleodefs.h
picture_maker.o: paleodefs.h paleoexterns.h
pmag.o: paleotypes.h paleodefs.h paleoexterns.h
quick_send_rot.o: paleotypes.h paleodefs.h paleoexterns.h
reconstructor.o: paleotypes.h paleodefs.h paleoexterns.h
send_vector.o: paleotypes.h paleodefs.h paleoexterns.h
timeline.o: paleotypes.h paleodefs.h paleoexterns.h
valuator.o: paleotypes.h paleodefs.h paleoexterns.h pex.h
view.o: paleodefs.h
xmainloop.o: pex.h
usbmidi.o: usbmidi.h
