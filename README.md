# Eschaton
`datetime` class and other support functions for `std::chrono`



## Examples

```cpp
using namespace std;
auto const origin = eschaton::current<chrono::microseconds>();
this_thread::sleep_for(chrono::milliseconds{10});
cout << eschaton::elapsed<chrono::microseconds>(origin) << endl;
cout << eschaton::datetime::utc() << endl;
```



## Usage

Just put `include/eschaton/eschaton.hpp` at your include path



## Dependencies

* [Howard Hinnant's date library](https://github.com/HowardHinnant/date) (expected as `<date/date.h>`)



## Tests

To run tests:

```shell
cd eschaton
mkdir build
cd build
cmake ../tests
```


