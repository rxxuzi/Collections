" rxx vim setting file."

" Basic settings"
set number
set tabstop=4
set shiftwidth=4
set expandtab
set autoindent

" Ex settings"
set incsearch
set hlsearch
set ignorecase
set smartcase
set scrolloff=8     
set sidescrolloff=8 
set showmatch       
set mouse=a         
set t_Co=256

"Syntax highlighting"
syntax enable

" Custom color settings"

highlight Normal ctermfg=15 ctermbg=NONE
highlight LineNr ctermfg=242 ctermbg=NONE
highlight Cursor ctermfg=15 ctermbg=225       
highlight CursorLine cterm=NONE ctermbg=NONE

"75 = #5fafff 159 = #afffff 123 = #87ffff 153 = #afd7ff "
highlight Identifier ctermfg=75 ctermbg=NONE
highlight String ctermfg=123 ctermbg=NONE
highlight SpecialChar ctermfg=159 ctermbg=NONE
highlight Number ctermfg=153 ctermbg=NONE


" #af87ff #d7afff "
highlight PreProc ctermfg=141 ctermbg=NONE 
highlight Constant ctermfg=183 ctermbg=NONE

" 205=#ff5faf"
highlight Statement ctermfg=205 ctermbg=NONE
highlight Type ctermfg=205 ctermbg=NONE  

" 219=#ffafff"
highlight Function ctermfg=219 ctermbg=NONE
highlight CustomFunc ctermfg=219 ctermbg=NONE

" 225=#ffd7ff"

highlight Operator ctermfg=225 ctermbg=NONE
highlight SpecialKey ctermfg=225 ctermbg=NONE 


"118 = #87ff00" 
highlight MethodMember ctermfg=118 ctermbg=NONE

" 45 = #00d7ff "
highlight ClassName ctermfg=45 ctermbg=NONE

" 238  guifg=#444444"
highlight Comment ctermfg=238 ctermbg=NONE

" シンタックスマッチングの設定"
syntax match CustomFunc /\<\h\w*\>\s*(/me=e-1
syntax match Operator /[+\-*/%=<>!&|^~]/
syntax match SpecialKey /[*&]/
syntax match MethodMember /\.\h\w*\>/
syntax match MethodMember /->\h\w*\>/
syntax match MethodMember /\<\h\w*\>::/
syntax match MethodMember /\<lambda\>/
syntax match ClassName /\<[A-Z]\w*\>/

" ファイルタイプ別の設定"
augroup SyntaxSettings
    autocmd!
    autocmd FileType * syntax match CustomFunc /\<\h\w*\>\s*(/me=e-1
    autocmd FileType * syntax match Operator /[+\-*/%=<>!&|^~]/
    autocmd FileType * syntax match SpecialKey /[*&]/
    autocmd FileType * syntax match MethodMember /\.\h\w*\>/
    autocmd FileType * syntax match MethodMember /->\h\w*\>/
    autocmd FileType * syntax match MethodMember /\<\h\w*\>::/
    autocmd FileType * syntax match MethodMember /\<lambda\>/
    autocmd FileType * syntax match ClassName /\<[A-Z]\w*\>/
    autocmd FileType * syntax match Comment /\/\/.*$/
    autocmd FileType * syntax region Comment start="/\*" end="\*/"
augroup END

" シンタックスの再適用"
autocmd BufEnter * syntax sync fromstart


"Status Line"
set laststatus=1
set statusline=%F%m%r%h%w\ [FORMAT=%{&ff}]\ [TYPE=%Y]\ [POS=%l,%v][%p%%]\ [BUFFER=%n]\ %{strftime('%c')}


" Auto-closing brackets"
inoremap ( ()<Left>
inoremap [ []<Left>
inoremap { {}<Left>
inoremap " ""<Left>
inoremap ' ''<Left>
inoremap ` ``<Left>

" Enable backspace and delete keys
set backspace=indent,eol,start

" Allow moving to the next/previous line with right/left arrow keys"
set whichwrap+=<,>,[,]

" Enable cursor to move to the next/previous line with h and l keys"
set whichwrap+=h,l

" コメントをトグルする関数"
function! ToggleComment()
    let l:line = getline('.')
    if l:line =~ '^\s*//'
        let l:line = substitute(l:line, '^\(\s*\)//\s*', '\1', '')
    else
        let l:line = substitute(l:line, '^\(\s*\)', '\1// ', '')
    endif
    call setline('.', l:line)
endfunction

" InsertモードでのCtrl + / キーマッピング"
inoremap <C-/> <Esc>:call ToggleComment()<CR>i

" Ctrl + Z のデフォルト動作を無効化"
nnoremap <C-Z> <Nop>
inoremap <C-Z> <Nop>
vnoremap <C-Z> <Nop>

" Ctrl + Z で元に戻す"
nnoremap <C-Z> u
inoremap <C-Z> <C-O>u
vnoremap <C-Z> <Esc>u<gv>

" Ctrl + Y でやり直す"
nnoremap <C-Y> <C-R>
inoremap <C-Y> <C-O><C-R>
vnoremap <C-Y> <Esc><C-R><gv>

" pastetoggle の設定 (例: F2キーで paste モードをトグル)"
set pastetoggle=<F2>

" Ctrl + V で paste モードをトグル"
nnoremap <C-V> :set paste!<CR>i
inoremap <C-V> <Esc>:set paste!<CR>gi

" フォールディングを有効化"
set foldmethod=indent
set foldlevel=99

" Ctrl+Spaceでフォールドの開閉"
nnoremap <C-Space> za
inoremap <C-Space> <C-O>za

" HTML/XMLタグの自動補完とカーソル位置の調整"
function! s:ClosingTagAndPosition()
    let line = getline('.')
    let col = col('.')
    let tagname = matchstr(line[0:col-2], '<\zs\w\+\ze')
    if tagname != ''
        return '</'.tagname.'>' . repeat("\<Left>", len(tagname) + 3)
    else
        return '>'
    endif
endfunction

augroup HtmlXmlAutoClose
    autocmd!
    autocmd FileType html,xml inoremap <buffer> > ><C-R>=<SID>ClosingTagAndPosition()<CR>
augroup END

