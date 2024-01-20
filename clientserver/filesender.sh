#!/bin/bash
PORT='1031'
FILENAME=".$2"
LOCALIP="$3"

sendfile() {
    cat $FILENAME | nc -w 10 $LOCALIP $PORT
    echo "File sending process terminated" 
}

receivefile() {
    nc -w 10 -l -p $PORT > $FILENAME
    echo "File receiving process terminated"
}

help() {
    printf "\nUsage: filesender.sh [send|receive] filename ip_address\n\nArguments:\n-send: using 'send' argument you MUST provide an IP address to send the file to;\n-receive: you must only give the name of the file to be received, as it will be saved on the local machine\n\n"
    exit 
}

if [ "$#" -gt  3 ] || [ "$#" -lt 1 ]; then
    help   
elif [ "$1" = 'send' ] && [ -z "$3" ] ; then
    help
elif [ "$1" = 'send' ] && [ -n "$3" ] ; then
    sendfile
elif [ "$1" = 'receive' ]; then
    receivefile
fi
