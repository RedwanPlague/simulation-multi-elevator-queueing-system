nnoremap <silent> <F9> :w<CR>:silent !gnome-terminal -- /bin/bash -c "~/.config/nvim/debug.sh main.cpp"<CR>
nnoremap <silent> <F10> :w<CR>:silent !gnome-terminal -- /bin/bash -c "~/.config/nvim/run.sh main.cpp; echo; read -rs -p 'Press Enter to continue'"<CR>
