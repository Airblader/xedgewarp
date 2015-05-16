# xedgewarp

## About

xedgewarp is a window manager agnostic standalone tool that warps the mouse pointer between outputs when necessary.
This is useful in setups with multiple screens where the screens are not aligned exactly next to each other or have different resolutions. Consider the following example:

    +-------+
    |      >|
    |   1   +-------+
    |       |       |
    +-------+   2   |
            |       |
            +-------+

If you move your cursor from left to right towards the edge that is marked with a `>`, the cursor will simply hit the edge and cannot be moved any further (unless your window manager / desktop environment already takes care of it). However, what you might *want* to happen is that the cursor is warped over to the second output.

If this is something you want, but your setup lacks -- xedgewarp is the tool for you!

## Credits

Having learned a lot by contributing to [i3/i3](https://github.com/i3/i3), this is also where a lot of the credit should go; namely to its author Michael Stapelberg. Credit for X11::XCB, which is used for testing, goes to the same person.

## Installation

### Arch / Manjaro (AUR)

xedgewarp is available in the AUR as [xedgewarp-git](https://aur.archlinux.org/packages/xedgewarp-git/).

### Manual

xedgewarp is make-based. Hence, you can clone the git repository and compile and install it via

````
git clone https://github.com/Airblader/xedgewarp
cd xedgewarp
make
sudo make install
```

## Usage

See `man xedgewarp` after installation.
