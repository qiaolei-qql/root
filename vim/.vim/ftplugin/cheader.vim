let s:i = 0
function! SelfAdd()
    let s:i = s:i + 1
    return s:i
endf

function! Nohi()
/define/+
endf

call setline(1, "/*")
call append(SelfAdd(), " * filename      : ".expand("%:t"))
call append(SelfAdd(), " * descriptor    :  ")
call append(SelfAdd(), " * author        : qiaolei")
call append(SelfAdd(), " * create time   : ".strftime("%Y-%m-%d %H:%M"))
call append(SelfAdd(), " * modify list   :")
call append(SelfAdd(), " * +----------------+---------------+---------------------------+")
call append(SelfAdd(), " * | date\t\t\t| who\t\t\t| modify summary\t\t\t|")
call append(SelfAdd(), " * +----------------+---------------+---------------------------+")
call append(SelfAdd(), " */")
call append(SelfAdd(), "#ifndef _".toupper(expand("%:t:r"))."_H_")
call append(SelfAdd(), "#define _".toupper(expand("%:t:r"))."_H_")
call append(SelfAdd(), "")
call append(SelfAdd(),"#endif")
unlet s:i
call Nohi()
