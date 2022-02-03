alias ll="ls -l"
alias l="ls -al"
alias vim=vi
alias gdb-remote="gdb -ex 'target extended-remote localhost:3333'"

export PATH=$PATH:/mnt/sdcard/bin

if test -f "/mnt/sdcard/.profile"; then
  source /mnt/sdcard/.profile
fi
