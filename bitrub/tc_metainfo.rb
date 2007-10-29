require 'test/unit'
require 'metainfo'

class TC_Bencode < Test::Unit::TestCase
  def test_fonts_metainfo
    metainfo = MetainfoReader.new("testdata/fonts.torrent").metainfo
    assert_equal("http://inferno.demonoid.com:3416/announce",
                 metainfo.announce.to_s)
    assert_equal("GE Inspira Fonts", metainfo.name)
    assert_equal("39e6d42f18b3d55f2e799b576b0b3beffa4ef189",
                 metainfo.info_hash.to_s)
    assert_equal(65536, metainfo.piece_length)
    assert_equal(
      ["3808ee71d7c6b9d27ce1b51555d748df13dceda3",
       "fd04f4a5653910c50e169d5e3398a37e8c8600f1",
       "973808e91b603730f06e5a07be84f7fb386830a7",
       "749940c8ee89e4707c1943c26d4888626bef5206",
       "0bcf4ae3087f6140dc6c978d83c979835a1a694b",
       "efb68d992bb8968fe86828f9255fdc7bd488b926",
       "1dcb961412b046e71ffb162a338dd3719c406a00",
       "e2784fd61a48b44c4b14ec86581c713ad7ca124e",
       "122e1fb595d84e8698faf24725a038314c45041e",
       "d881beae7bce236b47cb9936be778b6ebf53fcba",
       "741428f4acf4a4ada96febea05299705010ad40f",
       "b5ecb962cfb20fbf41b491e1d84fa1bf3b143aeb",
       "8a3ff63f8a3632bc33820e619a18a5ddc13a6e5e",
       "4e13e7dc8b7df3ecc5e61baeb72aabc400744a01",
       "d5fa3b8d6a6ef532c3d31df45cbf8a9814ef00be",
       "94ea0215a2f29300b7851a13aa430e6ee133ab35",
       "08dfa1626d4daeaf449a71ef616d2ae6f52b0d65",
       "f8b3b08ce7dc1f857e873d3acf810d8426a70c29",
       "efc6271392671f6d3bceb1404925e8e27d5f6123",
       "6194dd7d8cf46ea030baa61d0d1feb9365911760",
       "d4a2ea4c30017e5e05311b78b5076785b86a8e31",
       "034aa8e35bf34e4f8526c94a0991ca90982b3c08",
       "5fcd170787e45ee8a468763af8f942a326237526",
       "2361deadddafb1dabc2e44630a879a78d25951a4",
       "77b2a21ef84c6ee7d3b2c89e0fbe2be101d6088c",
       "c9a70b7b8f76cd805fde3e635099e5d2d7ee6b5d",
       "cd86255615943a47dc227f620d2aafb9c3979fd0",
       "bdc6fe63b318135a33a6e3beb7482449f1e8d2f4",
       "5eafa9df1c26a2536926ad3033dbbab005e73ad0",
       "df4f9b57e6c2a9d1e651d008ff592f68ac8f44a9",
       "9a3434d6d1df17f3389af40cf1064ffd594dc0b8",
       "b07990c53ae3105a523d7eb9c6514f589b88786e",
       "28739d032976fa81fa32adc426e6ee53aa42fe95",
       "23c678c4f2ccc678be92978e9801c0585e0e1618",
       "2daa07aa81a4310c2704007c996e4aa20b527462",
       "0b8573ac528398ae24ae7304ad44e391f7f805be",
       "66e7b482d4d6f6108111d5942c16f66ee7dfc456"],
      metainfo.pieces)
    assert_equal(37, metainfo.pieces.length)
    assert_equal(nil, metainfo.length)
    assert_equal(7, metainfo.files.length)
    assert_equal(
      [{"length"=>1597201, "path"=>["2006ecoreport.pdf"]},
       {"length"=>168936, "path"=>["GEInspBd.TTF"]},
       {"length"=>169464, "path"=>["GEInspBI.TTF"]},
       {"length"=>196884, "path"=>["GEInspIt.TTF"]},
       {"length"=>172428, "path"=>["GEInspRg.TTF"]},
       {"length"=>82444, "path"=>["GEInspSC.TTF"]},
       {"length"=>47, "path"=>["Torrent downloaded from Demonoid.com.txt"]}],
      metainfo.files)
    assert_equal(2387404, metainfo.files_length)
  end

  def test_macworld
    metainfo = MetainfoReader.new("testdata/macworld.torrent").metainfo
    assert_equal("http://inferno.demonoid.com:3416/announce",
                 metainfo.announce.to_s)
    assert_equal("MacWorld(US)-12-2007_noAds", metainfo.name)
    assert_equal("eedaa7ea19105e986d18407fe0978ca616e0b2da",
                 metainfo.info_hash.to_s)
    assert_equal(32768, metainfo.piece_length)
    assert_equal(489, metainfo.pieces.length)
    assert_equal(nil, metainfo.length)
    assert_equal(2, metainfo.files.length)
    assert_equal(
      [{"length"=>16008663, "path"=>["MacWorld(US)-12-2007_noAds.pdf"]},
       {"length"=>47, "path"=>["Torrent downloaded from Demonoid.com.txt"]}],
      metainfo.files)
    assert_equal(16008710, metainfo.files_length)
  end

  def test_vocals
    metainfo = MetainfoReader.new("testdata/vocals.torrent").metainfo
    assert_equal("http://inferno.demonoid.com:3416/announce",
                 metainfo.announce.to_s)
    assert_equal("vocal lessons", metainfo.name)
    assert_equal("42e3988a829bca537258e6c9d016daf80eb6e63a",
                 metainfo.info_hash.to_s)
    assert_equal(131072, metainfo.piece_length)
    assert_equal(1429, metainfo.pieces.length)
    assert_equal(nil, metainfo.length)
    assert_equal(106, metainfo.files.length)
  end
end
