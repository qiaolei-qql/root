"if exists("load_function_comment")
"    finish
"endif
"let load_function_comment=1
"
"function! FunctionComment() 
"    let s:cl = line(".")
"    call append(s:cl, "/*")
"    call append(s:cl+1, " * Function Descript : ")
"    call append(s:cl+2, " * Create Time       : ".strftime("%Y-%m-%d %H:%M"))
"    call append(s:cl+3, " * Parameter List    : ")
"    call append(s:cl+4, " * ")
"    call append(s:cl+5, " * Return            : ")
"    call append(s:cl+6, " * Modify Time       : ")
"    call append(s:cl+7, " */")
"endfunction
"
"nmap <leader>fc :call FunctionComment()<cr>
