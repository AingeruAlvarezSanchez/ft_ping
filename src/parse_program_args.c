/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_program_args.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aalvarez <aalvarez@student.42urduliz.com>  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/10 14:53:19 by aalvarez          #+#    #+#             */
/*   Updated: 2024/04/23 12:16:06 by aalvarez         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <ctype.h>
#include <string.h>
#include <getopt.h>
#include "ft_ping.h"

/**
 * Sets to true the options passed to the program as arguments.
 * @param max_opts the maximum parameter limit (argc).
 * @param opts_content the pointer to arrays containing the
 * options (argv).
 * @return EXIT_SUCCESS if all the options are specified as valid,
 * OTHER_ERROR if an option was not recognized or the option "-h"
 * was passed as argument.
 */
int set_program_flags(int max_opts, char **opts_content) {
  int opt;
  while ((opt = getopt(max_opts, opts_content, ALLOWED_FLAGS)) != -1) {
    if (opt == '?' || opt == 'h') {
      print_err_msg(HELP_MSG);
      return OTHER_ERROR;
    }

    if (isalpha(opt)) {
      isupper(opt) ? SET_UPPER_FLAG(opt) : SET_LOWER_FLAG(opt);
    } else if (isdigit(opt)) {
      SET_NUMERIC_FLAG(opt);
    }
  }

  return EXIT_SUCCESS;
}

/**
 * Fills a pointer to arrays with the addresses passed
 * as params to the program.
 * @param size the number of addresses.
 * @param addr_array the location of the addresses (argv).
 * @return EXIT_SUCCESS if the addresses could be stored,
 * EXIT_FAILURE otherwise.
 */
int set_program_addr_array(size_t size, char **addr_array) {
  if (size == 0) {
    print_err_msg(DEST_REQUIRED);
    return EXIT_FAILURE;
  }

  params.dst_addrs = (char **)calloc(size + 1, sizeof(char *));
  if (!params.dst_addrs) {
    fatal_error("calloc", NULL);
    return EXIT_FAILURE;
  }

  memmove(params.dst_addrs, &addr_array[optind], (size*sizeof(char *)));
  params.dst_addrs[size] = NULL;

  return EXIT_SUCCESS;
}

/**
 * Wrapper function to parse both the program options and target
 * addresses.
 * @param max_opts the maximum parameter limit (argc).
 * @param opts_content the pointer to arrays containing the
 * options (argv).
 * @return EXIT_SUCCESS if both parsers success, the corresponding
 * error return code otherwise.
 */
int parse_program_args(int max_opts, char **opts_content) {
  int ret_code;
  if ((ret_code = set_program_flags(max_opts, opts_content)) != EXIT_SUCCESS
      || (ret_code = set_program_addr_array(max_opts - optind, opts_content) != EXIT_SUCCESS)) {
    return ret_code;
  }

  return EXIT_SUCCESS;
}
