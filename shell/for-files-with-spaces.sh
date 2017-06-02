#!/bin/bash

dir=/tmp/${USER}/getlist.test/
mkdir -p "$dir"
cd "$dir"

touch       'file not starting foo' foo foobar barfoo 'foo with spaces'\
    'foo with'$'\n'newline 'foo with trailing whitespace      '

# while with process substitution, null terminated, empty IFS
getlist0() {
    while IFS= read -d $'\0' -r file ; do
            printf 'File found: '"'%s'"'\n' "$file"
    done < <(find . -iname 'foo*' -print0)
}

# while with process substitution, null terminated, default IFS
getlist1() {
    while read -d $'\0' -r file ; do
            printf 'File found: '"'%s'"'\n' "$file"
    done < <(find . -iname 'foo*' -print0)
}

# pipe to while, newline terminated
getlist2() {
    find . -iname 'foo*' | while read -r file ; do
            printf 'File found: '"'%s'"'\n' "$file"
    done
}

# pipe to while, null terminated
getlist3() {
    find . -iname 'foo*' -print0 | while read -d $'\0' -r file ; do
            printf 'File found: '"'%s'"'\n' "$file"
    done
}

# for loop over subshell results, newline terminated, default IFS
getlist4() {
    for file in "$(find . -iname 'foo*')" ; do
            printf 'File found: '"'%s'"'\n' "$file"
    done
}

# for loop over subshell results, newline terminated, newline IFS
getlist5() {
    IFS=$'\n'
    for file in $(find . -iname 'foo*') ; do
            printf 'File found: '"'%s'"'\n' "$file"
    done
}


# see how they run
for n in {0..5} ; do
    printf '\n\ngetlist%d:\n' $n
    eval getlist$n
done

rm -rf "$dir"
