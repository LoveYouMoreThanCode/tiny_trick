prepare 1:install jq tool
```
 winget install jqlang.jq
```

prepare 2:install git bash
```
 winget install --id Git.Git -e --source winget
```

generate json file in 'git bash'
```
make --always-make --dry-run  \
| grep -wE 'gcc|g\+\+'  \
| grep -w '\-c' \
| jq -nR '[inputs|{directory:".", command:., file: match(" [^ ]+\\.c").string[1:]}]' \
> build/compile_commands.json
```
