"""
    Date: 13/02/2026
    Abbate Emanuele
"""

from sys import argv, stderr

class Node:
    def __init__(self, key, line) -> None:
        self.key = key
        self.line = line

    def __repr__(self) -> str:
       return f"<{self.key:15}> <{self.line}>"

    def __hash__(self) -> int:
        return hash((self.key, self.line))

    def __eq__(self, other) -> bool:
        return self.key == other.key and self.line == other.line

    def __lt__(self, other) -> bool:
        # sort by key. Otherwise sort by line
        if self.key != other.key:
            return self.key < other.key
        return self.line < other.line

def make_dict_file(file_name) -> None:
    # store the resulting lines
    result = []
    # a set is used instead of an bst
    bst_replacement = set()
    with open(file_name, 'r') as f:
        for line in f:
            result.append(line.rstrip("\n"))
    
    """
    loop over 'result' array and store in bst_replacement a Node object:
    The key is the second word of the line. 
    If missing, it is replaced by the key 'missing'.
    """
    for l in result:
        words = l.split()
        if len(words) >= 2:
            token = words[1]
        # second word is missing
        else:
            token = "MANCA"
        # make a Node object
        node = Node(token, l)
        # found duplicated node
        if node in bst_replacement:
            print(f"Duplicate node found: {node}", file=stderr)
            continue

        bst_replacement.add(node)

    sorted_nodes = sorted(bst_replacement)
    for n in sorted_nodes: 
        print(n)
 
def main() -> None:
    if len(argv) != 2:
        print("Usage: python3 <program_name> <file_name>")
    else:
        make_dict_file(argv[1])
        return

# main driver
if __name__ == "__main__":
    main()
