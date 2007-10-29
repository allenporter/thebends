function uint32(b) {
  if (b < 0 || b > Math.pow(2, 32)) {
    throw "out of range: " + b;
  }
  this.appendToArray = function(data) {
    data.splice(data.length, 0, 
           (b >> 24) & 0xff,
           (b >> 16) & 0xff,
           (b >> 8) & 0xff,
           b & 0xff);
    return data;
  };
  return this;
}

function uint16(b) {
  if (b < 0 || b > Math.pow(2, 16)) {
    throw "out of range: " + b;
  }
  this.appendToArray = function(data) {
    data.splice(data.length, 0, this.getByte(1), this.getByte(0));
    return data;
  };
  this.getByte = function(byte) {
    var shift = byte * 8;
    var mask = 0xff << shift;
    var value = b & mask;
    return (value >>> shift);
  }
  return this;
}

function uint8(b) {
  if (b < 0 || b > Math.pow(2, 8)) {
    throw "out of  range: " + b;
  }
  this.getBit = function(bit) {
    var mask = (0x1 << bit);
    var value = b & mask;
    return (value >>> bit);
  }
  this.appendToArray = function(data) {
    data.splice(data.length, 0, b);
    return data;
  }
  return this;
}
