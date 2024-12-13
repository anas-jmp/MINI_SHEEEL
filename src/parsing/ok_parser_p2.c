#include "../../include/minishell.h"
#define INITIAL_REDIRECT_SIZE 4 // Initial size for redirection arrays
#define INITIAL_ARGS_SIZE 16
#include <stdbool.h> // for bool, true, false
 

t_command *initialize_command() 
{
    t_command *cmd = malloc(sizeof(t_command));
    if (!cmd)
        return NULL;

    cmd->command = NULL;
    cmd->args = malloc(INITIAL_ARGS_SIZE * sizeof(char *));
    cmd->input_redirect = malloc(INITIAL_REDIRECT_SIZE * sizeof(char *));
    cmd->output_redirect = malloc(INITIAL_REDIRECT_SIZE * sizeof(char *));
    cmd->here_docs = malloc(INITIAL_REDIRECT_SIZE * sizeof(char *));
    cmd->append_modes = malloc(INITIAL_REDIRECT_SIZE * sizeof(int));
    cmd->herdoc_last = 0;
    cmd->here_doc_last_fd = 0;
    cmd->next = NULL;

    if (!cmd->args || !cmd->input_redirect || !cmd->output_redirect || 
        !cmd->here_docs || !cmd->append_modes) {
        free_command(cmd);  // Ensure to clean up properly
        return NULL;
    }

    return cmd;
}


void parse_token(t_command *cmd, token **tokens, char **env) 
{
    if (!cmd || !tokens || !*tokens)
        return;

    if ((*tokens)->token_type == WORD || 
        (*tokens)->token_type == DOUBLE_QUOTED_STRING || 
        (*tokens)->token_type == SINGLE_QUOTED_STRING) {
        // Handle command and arguments
        if (!cmd->command) {
            cmd->command = strdup((*tokens)->value);
            cmd->args[0] = strdup((*tokens)->value);
        } else {
            int arg_count = 0; // Track args here or pass as arguments
            cmd->args[arg_count++] = strdup((*tokens)->value);
        }
    } 
    else if ((*tokens)->token_type == INPUT_REDIRECTION) {
        // Handle input redirection
        (*tokens) = (*tokens)->next;  // Move to the file token
        if (*tokens) {
            int input_count = 0; // Track inputs here or pass as arguments
            cmd->input_redirect[input_count++] = strdup((*tokens)->value);
        }
    }
    // Handle other cases similarly...

    (*tokens) = (*tokens)->next;  // Advance to the next token
}



 
void finalize_command(t_command *cmd, int arg_count, int input_count, int output_count, int heredoc_count) 
{
    if (!cmd)
        return;

    cmd->args[arg_count] = NULL;
    cmd->input_redirect[input_count] = NULL;
    cmd->output_redirect[output_count] = NULL;
    cmd->here_docs[heredoc_count] = NULL;
}
int handle_input_redirection(t_command *cmd, token **tokens, int *input_count, int *max_inputs)
 {
    if ((*tokens)->token_type == INPUT_REDIRECTION) {
        *tokens = (*tokens)->next;
        if (*tokens && ((*tokens)->token_type == WORD || (*tokens)->token_type == DOUBLE_QUOTED_STRING || (*tokens)->token_type == SINGLE_QUOTED_STRING)) {
            if (*input_count >= *max_inputs - 1) {
                *max_inputs *= 2;
                cmd->input_redirect = resize_array(cmd->input_redirect, *input_count, *max_inputs);
                if (!cmd->input_redirect) {
                    return 0;  // Memory allocation failure
                }
            }
            cmd->input_redirect[(*input_count)++] = strdup((*tokens)->value);
            return 1;
        } else {
            printf("\nError: No file specified for input redirection\n");
            return 0;
        }
    }
    return 1;  // No redirection
}

int handle_output_redirection(t_command *cmd, token **tokens, int *output_count, int *max_outputs) {
    if ((*tokens)->token_type == OUTPUT_REDIRECTION || (*tokens)->token_type == OUTPUT_REDIRECTION_APPEND_MODE) {
        int append = (*tokens)->token_type == OUTPUT_REDIRECTION_APPEND_MODE;
        *tokens = (*tokens)->next;
        if (*tokens && ((*tokens)->token_type == WORD || (*tokens)->token_type == DOUBLE_QUOTED_STRING || (*tokens)->token_type == SINGLE_QUOTED_STRING)) {
            if (*output_count >= *max_outputs - 1) {
                *max_outputs *= 2;
                cmd->output_redirect = resize_array(cmd->output_redirect, *output_count, *max_outputs);
                cmd->append_modes = resize_int_array(cmd->append_modes, *output_count, *max_outputs);
                if (!cmd->output_redirect || !cmd->append_modes) {
                    return 0;  // Memory allocation failure
                }
            }
            cmd->output_redirect[*output_count] = strdup((*tokens)->value);
            cmd->append_modes[*output_count] = append;
            (*output_count)++;
            return 1;
        } else {
            return 0;  // Missing file for redirection
        }
    }
    return 1;  // No redirection
}

int handle_heredoc_redirection(t_command *cmd, token **tokens, int *heredoc_count, int *max_heredocs)
 {
    if ((*tokens)->token_type == HERE_DOC) {
        *tokens = (*tokens)->next;
        if (*tokens && ((*tokens)->token_type == WORD || (*tokens)->token_type == DOUBLE_QUOTED_STRING || (*tokens)->token_type == SINGLE_QUOTED_STRING)) {
            if (*heredoc_count >= *max_heredocs - 1) {
                *max_heredocs *= 2;
                cmd->here_docs = resize_array(cmd->here_docs, *heredoc_count, *max_heredocs);
                if (!cmd->here_docs) {
                    return 0;  // Memory allocation failure
                }
            }
            cmd->here_docs[(*heredoc_count)++] = strdup((*tokens)->value);
            return 1;
        } else {
            printf("\nError: No file specified for heredoc redirection\n");
            return 0;
        }
    }
    return 1;  // No here-doc
}
 
int handle_command(t_command *cmd, token **tokens, int *arg_count)
{
    if (!cmd->command && (*tokens)->token_type == WORD) {
        cmd->command = strdup((*tokens)->value);
        cmd->args[(*arg_count)++] = strdup(cmd->command);
        return 1;
    }
    return 0;
}
int handle_argument(t_command *cmd, token **tokens, int *arg_count, int *max_args, char **env)
{
    if (!tokens || !*tokens) 
        return 0; // No token to process, return failure

    // Correctly assign the current token
    token *current_token = *tokens;

    // Check if the token is a valid argument type (WORD, DOUBLE_QUOTED_STRING, or SINGLE_QUOTED_STRING)
    if (current_token->token_type == WORD || 
        current_token->token_type == DOUBLE_QUOTED_STRING || 
        current_token->token_type == SINGLE_QUOTED_STRING) 
    {
        char *arg_value = current_token->value;

        // Check for variable expansion (if the argument starts with '$')
        if (arg_value[0] == '$') 
        {
            char *var_name = arg_value + 1; // Skip the '$'
            char *expanded_value = expand_variable(var_name, env);
            if (expanded_value) 
            {         
                 printf("\n@goood@\n");
                arg_value = expanded_value; // Use the expanded value if available
            }
        }

        // Resize arguments array if necessary
        if (*arg_count >= *max_args - 1) 
        {
            printf("\n@goood2@\n");
            *max_args *= 2;  // Double the size of the arguments array
            cmd->args = resize_array(cmd->args, *arg_count, *max_args);
            if (!cmd->args) 
            {
                return 0; // Memory allocation failure
            }
        }

        // Add the argument to the list
        cmd->args[(*arg_count)++] = strdup(arg_value);
        
        // Move to the next token
        *tokens = current_token->next;

        return 1; // Successfully handled an argument
    }

    return 0; // Not an argument (either a redirection or other token type)
}
void handle_command_name(token **tokens, t_command *cmd)
{
    if (!cmd->command && ((*tokens)->token_type == WORD ||(*tokens)->token_type == DOUBLE_QUOTED_STRING || (*tokens)->token_type == SINGLE_QUOTED_STRING)) {
        cmd->command = strdup((*tokens)->value);
        cmd->args[0] = strdup(cmd->command);
        *tokens = (*tokens)->next; // Move to the next token
    }
}


bool handle_single_quoted_string(token **tokens, t_command *cmd, int *arg_count) {
    if (!tokens || !*tokens || (*tokens)->token_type != SINGLE_QUOTED_STRING)
        return false; // Not a single-quoted string or invalid input

    // Add the single-quoted string as-is to the arguments
    cmd->args[(*arg_count)++] = strdup((*tokens)->value);
    if (!cmd->args[(*arg_count) - 1]) // Handle memory allocation failure
        return false;

    *tokens = (*tokens)->next; // Move to the next token
    return true; // Successfully handled the single-quoted string
}
 
bool set_command_and_args(token **tokens, t_command *cmd, int *arg_count) {
    if (!cmd->command && 
        ((*tokens)->token_type == WORD || 
         (*tokens)->token_type == DOUBLE_QUOTED_STRING || 
         (*tokens)->token_type == SINGLE_QUOTED_STRING)) 
    {
        cmd->command = strdup((*tokens)->value);
        if (!cmd->command) // Handle memory allocation failure
            return false;

        cmd->args[(*arg_count)++] = strdup(cmd->command);
        if (!cmd->args[(*arg_count) - 1]) { // Handle memory allocation failure
            free(cmd->command);
            cmd->command = NULL;
            return false;
        }

        *tokens = (*tokens)->next; // Move to the next token
        return true; // Successfully processed command and args
    }

    return false; // Logic was not applicable (cmd->command already set or invalid token)
}

int process_tokens(token **tokens, t_command *cmd, int *arg_count, int *input_count, int *output_count, int *heredoc_count) {
    // Declare and initialize necessary variables
    int max_args = INITIAL_ARGS_SIZE;
    int max_inputs = INITIAL_REDIRECT_SIZE;
    int max_outputs = INITIAL_REDIRECT_SIZE;
    int max_heredocs = INITIAL_REDIRECT_SIZE;

    while (*tokens && (*tokens)->token_type != PIPES) {
        if (set_command_and_args(tokens, cmd, arg_count) || handle_single_quoted_string(tokens, cmd, arg_count))
            continue;

        if ((*tokens)->token_type == WORD || (*tokens)->token_type == DOUBLE_QUOTED_STRING || (*tokens)->token_type == SINGLE_QUOTED_STRING)
            cmd->args[(*arg_count)++] = strdup((*tokens)->value);

        if (!handle_input_redirection(cmd, tokens, input_count, &max_inputs) || 
            !handle_output_redirection(cmd, tokens, output_count, &max_outputs) || 
            !handle_heredoc_redirection(cmd, tokens, heredoc_count, &max_heredocs))
            return 0; // Failure
        
        *tokens = (*tokens)->next;
    }
    return 1; // Success
}


t_command *parse_command(token *tokens)
{
               //  char *nb=ft_itoa(num);
              //   printf("\nfrom pars\n");
             //    store_env_variable("?","0");
            //     write_exit_status_to_file(0);

     if (!tokens)
        return NULL;

    t_command *head = NULL, *current = NULL;

    while (tokens) 
    {
        t_command *cmd = initialize_command();
        if (!cmd)
            return NULL;

        int arg_count = 0, input_count = 0, output_count = 0, heredoc_count = 0;

        if (!process_tokens(&tokens, cmd, &arg_count, &input_count, &output_count, &heredoc_count))
            return NULL;

        finalize_command(cmd, arg_count, input_count, output_count, heredoc_count);

        if (!head)
            head = cmd;
        else
            current->next = cmd;

        current = cmd;
        if (tokens && tokens->token_type == PIPES)
            tokens = tokens->next;
    }
    return head;
}
