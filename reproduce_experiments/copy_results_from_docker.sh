did=`docker ps | grep "reproducibility" | awk '{print $1}'`
docker cp `echo $did`:/Efficiently-Searching-In-Memory-Sorted-Arrays/reproduce_experiments/figures/* ./figures/
