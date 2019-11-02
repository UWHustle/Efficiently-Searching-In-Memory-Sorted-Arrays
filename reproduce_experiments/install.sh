cd .. && ./install_plus_clang.sh && cd reproduce_experiments
make clang5_searchbench -C ..
sudo apt-get install -y python3-pip python3-dev jupyter
