#! /bin/sh
#
# chkconfig: 345 80 20
# description: run crashfix service
#
### BEGIN INIT INFO
# Provides:          crashfixd
# Required-Start:    $remote_fs $syslog
# Required-Stop:     $remote_fs $syslog
# Default-Start:     2 3 4 5
# Default-Stop:      0 1 6
# Short-Description: Start crashfix service at boot time
# Description:       Enable service provided by CrashFix daemon.
### END INIT INFO

NAME=crashfixd
DAEMON=/usr/sbin/$NAME
CONFIG=/etc/crashfix/$NAME.conf
PIDFILE=/var/run/$NAME.pid
LOCKFILE=/var/lock/subsys/$NAME

# Check if service package is installed
if [ ! -x $DAEMON ]; then
    exit 1
fi

if [ -x /etc/rc.d/init.d/functions ]; then   
	# Get function from functions library
	. /etc/init.d/functions
fi

RETVAL=0

start () {
	# Start the service	
    $DAEMON --start --config $CONFIG
    RETVAL=$?
    return $RETVAL	
}

stop () {    
    # Stop the service
    $DAEMON --stop --config $CONFIG
    RETVAL=$?
    return $RETVAL
}

case $1 in
    start)
	start
	;;
    stop)
	stop
	;;
    restart)
	stop
	start
	;;
    status)
	status $DAEMON
	;;
    *)
	echo "Usage: $0 {start|stop|restart|status}"
	;;
esac

RETVAL=$?

exit $RETVAL
