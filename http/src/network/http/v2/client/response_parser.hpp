// Copyright (C) 2013 by Glyn Matthews
// Copyright Dean Michael Berris 2010.
// Copyright 2011 Dean Michael Berris (dberris@google.com).
// Copyright 2011 Google, Inc.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef NETWORK_HTTP_V2_CLIENT_RESPONSE_PARSER_INC
#define NETWORK_HTTP_V2_CLIENT_RESPONSE_PARSER_INC

#include <utility>
#include <boost/range.hpp>
#include <boost/logic/tribool.hpp>
#include <boost/algorithm/string/classification.hpp>

namespace network {
  namespace http {
    namespace v2 {

      /**
       * \ingroup http_client
       * \class response_parser response_parser.hpp network/http/v2/client/response_parser.hpp
       */
      struct response_parser {

        enum state_t {
          http_response_begin,
          http_version_h,
          http_version_t1,
          http_version_t2,
          http_version_p,
          http_version_slash,
          http_version_major,
          http_version_dot,
          http_version_minor,
          http_version_done,
          http_status_digit,
          http_status_done,
          http_status_message_char,
          http_status_message_cr,
          http_status_message_done,
          http_header_name_char,
          http_header_colon,
          http_header_value_char,
          http_header_line_cr,
          http_header_line_done,
          http_headers_end_cr,
          http_headers_done
        };

        explicit response_parser(state_t state = http_response_begin)
          : state_(state) {}

        ~response_parser() {}

        template <class Range>
        std::tuple<
          boost::logic::tribool,
          boost::iterator_range<typename Range::const_iterator>>
          parse_until(state_t stop_state,
                      const Range& range_) {
          boost::logic::tribool parsed_ok(boost::logic::indeterminate);
          typename Range::const_iterator start = boost::begin(range_),
            current = start,
            end = boost::end(range_);
          boost::iterator_range<typename Range::const_iterator> local_range =
            boost::make_iterator_range(start, end);
          if (boost::empty(local_range))
            parsed_ok = false;
          while (!boost::empty(local_range) && indeterminate(parsed_ok)) {
            current = boost::begin(local_range);
            if (state_ == stop_state) {
              parsed_ok = true;
            } else {
              switch (state_) {
              case http_response_begin:
                if (*current == ' ' || *current == '\r' || *current == '\n') {
                  // skip valid leading whitespace
                  ++start;
                  ++current;
                } else if (*current == 'H') {
                  state_ = http_version_h;
                  start = current;
                  ++current;
                } else {
                  parsed_ok = false;
                }
                break;
              case http_version_h:
                if (*current == 'T') {
                  state_ = http_version_t1;
                  ++current;
                } else {
                  parsed_ok = false;
                }
                break;
              case http_version_t1:
                if (*current == 'T') {
                  state_ = http_version_t2;
                  ++current;
                } else {
                  parsed_ok = false;
                }
                break;
              case http_version_t2:
                if (*current == 'P') {
                  state_ = http_version_p;
                  ++current;
                } else {
                  parsed_ok = false;
                }
                break;
              case http_version_p:
                if (*current == '/') {
                  state_ = http_version_slash;
                  ++current;
                } else {
                  parsed_ok = false;
                }
                break;
              case http_version_slash:
                if (boost::algorithm::is_digit()(*current)) {
                  state_ = http_version_major;
                  ++current;
                } else {
                  parsed_ok = false;
                }
                break;
              case http_version_major:
                if (*current == '.') {
                  state_ = http_version_dot;
                  ++current;
                } else {
                  parsed_ok = false;
                }
                break;
              case http_version_dot:
                if (boost::algorithm::is_digit()(*current)) {
                  state_ = http_version_minor;
                  ++current;
                } else {
                  parsed_ok = false;
                }
                break;
              case http_version_minor:
                if (*current == ' ') {
                  state_ = http_version_done;
                  ++current;
                } else {
                  parsed_ok = false;
                }
                break;
              case http_version_done:
                if (boost::algorithm::is_digit()(*current)) {
                  state_ = http_status_digit;
                  ++current;
                } else {
                  parsed_ok = false;
                }
                break;
              case http_status_digit:
                if (boost::algorithm::is_digit()(*current)) {
                  ++current;
                } else if (*current == ' ') {
                  state_ = http_status_done;
                  ++current;
                } else {
                  parsed_ok = false;
                }
                break;
              case http_status_done:
                if (boost::algorithm::is_alnum()(*current)) {
                  state_ = http_status_message_char;
                  ++current;
                } else {
                  parsed_ok = false;
                }
                break;
              case http_status_message_char:
                if (boost::algorithm::is_alnum()(*current) ||
                    boost::algorithm::is_punct()(*current) || (*current == ' ')) {
                  ++current;
                } else if (*current == '\r') {
                  state_ = http_status_message_cr;
                  ++current;
                } else {
                  parsed_ok = false;
                }
                break;
              case http_status_message_cr:
                if (*current == '\n') {
                  state_ = http_status_message_done;
                  ++current;
                } else {
                  parsed_ok = false;
                }
                break;
              case http_status_message_done:
              case http_header_line_done:
                if (boost::algorithm::is_alnum()(*current)) {
                  state_ = http_header_name_char;
                  ++current;
                } else if (*current == '\r') {
                  state_ = http_headers_end_cr;
                  ++current;
                } else {
                  parsed_ok = false;
                }
                break;
              case http_header_name_char:
                if (*current == ':') {
                  state_ = http_header_colon;
                  ++current;
                } else if (boost::algorithm::is_alnum()(*current) ||
                           boost::algorithm::is_space()(*current) ||
                           boost::algorithm::is_punct()(*current)) {
                  ++current;
                } else {
                  parsed_ok = false;
                }
                break;
              case http_header_colon:
                if (boost::algorithm::is_space()(*current)) {
                  ++current;
                } else if (boost::algorithm::is_alnum()(*current) ||
                           boost::algorithm::is_punct()(*current)) {
                  state_ = http_header_value_char;
                  ++current;
                } else {
                  parsed_ok = false;
                }
                break;
              case http_header_value_char:
                if (*current == '\r') {
                  state_ = http_header_line_cr;
                  ++current;
                } else if (boost::algorithm::is_cntrl()(*current)) {
                  parsed_ok = false;
                } else {
                  ++current;
                }
                break;
              case http_header_line_cr:
                if (*current == '\n') {
                  state_ = http_header_line_done;
                  ++current;
                } else {
                  parsed_ok = false;
                }
                break;
              case http_headers_end_cr:
                if (*current == '\n') {
                  state_ = http_headers_done;
                  ++current;
                } else {
                  parsed_ok = false;
                }
                break;
              default:
                parsed_ok = false;
              }
            }

            local_range = boost::make_iterator_range(current, end);
          }
          if (state_ == stop_state)
            parsed_ok = true;
          return std::make_tuple(parsed_ok,
                                 boost::make_iterator_range(start, current));
        }

        state_t state() { return state_; }

        void reset(state_t new_state = http_response_begin) { state_ = new_state; }

      private:
        state_t state_;

      };

    } // namespace v2
  } // namespace http
} // namespace network

#endif // NETWORK_HTTP_V2_CLIENT_RESPONSE_PARSER_INC
