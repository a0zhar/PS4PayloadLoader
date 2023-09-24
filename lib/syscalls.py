import urllib.request
import html

def get_freebsd_syscalls():
    file_path = 'syscalls.master'
    with open(file_path, 'r') as file:
        data = file.read()

    data = '\n'.join(i.strip() for i in data.split('\n') if not i.startswith(';')).replace('\\\n', ' ')
    assert data.startswith('$FreeBSD$\n')
    data = data[10:]
    syscalls = {}
    for line in data.split('\n'):
        line = line.strip()
        if not line or 'STD' not in line.split() or line.startswith('#include'):
            continue
        name = line.split('(', 1)[0].split()[-1]
        idx = int(line.split()[0])
        syscalls[idx] = name
    return syscalls

def get_sony_syscalls():
    url = 'http://web.archive.org/web/20210124215126js_/https://psdevwiki.com/ps4/edit/Syscalls'
    with urllib.request.urlopen(url) as response:
        data = response.read().decode('latin-1')
    data = html.unescape(data.split('<textarea ', 1)[1].split('</textarea>', 1)[0])
    syscalls = {}
    for line in data.split('\n'):
        if line.startswith('| '):
            try:
                syscno, fw, syscname, proto, notes = line[2:].split(' || ')
                syscno = int(syscno)
            except ValueError:
                continue
            if syscname.startswith('sys_'):
                syscalls[syscno] = syscname[4:]
    return syscalls

def get_syscalls():
    syscalls = {}
    syscalls.update(get_freebsd_syscalls())
    syscalls.update(get_sony_syscalls())
    syscalls[11] = 'kexec'
    return syscalls


print('section .text')
print('use64')
print()

syscalls = get_syscalls()
for idx, name in sorted(syscalls.items()):
    if '#' in name:
        continue
    print(f'section .text.{name} exec')
    print(f'global {name}')
    print(f'{name}:')
    print('mov rax,', idx)
    print('mov r10, rcx')
    print('syscall')
    print('jc set_err')
    print('ret')
    print()

print('section .text.set_err exec')
print('set_err:')
print('mov [rel errno], eax')
print('xor rax, rax')
print('dec rax')
print('ret')
print()
print('section .bss')
print('global errno')
print('errno resw 1')
