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

# dbsrv path
DBSRVPATH=/home/zjx/work/uiyiu/src/apps/dbsrv
export DBSRVPATH

# export exe path
PATH=$PATH:$DBSRVPATH/bin:$DBSRVPATH/tools
export PATH
##echo $PATH

# export lua path
MYLUA_PATH=$DBSRVPATH/scripts
export MYLUA_PATH

# export proto path
PROTO_PATH=$DBSRVPATH/proto
export PROTO_PATH

#export config path
dbsrv_configpath=$DBSRVPATH/config
export dbsrv_configpath

LOG_PATH=$DBSRVPATH/log
EXE="dbsrv -u root -r -c 1000 | logfile dbsrv 200000  '$LOG_PATH' | dblogfile dbsrv 10000  '$LOG_PATH' &"
EXENAME=dbsrv

##EXE="valgrind --leak-check=full -v --log-file=1 dbsrv -u root -r -c 1000 | logfile dbsrv 200000  '$LOG_PATH' &"
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

