#!/bin/bash
OBJPATH='objfiles'
VMPATH='LC3VM'

finish() { 
    exit
}

moveobjfiles() {
    mkdir $OBJPATH
    for f in *.o; do
        mv $f $OBJPATH
    done
}


installVM() {
    echo "Creating the VM folder"
    if [! -d "~/$VMPATH" ]; then
   	 mkdir "~/$VMPATH"
    fi
    printf "The VM will be placed in %s/%s\n" $(pwd) ${VMPATH}
    make
}

installCompiler() {
   cd src/compiler; echo "Installing the LC3-COMPILER..."
   flex lc3.l
   bison -d lc3parser.y
   printf  "Compiler successfully installed! Going back to "; cd -
}

installprogram() {
    installCompiler
    installVM
    moveobjfiles
    finish
}

uninstall() {
   rm -r $VMPATH $OBJPATH
   finish
}

dockerInstall() {
    docker build . -t lc3docker
}

clean_up() {
    while IFS= read -r j; do
        rm "$j"
    done < <(grep -E -v '(resources)|(src)|(Makefile)|(script)')
}


if [ "$#" -ne  1 ]; then
    echo "Usage ---"
    finish
elif [ "$1" = 'install' ]; then
    installprogram
elif [ "$1" = 'uninstall' ]; then
    uninstall
elif [ "$1" = 'docker' ]; then
    dockerInstall
fi
echo "Usage ---"
finish

trap clean_up EXIT
