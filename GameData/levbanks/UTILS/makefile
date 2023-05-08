c_obj_files = $(patsubst %.c,%.obj,$(wildcard *.c)) $(patsubst %.c,%.obj,$(wildcard *.C))

###############################################################
##  Compiler

compiler = ccn64
compile_opt = -c -O0 -I ../../Common -I ../../n64  -I ../../SDK/include -I ../


###############################################################
##  Linker

linker = psylink

objects = $(patsubst \/%,\\%,$(patsubst %.c2,%.nob,$(wildcard objects/*.c2)))
	
textures = $(patsubst textures/%,textures\\%,$(patsubst %.ntb,%.fla,$(wildcard textures/*.ntb)))
	
###############################################################
##  Main Binary

all: $(patsubst %.C,%.bin,$(wildcard *.C)) $(patsubst %.C2,%.bi2,$(wildcard *.C2)) $(patsubst %.c,%.bin,$(wildcard *.c)) $(patsubst %.c2,%.bi2,$(wildcard *.c2))

%.bin: %.c
	$(compiler) $(compile_opt) $<
        psylink /c /p $(patsubst %.c,%.obj,$<),$(patsubst %.c,%.bin,$<)
        del $(patsubst %.c,%.obj,$<)
        del $<

%.bi2: %.c2
        ren $< $(patsubst %.c2,%.c,$<)
        $(compiler) $(compile_opt) $(patsubst %.c2,%.c,$<)
        psylink /c /p $(patsubst %.c2,%.obj,$<),$(patsubst %.c2,%.bi2,$<)
        del $(patsubst %.c2,%.obj,$<)
