g++ -Wall -Werror -fpic -c libmymem.hpp -I . libmymem.cpp -std=c++14 -g 
g++ -shared -o libmymem.so libmymem.o -std=c++14  
g++ -I . -L . -Wall -o memutil memutil.cpp -l mymem -l pthread  -std=c++14 -g 
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:.
./memutil -n 100

Instead of executing the above commands we can execute the following command directly in bash by editing the parameters inside the execution.sh file 

bash execution.sh
