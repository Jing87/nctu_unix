from capstone import *
from pwn import *
import binascii as ba

r = remote('up.zoolab.org',2530)
count = 0
while count < 10:
    z = r.recvuntil('>>> ')
    data = r.recvline()
    data = data[:-1]
    z = r.recvuntil('Your answer: ')
    CODE = ba.a2b_hex(data)
    ans = ""
    md = Cs(CS_ARCH_X86, CS_MODE_64)
    for i in md.disasm(CODE, 0x1000):
        s = "%s %s\n" %(i.mnemonic, i.op_str)
        ans = ans + s
    r.sendline("".join("{:02x}".format(ord(c)) for c in ans))
    count = count + 1
r.interactive();