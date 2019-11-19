cd .. && ./install.sh && cd reproduce_experiments
make searchbench -C ..

if [[ `uname` == "Darwin" ]]; then
        pip3 install jupyter
        pip3 install matplotlib
        pip3 install numpy
else
        sudo apt-get install -y jupyter
        pip3 install matplotlib
        pip3 install numpy
fi
