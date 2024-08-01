/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpMethod.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aloubier <aloubier@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/01 18:45:38 by aloubier          #+#    #+#             */
/*   Updated: 2024/08/01 18:45:39 by aloubier         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <string>

class Request;
class Response;
class HttpMethod {
    public:
static void del(Request const &req, Response &res);
static void get(Request const &req, Response &res);
static void post(Request const &req, Response &res, std::string const &path);
};
