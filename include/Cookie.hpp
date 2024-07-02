/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Cookie.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aloubier <aloubier@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/02 18:46:23 by aloubier          #+#    #+#             */
/*   Updated: 2024/07/02 18:46:24 by aloubier         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <map>
#include <string>

typedef std::map<std::string, std::string> hashmap;
typedef std::pair<std::string, std::string> strPair;

class Cookie {
private:
    hashmap _data;
public:
  Cookie();
  Cookie(std::string const &data);
  ~Cookie();
  Cookie(Cookie const &src);
  Cookie &operator=(Cookie const &rhs);

  bool empty() const ;
  std::string full() const ; //returns the full cookieheader value as a string
  void import(std::string const &data);
  strPair find(std::string const &key) const;
  bool exist(std::string const &key) const;
  void clear();
};
