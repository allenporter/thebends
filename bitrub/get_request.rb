require 'uri'
require 'cgi'

class GetRequest
  attr_reader :uri
  def initialize(metainfo, local_peer)
    query = [ ]
    query.push("info_hash")
    query.push([metainfo.info_hash].pack("H*"))
    query.push("peer_id")
    query.push(local_peer.id)
    query.push("ip")
    query.push(local_peer.ip)
    query.push("port")
    query.push(local_peer.port)
    query.push("uploaded")
    query.push(0)
    query.push("downloaded")
    query.push(0)
    query.push("left")
    query.push(metainfo.files_length)
    query.push("compact")
    query.push(1)
    query.push("event")
    query.push("started")

    parts = [ ]
    for i in 0 .. query.length / 2
      key = query[i * 2]
      value = query[i * 2 + 1]
      if !value.nil?
        escaped = CGI.escape(value.to_s)
        parts.push("#{key}=#{escaped}")
      end
    end 
    @uri = URI.parse(metainfo.announce.to_s + "?" + parts.join("&"))
  end
end
