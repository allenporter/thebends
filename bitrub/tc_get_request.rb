require 'test/unit'
require 'get_request'
require 'peer'

class TC_Bencode < Test::Unit::TestCase
  def test_get_request
    metainfo = MetainfoReader.new("testdata/fonts.torrent").metainfo
    peer_id = new_peer_id()
    peer = LocalPeer.new(peer_id, 6882)
    uri = GetRequest.new(metainfo, peer).uri
    assert_equal("http://inferno.demonoid.com:3416/announce?info_hash=9%E6%D4%2F%18%B3%D5_.y%9BWk%0B%3B%EF%FAN%F1%89&peer_id=#{peer_id}&port=6882&uploaded=0&downloaded=0&left=2387404&compact=1&event=started",
                 uri.to_s)
  end
end
