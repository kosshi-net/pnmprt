# pnmprt
A silly tool to print PNM images in a terminal. Displays two pixels per 
character using half-blocks. 

## File support 
Only PNM files of type P6 (binary color) are supported. This format is default
output of tools such as 'pngtopnm'.

## Print modes
#### Linux Virtual Terminal (--vt)
Prints full RGB in the Linux TTY, by hacky abuse of color change escape sequences and 
framebuffer persistence. The printed image breaks on any scroll or redraw,
so clear the screen first. 

#### TrueColor (--tc)
Prints with TrueColor codes. The best method, but requires a "modern" 
terminal.

#### 256 Color (--256)
Prints with the standard 256 colors.

#### 16 Color (--ansi, --16)
Prints with the limited standard 16 colors. 

## Todo
The 256 and 16 color modes use a very naive color algorithm, the output could 
be much better.

PNM support is also bit shoddy. 

# Build and install
Requires C99, no dependencies. 

```
make && make install
```

# License
GPLv3
