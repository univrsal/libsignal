#!/bin/bash
grep -rl 'Copyright 2019' ./src | xargs sed -i 's/Copyright 2019/Copyright 2020/g'
grep -rl 'Copyright 2019' ./tests | xargs sed -i 's/Copyright 2019/Copyright 2020/g'
grep -rl 'Copyright 2019' ./demo | xargs sed -i 's/Copyright 2019/Copyright 2020/g'
find ./src -iname *.h* -o -iname *.c* | xargs clang-format -style=file -i -verbose
find ./tests -iname *.h* -o -iname *.c* | xargs clang-format -style=file -i -verbose
find ./demo -iname *.h* -o -iname *.c* | xargs clang-format -style=file -i -verbose
