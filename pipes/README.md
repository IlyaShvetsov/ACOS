Программа pipes: pipes имена программ
./pipes pr1 pr2  ... prN

Запускает параллельно программы pr1 ... prN (fork,execlp), связывает их каналами (pipe, dup2). 
Т.е. получен аналог в shell:
pr1 | pr2 | ... | prN
