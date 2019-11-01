cd .. && ./install.sh && cd reproduce_experiments
make -C ..
yes | sudo apt-get install -y python3-pip python3-dev jupyter
