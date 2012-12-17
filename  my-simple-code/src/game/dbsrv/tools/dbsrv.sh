#!/bin/sh
#
# description: Starts and stops the dbsrv daemon


# Source function library.
if [ -f /etc/init.d/functions ] ; then
  . /etc/init.d/functions
elif [ -f /etc/rc.d/init.d/functions ] ; then
  . /etc/rc.d/init.d/functions
else
  exit 1
fi

# Avoid using root's TMPDIR
unset TMPDIR

# Source networking configuration.
. /etc/sysconfig/network

# open trace log
export MYSDK_LOG_TRACE

# export exe path
PATH=$PATH:/home/zjx/work/uiyiu/src/game/dbsrv/tools
export PATH

# export lua path
MYLUA_PATH=/home/zjx/work/uiyiu/src/game/dbsrv/scripts
export MYLUA_PATH

# export proto path
PROTO_PATH=/home/zjx/work/uiyiu/src/game/dbsrv/proto
export PROTO_PATH

LOG_PATH=/home/zjx/work/uiyiu/src/game/dbsrv/log
EXE="bin/dbsrv -u root -r -c 1000 | logfile dbsrv 200000  '$LOG_PATH' &"
EXENAME=dbsrv

##EXE="valgrind --leak-check=full -v --log-file=1 bin/dbsrv -u root -r -c 1000 | logfile dbsrv 200000  '$LOG_PATH' &"
##EXENAME=valgrind

RETVAL=0
KIND="dbsrv"
start() {
        echo -n $"Starting $KIND services: "
        daemon   $EXE 
        RETVAL=$?
	echo 
        [ $RETVAL -eq 0 ]  
		RETVAL=1
	##return 0
	return $RETVAL
}

stop() {
        echo -n $"Shutting down $KIND services: "
        killproc $EXENAME 
        RETVAL=$?
	echo
	[ $RETVAL -eq 0 ] 
        return $RETVAL
}

restart() {
        stop
        start
}
rhstatus() {
        status  $EXENAME 
        return $?
}

case "$1" in
  start)
        start
        ;;
  stop)
        stop
        ;;
  restart)
        restart
        ;;
  status)
        rhstatus
        ;;
  *)
        echo $"Usage: $0 {start|stop|restart|status}"
        exit 2
esac

exit $?

