from sys import argv

def ordina_file(file):
    lines = []
    with open(file, 'r') as f:
        for line in f:
            lines.append(line.strip('\n'))
        
    lines.sort(reverse=True, key=len)
    with open("test.txt.dec", 'w') as g:
        for l in lines:
            print(l, file=g)

def main():
    if len(argv) != 2:
        print("Usage: python3 ordina.py <nomefile> ")
    else:
        ordina_file(argv[1])

if __name__ == "__main__":
    main()
