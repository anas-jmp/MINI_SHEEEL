# ifndef MINI_SHELL_H
# define MINI_SHELL_H
# include <stdlib.h>
# include <unistd.h>
# include <stdio.h>
# include <readline/readline.h>
# include <readline/history.h>
# include <string.h>
# include <fcntl.h>
# include <sys/stat.h>
#include <ctype.h>  
#include <stdio.h>
  
#define IS_STRING_TOKEN(type) ((type) == WORD || (type) == DOUBLE_QUOTED_STRING || (type) == SINGLE_QUOTED_STRING)
 // token.h

typedef enum {
    WORD,
    EXPANDABLE,
    INPUT_REDIRECTION,
    OUTPUT_REDIRECTION,
    OUTPUT_REDIRECTION_APPEND_MODE,
    PIPES,
    SINGLE_QUOTED_STRING,
    DOUBLE_QUOTED_STRING,
    HERE_DOC
} token_type;

typedef struct s_parser_state {
    const char **str; // Input string pointer
    char **env;       // Environment variables
    int status;       // Status for variable expansion (e.g., last exit code)
} t_parser_state;

typedef struct token 
{
    char *value;
    token_type token_type;
    struct token *next;
} token;

typedef struct s_counters
{
    int arg_count;
    int max_args;
    int input_count;
    int max_inputs;
    int output_count;
    int max_outputs;
    int heredoc_count;
    int max_heredocs;
} t_counters;

typedef struct s_env
{
    char *key;
    char *value;
    struct s_env *next;
} t_env;

typedef struct s_shell_state
 {
    int exit_status;
    int state;
} t_shell_state;
 

 t_command *parse_command(token *tokens);

//void handle_exiting_2(char *args,char *arg2);
// int handle_exit(char **args);
// void printing(int status);
void write_exit_status_to_file(int status);

int handle_exit(char **args, t_shell_state *shell_state);
char **handle_env(t_env *env_list);
 void handle_command_name(token **tokens, t_command *cmd);
void append_command(t_command **head, t_command **current, t_command *cmd);
char *next_tok_par(const char *start,const char **str,char **env, token_type *type);
char *next_tok_par1(const char *start,const char **str,char **env,int *status ,token_type *type);
char *next_tok_par2(const char *start,const char **str,char **env,int *status ,token_type *type);
char *next_tok_par3(const char *start,const char **str,char **env,int *status ,token_type *type);
char **resize_array(char **old_array, int old_size, int new_size);
int *resize_int_array(int *old_array, int old_size, int new_size);
void is_exit_with_signal(int *status);
 
//void    is_exit_with_signal(int *status);

// char *handle_echo_command(char *input, char **env);
  char	*ft_itoa(int nb);
// pid_t ending_child_processe(t_command *prompt , char *cmd_path , char **argv , int **pipefd , char **env , int j );
void display_ascii_art(void);
 char *expand_variable(char *var_name, char **env);
int validate_syntax(token *tokens);
void echo_exit_status(const char *input);
int check_unmatched_quotes(char *input);
 char *next_token(const char **str, char **env, token_type *type);
extern t_env *env_list;
extern int global_exit_status;

/******************************************/
char  *get_redirection_file(const char *command);
int   count_env_vars(t_env *env_list);
void  print_env(t_env *env_list);
char  **build_envp_array(t_env *env_list);
void  execute_command_fa(char *command_path, char **argv, t_env *env_list);
void  execute_env_command(t_env *env_list, const char *output_file); 
/**********************************************/
void split_key_value(char *input, char **key, char **value);
void handle_unset(const char *key); 
void handle_export_command(char *input);
void store_env_variable(char *key, char *value);
void free_command(t_command *cmd);
char *handle_quoted_string_with_expansion(char *str, char **env);
char *extract_quoted_string(const char **str, char **env);
void handle_expand_command(char *input, char **env);
/**********************************************/
char	*ft_strjoin(char const *s1, char const *s2);
int     pipex(t_command *prompt , char **env);
void    dup_fds(int old_fd , int new_fd);
void output_redirection(char *file_path);
int redidrect_stdin_to_herdoc(char *delimiter , int heredoc_fd) ;
void execute_command(char *command_path , char **argv , char **env);
void output_error_exit(char *error_msg , int status);
char	*is_command(char *cmd, char **env);
//char ft_cd(char* path);
char	*cmd_helper(char *cmd, char *PATH_VARIABLE);
char	*get_command_path(char *cmd, char *PATH_VARIABLE);
void 	ft_free_split(char **split);
void handle_echo_command(char *input, char **env);
/**** */
// t_command *parse_command(token *tokens, char **env);
int validate_syntax(token *tokens);
void print_parsed_command(t_command *cmd);
void execute_pipeline(t_command *cmd_list);
 
/* Lexer and parser functions */
    typedef int (*redir_func_ptr)(char *token);
// token *tokenizer(char *prompt);
token *tokenizer(char *prompt, char **env);
token *tokenize_input(const char *input, char **env);
/* Function declarations */
void split_key_value(char *input, char **key, char **value);
token_type identify_token_type(char *token_value);
char *extract_quoted_string(const char **str, char **env);
// print_parsed_command
void execute_pipeline(t_command *cmd_list);
// void handle_echo_command(char *input, char **env);
void     free_parsed_command(t_command *cmd);
void     free_token_list(token *head);
void     shell_loop(char **env) ;
void     print_tokens(token *head);
void     free_token_list(token *head);
token    *setup_first_node(char **split_prompt, redir_func_ptr *redirection_functions);
void     setup_nodes(char **split_prompt, redir_func_ptr *redirection_functions, token **first_node, int i);
char	 **ft_split(const char *str, char *delimiters);


int     is_INPUT_REDIRECTION(char *token);
int     is_OUTPUT_REDIRECTION_APPEND_MODE(char *token);
int     is_OUTPUT_REDIRECTION(char *token);
int     is_PIPES(char *token);
int     is_WORD(char *token);

 
// Parse tokens into a t_command structure
// t_command *parse_command(token *tokens, char **env);
// Resize the arguments array dynamically
char **resize_args(char **args, int current_size, int new_size);
// Free memory associated with a t_command structure
void free_command(t_command *cmd);
// Handle input redirection, output redirection, and append mode
void handle_redirections(t_command *cmd);
// Debugging utility for tokens (optional)
void print_tokens(token *head);

#endif