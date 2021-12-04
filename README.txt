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
