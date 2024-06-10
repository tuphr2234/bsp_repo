#!/bin/sh
set -e



BINARIES_DIR="${0%/*}/"
# shellcheck disable=SC2164
cd "${BINARIES_DIR}"

mode_serial=false
mode_sys_qemu=false
while [ "$1" ]; do
    case "$1" in
    --serial-only|serial-only) mode_serial=true; shift;;
    --use-system-qemu) mode_sys_qemu=true; shift;;
    --) shift; break;;
    *) echo "unknown option: $1" >&2; exit 1;;
    esac
done

if ${mode_serial}; then
    EXTRA_ARGS='-nographic'
else
    EXTRA_ARGS='-serial stdio'
fi

if ! ${mode_sys_qemu}; then
    export PATH="/home/ubuntu/Downloads/buildroot-2024.02.2/output/host/bin:${PATH}"
fi


exec qemu-system-arm -M versatilepb -kernel zImage -dtb versatile-pb.dtb -drive file=rootfs.ext2,if=scsi,format=raw -append "rootwait root=/dev/sda console=ttyAMA0,115200" ${EXTRA_ARGS} "$@" -netdev tap,id=mynet0,ifname=tap0,script="/home/ubuntu/Downloads/buildroot-2024.02.2/ifup.sh",downscript=no -device rtl8139,netdev=mynet0
