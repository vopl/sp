#!/bin/bash

if [ "$#" -ne 2 ]; then
  echo "usage: $0 start|restart|stop|status exe" >&2
  exit 1
fi


if [[ -x "$2" ]]
then
    exename=$2
else
    echo "executable '$2' is not executable or found"
    exit 1
fi

dir=`readlink -f $0`
dir=`dirname $dir`
cd $dir
exe=`realpath $dir/${exename}`



function start
{
    pid=`pidof -x $exe`
    if [ -z "$pid" ];
    then
        ($exe >>stdout 2>>stderr)&
        pid=$!
        disown
        if [ -z "$pid" ];
        then
            echo "unable to start $exe"
        else
            echo "started, pid: $pid"
            sleep 0.1
            pid=`pidof -x $exe`
            if [ -z "$pid" ];
            then
                echo "stopped"
            fi
        fi
    else
        echo "already started, pid:$pid" > /dev/null
    fi
}

function status
{
    pid=`pidof -x $exe`
    if [ -z "$pid" ];
    then
        echo "$exe stopped"
    else
        echo "$exe started, pid:$pid"
    fi
}

function stop
{
    pid=`pidof -x $exe`
    if [ -z "$pid" ];
    then
        echo "already stopped" > /dev/null
    else
        echo -n "kill $pid"
        cnt=0
        while [ $cnt -lt 10 ] && [ -n "$pid" ] && kill $pid 2>/dev/null; do
            sleep 0.02
            pid=`pidof -x $exe`
            let cnt=cnt+1
            echo -n "."
        done

        while [ $cnt -lt 20 ] && [ -n "$pid" ] && kill $pid 2>/dev/null; do
            sleep 0.2
            pid=`pidof -x $exe`
            let cnt=cnt+1
            echo -n "."
        done

        pid=`pidof -x $exe`
        if [ -n "$pid" ]; then
            echo -n " force"
            kill -s KILL $pid 2>/dev/null
        fi

        pid=`pidof -x $exe`
        if [ -n "$pid" ]; then
            echo -n " force2"
            kill -s ABRT $pid 2>/dev/null
            kill -s SEGV $pid 2>/dev/null
            kill -s ILL $pid 2>/dev/null
        fi

        pid=`pidof -x $exe`
        echo " done$pid"
    fi
}


case $1 in
    "start")
        start
    ;;
    "restart")
        stop
        start
    ;;
    "status")
        status
    ;;
    "stop")
        stop
    ;;
    *)
        echo "start|stop|status action required"
        exit 2
    ;;
esac
