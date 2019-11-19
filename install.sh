if [[ "$OSTYPE" == "darwin*" ]]; then
		brew update
        brew install libomp
        brew install python3
        brew postinstall python3
        pip3 install pandas
else
		sudo apt-get update
        sudo apt install -y libomp-dev python3 python3-pandas g++
fi