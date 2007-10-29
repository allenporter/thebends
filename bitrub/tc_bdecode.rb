require 'test/unit'
require 'bencode'

class TC_Bencode < Test::Unit::TestCase
  def test_string
    assert_equal("spam", bdecode("4:spam"))
  end

  def test_emptystring
    assert_equal("", bdecode("0:"))
  end

  def test_invalidstring
    begin
      bdecode("-1:")
      raise "Should have raised ArgumentError"
    rescue ArgumentError
    end
  end

  def test_number
    assert_equal(3, bdecode("i3e"))
  end

  def test_negative_number
    assert_equal(-3, bdecode("i-3e"))
  end

  def test_invalid_number
    begin
      bdecode("i-0e")
      raise "Should have raised ArgumentError"
    rescue ArgumentError
    end
    begin
      bdecode("i03e")
      raise "Should have raised ArgumentError"
    rescue ArgumentError
    end
    begin
      bdecode("ie")
      raise "Should have raised ArgumentError"
    rescue ArgumentError
    end
    begin
      bdecode("i")
      raise "Should have raised ArgumentError"
    rescue ArgumentError
    end
  end

  def test_zero
    assert_equal(0, bdecode("i0e"))
  end

  def test_list
    assert_equal(["spam", "eggs"], bdecode("l4:spam4:eggse"))
    assert_equal([3, "eggs"], bdecode("li3e4:eggse"))
    assert_equal([ "a", "b" ], bdecode("l1:a1:be"))
  end

  def test_list_invalid
    begin
      bdecode("l1:a")
      raise "Should have raised ArgumentError"
    rescue ArgumentError
    end
  end

  def test_misc_invalid
    begin
      bdecode("ld41:a")
      raise "Should have raised ArgumentError"
    rescue ArgumentError
    end
  end

  def test_dict
    assert_equal({'cow' => 'moo', 'spam' => 'eggs'},
                 bdecode("d3:cow3:moo4:spam4:eggse"))
    assert_equal({'spam' => ['a', 'b']},
                 bdecode("d4:spaml1:a1:bee"))
  end

  def test_dict_invalid
    begin
      bdecode("dl1:a1:bee")
      raise "Should have raised ArgumentError"
    rescue ArgumentError
    end
  end
end
