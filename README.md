# Bareflank Hypervisor Trap Interruption
This project is under development.
## Description
Trap Interruption for Current Version of Bareflank Hypervisor

## Compilation / Usage

To setup our extension, run the following (assuming Linux):

```
mkdir build; cd build
cmake ../hypervisor -DDEFAULT_VMM=starter_vmm -DEXTENSION=../trap_interrupt
make -j<# cores + 1>
```

To test out our extended version of Bareflank, run the following commands:

```
make driver_quick
make quick
```

to get status information, use the following:

```
make status
make dump
```

to reverse this:

```
make unload
make driver_unload
```
