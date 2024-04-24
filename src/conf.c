/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   conf.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aalvarez <aalvarez@student.42urduliz.com>  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/23 09:12:23 by aalvarez          #+#    #+#             */
/*   Updated: 2024/04/24 13:24:26 by aalvarez         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <string.h>
#include <getopt.h>
#include "ft_ping.h"

/**
 * Configures a set of addrinfo linked lists resultant from the calls to
 * getaddrinfo() (man getaddrinfo(3)) based on the number of destination
 * addresses specified by the user on program call.
 * @param addrs the addrinfo structs to be filled.
 * @param max_hosts the number of hosts.
 * @param hosts_addrs the destination hosts specified by the user.
 * @param domain the type of protocol domain, default is AF_INET.
 * @return EXIT_SUCCESS if all the calls to getaddrinfo() (man getaddrinfo(3))
 * were successful, EXIT_FAILURE if any call to the getaddrinfo()
 * (man socket(3)) function failed.
 */
static int configure_addr_info(struct addrinfo **addrs, int max_hosts, char **hosts_addrs, int domain) {
  struct addrinfo hints;
  memset(&hints, 0, sizeof(hints));

  hints.ai_socktype = SOCK_RAW;
  hints.ai_flags |= AI_CANONNAME;
  if (domain == AF_INET) {
    hints.ai_family = AF_INET;
    hints.ai_protocol = IPPROTO_ICMP;
  } else {
    hints.ai_family = AF_INET6;
    hints.ai_protocol = IPPROTO_ICMPV6;
  }

  int status;
  for (int i = optind; i < max_hosts; i++) {
    if ((status = getaddrinfo(hosts_addrs[i], NULL, &hints, &addrs[i - optind])) != 0) {
      fatal_error(hosts_addrs[i], gai_strerror(status));
      return EXIT_FAILURE;
    }
  }
  return EXIT_SUCCESS;
}

/**
 * Creates and configures a socket for each specified host.
 * @param max_hosts the number of hosts to create a socket from.
 * @param addrs the addrinfo structs to use as configuration for
 * the sockets.
 * @return EXIT_SUCCESS if every socket was successfully created,
 * EXIT_FAILURE if any call to the socket() (man socket(2)) function failed.
 */
static int configure_sockets(int max_hosts, struct addrinfo **addrs) {
  while (--max_hosts >= 0) {
    if ((params.sockets[max_hosts] = socket(addrs[max_hosts]->ai_family,
                                      addrs[max_hosts]->ai_socktype,
                                      addrs[max_hosts]->ai_protocol)) == -1) {
      fatal_error("socket", NULL);
      return EXIT_FAILURE;
    }
  }
  return EXIT_SUCCESS;
}

/**
 * Checks if the protocol will be configured to IPV4 or IPV6 and tries
 * to create all sockets configured with the corresponding configuration.
 * @param addrs the addrinfo structs to be configured and used as socket
 * configuration.
 * @param max_hosts the number of destination hosts.
 * @param hosts_addrs the destination hosts specified by the user.
 * @return EXIT_SUCCESS if all the addrinfo structs were successfully created and all
 * sockets were equally successfully created, EXIT_FAILURE if any of the configuration
 * functions failed, OTHER_ERROR if both IPV6 and IPV4 flags were set to true.
 */
int configure_icmp(struct addrinfo **addrs, int max_hosts, char **hosts_addrs) {
  // Tests if the flags in the bitset are set to true, it moves the IPv6 flag
  // one bit to the left, then uses a logical "or" comparison to check which bits were
  // set, the possible combinations would be ("0000", "0001", "0010", "0011"), corresponding
  // to the binary representation of the switch cases, being "0011" the default case.
  switch (IS_SET_FLAG(IPV6_FLAG) << 1 | IS_SET_FLAG(IPV4_FLAG)) {
    case 0:
    case 1:
      if (configure_addr_info(addrs, max_hosts + optind, hosts_addrs, AF_INET) != EXIT_SUCCESS) {
        return EXIT_FAILURE;
      }
      break;
    case 2:
      if (configure_addr_info(addrs, max_hosts + optind, hosts_addrs, AF_INET6)) {
        return EXIT_FAILURE;
      }
      break;
    default:
      print_err_msg(ONLY_ONE_IP_TYPE);
      return OTHER_ERROR;
  }

  return configure_sockets(max_hosts, addrs);
}
