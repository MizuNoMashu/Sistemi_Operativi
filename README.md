# Sistemi_Operativi
Shell-C

**Compilazione**
Per compilare è necessario installare la liberia readline:
- sudo apt-get install libreadline6 libreadline6-dev
- make e poi ./Term
In caso di errore dare i permessi di esecuzione a .my_bashrc.sh

**clean_term()**

Questa funzione pulisce il terminale.

**location()**

Questa funzione mi fornisce la posizione attuale.

**controll_keyboard()**

Questa funzione è in grado di riconoscere la pressione del tasto ESC e delle frecce sue e giù della tastiera.

**exiting()**

Questa funzione è usata per uscire dal terminale.

**keyup()**

Questa funzione è usata per scandire la cronologia.

**keydown()**

Questa funzione è usata per scandire la cronologia.

**get_command(FILE\* history)**

Questa funzione prende il comando in input e lo inserisce nella cronologia.

**get_num_token(char\* command)**

Questa funzione conta il numero di token (ogni elemento separato da spazio, tab o newline) che compongono il comando.

**allocate_term_arg(int length_command)**

Questa funzione serve ad allocare la struct utile per il comando.

**destroy_term_arg(terminal_arg\* term)**

Questa funzione distrugge la struct una volta che è stata allocata.

**get_token(char\* command , int length_command , int num_token)**

Questa funzione permette di ottenere i token necessari successivamente per la exec, inoltre permette di caricare uno script per avere alcuni comandi personalizzati. 

**count_ecom(char\*\* token)**

Questa funzione conta il numero di '&' all'interno del comando.

**count_pipe(char\*\* token)**

Questa funzione permette di contare il numero di '|' all'interno del comando.

**custom_execvp(char\*\* token , pid_t child , int n_pipe , int n_thread , int still_good)**

Questa funzione fornisce una gestione del comando per ogni caso riconosciuto, sia nel caso di concatenazione semplice '&&' , '||' , ';' sia nel caso di redirect di input e output.

**do_custom_execvp(char\*\* token , char\* command_separator , pid_t child , int n_pipe , int n_thread , int still_good)**

Questa funzione permette di gestire l'esecuzione di '&&' e '||'.

**do_custom_execvp_semicolon(char\*\* token , pid_t child)**

Questa funzione permette di gestire l'esecuzione di ';'.

**do_custom_execvp_ecom(char\*\* token , pid_t child , int n_thread , int n_pipe , int still_good)**

Questa funzione permette di gestire l'esecuzione di '&'.

**do_custom_execvp_pipe(char\*\* token , int n_pipe , int pipe_fd , int last_pipe , pid_t child)**

Questa funzione permette di gestire l'esecuzione di '|'.

**do_custom_execvp_redirect(char\*\* token , char\* command_separator , pid_t child , int n_thread , int n_pipe , int still_good)**

Questa funzione permette di gestire l'esecuzione per quanto riguarda il redirect di input e output.

**handle_signal(int signal)**

Questa funzione gestisce i segnali una volta fatta la fork.

**handle_signal_less(int signal)**

Questa funzione gestisce i segnali prima della fork.
