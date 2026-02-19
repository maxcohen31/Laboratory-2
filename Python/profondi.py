from sys import argv, exit
from os import R_OK, X_OK, F_OK, access
from os import path
from os import listdir
from sys import stderr


def elenco_file(directory):
    def aux(current, visited, depth):
        assert path.isdir(current), "Argument is not a directory" 
        result = []

        # Content of our starting directory
        current_content = listdir(current) 
        for file in current_content:
            complete_name = path.join(current, file)
            # Is a file and it is accessable?
            if not access(complete_name, F_OK):
                print(f"[Broken Link] {complete_name}", file=stderr)
                continue
            if not path.isdir(complete_name):
                # File found and is accessable
                new_dim = path.getsize(complete_name)
                new_name = complete_name
                # Adding file to result
                result.append((new_name, new_dim, depth))
            else:
                # Found a directory. Is it explorable?
                if not access(complete_name, R_OK | X_OK):
                    print(f"Directory {complete_name} not accessable")
                    continue
                # Get the absolute path of our directory
                real_name = path.abspath(complete_name)
                # Adding directory to visited
                if real_name in visited:
                    print(f"Directory {complete_name} already explored", file=stderr)
                    continue
                # add our current directory to visited
                visited.append(real_name)
                # New directory found: explore it 
                explored = aux(complete_name, visited, depth + 1)
                result += explored
                print(f"End: {complete_name}", file=stderr)
        return result
    return aux(directory, [], 0)



def main():
    if len(argv) != 2:
        print("Usage: python3 <program_name> <dir_name>")
        exit(1)
    if not path.isdir(argv[1]):
        print("Argument is not a directory")
        exit(2)

    # absolute name
    abs_name = argv[1]
    res = elenco_file(abs_name)

    print("******* LIST OF FILES ******* ")
    #print(res)
    depth = max([tup[2] for tup in res])
    files_at_max_depth = [(tup[0], tup[1]) for tup in sorted(res, key=lambda x: x[1], reverse=True) if tup[2] == depth]
    
    for f,dim in files_at_max_depth:
        print(f"{f} - Dimension: {dim}")
    
if __name__ == "__main__":
    main()
