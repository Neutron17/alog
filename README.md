# NALog
Asynchronous logger for c

check out [nlog](https://github.com/Neutron17/nlog)

# WARNING: This project is unstable(doesn't work) I stopped working on it, if anyone wants to take over this project, feel free to fork it

## Usage

```c
#include <alog/log.h>

int main() {
	loginit(
		// what to print to stdout, L_WARN would only print warnings and errors
		// there are: L_ALL, L_INFO, L_WARN, L_ERR and L_NONE
		L_ALL,
		// what to print to the logfile
		// in this case don't print anything to the file
		L_NONE
	);
	LOG(L_INFO, "program started");
	int x = 69;
	// printf like syntax
	LOGF(L_WARN, "x is %d", x);

	logdestroy();
	return 0;
}
```
compile with ```-lalog```

## Installation

```
sudo make install
```

## Contribution

anyone can create pull requests, fork the project, etc as long as it doesn't violate the
license (GNU GPL 3.0)

## Why I stopped the development of it

this was an asynchronous logger, but the hassle wasn't worth it,

 - async io is usually not real async, linux can(and often does) turn your carefully assembled asynchronous code
 into synchronous code, even with the ```O_DIRECT``` hint,

 for more details see [this](https://stackoverflow.com/a/46377629/14187193) and [this](https://stackoverflow.com/a/38969392/14187193)

 with the current state of async io on unix I don't think this project is worth struggling with.

 the latest (unstable)version of this is available  [on this pastebin(log.c)](https://pastebin.com/E3y2hWzT) with version 1.0

## Helping material for contributors
 - man page aio
 - [intel.com](https://www.intel.com/content/www/us/en/develop/documentation/cpp-compiler-developer-guide-and-reference/top/compiler-reference/libraries/intel-s-c-asynchronous-i-o-extensions-windows-os/intel-s-c-asynchronous-i-o-library-for-windows-os/example-for-aio-read-and-aio-write-functions.html)
 - google

