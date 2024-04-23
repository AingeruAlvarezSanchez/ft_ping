/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_ping.h                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aalvarez <aalvarez@student.42urduliz.com>  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/09 14:58:58 by aalvarez          #+#    #+#             */
/*   Updated: 2024/04/12 10:26:18 by aalvarez         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef FT_PING_H
#define FT_PING_H
#include <stdint.h>
#include <stdlib.h>

// 1. Program error related.

// Represents anything other than "EXIT_FAILURE".
#define OTHER_ERROR 2

typedef struct {
  int errnum;
  const char *err_msg;
} error_params;

enum error_codes {
  DEST_REQUIRED,
  HELP_MSG
};

// "error_params" array containing all the possible error messages associated
// to their corresponding error code defined in "error_codes".
extern const error_params errors[];

void print_err_msg(int errnum);
void fatal_error(const char *failed, const char *msg);

// 2. Program options and arguments related.

typedef struct {
  // 64 bit bitset to control program flags.
  // bits range from 0 to 9, then a to z, then A to Z and finally 3 wildcard bits.
  uint64_t program_flags;
  // Array containing each target address specified by the user in the command line.
  char **dst_addrs;
} program_params;

extern program_params params;

// You can add new single character flags by specifying them in this section.
// By program design, you are only allowed to use characters ranging
// from '0' to '9', from 'a' to 'z' and from 'A' to 'Z'.

// Allowed single-character flags, each character is a separate flag.
#define ALLOWED_FLAGS "h?"

// Number of ascii characters between '9' and 'A'.
#define UPPER_CASE_OFFSET 7

// Number of ascii characters between '9' and 'a'.
#define LOWER_CASE_OFFSET 13

// Sets the appropriate bits on the bitset to identify
// the program options selected by the user.
#define SET_UPPER_FLAG(opt)   (params.program_flags |= (1ul << (opt - UPPER_CASE_OFFSET - '0')))
#define SET_LOWER_FLAG(opt)   (params.program_flags |= (1ul << (opt - LOWER_CASE_OFFSET - '0')))
#define SET_NUMERIC_FLAG(opt) (params.program_flags |= (1ul << (opt - '0')))

int parse_program_args(int max_opts, char **opts_content);

#endif //FT_PING_H
