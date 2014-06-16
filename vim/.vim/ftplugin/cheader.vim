let s:i = 0
function! SelfAdd()
    let s:i = s:i + 1
    return s:i
endf

function! Nohi()
/define/+
endf

call setline(1, "/*")
call append(SelfAdd(), " @ file     : ".expand("%:t"))
call append(SelfAdd(), " @ author   : rayqiao")
call append(SelfAdd(), " @ date     : ".strftime("%Y-%m-%d"))
call append(SelfAdd(), " @ brief    :  ")
call append(SelfAdd(), " @ ")
call append(SelfAdd(), " @ ")
call append(SelfAdd(), "*/ ")
call append(SelfAdd(), "#ifndef _".toupper(expand("%:t:r"))."_H_")
call append(SelfAdd(), "#define _".toupper(expand("%:t:r"))."_H_")
call append(SelfAdd(), "")
call append(SelfAdd(),"#endif")
unlet s:i
call Nohi()
