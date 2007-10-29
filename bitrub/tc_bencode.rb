require 'test/unit'
require 'bencode'

class TC_Bencode < Test::Unit::TestCase
  def test_string
    assert_equal("4:spam", bencode("spam"))
  end

  def test_emptystring
    assert_equal("0:", bencode(""))
  end

  def test_number
    assert_equal("i3e", bencode(3))
  end

  def test_negative_number
    assert_equal("i-3e", bencode(-3))
  end

  def test_zero
    assert_equal("i0e", bencode(0))
  end

  def test_list
    assert_equal("l4:spam4:eggse", bencode(["spam", "eggs"]))
    assert_equal("li3e4:eggse",  bencode([3, "eggs"]))
    assert_equal("l1:a1:be", bencode([ "a", "b" ]))
  end

  def test_dict
    assert_equal("d3:cow3:moo4:spam4:eggse",
                 bencode({'cow' => 'moo', 'spam' => 'eggs'}))
    assert_equal("d4:spaml1:a1:bee",
                 bencode({'spam' => ['a', 'b']}))
  end

  def test_dict_invalid
    begin
      bencode({ 2 => "2" })
      raise "Should have raised ArgumentError"
    rescue ArgumentError
    end
  end
end
