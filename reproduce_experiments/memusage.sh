for i in `seq 0 60`; do
  echo `cat /proc/meminfo | grep Active: | sed 's/Active: //g'` >> usage.txt
  sleep 1m
done
