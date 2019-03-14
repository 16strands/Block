# Master Block & Block Map
The low-level interface that a disk device presents to filesystems is block-level input/output. This project is an extension of my [Bit Set](https://github.com/16strands/BitSet) project, where the Bit Set takes the form of a Block Map. This project also defines the structure of and functions related to the Master Block. In future editions of the project I will extend this into a functional filesystem.

## Getting Started

Use the makefile provided to create the executable shell. Run
```
./myShell
```
to use the shell. For a list of options, use
```
help
```
within the shell.

## Testing

Testing for this project was mostly done in the console by calling the shell functions and testing that the Bit Map is set correctly (and persists through unmounting and remounting) and that allocating a new block returns a new number each time and updates the Bit Map. If I were to write specific test code (which would be benificial), I'd borrow memory management testing code from the Bit Set project, and I'd write some python code to print a robust series of relevant shell commands, then I'd pipe that output to the shell for this project and watch for abnormalities. 

## Acknowledgments 

* Thanks to Dylan McNamee for writing and assisting with the assignment 




