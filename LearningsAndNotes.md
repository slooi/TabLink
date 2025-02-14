g++ -Wall -Wextra -g3 client.cpp -o client.exe -lws2_32
g++ -Wall -Wextra -g3 server.cpp -o server.exe -lws2_32

g++ -Wall -Wextra -g3 client.cpp -o client.exe -lws2_32 && g++ -Wall -Wextra -g3 server.cpp -o server.exe -lws2_32

g++ -Wall -Wextra -g3 pressureReader.cpp -o pressureReader.exe -lgdi32 -municode

g++ -Wall -Wextra -g3 pressureClient.cpp -o pressureClient.exe -lws2_32 -lgdi32 -municode

InputInjector.InjectPenInput

Realtimestylus API
https://stackoverflow.com/questions/42743699/pen-input-in-windows-using-c

# THIS GUYS!!! PEN PRESSURE

https://stackoverflow.com/questions/67648316/windows-cpp-injectsyntheticpointerinput

-   InjectSyntheticPointerInput api simulate pen pressure

# other

"C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat"

cl /EHsc /std:c++17 simulate.cpp /link User32.lib

cl /EHsc /std:c++17 /Fe:server.exe server.cpp /link User32.lib Ws2_32.lib
