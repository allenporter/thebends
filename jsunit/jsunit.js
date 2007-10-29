function TestCase() {
  this._tests = new Array();
}

TestCase.prototype._tests;

TestCase.prototype.addTest = function(callback) {
  this._tests.push(callback);
}

TestCase.prototype.runTests = function() {
  var status = document.createElement("DIV");
  var html = "<ul>";
  document.body.appendChild(status);

  var pass = 0;
  var fail = 0;
  for (test in this._tests) {
    var func = this._tests[test];
    html += "<li>Running " + funcname(func) + ":&nbsp;";
    try {
      func();
      html += "<font color=green>PASS</font>";
      pass++;
    } catch (err) {
      html += "<font color=red>FAIL</font>; ";
      html += "&nbsp;&nbsp;" + err;
      fail++;
    }
    html += "</li>";
  }
  html += "</ul><p>" + pass + " passed, " + fail + " failed";
  status.innerHTML = html;
  return (fail == 0);
}

function funcname(f) {
  var s = f.toString().match(/function (\w*)/)[1];
  if (s == null || s.length == 0) return "anonymous";
  return s;
}

function assertTrue(val) {
  if (val != true) {
    throw "assertTrue failed";
  }
}

function assertFalse(val) {
  return assertTrue(!val);
}

function assertEquals(a, b) {
  try {
    // TODO(aporter): Type safe comparison?
    return assertTrue(""+a == ""+b);
  } catch (e) {
    var s = "assertEquals('";
    s += a;
    s += "', '";
    s += b;
    s += "') failed";
    throw s;
  }
}

function assertNotEquals(a, b) {
  try {
    return assertTrue(a != b);
  } catch (e) {
    throw "assertNotEquals(" + a + ", " + b + ") failed";
  }
}
