while(true); do
  echo `cat /proc/meminfo | grep Active: | sed 's/Active: //g'` >> usage.txt
  sleep 1m
done
