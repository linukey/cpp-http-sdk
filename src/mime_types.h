#ifndef __HTTP_HTTPSERVER_MIME_TYPES__
#define __HTTP_HTTPSERVER_MIME_TYPES__

#include <iostream>
#include <map>
#include <string>

namespace http {
namespace mime_types {

std::string extension_to_type(std::string extension);

}}

#endif
