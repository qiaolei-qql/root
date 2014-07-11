
let g:project_data = "project_vim"
let proj_data = finddir(g:project_data, getcwd() . ',.;')
echo proj_data
echo getcwd() . '/' .proj_data
"echo "find . -name '*.h' -o -name '*.c' -o -name '*.cpp'  | grep -v thirdparty > ".  "/tag.files " . getcwd()
"call system("echo a >" .  "./tag.files " . getcwd())
call system("find . -name '*.h' -o -name '*.c' -o -name '*.cpp'  | grep -v thirdparty >  " . proj_data . "/tag.files ")
call system('ctags -R --c++-kinds=+p --fields=+iaS --extra=+q -o ' . proj_data . '/tags '. ' -L '  . proj_data . '/tag.files ' )
echo 'cscope -Rbqk -f ' . proj_data . "/cstags" . ' -i ' . proj_data . '/tag.files '
call system('cscope -Rbqk -f ' . proj_data . "/cstags " . ' -i ' . proj_data . '/tag.files ')
