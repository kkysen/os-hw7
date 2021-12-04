#!/usr/bin/env python3

from typing import Iterable, Optional
import subprocess
from dataclasses import dataclass
from pathlib import Path


@dataclass
class PsAux:
    user: str
    pid: int
    cpu: str
    mem: str
    vsz: str
    rss: str
    tty: str
    stat: str
    start: str
    time: str
    command: str


def ps_aux() -> Iterable[str]:
    output = subprocess.check_output(args=["ps", "aux"])
    for line in output.decode("utf-8").split("\n")[1:]:
        parts = line.split(maxsplit=10)
        if len(parts) != 11:
            continue
        user, pid, cpu, mem, vsz, rss, tty, stat, start, time, command = parts
        yield PsAux(
            user=user,
            pid=int(pid),
            cpu=cpu,
            mem=mem,
            vsz=vsz,
            rss=rss,
            tty=tty,
            stat=stat,
            start=start,
            time=time,
            command=command,
        )


def find_sshds() -> Iterable[PsAux]:
    for p in ps_aux():
        for name in ("sshd", "NetworkManager"):
            if f"/usr/sbin/{name}" in p.command:
                yield p


@dataclass
class ProcMap:
    start: int
    end: int
    permissions: str
    x: str
    time: str
    y: str
    file: str


def read_proc_maps(pid: int) -> Iterable[ProcMap]:
    for line in open(f"/proc/{pid}/maps"):
        parts = line.split(maxsplit=5)
        if len(parts) != 6:
            continue
        range, permissions, x, time, y, file = parts
        start, end = range.split("-")
        yield ProcMap(
            start=int(start, base=16),
            end=int(end, base=16),
            permissions=permissions,
            x=x,
            time=time,
            y=y,
            file=file,
        )


def libc_addr(pid: int, offset: int) -> int:
    for map in read_proc_maps(pid=pid):
        if "libc-" in map.file:
            addr = map.start + offset
            assert addr < map.end
            return addr

@dataclass
class CabInfo:
    paddr: int
    pf_paddr: int
    pte_paddr: int
    pmd_paddr: int
    pud_paddr: int
    p4d_paddr: int
    pgd_paddr: int
    dirty: bool
    refcount: int
    exclusively_mapped: bool
    file_page: bool
    swapped: bool
    present: bool


def inspect_cabinet(addr: Optional[int], pid: Optional[int]) -> CabInfo:
    current_file = Path(__file__)
    test_dir = current_file.parent.parent
    exe_path = test_dir / "cabinet_inspector" / "cabinet_inspector"
    args = [exe_path]
    if addr is not None:
        args.append(hex(addr))
    if pid is not None:
        args.append(str(pid))
    output = subprocess.check_output(args=args)
    lines = output.decode("utf-8").split("\n")
    info = {}
    for line in lines:
        parts = line.split(": ")
        if len(parts) != 2:
            continue
        name, value = parts
        name = name.replace(" ", "_").replace("-", "_")
        if value == "yes":
            value = True
        elif value == "no":
            value = False
        elif value.startswith("0x"):
            value = int(value, base=16)
        else:
            try:
                value = int(value)
            except ValueError:
                pass
        if name in info:
            old_value = info[name]
            assert old_value == value
        else:
            info[name] = value
    return CabInfo(**info)


def session1():
    # check the two dynamically linked libc addresses use the same page
    sshds = find_sshds()
    p1: PsAux = next(sshds)
    p2: PsAux = next(sshds)
    addr1 = libc_addr(pid=p1.pid, offset=0x69)
    addr2 = libc_addr(pid=p2.pid, offset=0x420)
    cab1 = inspect_cabinet(addr=addr1, pid=p1.pid)
    cab2 = inspect_cabinet(addr=addr2, pid=p2.pid)
    assert cab1.pf_paddr == cab2.pf_paddr

def session2():
    # just do internal check
    inspect_cabinet(addr=None, pid=None)


def main():
    session1()
    session2()


if __name__ == "__main__":
    main()
