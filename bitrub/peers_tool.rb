# Command line tool that repeatedly contacts a tracker to get as many peers
# for a .torrent file as possible.
# NOTE: Use with caution, since this ignores the interval specified by the
# tracker server.  Maybe they won't like this?
require 'net/http'
require 'metainfo'
require 'get_request'
require 'get_response'
require 'peer'

if ARGV.length != 1 
  $stderr.puts "Usage: peers_tool <file.torrent>\n"
  exit
end

metainfo = MetainfoReader.new(ARGV[0]).metainfo
peer_id = new_peer_id()
peer = LocalPeer.new(peer_id, 6882)
uri = GetRequest.new(metainfo, peer).uri

peers = { }
last_peers_length = peers.length

while true
  h = Net::HTTP.new(uri.host, uri.port)
  resp, data = h.get(uri.request_uri, nil)
  resp.each {|key, val| printf "%-14s = %-40.40s\n", key, val }

  response = GetResponse.new(data)

  if !response.success?
    $stderr.puts "FAILED"
    $stderr.puts response.error_message
    exit(1)
  end
  response.peers.each { |peer| peers[peer] = 1 }
  new_peers = peers.length - last_peers_length
  if new_peers <= 0
    break
  end
  $stderr.puts "Got #{new_peers} new peers, #{peers.length} total"
  last_peers_length = peers.length
end

$stderr.puts "Finished with #{peers.length} peers:"
puts peers.keys
