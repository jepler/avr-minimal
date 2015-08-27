# avr-minimal
A minimal avr-libc project with stdio

You might want a udev rule that looks like this:

```
ATTRS{idVendor}=="0403", ATTRS{idProduct}=="6001", OWNER="you"
```

`make` builds main.elf and programs the device if it is attached.

`make communicate` opens GNU screen to interact with the device over serial.

Use `Makefile.local` to make custom settings such as the correct path to your
device in /dev/serial/by-id.

The Makefile does not have dependency tracking, so put all code in `main.cc`
directly.

License: GNU Permissive License
