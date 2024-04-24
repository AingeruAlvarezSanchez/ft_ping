/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   icmp.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aalvarez <aalvarez@student.42urduliz.com>  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/12 14:08:33 by aalvarez          #+#    #+#             */
/*   Updated: 2024/04/24 09:32:43 by aalvarez         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_ping.h"

/**
 * Main handler for the icmp addresses and socket configuration and
 * packet echo requests.
 * @param max_hosts the number of hosts.
 * @param hosts_addrs the destination hosts specified by the user.
 * @return EXIT_SUCCESS if the packets were successfully delivered, ret_code,
 * which contains EXIT_FAILURE or OTHER_ERROR, depending on the rest of the
 * handlers otherwise.
 */
int icmp_requests(int max_hosts, char **hosts_addrs) {
  struct addrinfo  *addrs[max_hosts] = {};
  int sock_arr[max_hosts] = {};
  params.sockets = sock_arr;

  int ret_code;
  if ((ret_code = configure_icmp(addrs, max_hosts, hosts_addrs)) != EXIT_SUCCESS) {
    clean_program(addrs, max_hosts);
    return ret_code;
  }

  return clean_program(addrs, max_hosts);
}
