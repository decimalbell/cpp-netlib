//
// reply.hpp
// ~~~~~~~~~
//
// Copyright (c) 2003-2008 Christopher M. Kohlhoff (chris at kohlhoff dot com)
// Copyright (c) 2009 Dean Michael Berris (mikhailberis@gmail.com)
// Copyright (c) 2009 Tarroo, Inc.
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Note: This implementation has significantly changed from the original example
// from a plain header file into a header-only implementation using C++ templates
// to reduce the dependence on building an external library.
//

#ifndef BOOST_NETWORK_HTTP_REPLY_HPP_
#define BOOST_NETWORK_HTTP_REPLY_HPP_

#include <string>
#include <vector>
#include <boost/asio.hpp>
#include <boost/lexical_cast.hpp>
#include "header.hpp"

namespace boost { namespace network { namespace http {

    namespace tag {
        struct default_;
    }

    /// A reply to be sent to a client.
    template <class Tag>
    struct basic_reply
    {
      /// The status of the reply.
      enum status_type
      {
        ok = 200,
        created = 201,
        accepted = 202,
        no_content = 204,
        multiple_choices = 300,
        moved_permanently = 301,
        moved_temporarily = 302,
        not_modified = 304,
        bad_request = 400,
        unauthorized = 401,
        forbidden = 403,
        not_found = 404,
        not_supported = 405,
        not_acceptable = 406,
        internal_server_error = 500,
        not_implemented = 501,
        bad_gateway = 502,
        service_unavailable = 503
      } status;

      /// The headers to be included in the reply.
      std::vector<request_header> headers;

      /// The content to be sent in the reply.
      std::string content;

      /// Convert the reply into a vector of buffers. The buffers do not own the
      /// underlying memory blocks, therefore the reply object must remain valid and
      /// not be changed until the write operation has completed.
      std::vector<boost::asio::const_buffer> to_buffers() {
          using boost::asio::const_buffer;
          using boost::asio::buffer;
          static const char name_value_separator[] = { ':', ' ' };
          static const char crlf[] = { '\r', '\n' };
          std::vector<const_buffer> buffers;
          buffers.push_back(to_buffer(status));
          for (std::size_t i = 0; i < headers.size(); ++i)
          {
            request_header& h = headers[i];
            buffers.push_back(buffer(h.name));
            buffers.push_back(buffer(name_value_separator));
            buffers.push_back(buffer(h.value));
            buffers.push_back(buffer(crlf));
          }
          buffers.push_back(buffer(crlf));
          buffers.push_back(buffer(content));
          return buffers;
      }

      /// Get a stock reply.
      static basic_reply stock_reply(status_type status) {
          using boost::lexical_cast;
          basic_reply rep;
          rep.status = status;
          rep.content = to_string(status);
          rep.headers.resize(2);
          rep.headers[0].name = "Content-Length";
          rep.headers[0].value = lexical_cast<std::string>(rep.content.size());
          rep.headers[1].name = "Content-Type";
          rep.headers[1].value = "text/html";
          return rep;
      }

      private:

        static std::string to_string(status_type status) {
            static const char ok[] = "";
            static const char created[] =
              "<html>"
              "<head><title>Created</title></head>"
              "<body><h1>201 Created</h1></body>"
              "</html>";
            static const char accepted[] =
              "<html>"
              "<head><title>Accepted</title></head>"
              "<body><h1>202 Accepted</h1></body>"
              "</html>";
            static const char no_content[] =
              "<html>"
              "<head><title>No Content</title></head>"
              "<body><h1>204 Content</h1></body>"
              "</html>";
            static const char multiple_choices[] =
              "<html>"
              "<head><title>Multiple Choices</title></head>"
              "<body><h1>300 Multiple Choices</h1></body>"
              "</html>";
            static const char moved_permanently[] =
              "<html>"
              "<head><title>Moved Permanently</title></head>"
              "<body><h1>301 Moved Permanently</h1></body>"
              "</html>";
            static const char moved_temporarily[] =
              "<html>"
              "<head><title>Moved Temporarily</title></head>"
              "<body><h1>302 Moved Temporarily</h1></body>"
              "</html>";
            static const char not_modified[] =
              "<html>"
              "<head><title>Not Modified</title></head>"
              "<body><h1>304 Not Modified</h1></body>"
              "</html>";
            static const char bad_request[] =
              "<html>"
              "<head><title>Bad Request</title></head>"
              "<body><h1>400 Bad Request</h1></body>"
              "</html>";
            static const char unauthorized[] =
              "<html>"
              "<head><title>Unauthorized</title></head>"
              "<body><h1>401 Unauthorized</h1></body>"
              "</html>";
            static const char forbidden[] =
              "<html>"
              "<head><title>Forbidden</title></head>"
              "<body><h1>403 Forbidden</h1></body>"
              "</html>";
            static const char not_found[] =
              "<html>"
              "<head><title>Not Found</title></head>"
              "<body><h1>404 Not Found</h1></body>"
              "</html>";
            static const char not_supported[] =
              "<html>"
              "<head><title>Method Not Supported</title></head>"
              "<body><h1>Method Not Supported</h1></body>"
              "</html>";
            static const char not_acceptable[] =
              "<html>"
              "<head><title>Request Not Acceptable</title></head>"
              "<body><h1>Request Not Acceptable</h1></body>"
              "</html>";
            static const char internal_server_error[] =
              "<html>"
              "<head><title>Internal Server Error</title></head>"
              "<body><h1>500 Internal Server Error</h1></body>"
              "</html>";
            static const char not_implemented[] =
              "<html>"
              "<head><title>Not Implemented</title></head>"
              "<body><h1>501 Not Implemented</h1></body>"
              "</html>";
            static const char bad_gateway[] =
              "<html>"
              "<head><title>Bad Gateway</title></head>"
              "<body><h1>502 Bad Gateway</h1></body>"
              "</html>";
            static const char service_unavailable[] =
              "<html>"
              "<head><title>Service Unavailable</title></head>"
              "<body><h1>503 Service Unavailable</h1></body>"
              "</html>";

             switch (status)
              {
              case basic_reply::ok:
                return ok;
              case basic_reply::created:
                return created;
              case basic_reply::accepted:
                return accepted;
              case basic_reply::no_content:
                return no_content;
              case basic_reply::multiple_choices:
                return multiple_choices;
              case basic_reply::moved_permanently:
                return moved_permanently;
              case basic_reply::moved_temporarily:
                return moved_temporarily;
              case basic_reply::not_modified:
                return not_modified;
              case basic_reply::bad_request:
                return bad_request;
              case basic_reply::unauthorized:
                return unauthorized;
              case basic_reply::forbidden:
                return forbidden;
              case basic_reply::not_found:
                return not_found;
              case basic_reply::not_supported:
                return not_supported;
              case basic_reply::not_acceptable:
                return not_acceptable;
              case basic_reply::internal_server_error:
                return internal_server_error;
              case basic_reply::not_implemented:
                return not_implemented;
              case basic_reply::bad_gateway:
                return bad_gateway;
              case basic_reply::service_unavailable:
                return service_unavailable;
              default:
                return internal_server_error;
              }
        }

        boost::asio::const_buffer to_buffer(status_type status) {
            using boost::asio::buffer;
            static const std::string ok =
              "HTTP/1.0 200 OK\r\n";
            static const std::string created =
              "HTTP/1.0 201 Created\r\n";
            static const std::string accepted =
              "HTTP/1.0 202 Accepted\r\n";
            static const std::string no_content =
              "HTTP/1.0 204 No Content\r\n";
            static const std::string multiple_choices =
              "HTTP/1.0 300 Multiple Choices\r\n";
            static const std::string moved_permanently =
              "HTTP/1.0 301 Moved Permanently\r\n";
            static const std::string moved_temporarily =
              "HTTP/1.0 302 Moved Temporarily\r\n";
            static const std::string not_modified =
              "HTTP/1.0 304 Not Modified\r\n";
            static const std::string bad_request =
              "HTTP/1.0 400 Bad Request\r\n";
            static const std::string unauthorized =
              "HTTP/1.0 401 Unauthorized\r\n";
            static const std::string forbidden =
              "HTTP/1.0 403 Forbidden\r\n";
            static const std::string not_found =
              "HTTP/1.0 404 Not Found\r\n";
            static const std::string not_supported =
              "HTTP/1.0 405 Method Not Supported\r\n";
            static const std::string not_acceptable =
              "HTTP/1.0 406 Method Not Acceptable\r\n";
            static const std::string internal_server_error =
              "HTTP/1.0 500 Internal Server Error\r\n";
            static const std::string not_implemented =
              "HTTP/1.0 501 Not Implemented\r\n";
            static const std::string bad_gateway =
              "HTTP/1.0 502 Bad Gateway\r\n";
            static const std::string service_unavailable =
              "HTTP/1.0 503 Service Unavailable\r\n";

            switch (status) {
                case basic_reply::ok:
                    return buffer(ok);
                case basic_reply::created:
                    return buffer(created);
                case basic_reply::accepted:
                    return buffer(accepted);
                case basic_reply::no_content:
                    return buffer(no_content);
                case basic_reply::multiple_choices:
                    return buffer(multiple_choices);
                case basic_reply::moved_permanently:
                    return buffer(moved_permanently);
                case basic_reply::moved_temporarily:
                    return buffer(moved_temporarily);
                case basic_reply::not_modified:
                    return buffer(not_modified);
                case basic_reply::bad_request:
                    return buffer(bad_request);
                case basic_reply::unauthorized:
                    return buffer(unauthorized);
                case basic_reply::forbidden:
                    return buffer(forbidden);
                case basic_reply::not_found:
                    return buffer(not_found);
                case basic_reply::not_supported:
                    return buffer(not_supported);
                case basic_reply::not_acceptable:
                    return buffer(not_acceptable);
                case basic_reply::internal_server_error:
                    return buffer(internal_server_error);
                case basic_reply::not_implemented:
                    return buffer(not_implemented);
                case basic_reply::bad_gateway:
                    return buffer(bad_gateway);
                case basic_reply::service_unavailable:
                    return buffer(service_unavailable);
                default:
                    return buffer(internal_server_error);
            }
        }


    };

    typedef basic_reply<tag::default_> reply;

} // namespace http

} // namespace network

} // namespace boost

#endif // BOOST_NETWORK_HTTP_REPLY_HPP_

