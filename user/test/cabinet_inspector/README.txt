Tester
======

`cabinet_inspector`
-------------------

### Build

`cabinet_inspector` is the default build target; build it with:

    $ make


### Usage

The usage for `cabinet_inspector` is as follows:

	# ./cabinet_inspector [<vaddr> [<pid>]]

*   If `pid` is omitted, then -1 is used as the `pid` argument,
	i.e. it will be interpreted as the PID of the calling process

*   If `vaddr` is omitted as well, then the `vaddr` argument supplied to the
    syscall is the address of a local (stack) variable of the calling process


### Behavior

The `cabinet_inspector` will first call `cabinet_inspector()` with the
specified virtual address and PID, and print out the information returned, in
the following format:

	inspecting cabinet for <virtual address> of pid=<pid>
	paddr: <physical address>
	pf_paddr: <physical address>
	pte_paddr: <physical address>
	pmd_paddr: <physical address>
	pud_paddr: <physical address>
	p4d_paddr: <physical address>
	pgd_paddr: <physical address>
	dirty: [yes|no]
	refcount: <reference count>

Then the `cabinet_inspector` will perform a lookup of the same virtual address
using [pagemap][pagemap]. This interface, provided by the `/proc/` filesystem,
allows us to find the physical address of the page frame the given virtual
address is mapped into, which may be used a point of reference.


### Tips

You may use `/proc/<pid>/maps` to find useful virtual memory addresses to test
this on.

[pagemap]: https://elixir.bootlin.com/linux/v5.10.57/source/Documentation/admin-guide/mm/pagemap.rst


`mmapper`
---------

### Build

Note that `mmapper` will not be built by default.
To build this executable, you will need to run:

	$ make util

This will also create a text file `foo` which you may test with.


### Usage

The usage for `mmapper` is as follows:

	$ ./mmapper [-m] <filepath>

This will `mmap()` the file at `filepath`, read its first byte, and print out
some information that might be useful for testing, including its PID and the
file-mapped virtual address.

The `-m` option will modify the file, writing `C` to its first byte.
