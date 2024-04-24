/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_ping.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aalvarez <aalvarez@student.42urduliz.com>  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/09 14:58:51 by aalvarez          #+#    #+#             */
/*   Updated: 2024/04/24 09:04:03 by aalvarez         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <getopt.h>
#include "ft_ping.h"

program_params params = {0, NULL, NULL};

int main(int argc, char **argv) {
  int ret_code;
  if ((ret_code = parse_program_args(argc, argv)) != EXIT_SUCCESS) {
    return ret_code;
  }

  if ((ret_code = icmp_requests(argc - optind, argv)) != EXIT_SUCCESS) {
    free(params.dst_addrs);
    return ret_code;
  }

  free(params.dst_addrs);
  return EXIT_SUCCESS;
}
