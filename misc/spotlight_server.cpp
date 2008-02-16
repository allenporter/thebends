// spotlight_server.cpp
// Author: Allen Porter <allen@thebends.org>
// $ g++ -Wall -o spotlight_server spotlight_server.cpp -lyhttp -lythread -lynet
//       -framework CoreFoundation -framework CoreServices
#include <CoreFoundation/CoreFoundation.h>
#include <CoreServices/CoreServices.h>
#include <ythread/callback-inl.h>
#include <yhttp/httpserver.h>
#include <ynet/select.h>

class SpotlightServer {
 public:
  void Start() {
    ynet::Select select;
    yhttpserver::HTTPServer server(&select, 8080);
    server.RegisterHandler(
      "", ythread::NewCallback(this, &SpotlightServer::Input));
    server.RegisterHandler(
      "search", ythread::NewCallback(this, &SpotlightServer::Search));
    server.Start();
    select.Start();
  }

 protected:
  void Input(yhttpserver::HTTPRequest* request) {
    request->SetContentType("text/html");
    request->WriteString(
      "<html><body><form method=get action=search><input type=text name=q>"
      "<input type=submit name=btn value=Submit>"
      "</form></body></html>");
    request->Reply();
  }

  void Search(yhttpserver::HTTPRequest* request) {
    yhttpserver::URL* url = request->Url();
    yhttpserver::URL::ParamMap* map = url->Params();
    request->SetContentType("text/html");
    request->WriteString("<html><body>");
    yhttpserver::URL::ParamMap::const_iterator it = map->find("q");
    if (it != map->end()) {
      std::string q = it->second;
      request->WriteString("query is: ");
      char buf[BUFSIZ];
      snprintf(buf, BUFSIZ, "kMDItemTextContent == '*%s*'", q.c_str());
      request->WriteString(std::string(buf));
      request->WriteString("<br>");

      CFStringRef str =
        CFStringCreateWithCString(kCFAllocatorDefault, buf,
                                  kCFStringEncodingMacRoman);
      assert(str);

      MDQueryRef query = MDQueryCreate(kCFAllocatorDefault, str, NULL, NULL);
      assert(query);
      MDQueryExecute(query, kMDQuerySynchronous);
      CFIndex count = MDQueryGetResultCount(query);
      request->WriteString("results:<br><ul>");
      char title[BUFSIZ];
      for (int i = 0; i < count; ++i) {
        MDItemRef resultItem = (MDItemRef)MDQueryGetResultAtIndex(query, i);
        CFStringRef titleRef =
          (CFStringRef)MDItemCopyAttribute(resultItem, CFSTR("kMDItemFSName"));
        CFStringGetCString(titleRef, title, BUFSIZ, kCFStringEncodingMacRoman);
        request->WriteString("<li>");
        request->WriteString(title);
        request->WriteString("<br>\n");
// Iterate all attributes:
//       CFArrayRef attributeNames = MDItemCopyAttributeNames(resultItem);
//        CFIndex a_count = CFArrayGetCount(attributeNames);
//        for (int j = 0; j < a_count; ++j) {
//          CFShow(CFArrayGetValueAtIndex(attributeNames, j));
//        }
//        CFShow("---\n");
      }
      request->WriteString("</ul>eof");
      CFRelease(str);
    } else {
      request->WriteString("error");
    }
    request->WriteString("</body></html>");
    request->Reply();
  }

};

int main(int argc, char* argv[]) {
  SpotlightServer server;
  server.Start();
}
