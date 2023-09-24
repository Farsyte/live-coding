#!/usr/bin/env python3

def recov(arg):
    with open(arg, "r") as f:
        src = f.readline()
        src = src[23:].strip()
        for line in f:
            if line.startswith('    #####:'):
                line = line[11:].strip()
                print(f'{src}:{line}')

def main(argv):
    for arg in argv[1:]:
        recov(arg)

if __name__ == "__main__":
    import sys
    main(sys.argv)
