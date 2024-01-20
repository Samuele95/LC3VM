### --- Path variables --- ###
OBJPATH='objfiles'
VMPATH='LC3VM'
RESOURCESPATH='resources'
MEMDIR = src/memory
OSDIR = src/os
INSTRDIR = src/instructionset
COMPDIR = src/compiler
EDITORDIR = src/editor
PROGDIR = src/runprogram
GUIDIR = src/gui
BINARIESDIR = bin

SHELL := /bin/bash

### --- Install script order --- ###
install_core = install_compiler install_vm 
shared = instructionset.o
exec_files = run_program os lc3compiler texor gui
install_post = install_file_sender move_obj_files complete_install

all: $(install_core) $(shared) $(exec_files) $(install_post)


### --- Object files --- ###
os: $(OSDIR)/os.c $(OSDIR)/ui.c $(OSDIR)/fsystem.c memory.o
	cc -w $^ -o $@ -lpthread -lrt

run_program: $(PROGDIR)/run_program.c $(OSDIR)/os.h instructionset.o
	cc -w $^ -o $@ -lpthread -lrt

lc3compiler: $(COMPDIR)/lc3parser.tab.c instructionset.o
	cc -w $^ -o $@ -ly -lfl

instructionset.o: memory.o opcodes.o
	cc -w -r $^ -o $@

opcodes.o: $(INSTRDIR)/opcodes.c
	cc -w -c $^

memory.o: $(MEMDIR)/memory.c $(MEMDIR)/condflags.h
	cc -w -c $^
	
texor: $(EDITORDIR)/texor.c
	cc -w $< -o $@
	
gui: $(GUIDIR)/gui.c
	cc -w $< -o $@ `pkg-config --cflags --libs gtk+-3.0 vte-2.91`
        

### --- Install procedures --- ###
install_compiler:
	{ set -e ; cd src/compiler ; echo "Installing the LC3-COMPILER..." ; flex lc3.l ; bison -d lc3parser.y ; echo "Compiler successfully installed!" ; cd - ; }
	
install_vm:
	{ set -e ; echo "Creating the VM folder" ; if [ ! -d "~/LC3VM" ]; then mkdir LC3VM ; fi ; printf "The VM will be placed in %s/%s\n" $$(pwd) ${VMPATH} ; }
	
install_file_sender:
	{ set -e ; cp ./${RESOURCESPATH}/filesender.sh ./${VMPATH}/filesender.sh ;echo "File sender successfully installed!" ; }

move_obj_files:
	{ set -e ; mkdir ${OBJPATH} ; for f in *.o; do mv $$f ${OBJPATH} ; done ; mv $(exec_files) LC3VM; }

complete_install:
	echo "Program successfully installed! Go to LC3VM folder to use it!"

### --- Uninstall --- ###
clean:
	rm -r ${VMPATH} ${OBJPATH}
