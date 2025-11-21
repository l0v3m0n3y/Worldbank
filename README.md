# Worldbank
api for worldbank.org official site world bank
# main
```cpp
#include "Worldbank.h"
#include <iostream>

int main() {
   Worldbank api;

    auto regions = api.get_region_list().then([](json::value result) {
        std::cout << result<< std::endl;
    });
    regions.wait();
    
    return 0;
}
```

# Launch (your script)
```
g++ -std=c++11 -o main main.cpp -lcpprest -lssl -lcrypto -lpthread -lboost_system -lboost_chrono -lboost_thread
./main
```
