cd .. && ./install.sh && cd reproduce_experiments
make searchbench -C ..

if [[ "$OSTYPE" == "darwin*" ]]; then
        pip3 install jupyter
else
        sudo apt-get install -y jupyter
fi
