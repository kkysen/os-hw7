# hw7

## Authors
Khyber Sen, ks3343
Isabelle Arevalo, ia2422
Wenching Li, wl2795

## Instructions

This file should contain:

- Your name & UNI (or those of all group members for group assignments)
- Homework assignment number
- Description for each part

The description should indicate whether your solution for the part is working
or not. You may also want to include anything else you would like to
communicate to the grader, such as extra functionality you implemented or how
you tried to fix your non-working code.

## Description
We think our solution is working.

Definitely for the `pf_paddr` and `refcount` fields,
which we tested in the simulated shell sessions.
Though we were a bit unsure for the `p**_paddr` fields,
specifically whether they should have indices added
like the `p**_offset` functions do.
But example shell sessions showed page-aligned addresses,
so we did not add the indices.

For testing, we replicated the 3 example sessions in
`./user/test/FireFerrises/test.py`.
The tests can be run by running `make test` in `user/test/FireFerrises/`.
It runs the commands (or Python equivalents) in the shell sessions
and verifies their invariants hold.
However, we used our slightly-modified copy of `mmapper.c`,
which just inserts `fflush(stdout)` before `pause()`
so Python doesn't hang on reading.
Also, this only checks the `pf_paddr` and `refcount` fields.

In running these, tests, besides seeing that these invariants held:
* the `pf_paddr` matching proc pagemap
* `libc.so` having the same frame address across multiple executables
* the `refcount` increasing when we `mmapper`ed the same file

We also noticed that our `refcount` starts at 4,
unlike 3 in the example sessions.
But the difference in `refcount`s as we re-mapped was the same.

We also checked a bunch of error conditions on the command line.
When we tried a low pid, like 16,
we found that `task->mm == NULL` because it's a kernel thread.
Since kernel threads use the kernel address space,
we used init's mm instead for this.
However, since `init_mm` is not exported,
we had to set `pid = 1` and redo the lookup
(`init_task.mm` was also `NULL`).
