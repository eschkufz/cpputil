// Copyright 2014 eric schkufza
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef CPPUTIL_INCLUDE_IO_REDIRECTSTREAM_H
#define CPPUTIL_INCLUDE_IO_REDIRECTSTREAM_H

#include <iostream>
#include <streambuf>

namespace cpputil {

template <typename Ch, typename Tr>
class basic_redirectbuf : public std::basic_streambuf<Ch,Tr> {
  public:
		typedef typename std::basic_streambuf<Ch,Tr>::char_type char_type;
    typedef typename std::basic_streambuf<Ch,Tr>::int_type int_type;   
		typedef typename std::basic_streambuf<Ch,Tr>::off_type off_type;
		typedef typename std::basic_streambuf<Ch,Tr>::pos_type pos_type;
    typedef typename std::basic_streambuf<Ch,Tr>::traits_type traits_type; 

    explicit basic_redirectbuf(std::basic_streambuf<Ch,Tr>* buf) 
      : std::basic_streambuf<Ch,Tr>(), buf_(buf) { }

		virtual ~basic_redirectbuf() { }

  protected:
    virtual void imbue(const std::locale& loc) { 
			buf_->pubimbue(loc); 
		}

    virtual std::basic_streambuf<Ch,Tr>* setbuf(char_type* s, std::streamsize n) { 
			return buf_->pubsetbuf(s, n); 
		}

    virtual pos_type seekoff(off_type off, std::ios_base::seekdir dir, std::ios_base::openmode which = std::ios_base::in | std::ios_base::out) { 
			return buf_->pubseekoff(off, dir, which); 
		}

    virtual pos_type seekpos(pos_type pos, std::ios_base::openmode which = std::ios_base::in | std::ios_base::out) { 
			return buf_->pubseekpos(pos, which); 
		}

    virtual int sync() { 
			return buf_->pubsync(); 
		}

    virtual int_type overflow(int_type c = traits_type::eof()) { 
			return buf_->sputc(traits_type::to_char_type(c)); 
		}

    virtual int_type underflow() { 
			return buf_->sgetc(); 
		}

    virtual int_type uflow() { 
			return buf_->sbumpc(); 
		}

  private:
    std::basic_streambuf<Ch,Tr>* buf_; 
};

template <typename Ch, typename Tr>
class basic_iredirectstream : public std::basic_istream<Ch,Tr> {
  public:
		explicit basic_iredirectstream(std::basic_istream<Ch,Tr>& is)   
      : std::basic_istream<Ch,Tr>(&buf_), buf_(is.rdbuf()) { }

		explicit basic_iredirectstream(std::basic_streambuf<Ch,Tr>* sb) 
      : std::basic_istream<Ch,Tr>(&buf_), buf_(sb) { }

		virtual ~basic_iredirectstream() { }

  private:
    basic_redirectbuf<Ch,Tr> buf_; 
};

typedef basic_iredirectstream<char, std::char_traits<char>> iredirectstream;        
typedef basic_iredirectstream<wchar_t, std::char_traits<wchar_t>> wiredirectstream; 

template <typename Ch, typename Tr>
class basic_oredirectstream : public std::basic_ostream<Ch,Tr> {
  public:
		explicit basic_oredirectstream(std::basic_ostream<Ch,Tr>& os)   
      : std::basic_ostream<Ch,Tr>(&buf_), buf_(os.rdbuf()) { }

		explicit basic_oredirectstream(std::basic_streambuf<Ch,Tr>* sb) 
      : std::basic_ostream<Ch,Tr>(&buf_), buf_(sb) { }

		virtual ~basic_oredirectstream() { }

  private:
    basic_redirectbuf<Ch,Tr> buf_; 
};

typedef basic_oredirectstream<char, std::char_traits<char>> oredirectstream;        
typedef basic_oredirectstream<wchar_t, std::char_traits<wchar_t>> woredirectstream; 

template <typename Ch, typename Tr>
class basic_redirectstream : public std::basic_iostream<Ch,Tr> {
  public:
		explicit basic_redirectstream(std::basic_iostream<Ch,Tr>& ios) 
      : std::basic_iostream<Ch,Tr>(&buf_), buf_(ios.rdbuf()) { }

		explicit basic_redirectstream(std::basic_streambuf<Ch,Tr>* sb) 
      : std::basic_iostream<Ch,Tr>(&buf_), buf_(sb) { }

		virtual ~basic_redirectstream() { }

  private:
    basic_redirectbuf<Ch,Tr> buf_; 
};

typedef basic_redirectstream<char, std::char_traits<char>> redirectstream;        
typedef basic_redirectstream<wchar_t, std::char_traits<wchar_t>> wredirectstream; 

} // namespace cpputil

#endif