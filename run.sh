#!/bin/bash
if [[ "$1" = "gdb" ]]; then
    qemu-system-x86_64 -s -S -hda Image/x64BareBonesImage.qcow2 -m 512 -audiodev coreaudio,id=speaker -machine pcspk-audiodev=speaker 
else
    qemu-system-x86_64 -hda Image/x64BareBonesImage.qcow2 -m 512 -audiodev coreaudio,id=speaker -machine pcspk-audiodev=speaker
fi
# -d int 2>&1 | grep "v="
