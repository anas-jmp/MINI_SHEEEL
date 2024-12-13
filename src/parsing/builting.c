// #include "../../include/minishell.h"
// #include <stdio.h>
// #include <stdlib.h>
// #include <string.h>
// #define INITIAL_REDIRECT_SIZE 4 // Initial size for redirection arrays
// #define INITIAL_ARGS_SIZE 16
// /*
//   next_tok_par(start,str,env,type);
//   next_tok_par1(start,str,env,status ,type);
//   next_tok_par2(start,str,env,status ,type);
//   next_tok_par3(start,str,env,status ,type);

//  */
// char *next_tok_par(const char *start,const char **str,char **env, token_type *type)
// {
//   if (strncmp(start, "echo", 4) == 0 && (isspace(start[4]) || start[4] == '\0')) 
//   {
//     *str += 4;
//     char *remain = strdup(str[0]);
//      *type = WORD;
//     return strdup("echo");
//   }
// if (strncmp(start, "unset", 5) == 0 && (isspace(start[5]) || start[5] == '\0'))
//     {
//       *str += 5;  // Move past 'unset'    
//        while (isspace(**str))
//           (*str)++;
//        const char *variable_name = *str;//strtok(*str, " \t\n");
//       if (variable_name)
//           handle_unset(variable_name);
//     }
//   if (strncmp(start, "export", 6) == 0 && (isspace(start[6]) || start[6] == '\0')) 
//   {
//     *str += 6;
//     char *remain = strdup(str[0]);
//      handle_export_command(remain);
//     *type = WORD;
//     return strdup("export");
//   }
//       return NULL; // Return NULL to indicate no result
// }
// char *next_tok_par1(const char *start,const char **str,char **env,int *status ,token_type *type)
// {
  
// if (**str == '"' || **str == '\'') 
//   {
//     char quote = **str;
//     (*str)++;

//     const char *quoted_start = *str;
//     while (**str && **str != quote)
//       (*str)++;
//     size_t len = *str - quoted_start;
//     char *token_value = malloc(len + 1);
//     strncpy(token_value, quoted_start, len);
//     token_value[len] = '\0';

//     if (**str == quote)
//       (*str)++;

//     if (quote == '\'') 
//     {
//       *type = SINGLE_QUOTED_STRING;
//       return token_value;
//     } else {
//       char *expanded_token =
//           handle_quoted_string_with_expansion(token_value, env);
//       free(token_value);
//       *type = DOUBLE_QUOTED_STRING;
//       return expanded_token;
//     }
//   }
// }
// char *next_tok_par2(const char *start,const char **str,char **env,token_type *type)
// {
//   if (**str == '$')
//    {
//     (*str)++;
//     const char *var_start = *str;

//     while (isalnum(**str) || **str == '_' || **str == '?')
//       (*str)++;
//     size_t len = *str - var_start;
//     char var_name[len + 1];
//     strncpy(var_name, var_start, len);
//     var_name[len] = '\0';

//     char *expanded_value = expand_variable(var_name, env);
//     if (expanded_value) {
//       return expanded_value;
//     } 
//     else 
//     {
//       return strdup("");
//     }
//   }
//   return NULL;
// }
// char *next_tok_par3(const char **str,token_type *type)
// {
//       if (**str == '>' || **str == '<' || **str == '|') 
//   {
//     char symbol = **str;
//     (*str)++;

//     if (symbol == '>' && **str == '>')
//     {
//       (*str)++;
//       *type = OUTPUT_REDIRECTION_APPEND_MODE;
//       return strdup(">>");
//     }

//     if (symbol == '<' && **str == '<') {
//       (*str)++;
//       *type = HERE_DOC;
//       return strdup("<<");
//     }
//     else {
//       char temp[2] = {symbol, '\0'};
//       if (symbol == '>')
//         *type = OUTPUT_REDIRECTION;
//       else if (symbol == '<')
//         *type = INPUT_REDIRECTION;
//       else if (symbol == '|')
//         *type = PIPES;
//       return strdup(temp);
//     }
//   }
// return NULL;
// }