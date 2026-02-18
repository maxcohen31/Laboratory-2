from posixpath import realpath
from sys import argv, stdlib_module_names
from os import R_OK, X_OK, F_OK, access
from os import path
from os import listdir
from sys import stderr


def elenco_file(directory):
    def aux(current, visited, depth):
        assert path.isdir(directory), "Argument is not a directory" 
        result = []

        # Content of our starting directory
        current_content = listdir(current) 

        for file in current_content:
            complete_name = path.join(current, file)
            # Is a file and it is accessable?
            if not access(complete_name, F_OK):
                print(f"[Broken Link] {complete_name}", stderr)
                continue
            if not path.isdir(complete_name):
                # file found and is accessable
                new_dim = path.getsize(complete_name)
                new_name = complete_name

                # get the real name of the directory and add it to visited
                real_name = path.abspath(complete_name)
                visited.add(real_name)



        return result
    aux(directory, [], 0)



def main():
    if len(argv) != 2:
        print("Usage: python3 <program_name> <dir_name>")
    else:
        elenco_file(argv[1])

if __name__ == "__main__":
    main()
