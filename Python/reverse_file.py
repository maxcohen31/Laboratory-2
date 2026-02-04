from sys import argv


def reverse_file(file):
    lines = []
    with open(file, "r") as f:
        for line in f:
            lines.append(line.strip('\n'))
    with open("test.txt.rev", "w") as g:
        for l in lines[::-1]:
            print(l, file=g)

def main():
    if len(argv) != 2:
        print("Usage: python3 reverse.py <filename>")
    else:
        reverse_file(argv[1])

if __name__ == "__main__":
    main()
    

