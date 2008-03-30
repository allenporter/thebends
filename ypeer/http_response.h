// http_response.h
// Author: Allen Porter <allen@thebends.org>

#include <string>

namespace ypeer {

bool ParseHTTPResponse(const std::string& input,
                       std::string* body);

}  // namespace ypeer
