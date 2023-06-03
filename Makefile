MEMDIR = src/memory
OSDIR = src/os
INSTRDIR = src/instructionset
COMPDIR = src/compiler
EDITORDIR = src/editor
PROGDIR = src/runprogram
BINARIESDIR = bin
execs = run_program os lc3compiler texor

all: instructionset.o $(execs) complete_install

os: $(OSDIR)/os.c $(OSDIR)/ui.c $(OSDIR)/fsystem.c memory.o
	cc $^ -o $@ -lpthread -lrt

run_program: $(PROGDIR)/run_program.c $(OSDIR)/os.h instructionset.o
	cc $^ -o $@ -lpthread -lrt

lc3compiler: $(COMPDIR)/lc3parser.tab.c instructionset.o
	cc $^ -o $@ -ly -lfl

instructionset.o: memory.o opcodes.o
	cc -r $^ -o $@

opcodes.o: $(INSTRDIR)/opcodes.c
	cc -c $^

memory.o: $(MEMDIR)/memory.c $(MEMDIR)/condflags.h
	cc -c $^
	
texor: $(EDITORDIR)/texor.c
	cc $< -o $@ 
complete_install:
	if [ ! -d "~/LC3VM" ]; then mkdir LC3VM; fi;  mv $(execs) LC3VM

clean:
	rm  memory.o opcodes.o instructionset.o
