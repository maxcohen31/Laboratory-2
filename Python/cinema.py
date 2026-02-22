from argparse import Namespace
from os import name, write
from sys import argv


def get_professions(filename):
    """
    Take an file as parameter and returns a map built as follow:
        key: name - professions: list_of_professions
    """
    profesions = dict()
    names = dict()
    with open(filename, "r") as f:
        line = f.readlines()
        for n in line:
            profesions[n.split("\t")[1]] = n.split("\t")[4]
            for p in n.split("\t")[4].split(","):
                names.setdefault(p, []).append(n.split("\t")[1])
    #return result

    #print(names)
    # sort dictionary by key
    sorted_prof = dict(sorted(profesions.items()))
    # write the file actress.txt
    with open("attrici.txt", "w") as f:
        for k,v in sorted_prof.items():
            if "actress" in v:
                f.write(k + '\n')
    with open("attrice.txt", "w") as f2:
        for k,v in names.items():
            if "actress" in k:
                for act in v:
                    f2.write(act + "\n")

def main():
    if (len(argv) < 2):
        print("Usage: <program_name> <file_name>")
    else:
        get_professions(argv[1])

if __name__ == "__main__":
    main()
