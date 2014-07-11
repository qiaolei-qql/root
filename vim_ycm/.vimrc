set nocompatible

let $LANG="zh_CN.UTF-8"
set encoding=utf-8 
set langmenu=zh_CN.UTF-8
language messages zh_CN.utf-8

"set fileformats=unix,dos
set fileformats=dos,unix
"set fileformat=unix


set termencoding=utf-8
set fileencoding=cp936
"set fileencodings=ucs-bom,utf-8,cp936
set fileencodings=ucs-bom,utf-8,cp936,gb18030,big5,euc-jp,euc-kr,latin1
set nu
"backup
set backup
set writebackup
set backupext=.bak
set backupdir=~/.vim_backup
if !isdirectory($HOME . "/.vim_backup")
	call mkdir($HOME . "/.vim_backup","p",0744)
endif

"set tab width
set tabstop=4
set softtabstop=4
set smarttab
set shiftwidth=4
set expandtab
autocmd FileType c,cpp,xml set expandtab

autocmd FileType c,cpp setl fdm=syntax


" syntax enable
syntax enable

" filetype indent on
:filetype plugin indent on
set nocp
filetype plugin on
" high light seach
set hlsearch

" ignorecase
set ignorecase smartcase

" search when you type the search case
set incsearch

set backspace=indent,eol,start      " backspace can delete everything

"ruler
set ruler

map \{ o{}<Esc>i<CR><Esc>O

" vimgdb
if has("gdb")
	:set previewheight=8
	map <leader>g :run<space>macros/gdb_mappings.vim<CR>
endif

" gui
if has("gui")
	color evening
	:set go=normal
endif

" manpage
: runtime! ftplugin/man.vim

"encoding
":set enc=utf-8,gbk
":set fenc=utf-8,gbk
":set tenc=utf-8,gbk


" NERDTree
"map <leader>l :NERDTreeToggle<CR>

"vimgrep
"nmap <leader>gr :vimgrep /\<<C-R>=expand("<cword>")<CR>\>/ **/*.{c,cc,cpp,hpp,cxx,h}<CR>

"grep
autocmd FileType cpp,c set grepprg=grep\ -nR\ --include=*.{cc,cpp,c,h,cxx,hpp}\ \"$*\"\ .
autocmd FileType python set grepprg=grep\ -nR\ --include=*.py\ \"$*\"\ .
nmap \gr :grep \<<C-R>=expand("<cword>")<CR>\><CR>

"tags
set tags=tags

:helptags ~/.vim/doc/


map <leader>rr <esc>:source ~/.vimrc<CR>
map <leader>ee <esc>:e ~/.vimrc<cr>

" ,n to get the next location (compilation errors, grep etc)
nmap <leader>n :cn<CR>
nmap <leader>p :cp<CR>

" move between windows
nmap <C-h> <C-w>h
nmap <C-j> <C-w>j
nmap <C-k> <C-w>k
nmap <C-l> <C-w>l

"""""""""""""""""""""""
"lookupfile
"""""""""""""""""""""""
"lookupfile plugin use tag
if filereadable("./filenametags")
	let g:LookupFile_TagExpr = '"./filenametags"'
endif
let g:LookupFile_MinPatLength = 2
let g:LookupFile_PreserveLastPattern = 0
let g:LookupFile_PreservePatternHistory = 1
let g:LookupFile_AlwaysAcceptFirst = 1
let g:LookupFile_AllowNewFiles = 0

nmap <silent> <leader>lk :LookupFile<cr>
nmap <silent> <leader>lb :LUBufs<cr>
nmap <silent> <leader>lw :LUWalk<cr>

" lookup file with ignore case
function! LookupFile_IgnoreCaseFunc(pattern)
	let _tags = &tags
	try
		let &tags = eval(g:LookupFile_TagExpr)
		let newpattern = '\c' . a:pattern
		let tags = taglist(newpattern)
	catch
		echohl ErrorMsg | echo "Exception: " . v:exception | echohl NONE
		return ""
	finally
		let &tags = _tags
	endtry
	" Show the matches for what is typed so far.
	let files = map(tags, 'v:val["filename"]')
	return files
endfunction
let g:LookupFile_LookupFunc = 'LookupFile_IgnoreCaseFunc'


"cscope
if has("cscope")
	set cscopequickfix=s-,g-,c-,d-,t-,e-,f-,i- " cscope in quick fix
	nmap <leader>fs :cs find s <C-R>=expand("<cword>")<CR><CR> " find simbol
	nmap <leader>fg :cs find g <C-R>=expand("<cword>")<CR><CR> " find global define
	nmap <leader>cc :cs find c <C-R>=expand("<cword>")<CR><CR> " find this function be called
	nmap <leader>ft :cs find t <C-R>=expand("<cword>")<CR><CR> " find this string
	nmap <leader>eg :cs find e <C-R>=expand("<cword>")<CR><CR> " egrep
	nmap <leader>ff :cs find f <C-R>=expand("<cfile>")<CR><CR> " find this file
	nmap <leader>si :cs find i <C-R>=expand("<cfile>")<CR><CR> " find files include this file
	nmap <leader>sd :cs find d <C-R>=expand("<cword>")<CR><CR> " find functions be called by this function
endif

" load project.vim
if filereadable("project.vim")
	source project.vim
endif

" platform
function! MySys()
	if has("win32")
		return "windows"
	else
		return "linux"
	endif
endfunction

" omnicomplete
set completeopt=longest,menu " don't show the preview window

" taglist
let Tlist_Show_One_File = 1
let Tlist_Exit_OnlyWindow = 1
let Tlist_Sort_Type="name"
"let Tlist_Use_Right_Window = 1
"nmap <silent> <leader>tl :!ctags -R --c++-kinds=+p --fields=+iaS --extra=+q<cr>

"winmanager
let g:winManagerWindowLayout = "FileExplorer,BufExplorer,TagList"
let g:winManagerWidth = 30
let g:defaultExplorer = 0
let g:persistentBehaviour = 0
nmap <C-W><C-F> :FirstExplorerWindow<cr>
nmap <C-W><C-B> :BottomExplorerWindow<cr>
nmap <silent> <leader>wm :WMToggle<cr>

"bufexplorer
let g:bufExplorerDefaultHelp=0
let g:bufExplorerShowRelativePath=1
let g:bufExplorerSplitRight=0        " Split left.
let g:bufExplorerSplitVertical=1     " Split vertically.
let g:bufExplorerSplitVertSize=30  " Split width
let g:bufExplorerUseCurrentWindow=0  " Open in new window.

" csafeguard
autocmd BufNewFile *.h so ~/.vim/ftplugin/cheader.vim

" c/c++ comment mapkeys
"autocmd FileType c,cpp so ~/.vim/ftplugin/ccomment.vim

" script comment mapkeys
"autocmd BufNewFile,BufRead *.{sh,py} so ~/.vim/ftplugin/scomment.vim

" load function comment plugin
"autocmd FileType c,cpp so ~/.vim/ftplugin/cfcomment.vim

" map <f12> to no high light
nmap <f12> :noh<cr>
imap <f12> <esc>:noh<cr>

"by shufawei begin.
"
"map <F2> :ls<Enter>:b
"set path+=/usr/lib/x86_64-redhat-linux4E/include
"set path+=/usr/local/include
set autowrite
set autoread


"" Disable AutoComplPop. 
"let g:acp_enableAtStartup = 0 
"" Use neocomplcache. 
"let g:neocomplcache_enable_at_startup = 1 
"" Use smartcase. 
"let g:neocomplcache_enable_smart_case = 1 
"" Use camel case completion. 
"let g:neocomplcache_enable_camel_case_completion = 1 
"" Use underbar completion. 
"let g:neocomplcache_enable_underbar_completion = 1 
"" Set minimum syntax keyword length. 
"let g:neocomplcache_min_syntax_length = 3 
"let g:neocomplcache_lock_buffer_name_pattern = '\*ku\*' 
"
"let g:neocomplcache_enable_auto_select = 0


"" Recommended key-mappings. 
"" <CR>: close popup and save indent. 
"inoremap <expr><CR>  neocomplcache#smart_close_popup() . "\<CR>" 
"" <TAB>: completion. 
""inoremap <expr><TAB>  pumvisible() ? "\<C-n>" : "\<TAB>" 
""inoremap <expr><space>  pumvisible() ? "\<C-n>" . neocomplcache#close_popup() . "\<SPACE>" : "\<SPACE>"
"inoremap <expr><space>  pumvisible() ? "\<C-n>" : "\<SPACE>"
"" <C-h>, <BS>: close popup and delete backword char. 
"inoremap <expr><C-h> neocomplcache#smart_close_popup()."\<C-h>" 
"inoremap <expr><BS> neocomplcache#smart_close_popup()."\<C-h>" 
"inoremap <expr><C-y>  neocomplcache#close_popup() 
"inoremap <expr><C-e>  neocomplcache#cancel_popup() 
""inoremap <expr><Enter>  pumvisible() ? "\<C-Y>" . neocomplcache#smart_close_popup(): "\<Enter>"
"inoremap <expr><Enter>  pumvisible() ? "\<C-Y>" : "\<Enter>"

" doc gen
"let g:DoxygenToolkit_briefTag_pre="@briedf: "
let g:DoxygenToolkit_paramTag_pre="@Param: "
let g:DoxygenToolkit_returnTag   ="@Returns: "
let g:DoxygenToolkit_blockHeader=""
let g:DoxygenToolkit_blockFooter=""
let g:DoxygenToolkit_authorName="rayqiao"
let g:DoxygenToolkit_licenseTag=""
let g:DoxygenToolkit_briefTag_funcName="yes"
let g:doxygen_enhanced_color=1

let g:DoxygenToolkit_versionString ="" 
map dx :Dox<cr>
map da :DoxAuthor<cr>

filetype off                  " required

"set the runtime path to include Vundle and initialize
set rtp+=~/.vim/bundle/Vundle.vim
call vundle#begin()
" alternatively, pass a path where Vundle should install plugins
"call vundle#begin('~/some/path/here')

" let Vundle manage Vundle, required
Plugin 'gmarik/Vundle.vim'

" The following are examples of different formats supported.
" Keep Plugin commands between vundle#begin/end.
" plugin on GitHub repo
Plugin 'tpope/vim-fugitive'
Plugin 'Valloric/YouCompleteMe'
" plugin from http://vim-scripts.org/vim/scripts.html
Plugin 'L9'
" Git plugin not hosted on GitHub
"Plugin 'git://git.wincent.com/command-t.git'
" git repos on your local machine (i.e. when working on your own plugin)
"Plugin 'file:///home/gmarik/path/to/plugin'
" The sparkup vim script is in a subdirectory of this repo called vim.
" Pass the path to set the runtimepath properly.
Plugin 'rstacruz/sparkup', {'rtp': 'vim/'}
" Avoid a name conflict with L9
"Plugin 'user/L9', {'name': 'newL9'}

" All of your Plugins must be added before the following line
call vundle#end()            " required
filetype plugin indent on    " required
" To ignore plugin indent changes, instead use:
"filetype plugin on
"
" Brief help
" :PluginList          - list configured plugins
" :PluginInstall(!)    - install (update) plugins
" :PluginSearch(!) foo - search (or refresh cache first) for foo
" :PluginClean(!)      - confirm (or auto-approve) removal of unused plugins
"
" see :h vundle for more details or wiki for FAQ
" Put your non-Plugin stuff after this line







"自动补全配置
"让Vim的补全菜单行为与一般IDE一致(参考VimTip1228)
set completeopt=longest,menu
"离开插入模式后自动关闭预览窗口
autocmd InsertLeave * if pumvisible() == 0|pclose|endif
"回车即选中当前项
inoremap <expr> <CR>       pumvisible() ? "\<C-y>" : "\<CR>"
"上下左右键的行为 会显示其他信息
"inoremap <expr> <Down>     pumvisible() ? "\<C-n>" : "\<Down>"
"inoremap <expr> <Up>       pumvisible() ? "\<C-p>" : "\<Up>"
"inoremap <expr> <PageDown> pumvisible() ? "\<PageDown>\<C-p>\<C-n>" :
"\<PageDown>"
"inoremap <expr> <PageUp>   pumvisible() ? "\<PageUp>\<C-p>\<C-n>" :
"\<PageUp>"

"youcompleteme  默认tab  s-tab 和自动补全冲突
let g:ycm_key_list_select_completion=['<c-n>', '<space>']
let g:ycm_key_list_previous_completion=['<c-p>']

let g:ycm_collect_identifiers_from_tags_files=1	" 开启 YCM 基于标签引擎
"从第2个键入字符就开始罗列匹配项
let g:ycm_min_num_of_chars_for_completion=2
let g:ycm_cache_omnifunc=0	" 禁止缓存匹配项,每次都重新生成匹配项
let g:ycm_seed_identifiers_with_syntax=1	" 语法关键字补全
"force recomile with syntastic
nnoremap <leader>yc :YcmForceCompileAndDiagnostics<CR>
nnoremap <leader>ys :YcmRestartServer<CR>
"nnoremap <leader>lo :lopen<CR>	"open locationlist
"nnoremap <leader>lc :lclose<CR>	"close locationlist
inoremap <leader><leader> <C-x><C-o>
"在注释输入中也能补全
let g:ycm_complete_in_comments = 1
"在字符串输入中也能补全
let g:ycm_complete_in_strings = 1
"注释和字符串中的文字也会被收入补全
let g:ycm_collect_identifiers_from_comments_and_strings = 0

" for ycm
let g:ycm_error_symbol = '>>'
let g:ycm_warning_symbol = '>*'
nnoremap <leader>gl :YcmCompleter GoToDeclaration<CR>
nnoremap <leader>gf :YcmCompleter GoToDefinition<CR>
nnoremap <leader>gg :YcmCompleter GoToDefinitionElseDeclaration<CR>
nmap <F4> :YcmDiags<CR>

let g:ycm_collect_identifiers_from_tag_files = 1
let g:ycm_global_ycm_extra_conf = '~/.vim/bundle/YouCompleteMe/third_party/ycmd/cpp/ycm/.ycm_extra_conf.py'
"let g:ycm_global_ycm_extra_conf = '/home/rayqiao/workspace/phero_server_proj/.ycm_extra_conf.py'
"let g:ycm_global_ycm_extra_conf = '/home/rayqiao/179_data/phero_server_proj/.ycm_extra_conf.py'
"关闭加载.ycm_extra_conf.py提示
let g:ycm_confirm_extra_conf=0

let g:ycm_filetype_blacklist = {
      \ 'tagbar' : 1,
      \ 'qf' : 1,
      \ 'notes' : 1,
      \ 'markdown' : 1,
      \ 'unite' : 1,
      \ 'text' : 1,
      \ 'vimwiki' : 1,
      \ 'gitcommit' : 1,
      \ 'svn' : 1,
      \ 'd' : 1,
      \}
