# Color Code Converter
    
    Program to Convert/Show Color Code
    
    - Format: rgb, hex, hsv
    - Make Color Lighter
    - Make Color Darker

## How to use
#### NOTE: You Need a Terminal which supports Direct/True Color

```
git clone https://github.com/zakarouf/colorcodeconverter
cd colorcodeconverter
./compile
./ccc -h
```

### Arguments
1. `-i[d,x,h]`
    Will take a value in `d`Comma Seperated RGB `x`Hexadecimal `h`Comma Seperated HSV. And then thow the details.
    Example:-
    `./ccc -ix 90a355`
    `./ccc -id 12,34,100`

2. `-l`
    Will Brighten the given color by a given percentage.
    Example:-
    `./ccc -ix 09117d -l 4` <= Will Brigten it up by 4%.


## Screenshots

![1](docs/scr/scr1.png?raw=true)
![2](docs/scr/scr2.png?raw=true)
![3](docs/scr/scr3.png?raw=true)
