clang_ling=http://releases.llvm.org/5.0.0/clang+llvm-5.0.0-linux-x86_64-ubuntu16.04.tar.xz
sudo apt-get update
sudo apt install -y libomp-dev libc++-dev python3 python3-pandas

echo $clang_ling
wget $(echo $clang_ling)
tar xvf clang+llvm*.tar.*

rm clang+llvm*.tar.*
mv clang+llvm* clang5
