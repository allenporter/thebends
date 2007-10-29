require 'test/unit'
require 'get_response'

class TC_Bencode < Test::Unit::TestCase
  def test_get_success
    response = GetResponse.new("d8:completei13e10:incompletei2e8:intervali7200e12:min intervali7200e5:peers90:;^\361\247\367,\030\017'\264R3V\217J\264\005HW\022-\272\017\253z\2479\321\300\000Vx\235\v\201T\324\317Y\016\203\367K\244\022\027~\357V\225\r=g\206Kk\315L#\202E\f\225Q\032\342E{\214q\032\341W~\246x]\213Sl\020z\363\326B5\333}\272\220e")
    assert_equal(13, response.complete)
    assert_equal(2, response.incomplete)
    assert_equal(7200, response.interval)
    assert_equal(7200, response.min_interval)
    assert_equal(15, response.peers.length)
    assert_equal([ "59.94.241.167:63276",
                   "24.15.39.180:21043",
                   "86.143.74.180:1352",
                   "87.18.45.186:4011",
                   "122.167.57.209:49152",
                   "86.120.157.11:33108",
                   "212.207.89.14:33783",
                   "75.164.18.23:32495",
                   "86.149.13.61:26502",
                   "75.107.205.76:9090",
                   "69.12.149.81:6882",
                   "69.123.140.113:6881",
                   "87.126.166.120:23947",
                   "83.108.16.122:62422",
                   "66.53.219.125:47760" ],
                 response.peers)
  end

  def test_get_failure
    response = GetResponse.new("d14:failure reason97:Cant access: your BitTorrent client is too old. Go to Demonoid.com and read the FAQ for more infoe")
    assert_equal(false, response.success?)
    assert_equal("Cant access: your BitTorrent client is too old. Go to Demonoid.com and read the FAQ for more info",
                 response.error_message)
  end
end
