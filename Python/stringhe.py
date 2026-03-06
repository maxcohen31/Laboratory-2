from os.path import isdir
from sys import argv
import os
from typing import List


class Stringa:
    def __init__(self, s) -> None:
        self.stringa = s
        self.occorrenze = 0
        self.numfiles = 0
        self.files = [] # store the files where the string is found
    
    def __str__(self) -> str:
        return f"{self.stringa:<20} {self.occorrenze:>4} {self.numfiles:>4}"


def build_string(file, word) -> Stringa:
    word_count = 0
    with open(file, "r") as f:
        for line in f:
            word_count += line.count(word)

    string_res = Stringa(word)
    string_res.occorrenze = word_count
    string_res.numfiles = 1 if word_count > 0 else 0    
    if word_count > 0: 
        string_res.files.append(file)
    return string_res

def find_words_in_src(src, word) -> List[Stringa]:
    matches = []

    # scr must be a directory
    assert(isdir(src)), "[Error]: src must be a directory"

    src_files = os.listdir(src)
    for file in src_files:
        #print(file)
        full_name = os.path.join(src, file)
        # Distinguish between a file and a directory
        if not isdir(full_name):
            # full_name is a file
            matches.append(build_string(full_name, word))
        else:
            # full_name is a directory
            if not os.access(full_name, os.R_OK | os.X_OK):
                print(f"{full_name} is not accessible. Permession denied")
            matches.extend(find_words_in_src(full_name, word))

    return matches

def main():
    result = {}
    list_of_strings = []
    src = argv[1]
    destination_dir = argv[2]

    for w in argv[3:]:
        list_of_strings.extend(find_words_in_src(src, w))
        if w not in result:
            result[w] = Stringa(w)
        
    for s in list_of_strings:
        if not s.stringa:
            continue
        result[s.stringa].occorrenze += s.occorrenze
        result[s.stringa].numfiles += s.numfiles
        result[s.stringa].files.extend(s.files)

    sorted_output = sorted(result.values(), key=lambda x: x.occorrenze, reverse=True)

    
    # for v in sorted_output:
        # print(v, v.files)
    
    try:
        os.mkdir(destination_dir)
    except FileExistsError:
        print(f" {destination_dir} already exists!")
        
    for s in sorted_output:
        path = os.path.join(destination_dir, s.stringa)
        os.makedirs(path, exist_ok=True)

        for idx, file in enumerate(s.files):
            link_name = os.path.join(path, f"{idx:04d}")
            try:
                os.symlink(os.path.abspath(file), link_name)
            except FileExistsError as e:
                print(f"{e}: File already exists")
        
        
                
if __name__ == "__main__":
    main()
