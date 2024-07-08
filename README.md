# c_static_analyzer_1c
## test
make test;
## debug
make debug;
## clean
make clean;
# DEPENDENCIES
## FOR UBUNTU
sudo apt install -y re2c bison pkg-config ; 
### for static analyzer clang
sudo apt install clang-tools;  
make analyzer_clang  
### for static analyzer pvs-studio
sudo apt install sudo dpkg  
download .deb package [PVS-Studio](https://pvs-studio.ru/ru/pvs-studio/download/) to package_name.deb  
sudo dpkg -i package_name.deb  
Get open-souce license[PVS-Studio_OPEN_SOUCE_LICENSE](https://pvs-studio.com/en/order/open-source-license/)  
Install license(USER_NAME and LICENSE_KEY will come by e-mail):  
pvs-studio-analyzer credentials USER_NAME LICENSE_KEY  
make pvs_studio_analyzer  
### test memory leak
sudo apt install valgrind  
make memory_leak_test
# SAST Tools
[PVS-Studio](https://pvs-studio.com/pvs-studio/?utm_source=website&utm_medium=github&utm_campaign=open_source) - static analyzer for C, C++, C#, and Java code.  
[Clang Static Analyzer](https://clang-analyzer.llvm.org/) - The Clang Static Analyzer is a source code analysis tool that finds bugs in C, C++, and Objective-C programs.
